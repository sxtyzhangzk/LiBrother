#include "config.h"
#include "netserver.h"
#include "session_manager.h"
#include "session.h"
#include "credentials_manager.h"

#include <cassert>
#include <ctime>
#include <cstring>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

//#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#undef X942_DH_PARAMETERS
#undef min
#undef max

#include <liblog.h>
#include <botan/auto_rng.h>
#include <botan/tls_server.h>

MODULE_LOG_NAME("NetServer");

//服务端网络模块 - Windows平台

const size_t BufferSize = 1024;				//接收缓存区的大小
const int Accept_Post_Count = 8;			//同时投递的Accept请求数
const ULONG_PTR Exit_Key = 0;				//指示工作线程退出的CompletionKey
const size_t IPLength = 50;
const int WaitTime_WorkerThread = 30;

//完成端口处理的请求类型
enum TIOType
{
	IoAccept,
	IoRecv,
	IoSend
};

//工作线程的参数
struct TWorkerThreadParam
{
	CNetServer * pNetServer;
	int nThreadID;
};

//单IO上下文类型
struct TPerIOContext
{
	OVERLAPPED overlapped;
	char buffer[BufferSize];
	WSABUF wsaBuffer;			//Used in IoAccept & IoRecv
	TIOType ioType;
	SOCKET sockClient;			//Used in IoAccept

	TPerIOContext(TIOType ioType) : ioType(ioType)
	{
		memset(&overlapped, 0, sizeof(overlapped));
		wsaBuffer.buf = buffer;
		wsaBuffer.len = BufferSize;
	}
};

//套接字扩展类型
struct TSocketEx
{
	CNetServer * pNetServer;
	//TODO: Use Critical Section to Protect vPIOContexts
	SOCKET socket;			//套接字
	sockaddr_in client;		//客户端连接信息
	bool TLS;				//是否使用TLS加密
	std::string recvBuffer;	//缓存的数据
	bool bReadyToClose;
	bool bIsClientSock;

	TPerIOContext *pIOContextSend, *pIOContextRecv;
	std::set<TPerIOContext *> vPIOContexts;

	std::queue<std::pair<char *, size_t> > qSend;
	CRITICAL_SECTION csIOContext;

	Botan::TLS::Server * pTLSServer;

	unsigned int nRefCount;

	TSocketEx(CNetServer * pNetServer, SOCKET socket, bool bIsClientSock, sockaddr_in client = { 0 }, bool TLS = false) :
		pNetServer(pNetServer), socket(socket), client(client), TLS(TLS), pTLSServer(nullptr), 
		bReadyToClose(false), pIOContextSend(nullptr), pIOContextRecv(nullptr),
		nRefCount(1), bIsClientSock(bIsClientSock)
	{
		InitializeCriticalSection(&csIOContext);
		if (bIsClientSock)
		{
			EnterCriticalSection((LPCRITICAL_SECTION)pNetServer->m_pcsClientSocks);
			pNetServer->m_vpClientSocks.insert(this);
			LeaveCriticalSection((LPCRITICAL_SECTION)pNetServer->m_pcsClientSocks);
		}
	}

	~TSocketEx()
	{
		DeleteCriticalSection(&csIOContext);
		if (pTLSServer)
		{
			delete pTLSServer;
		}
		while (!qSend.empty())
		{
			char * buffer = qSend.front().first;
			qSend.pop();
			delete buffer;
		}

		if (socket != INVALID_SOCKET)
			closesocket(socket);
		//清理单IO上下文
		for (auto i = vPIOContexts.begin(); i != vPIOContexts.end(); ++i)
		{
			if (*i)
				delete (*i);
		}
		if (pIOContextSend)
			delete pIOContextSend;
		if (pIOContextRecv)
			delete pIOContextRecv;
		vPIOContexts.clear();
	}

	//创建一个单IO上下文
	TPerIOContext * createIoContext(TIOType ioType)
	{
		EnterCriticalSection(&csIOContext);
		TPerIOContext * ioContext = nullptr;
		switch (ioType)
		{
		case IoRecv:
			assert(!pIOContextRecv);
			pIOContextRecv = new TPerIOContext(IoRecv);
			ioContext = pIOContextRecv;
			break;
		case IoSend:
			assert(!pIOContextSend);
			pIOContextSend = new TPerIOContext(IoSend);
			ioContext = pIOContextSend;
			break;
		default:
			ioContext = new TPerIOContext(ioType);
			vPIOContexts.insert(ioContext);
		}
		LeaveCriticalSection(&csIOContext);
		return ioContext;
	}

