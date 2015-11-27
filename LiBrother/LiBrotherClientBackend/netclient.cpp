#include "config.h"
#include "netclient.h"
#include "credentials_manager.h"

#include <liblog.h>
#include <cassert>
#include <cstring>
#include <utility>
#include <queue>
#include <string>
#include <algorithm>
#include <sstream>
#include <thread>
#include <condition_variable>

#include <botan/auto_rng.h>
#include <botan/tls_client.h>

#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;

/*******************************
 * ��ͨѶЭ��
 * ���󣺣���ȡSession ID��
 *      HELLO\n
 *      \n
 *
 * ��Ӧ��
 *   һ��Session ID
 *
 * ���󣺣��������ݣ�
 *      DATA\n
 *      ��������\n
 *      \n
 *
 * ��Ӧ����������
 *   OK\n
 *   ��Ӧ����\n
 *   \n
 *
 * ��Ӧ������ЧSessionID��
 *   INVALID_SESSION\n
 *   \n
 *
 * ���豣������������ͷ���룺
 *   KEEP-ALIVE\n
 *
 * SessionID��ʽ
 *   63-37λ		0
 *   36   λ		1
 *   35-4 λ		��������
 *   3-0  λ		У���
 *******************************/

MODULE_LOG_NAME("NetClient");

static const size_t BufferSize = 1024;

io_service ioService;
io_service::work *work;
std::thread *pThreadWorker;
ip::tcp::socket *pSocket;
ip::tcp::endpoint epServer;

Botan::AutoSeeded_RNG rng;
CCredentialsManager credManager;
Botan::TLS::Policy TLSPolicy;
Botan::TLS::Session_Manager_In_Memory TLSSM(rng);
Botan::TLS::Client *pTLSClient;

char recvBuffer[BufferSize];
std::string strRecv;
std::condition_variable strRecvCV;
std::mutex strRecvMutex;
int recvError;

std::mutex mutexHandshake;
std::condition_variable cvHandshake;
int nHandshakeStatus;

unsigned long long sessionID = 0;

void workerThread();
//��֤SessionID
bool verifySession();
//���ӵ�������
bool connectToServer();
//��������
bool sendData(const std::string& strBuffer);
//��������
bool recvData(std::string& strBuffer);
//�������ݲ����ջظ�
bool postData(const std::string& strSend, std::string& strRecv, bool retry = false);
//��ȡSessionID
bool getSession();

void doRecv(boost::system::error_code errcode, size_t nLen);

bool sendDataRaw(const char *pBuffer, size_t nBuffer);

bool initTLS();

bool initTLSSession();


bool initNetClient()
{
	if (g_configClient.bTLS && !initTLS())
		return false;
	work = new io_service::work(ioService);
	pThreadWorker = new std::thread(workerThread);
	pSocket = new ip::tcp::socket(ioService);
	ip::tcp::resolver resolver(ioService);
	ip::tcp::resolver::query query(g_configClient.strServer, boost::lexical_cast<std::string>(g_configClient.nPort));
	boost::system::error_code error;
	auto iter = resolver.resolve(query, error);
	decltype(iter) end;
	if (error || iter == end)
	{
		lprintf_e("Cannot resolve server ip");
		return false;
	}
	epServer = *iter;
	return true;
}

void workerThread()
{
	lprintf("Worker started");
	ioService.run();
	lprintf("Worker stopped");
}

void cleanupNetClient()
{
	if (pSocket->is_open())
	{
		if (pTLSClient)
			pTLSClient->close();
		pTLSClient = nullptr;
		pSocket->shutdown(socket_base::shutdown_type::shutdown_both);
		pSocket->close();
	}

	if (work)
		delete work;
	
	ioService.stop();
	pThreadWorker->join();
	delete pThreadWorker;
}

bool sendRequest(const std::string& strRequest, std::string& strRespond)
{
	if (sessionID == 0 && !getSession())
		return false;
	
	bool retry = false;
	while (true)
	{
		std::stringstream ssend;
		ssend << "DATA\n";
		ssend << std::hex << sessionID;
		ssend << "\n";
		ssend << strRequest;
		std::string strRecvNow, strRetcode;

		if (!postData(ssend.str(), strRecvNow))
			return false;
		size_t posContent = strRecvNow.find_first_of('\n');
		if (posContent == std::string::npos)
		{
			lprintf_e("Failed to Send Request : Invalid Respond from Server. (NO RETCODE)");
			return false;
		}
		strRetcode.assign(strRecvNow, 0, posContent);
		if (strRetcode == "OK")
		{
			strRespond.assign(strRecvNow, posContent + 1);
			return true;
		}
		else if (strRetcode == "INVALID_SESSION")
		{
			sessionID = 0;
			if (!retry)
			{
				lprintf_w("Session Verified Failed, Try to Get a New Session ID.");
				//���»�ȡһ��SessionID
				getSession();
				retry = true;
				continue;
			}
			else
			{
				lprintf_e("Session Verified Failed.");
				return false;
			}
		}
		else
		{
			lprintf_e("Failed to Send Request: Invalid Respond from Server. (INVALID RETCODE)");
			return false;
		}
	}
}

