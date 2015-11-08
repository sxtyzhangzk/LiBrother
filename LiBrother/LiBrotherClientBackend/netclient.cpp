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

MODULE_LOG_NAME("NetClient");

static socket_t sockAlive = INVALID_SOCKET;
static unsigned long long sessionID = 0;

bool verifySession();
socket_t connectToServer();
socket_t prepareSocket();
bool sendData(socket_t sockNow, const std::string& strBuffer);
bool recvData(socket_t sockNow, std::string& strBuffer);
bool postData(const std::string& strSend, std::string& strRecv);
bool getSession();

bool initNetClient()
{
#ifdef _WIN32
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
			return false;
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
				getSession();
				retry = true;
				continue;
			}
			else
				return false;
		}
		else
			return false;
	}
}

socket_t connectToServer()
{
	socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return INVALID_SOCKET;

	hostent * host = gethostbyname(g_configClient.strServer.c_str());
	if (!host)
	{
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
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}

bool sendData(socket_t sockNow, const std::string& strSend)
{
	std::string strBuffer = strSend + "\n\n";
	size_t nBuffer = strBuffer.size();
	const char * buffer = strBuffer.c_str();
	for (size_t pos = 0; pos < nBuffer; pos += g_configClient.nBufferSize)
	{
		size_t nNow = nBuffer - pos;
		if (nNow > g_configClient.nBufferSize)
			nNow = g_configClient.nBufferSize;
		int ret = send(sockNow, buffer + pos, nNow, 0);
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
	bool bNewSocket;
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

	if (!sendData(sockNow, strSend))
	{
		if (bNewSocket)
		{
			closesocket(sockNow);
			return false;
		}
		else
		{
			closesocket(sockNow);
			sockAlive = INVALID_SOCKET;
			return postData(strSend, strRecv);
		}
	}
	if (!recvData(sockNow, strRecv))
	{
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
		sessionID = 0;
		return false;
	}
	return true;
}

bool verifySession()
{
	int checkSum = 0;
	for (int bit = 63; bit >= 37; bit--)
		if (sessionID & (1ULL << bit))
			return false;
	for (int bit = 36; bit >= 4; bit--)
		checkSum += bool((1ULL << bit) & sessionID) * bit;
	checkSum ^= 0xF;
	if ((checkSum & 0xF) != (sessionID & 0xF))
		return false;
	return true;
}