	void eraseIoContext(TPerIOContext * ioContext)
	{
		assert(ioContext);
		EnterCriticalSection(&csIOContext);
		switch (ioContext->ioType)
		{
		case IoRecv:
			assert(ioContext == pIOContextRecv);
			delete pIOContextRecv;
			pIOContextRecv = nullptr;
			break;
		case IoSend:
			assert(ioContext == pIOContextSend);
			delete pIOContextSend;
			pIOContextSend = nullptr;
			break;
		default:
			vPIOContexts.erase(ioContext);
			delete ioContext;
		}
		LeaveCriticalSection(&csIOContext);
	}

	unsigned int AddRef()
	{
		return InterlockedIncrement(&nRefCount);
	}
	unsigned int Release()
	{
		unsigned int refCount = InterlockedDecrement(&nRefCount);
		if (refCount == 0)
		{
			if (bIsClientSock)
			{
				LPCRITICAL_SECTION csClientSocks = (LPCRITICAL_SECTION)pNetServer->m_pcsClientSocks;
				EnterCriticalSection(csClientSocks);
				pNetServer->m_vpClientSocks.erase(this);
				delete this;
				LeaveCriticalSection(csClientSocks);
			}
			else
				delete this;
		}
		return refCount;
	}

	bool postToSendQueue(const char * pData, size_t nLen, TPerIOContext * &pIOContext)
	{
		assert(pData);
		bool ret = false;
		char * buffer = new char[nLen];
		memcpy_s(buffer, sizeof(buffer), pData, nLen);
		EnterCriticalSection(&csIOContext);
		if (pIOContextSend)
		{
			ret = true;
			qSend.push(std::make_pair(buffer, nLen));
			pIOContext = pIOContextSend;
		}
		else
		{
			pIOContext = new TPerIOContext(IoSend);
			pIOContext->wsaBuffer.buf = buffer;
			pIOContext->wsaBuffer.len = (ULONG)nLen;
			pIOContextSend = pIOContext;
		}
		LeaveCriticalSection(&csIOContext);
		return ret;
	}
	size_t getSendQueue(char * buffer)
	{
		size_t sizeBuffer = 0;
		EnterCriticalSection(&csIOContext);
		if (!qSend.empty())
		{
			buffer = qSend.front().first;
			sizeBuffer = qSend.front().second;
			qSend.pop();
		}
		LeaveCriticalSection(&csIOContext);
		return sizeBuffer;
	}
};


CNetServer::CNetServer() :
	m_psockListen(nullptr),
	m_psockListenTLS(nullptr),
	m_hIOCP(NULL),
	m_pCredManager(nullptr),
	m_pPolicy(nullptr),
	m_pTLSSM(nullptr)
{
	m_vhThreads.clear();
	m_pcsClientSocks = new CRITICAL_SECTION;
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
	delete (LPCRITICAL_SECTION)m_pcsClientSocks;
}

bool CNetServer::initNetServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return false;
	InitializeCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);
	return true;
}

bool CNetServer::startServer(ILibClassFactory * pClassFactory)
{
	m_pSManager->startServer(pClassFactory);
	if (!initIOCP())
		return false;
	if (g_configSvr.nTLS && !initTLS())
	{
		if (g_configSvr.nTLS == 2)
			return false;
		g_configSvr.nTLS = 0;
		lprintf_w("TLS will be disabled");
	}
	if (!initListenSocket())
		return false;
	return true;
}

bool CNetServer::stopServer()
{
	EnterCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);
	for (auto clientSock : m_vpClientSocks)
	{
		if (!clientSock->bReadyToClose)
			setSocketClose(clientSock);
	}
	LeaveCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);

	for (int i = 0; i < m_vhThreads.size(); i++)
		PostQueuedCompletionStatus(m_hIOCP, 0, Exit_Key, nullptr);
	if (WaitForMultipleObjects((DWORD)m_vhThreads.size(), &m_vhThreads[0], TRUE, WaitTime_WorkerThread * 1000) == WAIT_TIMEOUT)
	{
		lprintf_w("Worker Thread Timeout");
		for (auto hThread : m_vhThreads)
		{
			DWORD dwExitCode;
			GetExitCodeThread(hThread, &dwExitCode);
			if (dwExitCode == STILL_ACTIVE)
				TerminateThread(hThread, ~0UL);
		}
	}

	if (!m_vpClientSocks.empty())
	{
		for (auto clientSock : m_vpClientSocks)
			delete clientSock;
	}
	if (g_configSvr.nTLS)
		delete m_psockListenTLS;
	delete m_psockListen;

	m_pSManager->stopServer();
	return true;
}