bool connectToServer()
{
	if (pSocket->is_open())
	{
		pSocket->shutdown(socket_base::shutdown_type::shutdown_both);
		pSocket->close();
		return true;
	}
	recvError = 0;
	nHandshakeStatus = 0;

	boost::system::error_code error;
	pSocket->connect(epServer, error);
	if (error)
	{
		lprintf_e("Failed to connect to server: %s", error.message().c_str());
		return false;
	}

	pSocket->async_receive(buffer(recvBuffer), doRecv);

	if (g_configClient.bTLS && !initTLSSession())
	{
		pSocket->close();
		return false;
	}

	lprintf("Connected to Server Successfully.");
	return true;
}

bool sendData(const std::string& strSend)
{
	if (g_configClient.bTLS)
	{
		assert(pTLSClient);
		try
		{
			/*if (!pTLSClient->is_active())
			{
				lprintf_e("Failed to send data: TLS is not active");
				return false;
			}*/
			pTLSClient->send((const Botan::byte *)strSend.data(), strSend.length());
		}
		catch (std::exception& e)
		{
			lprintf_e("TLS Send Error: %s", e.what());
			return false;
		}
	}
	else
		return sendDataRaw(strSend.c_str(), strSend.length());
}

bool recvData(std::string& strBuffer)
{
	strBuffer.clear();
	bool isEmpty = true;
	while (true)
	{
		std::unique_lock<std::mutex> lock(strRecvMutex);
		if (strRecv.empty() && !recvError)
		{
			if (strRecvCV.wait_for(lock, std::chrono::milliseconds(20000)) == std::cv_status::timeout)
			{
				lprintf_e("Recv data time out");
				return false;
			}
		}

		if (recvError)
		{
			recvError--;
			return false;
		}
		size_t nLen = strRecv.length();
		int cntLine = 0;
		for (size_t i = 0; i < nLen; i++)
		{
			if (strRecv[i] == '\n')
			{
				cntLine++;
				if (!isEmpty)	//ȥ�����ݰ���ͷ�Ļس�
					strBuffer += strRecv[i];
			}
			else
			{
				cntLine = 0;
				strBuffer += strRecv[i];
				isEmpty = false;
			}
			if (cntLine == 2)
			{
				if (!strBuffer.empty() && strBuffer.back() == '\n')
					strBuffer.pop_back();
				if (i == nLen - 1)
					strRecv.clear();
				else
					strRecv = strRecv.substr(i + 1);
				return true;
			}
		}
		strRecv.clear();
	}
}

bool postData(const std::string& strSend, std::string& strRecv, bool retry)
{
	if (!pSocket->is_open() && !connectToServer())
		return false;

	strRecvMutex.lock();
	strRecv.clear();
	strRecvMutex.unlock();

	bool ret;
	if (g_configClient.bKeepAlive)
		ret = sendData(std::string("KEEP-ALIVE\n") + strSend + "\n\n");
	else
		ret = sendData(strSend + "\n\n");
	if (!ret)
	{
		pSocket->close();	//TLS?
		if (!retry)
			return postData(strSend, strRecv, true);
		return false;
	}
	if (!recvData(strRecv))
	{
		pSocket->close();
		return false;
	}

	if (!g_configClient.bKeepAlive)
	{
		if (g_configClient.bTLS)
		{
			if (pTLSClient)
			{
				pTLSClient->close();
				delete pTLSClient;
				pTLSClient = nullptr;
			}
		}
		pSocket->shutdown(socket_base::shutdown_type::shutdown_both);
		pSocket->close();
	}
	return true;
}

bool getSession()
{
	std::string strReqID = "HELLO";
	std::string strSesID;
	if (!postData(strReqID, strSesID))
		return false;

	std::stringstream ss(strSesID);
	ss >> std::hex >> sessionID;
	if (!verifySession())
	{
		lprintf_e("Invalid Session: %lld", sessionID);
		sessionID = 0;
		return false;
	}
	lprintf("Session ID Got: %lld", sessionID);
	return true;
}

bool verifySession()
{
	int checkSum = 0;
	for (int bit = 63; bit >= 37; bit--)
		if (sessionID & (1ULL << bit))
			return false;
	if (!(sessionID & (1ULL << 36)))
		return false;
	for (int bit = 35; bit >= 4; bit--)
		checkSum += (((1ULL << bit) & sessionID) > 0) * bit;
	checkSum ^= 0xF;
	if ((checkSum & 0xF) != (sessionID & 0xF))
		return false;
	return true;
}

