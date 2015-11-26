#include "config.h"
#include "netserver.h"
#include "session_manager.h"
#include "credentials_manager.h"

#include <cassert>
#include <atomic>
#include <queue>

#include <liblog.h>
#include <botan/tls_server.h>

MODULE_LOG_NAME("NetServer");

//服务端网络模块(多平台)

const size_t BufferSize = 1024;				//接收缓存区的大小

struct TSocketEx
{
	std::atomic_int nRefCount;

	CNetServer * pNetServer;
	ip::tcp::socket * socket;
	char rawBuffer[BufferSize];
	std::string recvBuffer;
	bool bReadyToClose;

	std::queue<std::pair<char *, size_t> > qSend;
	std::mutex mutexSend;

	bool TLS;
	Botan::TLS::Server * pTLSServer;

	TSocketEx(CNetServer * pNetServer, ip::tcp::socket * socket, bool bTLS) :
		pNetServer(pNetServer), socket(socket), nRefCount(1),
		pTLSServer(nullptr), TLS(bTLS), bReadyToClose(false)
	{
		lprintf_d("Socket %p Created", this);
		pNetServer->m_mutexClientSocks.lock();
		pNetServer->m_vpClientSocks.insert(this);
		pNetServer->m_mutexClientSocks.unlock();
	}
	~TSocketEx()
	{
		lprintf_d("Socket %p Deleted", this);
		if (pTLSServer)
			delete pTLSServer;
		while (!qSend.empty())
		{
			delete qSend.front().first;
			qSend.pop();
		}
		socket->close();
		delete socket;
	}

	void AddRef()
	{
		nRefCount++;
		lprintf_d("Socket %p AddRef", this);
	}
	void Release()
	{
		int refNow = --nRefCount;
		lprintf_d("Socket %p Release", this);
		if (refNow <= 0)
		{
			std::mutex& mutexSocks = pNetServer->m_mutexClientSocks;
			mutexSocks.lock();
			pNetServer->m_vpClientSocks.erase(this);
			delete this;
			mutexSocks.unlock();
		}
	}

	bool postToSendQueue(const char * pData, size_t nLen)
	{
		bool ret;
		char * pBuffer = new char[nLen];
		memcpy(pBuffer, pData, nLen);
		mutexSend.lock();
		ret = !qSend.empty();
		qSend.push(std::make_pair(pBuffer, nLen));
		mutexSend.unlock();
		return ret;
	}
};

CNetServer::CNetServer() :
	m_pCredManager(nullptr),
	m_pPolicy(nullptr),
	m_pTLSSM(nullptr)
{
	m_rng = new Botan::AutoSeeded_RNG;
	m_pSManager = new CSessionManager;
}

CNetServer::~CNetServer()
{
	if (m_pCredManager)
		delete m_pCredManager;
	if (m_pPolicy)
		delete m_pPolicy;
	if (m_pTLSSM)
		delete m_pTLSSM;

	delete m_pSManager;
	delete m_rng;
}

bool CNetServer::initNetServer()
{
	return true;
}

bool CNetServer::startServer(ILibClassFactory * pClassFactory)
{
	m_pSManager->startServer(pClassFactory);
	if (g_configSvr.nTLS && !initTLS())
	{
		if (g_configSvr.nTLS == 2)
			return false;
		g_configSvr.nTLS = 0;
		lprintf_w("TLS will be disabled");
	}
	if (!initListenSocket())
		return false;

	int nThread = std::thread::hardware_concurrency() * g_configSvr.nThreadPerCPU;
	for (int i = 0; i < nThread; i++)
	{
		std::shared_ptr<std::thread> pThread(new std::thread(&CNetServer::workerThread, this));
		m_vThreads.push_back(pThread);
	}
	return true;
}

bool CNetServer::stopServer()
{
	m_mutexClientSocks.lock();
	for (auto clientSock : m_vpClientSocks)
	{
		if (!clientSock->bReadyToClose)
			setSocketClose(clientSock);
	}
	m_mutexClientSocks.unlock();

	m_ioService.stop();
	for (auto thread : m_vThreads)
		thread->join();

	for (auto clientSock : m_vpClientSocks)
		delete clientSock;

	m_pSManager->stopServer();
	return true;
}