bool CNetServer::initIOCP()
{
	//TODO: Check Errors
	//创建完成端口
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (!m_hIOCP)
	{
		DWORD dwLastError = GetLastError();
		lprintf_e("Failed to create IOCP, LAST ERROR %x.", dwLastError);
		return false;
	}

	//获取系统中处理器个数
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	int nCPU = sysInfo.dwNumberOfProcessors;

	//创建工作线程
	for (int i = 0; i < nCPU * g_configSvr.nThreadPerCPU; i++)
	{
		TWorkerThreadParam * pParam = new TWorkerThreadParam;
		pParam->pNetServer = this;
		pParam->nThreadID = i;
		HANDLE hThread = CreateThread(0, 0, WorkerThread, pParam, 0, 0);
		m_vhThreads.push_back(hThread);
	}

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

bool CNetServer::createTLSSession(TSocketEx * pSocket)
{
	assert(pSocket);

	try
	{
		//TODO: Handle Alert
		auto fnOutput = std::bind(&CNetServer::sendData, this, pSocket, std::placeholders::_1, std::placeholders::_2);
		auto fnData = std::bind(&CNetServer::receivedData, this, pSocket, std::placeholders::_1, std::placeholders::_2);
		auto fnAlert = [](Botan::TLS::Alert alert, const Botan::byte * pData, size_t size) {};
		auto fnHandshake = [](const Botan::TLS::Session& session)->bool { return true; };
		pSocket->pTLSServer = new Botan::TLS::Server(
			[&fnOutput](const Botan::byte * pData, size_t size) { fnOutput((const char *)pData, size); },
			[&fnData](const Botan::byte * pData, size_t size) { fnData((const char *)pData, size); },
			fnAlert, fnHandshake,
			*m_pTLSSM, *m_pCredManager, *m_pPolicy, *m_rng);
	}
	catch (std::exception& e)
	{
		lprintf_e("An error occurred while creating TLS Session: \n%s", e.what());
		return false;
	}
	return true;
}

bool CNetServer::initListenSocket()
{
	//创建监听套接字
	if (!(m_psockListen = createListenSocket(g_configSvr.nPort)))
		return false;
	m_psockListen->TLS = false;

	//创建监听套接字(TLS)
	if (g_configSvr.nTLS)
	{
		if (!(m_psockListenTLS = createListenSocket(g_configSvr.nTLSPort)))
			return false;
		m_psockListenTLS->TLS = true;
	}

	//获取AcceptEx和GetAcceptExSockAddrs的指针
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	DWORD dwRet;
	if (WSAIoctl(m_psockListen->socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx),
		&m_pfn_AcceptEx, sizeof(m_pfn_AcceptEx), &dwRet, nullptr, nullptr) == SOCKET_ERROR)
		return false;

	if (WSAIoctl(m_psockListen->socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidGetAcceptExSockAddrs, sizeof(guidGetAcceptExSockAddrs),
		&m_pfn_GetAcceptExSockAddrs, sizeof(m_pfn_GetAcceptExSockAddrs), &dwRet, nullptr, nullptr) == SOCKET_ERROR)
		return false;

	//投递Accept请求，以接受客户端连接
	for (int i = 0; i < Accept_Post_Count; i++)
	{
		if (!postAcceptRq(m_psockListen, nullptr))
		{
			if (i == 0)
				return false;
			break;
		}
		if (g_configSvr.nTLS && !postAcceptRq(m_psockListenTLS, nullptr))
		{
			if (i == 0)
				return false;
			break;
		}
	}

	return true;
}

TSocketEx * CNetServer::createListenSocket(int nPort)
{
	sockaddr_in addr;
	SOCKET sockListen;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	if (inet_pton(AF_INET, g_configSvr.strServerIP.c_str(), &addr.sin_addr) != 1)
		return nullptr;

	sockListen = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (sockListen == INVALID_SOCKET)
		return false;

	TSocketEx * sockEx = new TSocketEx(this, sockListen, false);

	if (!CreateIoCompletionPort((HANDLE)sockListen, m_hIOCP, (ULONG_PTR)sockEx, 0))
	{
		delete sockEx;
		closesocket(sockListen);
		return false;
	}

	if (bind(sockListen, (sockaddr *)&addr, sizeof(addr)) != 0)
	{
		delete sockEx;
		closesocket(sockListen);
		return false;
	}

	if (listen(sockListen, SOMAXCONN) != 0)
	{
		delete sockEx;
		closesocket(sockListen);
		return false;
	}

	return sockEx;
}