void doRecv(boost::system::error_code errcode, size_t nLen)
{	//TODO: TLS
	//std::unique_lock<std::mutex> lock(strRecvMutex);
	if (errcode)
	{
		lprintf_e("ERROR: %s", errcode.message().c_str());
		strRecvMutex.lock();
		recvError++;
		pSocket->close();
		strRecvMutex.unlock();
		strRecvCV.notify_all();
		return;
	}
	if (nLen == 0)
	{
		strRecvMutex.lock();
		pSocket->close();
		if (strRecv.length() >= 2 && strRecv.substr(strRecv.length() - 2) == "\n\n")
			return;
		if (strRecv.length() >= 1 && strRecv.back() == '\n')
			strRecv += "\n";
		else
			strRecv += "\n\n";	//ȷ�����ջ�������\n\n��β
		strRecvMutex.unlock();
		strRecvCV.notify_all();
		return;
	}
	if (g_configClient.bTLS)
	{
		assert(pTLSClient);
		try
		{
			pTLSClient->received_data((const Botan::byte *)recvBuffer, nLen);
		}
		catch (std::exception& e)
		{
			lprintf_e("%s", e.what());
			if (nHandshakeStatus == 1)
			{
				strRecvMutex.lock();
				recvError++;
				strRecvMutex.unlock();
				strRecvCV.notify_all();
			}
			else
			{
				std::unique_lock<std::mutex> lock(mutexHandshake);
				nHandshakeStatus = 2;
				cvHandshake.notify_all();
			}
		}
	}
	else
	{
		strRecvMutex.lock();
		strRecv.append(recvBuffer, nLen);
		strRecvMutex.unlock();
		strRecvCV.notify_all();
	}

	pSocket->async_receive(buffer(recvBuffer), doRecv);
}

bool sendDataRaw(const char *pBuffer, size_t nBuffer)
{
	size_t pos = 0;
	while(pos < nBuffer)
	{
		//����sendҪ���͵����ݣ�������nBufferSize
		size_t nNow = std::min(BufferSize, nBuffer - pos);
		if (nNow > g_configClient.nBufferSize)
			nNow = g_configClient.nBufferSize;
		if (!pSocket->send(buffer(pBuffer + pos, nNow)))
			return false;
		pos += nNow;
	}
	return true;
}

bool initTLS()
{
	try
	{
		credManager.loadCA(g_configClient.strCAPath);
	}
	catch (std::exception& e)
	{
		lprintf_e("Failed to init TLS: %s", e.what());
		return false;
	}
	return true;
}

bool initTLSSession()
{
	try
	{
		auto fnOutput = [](const Botan::byte *buffer, size_t len) { sendDataRaw((const char *)buffer, len); };
		auto fnData = [](const Botan::byte *buffer, size_t len)
		{
			strRecvMutex.lock();
			strRecv.append((const char *)buffer, len);
			strRecvMutex.unlock();
			strRecvCV.notify_all();
		};
		auto fnAlert = [](Botan::TLS::Alert alert, const Botan::byte *buffer, size_t len)
		{
			lprintf_w("TLS Alert: %s", alert.type_string().c_str());
		};
		auto fnHandshake = [](const Botan::TLS::Session& session)
		{
			std::unique_lock<std::mutex> lckHandshake(mutexHandshake);
			nHandshakeStatus = 1;
			cvHandshake.notify_all();
			return true; 
		};
		if (pTLSClient)
			delete pTLSClient;

		if (!g_configClient.bTLSVerifyHostname)
		{
			pTLSClient = new Botan::TLS::Client(
				fnOutput, fnData, fnAlert, fnHandshake,
				TLSSM, credManager, TLSPolicy, rng/*,
				Botan::TLS::Server_Information(g_configClient.strServer, g_configClient.nPort)*/);
		}
		else
		{
			pTLSClient = new Botan::TLS::Client(
				fnOutput, fnData, fnAlert, fnHandshake,
				TLSSM, credManager, TLSPolicy, rng,
				Botan::TLS::Server_Information(g_configClient.strServer, g_configClient.nPort));
		}

		std::unique_lock<std::mutex> lckHandshake(mutexHandshake);
		if(!nHandshakeStatus)
			cvHandshake.wait(lckHandshake);

		if (nHandshakeStatus != 1)
			return false;

		int cnt = 0;
		while (!pTLSClient->is_active() && ++cnt <= 100)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (!pTLSClient->is_active())
			return false;
	}
	catch (std::exception& e)
	{
		lprintf_e("Failed to init TLS session: %s", e.what());
		return false;
	}
	return true;
}