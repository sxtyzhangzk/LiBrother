#include "config.h"
#include "netclient.h"

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

char recvBuffer[BufferSize];
std::string strRecv;
std::condition_variable strRecvCV;
std::mutex strRecvMutex;
int recvError;

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

bool initNetClient()
{
	work = new io_service::work(ioService);
	pThreadWorker = new std::thread(workerThread);
	lprintf("Thread Created.");
	pSocket = new ip::tcp::socket(ioService);
	ip::tcp::resolver resolver(ioService);
	ip::tcp::resolver::query query(g_configClient.strServer, boost::lexical_cast<std::string>(g_configClient.nPort));
	boost::system::error_code error;
	auto iter = resolver.resolve(query, error);
	decltype(iter) end;
	lprintf("ITER");
	if (error || iter == end)
	{
		lprintf_e("Cannot resolve server ip");
		return false;
	}
	epServer = *iter;
	lprintf("POST-ITEr");
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
	if (work)
		delete work;
	if (pSocket->is_open())
		pSocket->close();
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
		return true;

	boost::system::error_code error;
	pSocket->connect(epServer, error);
	if (error)
	{
		lprintf_e("Failed to connect to server: %s", error.message().c_str());
		return false;
	}

	pSocket->async_receive(buffer(recvBuffer), doRecv);

	lprintf("Connected to Server Successfully.");
	return true;
}

bool sendData(const std::string& strSend)
{
	return sendDataRaw(strSend.c_str(), strSend.length());
}

bool recvData(std::string& strBuffer)
{
	strBuffer.clear();
	bool isEmpty = true;
	while (true)
	{
		std::unique_lock<std::mutex> lock(strRecvMutex);
		if (strRecv.empty())
			strRecvCV.wait(lock);
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
		pSocket->close();	//TLS?
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
	strRecvMutex.lock();
	if (errcode)
	{
		lprintf_e("ERROR: %s", errcode.message().c_str());
		recvError++;
		strRecvMutex.unlock();
		pSocket->close();
		return;
	}
	if (nLen == 0)
	{
		pSocket->close();
		if (strRecv.length() >= 2 && strRecv.substr(strRecv.length() - 2) == "\n\n")
			return;
		if (strRecv.length() >= 1 && strRecv.back() == '\n')
			strRecv += "\n";
		else
			strRecv += "\n\n";	//ȷ�����ջ�������\n\n��β
		return;
	}
	strRecv.append(recvBuffer, nLen);
	strRecvCV.notify_all();
	strRecvMutex.unlock();

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