bool CNetServer::postAcceptRq(TSocketEx * pSocket, TPerIOContext * pIOContext)
{
	assert(pSocket);

	bool bNewIOContext = false;
	if (!pIOContext)
	{
		//第一次创建IO上下文
		pIOContext = pSocket->createIoContext(IoAccept);
		pSocket->AddRef();
		bNewIOContext = true;
	}
	
	SOCKET sockClient = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (sockClient == INVALID_SOCKET)
	{
		pSocket->eraseIoContext(pIOContext);
		pSocket->Release();
		return false;
	}

	DWORD dwBytes;
	if (!(LPFN_ACCEPTEX(m_pfn_AcceptEx))(pSocket->socket, sockClient, pIOContext->wsaBuffer.buf,
		BufferSize - (sizeof(sockaddr_in) + 16) * 2, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		&dwBytes, (LPOVERLAPPED)pIOContext))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			pSocket->eraseIoContext(pIOContext);
			pSocket->Release();
			return false;
		}
	}
	
	pIOContext->sockClient = sockClient;

	return true;
}

bool CNetServer::postRecvRq(TSocketEx * pSocket, TPerIOContext * pIOContext)
{
	assert(pSocket);

	bool bNewIOContext = false;
	
	if (!pIOContext)
	{
		pIOContext = pSocket->createIoContext(IoRecv);
		pSocket->AddRef();
		bNewIOContext = true;
	}

	DWORD dwBytes, dwFlags = 0;
	int nRet = WSARecv(pSocket->socket, &pIOContext->wsaBuffer, 1, &dwBytes, &dwFlags, (LPOVERLAPPED)pIOContext, nullptr);

	if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		pSocket->eraseIoContext(pIOContext);
		pSocket->Release();
		return false;
	}

	return true;
}

bool CNetServer::postSendRq(TSocketEx * pSocket, TPerIOContext * pIOContext)
{
	assert(pSocket && pIOContext);

	DWORD dwBytes, dwFlags = 0;
	int nRet = WSASend(pSocket->socket, &pIOContext->wsaBuffer, 1, &dwBytes, 0, (LPOVERLAPPED)pIOContext, nullptr);
	if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		pSocket->eraseIoContext(pIOContext);
		pSocket->Release();
		return false;
	}
	return true;
}

bool CNetServer::sendData(TSocketEx * pSocket, const char * pData, size_t nLen)
{
	while (nLen > 0)
	{
		size_t sizeNow = std::max(nLen, BufferSize);
		TPerIOContext * pIOContext;
		if (!pSocket->postToSendQueue(pData, sizeNow, pIOContext))
		{
			pSocket->AddRef();
			if (!postSendRq(pSocket, pIOContext))
				return false;
		}
		pData += sizeNow;
		nLen -= sizeNow;
	}
	return true;
}

unsigned long WINAPI CNetServer::WorkerThread(void * pParam)
{
	TWorkerThreadParam * param = (TWorkerThreadParam *)pParam;
	CNetServer * netServer = param->pNetServer;
	DWORD dwBytes;
	ULONG_PTR completionKey;
	LPOVERLAPPED pOverlapped;

	while (true)
	{
		BOOL bRet = GetQueuedCompletionStatus(netServer->m_hIOCP, &dwBytes, &completionKey, &pOverlapped, INFINITE);
		if (!bRet)
		{
			//TODO: Handle Error
		}
		if (completionKey == Exit_Key)
			break;

		TSocketEx * pSocket = (TSocketEx *)completionKey;
		TPerIOContext * pPIOContext = CONTAINING_RECORD(pOverlapped, TPerIOContext, overlapped);

		switch (pPIOContext->ioType)
		{
		case IoAccept:
			netServer->doAccept(pSocket, pPIOContext, dwBytes);
			break;
		case IoRecv:
			netServer->doRecv(pSocket, pPIOContext, dwBytes);
			break;
		default:
			break;
		}
	}
	delete param;
	return 0;
}