bool CNetServer::initTLS()
{
	try
	{
		m_pCredManager = new CCredentialsManager(m_rng);
		for (auto cert : g_configSvr.vCerts)
		{
			m_pCredManager->loadCertificate(cert.strCert, cert.strKey, cert.strPassphrase);
		}
		m_pPolicy = new Botan::TLS::Policy;
		m_pTLSSM = new Botan::TLS::Session_Manager_In_Memory(*m_rng);
	}
	catch (std::exception& e)
	{
		lprintf_e("An error occurred while init TLS: \n%s", e.what());
		return false;
	}
	return true;
}

bool CNetServer::initListenSocket()
{
	ip::address_v4 addr;
	addr.from_string(g_configSvr.strServerIP);

	m_pacceptorMain = new ip::tcp::acceptor(m_ioService, ip::tcp::endpoint(addr, g_configSvr.nPort));
	createNewSocket(false);

	if (g_configSvr.nTLS)
	{
		m_pacceptorTLS = new ip::tcp::acceptor(m_ioService, ip::tcp::endpoint(addr, g_configSvr.nTLSPort));
		createNewSocket(true);
	}
	return true;
}

void CNetServer::createNewSocket(bool bTLS)
{
	TSocketEx * pListenSocket = new TSocketEx(this, new ip::tcp::socket(m_ioService), bTLS);
	ip::tcp::acceptor * pAcceptor = (bTLS ? m_pacceptorTLS : m_pacceptorMain);
	pAcceptor->async_accept(*pListenSocket->socket, std::bind(&CNetServer::doAccept, this, pListenSocket, std::placeholders::_1));
}

bool CNetServer::createTLSSession(TSocketEx * pSocket)
{
	assert(pSocket);

	try
	{
		pSocket->pTLSServer = new Botan::TLS::Server(
			[this, pSocket](const Botan::byte * pData, size_t size) { sendData(pSocket, (const char*)pData, size); },
			[this, pSocket](const Botan::byte * pData, size_t size) { receivedData(pSocket, (const char*)pData, size); },
			[](Botan::TLS::Alert alert, const Botan::byte * pData, size_t size) {},
			[](const Botan::TLS::Session& session)->bool { return true; },
			*m_pTLSSM, *m_pCredManager, *m_pPolicy, *m_rng);
	}
	catch (std::exception& e)
	{
		lprintf_e("An error occurred while creating TLS Session: \n%s", e.what());
		return false;
	}
	return true;
}

void CNetServer::doAccept(TSocketEx * pSocket, boost::system::error_code errcode)
{
	if (errcode)
	{
		lprintf_w("doAccept Error: %s", errcode.message().c_str());
		pSocket->Release();
		return;
	}
	createNewSocket(pSocket->TLS);
	createTLSSession(pSocket);
	pSocket->socket->async_receive(
		buffer(pSocket->rawBuffer),
		std::bind(&CNetServer::doRecv, this, pSocket, std::placeholders::_2, std::placeholders::_1));
}

void CNetServer::doRecv(TSocketEx * pSocket, size_t nLen, boost::system::error_code errcode)
{
	if (errcode)
	{
		lprintf_w("doRecv Error: %s", errcode.message().c_str());
		pSocket->Release();
		return;
	}
	lprintf_d("doRecv called, socket %p, nLen %d", pSocket, (int)nLen);
	recvRawData(pSocket, pSocket->rawBuffer, nLen);
	if (!pSocket->bReadyToClose)
	{
		pSocket->socket->async_receive(
			buffer(pSocket->rawBuffer),
			std::bind(&CNetServer::doRecv, this, pSocket, std::placeholders::_2, std::placeholders::_1));
	}
	else
		pSocket->Release();
}

