#include "config.h"
#include "netclient.h"

#include <liblog.h>
#include <cstring>
#include <string>
#include <sstream>

#ifdef _WIN32
#include <winsock.h>
#else
#endif

#if defined(_WIN64)
typedef unsigned long long socket_t;
#elif defined(_WIN32)
typedef unsigned int socket_t;
#else
typedef int socket_t;
const socket_t INVALID_SOCKET = 0;
#endif

/*******************************
 * 简单通讯协议
 * 请求：（获取Session ID）
 *      HELLO\n
 *      \n
 *
 * 回应：
 *   一串Session ID
 *
 * 请求：（传输数据）
 *      DATA\n
 *      数据内容\n
 *      \n
 *
 * 回应：（正常）
 *   OK\n
 *   回应内容\n
 *   \n
 *
 * 回应：（无效SessionID）
 *   INVALID_SESSION\n
 *   \n
 *
 * 如需保持连接在请求开头加入：
 *   KEEP-ALIVE\n
 *
 * SessionID格式
 *   63-38位		0
 *   37   位		1
 *   36-4 位		随机身份码
 *   3-0  位		校验和
 *******************************/

MODULE_LOG_NAME("NetClient");

static socket_t sockAlive = INVALID_SOCKET;
static unsigned long long sessionID = 0;

//验证SessionID
bool verifySession();
//连接到服务器
socket_t connectToServer();
//发送数据
bool sendData(socket_t sockNow, const std::string& strBuffer);
//接收数据
bool recvData(socket_t sockNow, std::string& strBuffer);
//发送数据并接收回复
bool postData(const std::string& strSend, std::string& strRecv);
//获取SessionID
bool getSession();

bool initNetClient()
{
#ifdef _WIN32
	//初始化Winsock
	WORD wVersion = MAKEWORD(1, 1);
	WSADATA wsaData;
	int ret = WSAStartup(wVersion, &wsaData);
	if (ret != 0)
	{
		lprintf_e("WSAStartup Failed, Retcode %d", ret);
		return false;
	}
#endif
	return true;
}

void cleanupNetClient()
{
	if (g_configClient.bKeepAlive && sockAlive != INVALID_SOCKET)
	{
		shutdown(sockAlive, 2);
		closesocket(sockAlive);
	}
#ifdef _WIN32
	WSACleanup();
#endif
}

bool sendRequest(const std::string& strRequest, std::string& strRespond)
{
	if (sessionID == 0 && !getSession())
		return false;

	std::stringstream ssend;
	ssend << "DATA\n";
	ssend << std::hex << sessionID;
	ssend << "\n";
	ssend << strRequest;
	std::string strRecv, strRetcode;
	
	bool retry = false;
	while (true)
	{
		if (!postData(ssend.str(), strRecv))
			return false;
		size_t posContent = strRecv.find_first_of('\n');
		if (posContent == std::string::npos)
		{
			lprintf_e("Failed to Send Request : Invalid Respond from Server. (NO RETCODE)");
			return false;
		}
		strRetcode.assign(strRecv, 0, posContent - 1);
		if (strRetcode == "OK")
		{
			strRespond.assign(strRecv, posContent + 1);
			return true;
		}
		else if (strRetcode == "INVALID_SESSION")
		{
			sessionID = 0;
			if (!retry)
			{
				lprintf_w("Session Verified Failed, Try to Get a New Session ID.");
				//重新获取一遍SessionID
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

socket_t connectToServer()
{
	socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		lprintf_e("Failed to Call socket()");
		return INVALID_SOCKET;
	}

	hostent * host = gethostbyname(g_configClient.strServer.c_str());
	if (!host)
	{
		lprintf_e("Failed to Get Host.");
		closesocket(sock);
		return INVALID_SOCKET;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(g_configClient.nPort);
	memcpy(&addr.sin_addr, host->h_addr_list[0], sizeof(in_addr));

	if (connect(sock, (sockaddr *)&addr, sizeof(addr)) != 0)
	{
		lprintf_e("Failed to Connect to Server.");
		closesocket(sock);
		return INVALID_SOCKET;
	}

	lprintf("Connected to Server Successfully.");

	return sock;
}

bool sendData(socket_t sockNow, const std::string& strSend)
{
	std::string strBuffer = strSend + "\n\n";
	size_t nBuffer = strBuffer.size();
	const char * buffer = strBuffer.c_str();
	for (size_t pos = 0; pos < nBuffer; pos += g_configClient.nBufferSize)
	{
		//本次send要发送的内容，不超过nBufferSize
		size_t nNow = nBuffer - pos;
		if (nNow > g_configClient.nBufferSize)
			nNow = g_configClient.nBufferSize;
		int ret = send(sockNow, buffer + pos, int(nNow), 0);
		if (ret == -1)
			return false;
	}
	return true;
}

bool recvData(socket_t sockNow, std::string& strBuffer)
{
	strBuffer.clear();
	char * recvBuffer = new char[g_configClient.nBufferSize];
	while (true)
	{
		int ret = recv(sockNow, recvBuffer, g_configClient.nBufferSize - 1, 0);
		if (ret == 0)
			break;
		if (ret == -1)
		{
			delete[] recvBuffer;
			return false;
		}
		recvBuffer[ret] = '\0';
		strBuffer += recvBuffer;

		//以\n\n结尾表示传输完毕
		if (strBuffer.size() >= 2 && strBuffer[strBuffer.size() - 1] == '\n' && strBuffer[strBuffer.size() - 2] == '\n')
		{
			strBuffer.erase(strBuffer.size() - 2, 2);
			break;
		}
	}
	delete[] recvBuffer;
	return true;
}

bool postData(const std::string& strSend, std::string& strRecv)
{
	socket_t sockNow;
	bool bNewSocket;	//当前的Socket是否是新建的
	if (g_configClient.bKeepAlive)
	{
		if (sockAlive == INVALID_SOCKET)
			sockAlive = sockNow = connectToServer(), bNewSocket = true;
		else
			sockNow = sockAlive, bNewSocket = false;
	}
	else
		sockNow = connectToServer(), bNewSocket = true;
	if (sockNow == INVALID_SOCKET)
		return false;

	if (!sendData(sockNow, g_configClient.bKeepAlive ? std::string("KEEP-ALIVE\n") + strSend : strSend))
	{
		if (bNewSocket)
		{
			lprintf_e("Failed to Send Data.");
			closesocket(sockNow);
			return false;
		}
		else
		{
			lprintf_w("Failed to Send Data using Keep-Alive Socket, Try to Reconnect.");
			//可能连接已断开，重试一次
			closesocket(sockNow);
			sockAlive = INVALID_SOCKET;
			return postData(strSend, strRecv);
		}
	}
	if (!recvData(sockNow, strRecv))
	{
		lprintf_e("Failed to Recv Data.");
		closesocket(sockNow);
		if (!bNewSocket)
			sockAlive = INVALID_SOCKET;
		return false;
	}

	if (!g_configClient.bKeepAlive)
	{
		shutdown(sockNow, 2);
		closesocket(sockNow);
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
	for (int bit = 63; bit >= 38; bit--)
		if (sessionID & (1ULL << bit))
			return false;
	if (!(sessionID & (1ULL << 37)))
		return false;
	for (int bit = 36; bit >= 4; bit--)
		checkSum += (((1ULL << bit) & sessionID) > 0) * bit;
	checkSum ^= 0xF;
	if ((checkSum & 0xF) != (sessionID & 0xF))
		return false;
	return true;
}