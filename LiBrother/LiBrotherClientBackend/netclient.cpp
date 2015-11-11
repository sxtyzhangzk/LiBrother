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
 *   63-38λ		0
 *   37   λ		1
 *   36-4 λ		��������
 *   3-0  λ		У���
 *******************************/

MODULE_LOG_NAME("NetClient");

static socket_t sockAlive = INVALID_SOCKET;
static unsigned long long sessionID = 0;

//��֤SessionID
bool verifySession();
//���ӵ�������
socket_t connectToServer();
//��������
bool sendData(socket_t sockNow, const std::string& strBuffer);
//��������
bool recvData(socket_t sockNow, std::string& strBuffer);
//�������ݲ����ջظ�
bool postData(const std::string& strSend, std::string& strRecv);
//��ȡSessionID
bool getSession();

bool initNetClient()
{
#ifdef _WIN32
	//��ʼ��Winsock
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
		//����sendҪ���͵����ݣ�������nBufferSize
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

		//��\n\n��β��ʾ�������
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
	bool bNewSocket;	//��ǰ��Socket�Ƿ����½���
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
			//���������ѶϿ�������һ��
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