int CNetServer::doAccept(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize)
{
	assert(pSocket && pSocket->socket != INVALID_SOCKET);
	assert(pIOContext && pIOContext->sockClient != INVALID_SOCKET);
	
	sockaddr_in *localAddr = nullptr, *remoteAddr = nullptr;
	int sizeLocal = sizeof(localAddr), sizeRemote = sizeof(remoteAddr);

	(LPFN_GETACCEPTEXSOCKADDRS(m_pfn_GetAcceptExSockAddrs))(pIOContext->wsaBuffer.buf,
		BufferSize - 2 * (sizeof(sockaddr_in) + 16),
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		(sockaddr **)&localAddr, &sizeLocal,
		(sockaddr **)&remoteAddr, &sizeRemote);

	TSocketEx * pClientSocket = new TSocketEx(this, pIOContext->sockClient, true, *remoteAddr, pSocket->TLS);

	bool bPostedAcceptRq = postAcceptRq(pSocket, pIOContext);
	if (!bPostedAcceptRq)
	{
		//TODO:
	}

	if (pSocket->TLS && !createTLSSession(pClientSocket))
	{
		//delete pClientSocket;
		pClientSocket->Release();
		if (!bPostedAcceptRq)
			return -1;
		return 1;
	}
	//处理客户端发来的第一组数据
	receivedData(pClientSocket, pIOContext->wsaBuffer.buf, nDataSize);

	if (!CreateIoCompletionPort((HANDLE)pClientSocket->socket, m_hIOCP, (ULONG_PTR)pClientSocket, 0))
	{
		//delete pClientSocket;
		pClientSocket->Release();
		if (!bPostedAcceptRq)
			return -1;
		return 1;	//Error - Client Socket Error
	}

	if (!postRecvRq(pClientSocket, nullptr))
	{
		//delete pClientSocket;
		pClientSocket->Release();
		if (!bPostedAcceptRq)
			return -1;
		return 1;
	}

	if (!bPostedAcceptRq)
		return -1;
	return 0;	//Success
}

bool CNetServer::doRecv(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize)
{
	assert(pSocket && pIOContext);

	if (nDataSize == 0)
	{
		//关闭连接
		EnterCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);
		m_vpClientSocks.erase(pSocket);
		LeaveCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);
		delete pSocket;
		return true;
	}
	receivedData(pSocket, pIOContext->wsaBuffer.buf, nDataSize);
	if (pSocket->bReadyToClose)
	{
		pSocket->eraseIoContext(pIOContext);
		pSocket->Release();
		return true;
	}
	return postRecvRq(pSocket, pIOContext);
}

bool CNetServer::doSend(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize)
{
	assert(pSocket && pIOContext && pIOContext->wsaBuffer.buf);

	delete pIOContext->wsaBuffer.buf;
	char * buffer = nullptr;
	size_t sizeBuffer = pSocket->getSendQueue(buffer);
	if (sizeBuffer == 0)
	{
		pSocket->eraseIoContext(pIOContext);
		pSocket->Release();
		return true;
	}
	pIOContext->wsaBuffer.buf = buffer;
	pIOContext->wsaBuffer.len = (ULONG)sizeBuffer;
	return postSendRq(pSocket, pIOContext);
}

void CNetServer::receivedData(TSocketEx * pSocket, const char * pData, size_t nLen)
{
	assert(pSocket);
	
	int nLine = 0;
	if (!pSocket->recvBuffer.empty() && pSocket->recvBuffer.back() == '\n')
		nLine = 1;
	size_t lasti = 0;
	for (size_t i = 0; i < nLen; i++)
	{
		if (pData[i] == '\n')
			nLine++;
		else
			nLine = 0;
		if (nLine >= 2)
		{
			nLine = 0;
			pSocket->recvBuffer.append(pData + lasti, i - lasti + 1);
			pSocket->recvBuffer.erase(pSocket->recvBuffer.size() - 2, 2);

			char clientIP[IPLength];
			inet_ntop(pSocket->client.sin_family, &pSocket->client.sin_addr, clientIP, sizeof(clientIP));
			std::string strResponse;
			bool bKeepAlive;
			bKeepAlive = m_pSManager->recvRequest(std::string(clientIP), pSocket->recvBuffer, strResponse);
			if (strResponse.back() == '\n')
				strResponse += "\n";
			else
				strResponse += "\n\n";
			sendResponse(pSocket, strResponse);
			if (!bKeepAlive)
				setSocketClose(pSocket);

			lasti = i + 1;
		}
	}
	if (lasti != nLen)
		pSocket->recvBuffer.append(pData + lasti, nLen - lasti + 1);
}

void CNetServer::sendResponse(TSocketEx * pSocket, const std::string& strResponse)
{
	assert(pSocket);
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
	if (pSocket->TLS)
		pSocket->pTLSServer->close();
	pSocket->bReadyToClose = true;
}