void CNetServer::doSend(TSocketEx * pSocket, size_t nLen, boost::system::error_code errcode)
{
	if (errcode)
	{
		lprintf_w("doSend Error: %s", errcode.message().c_str());
		pSocket->Release();
		return;
	}
	lprintf_d("doSend called, socket %p, nLen %d", pSocket, (int)nLen);

	bool flag = false;

	pSocket->mutexSend.lock();
	char * pBuffer = pSocket->qSend.front().first;
	delete pBuffer;
	pSocket->qSend.pop();
	if (!pSocket->qSend.empty())
	{
		pSocket->socket->async_send(
			buffer(pSocket->qSend.front().first, pSocket->qSend.front().second),
			std::bind(&CNetServer::doSend, this, pSocket, std::placeholders::_2, std::placeholders::_1));
		flag = true;
	}
	pSocket->mutexSend.unlock();

	if (!flag)
		pSocket->Release();
}

bool CNetServer::sendData(TSocketEx * pSocket, const char * pData, size_t nLen)
{
	assert(pSocket && pData);
	lprintf_d("sendData called, socket %p, len %d", pSocket, (int)nLen);
	if (pSocket->bReadyToClose)
		return false;

	while (nLen > 0)
	{
		size_t nNow = std::min(nLen, BufferSize);
		if (!pSocket->postToSendQueue(pData, nLen))
		{
			pSocket->mutexSend.lock();
			if (!pSocket->qSend.empty())
			{
				char * pBuffer = pSocket->qSend.front().first;
				size_t nLen = pSocket->qSend.front().second;
				pSocket->AddRef();
				pSocket->socket->async_send(
					buffer(pBuffer, nNow),
					std::bind(&CNetServer::doSend, this, pSocket, std::placeholders::_2, std::placeholders::_1));
			}
			pSocket->mutexSend.unlock();
		}
		pData += nNow;
		nLen -= nNow;
	}
	return true;
}

void CNetServer::receivedData(TSocketEx * pSocket, const char * pData, size_t nLen)
{
	assert(pSocket);

	int nLine = 0;
	if (!pSocket->recvBuffer.empty() && pSocket->recvBuffer.back() == '\n')
		nLine = 1;
	for (size_t i = 0; i < nLen; i++)
	{
		pSocket->recvBuffer += pData[i];
		if (pData[i] == '\n')
			nLine++;
		else
			nLine = 0;
		if (nLine >= 2)
		{
			nLine = 0;
			pSocket->recvBuffer.erase(pSocket->recvBuffer.size() - 2, 2);

			size_t fi, len = pSocket->recvBuffer.length();
			for (fi = 0; fi < len; fi++)
			{
				if (pSocket->recvBuffer[fi] != '\n')
					break;
			}
			if (fi)
				pSocket->recvBuffer = pSocket->recvBuffer.substr(fi);

			std::string strResponse;
			bool bKeepAlive;
			bKeepAlive = m_pSManager->recvRequest(
				pSocket->socket->remote_endpoint().address().to_string(),
				pSocket->recvBuffer, strResponse);
			pSocket->recvBuffer.clear();
			if (!strResponse.empty() && strResponse.back() == '\n')
				strResponse += "\n";
			else
				strResponse += "\n\n";
			sendResponse(pSocket, strResponse);
			if (!bKeepAlive)
				setSocketClose(pSocket);
		}
	}
}

void CNetServer::sendResponse(TSocketEx * pSocket, const std::string& strResponse)
{
	assert(pSocket);
	lprintf_d("sendResponse: socket %p, res:\n[\n%s\n]", pSocket, strResponse.c_str());
	if (pSocket->TLS)
	{
		assert(pSocket->pTLSServer);
		pSocket->pTLSServer->send(strResponse);
	}
	else
		sendData(pSocket, strResponse.data(), strResponse.size());
}

void CNetServer::recvRawData(TSocketEx * pSocket, const char * pData, size_t nLen)
{
	if (pSocket->TLS)
		pSocket->pTLSServer->received_data((const Botan::byte *)pData, nLen);
	else
		receivedData(pSocket, pData, nLen);
}

void CNetServer::setSocketClose(TSocketEx * pSocket)
{
	if (pSocket->TLS && pSocket->pTLSServer)
		pSocket->pTLSServer->close();
	pSocket->bReadyToClose = true;
}

void CNetServer::workerThread()
{
	m_ioService.run();
}