#include "config.h"
#include "netserver.h"
#include "session.h"

#include <ctime>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#include <liblog.h>
#include <botan/auto_rng.h>
#include <botan/tls_server.h>

MODULE_LOG_NAME("NetServer");

//服务端网络模块 - Windows平台

const int Thread_per_CPU = 2;				//每个处理器的工作线程数
const size_t BufferSize = 1024;				//接收缓存区的大小
const int Accept_Post_Count = 8;			//同时投递的Accept请求数
const ULONG_PTR Exit_Key = 0;				//指示工作线程退出的CompletionKey
const int Cleaner_Short_Wait_Time = 1000;	//清理线程的短等待时间

//完成端口处理的请求类型
enum TIOType
{
	IoAccept,
	IoRecv
};

//工作线程的参数
struct TWorkerThreadParam
{
	CNetServer * pNetServer;
	int nThreadID;
};

//会话扩展类型
struct TSessionEx
{
	session_t idSession;		//会话ID
	CSession * session;			//会话对象
	in_addr ipClient;			//客户端的IP地址
	time_t timeLastAccess;		//最后访问会话的时间
	CRITICAL_SECTION csSession;	//保护session的临界区
	TSessionEx *prev, *next;	//清理队列中的前后元素
	TSessionEx() : prev(nullptr), next(nullptr) {}
	TSessionEx(session_t sessionID) : idSession(sessionID), prev(nullptr), next(nullptr)
	{
		session = new CSession;
		InitializeCriticalSection(&csSession);
	}
	~TSessionEx()
	{
		if (session)
		{
			delete session;
			DeleteCriticalSection(&csSession);
		}
	}
};

//套接字扩展类型
struct TSocketEx
{
	SOCKET socket;			//套接字
	sockaddr_in client;		//客户端连接信息
	bool TLS;				//是否使用TLS加密
	std::vector<TPerIOContext *> vPIOContexts;	//单IO上下文的列表，用于回收内存
	std::string recvBuffer;	//缓存的数据

	TSocketEx(SOCKET socket) : socket(socket), TLS(false)
	{
		memset(&client, 0, sizeof(client));
	}
	TSocketEx(SOCKET socket, sockaddr_in client, bool TLS) : socket(socket), client(client), TLS(TLS) {}

	~TSocketEx()
	{
		if (socket != INVALID_SOCKET)
			closesocket(socket);
		//清理单IO上下文
		for (auto i = vPIOContexts.begin(); i != vPIOContexts.end(); ++i)
		{
			if (*i)
				delete (*i);
		}
		vPIOContexts.clear();
	}

	//创建一个单IO上下文
	TPerIOContext * createIoContext(TIOType ioType)
	{
		TPerIOContext * ioContext = new TPerIOContext(ioType);
		vPIOContexts.push_back(ioContext);
		return ioContext;
	}

	//删除最后一个单IO上下文
	void popIoContext()
	{
		if (vPIOContexts.empty())
			return;
		TPerIOContext * ioContext = vPIOContexts.back();
		delete ioContext;
		vPIOContexts.pop_back();
	}
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
		wsaBuffer.buf = buffer;
		wsaBuffer.len = BufferSize;
	}
};

CNetServer::CNetServer() :
	m_psockListen(nullptr),
	m_psockListenTLS(nullptr),
	m_hIOCP(NULL)
{
	m_vhThreads.clear();
	m_pcsClientSocks = new CRITICAL_SECTION;
	m_pcsSessionMap = new CRITICAL_SECTION;
	m_rng = new Botan::AutoSeeded_RNG;
	m_qCleanerHead = new TSessionEx;
	m_qCleanerTail = new TSessionEx;
	m_qCleanerHead->next = m_qCleanerTail;
	m_qCleanerTail->prev = m_qCleanerHead;
}

CNetServer::~CNetServer()
{
	delete m_qCleanerHead;
	delete m_qCleanerTail;
	delete m_rng;
	delete (LPCRITICAL_SECTION)m_pcsSessionMap;
	delete (LPCRITICAL_SECTION)m_pcsClientSocks;
}

bool CNetServer::initNetServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return false;
	InitializeCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);
	InitializeCriticalSection((LPCRITICAL_SECTION)m_pcsSessionMap);
	return true;
}

bool CNetServer::startServer()
{
	
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

	TSocketEx * sockEx = new TSocketEx(sockListen);

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
	bool bNewIOContext = false;
	if (!pSocket)
		return false;
	if (!pIOContext)
	{
		//第一次创建IO上下文
		pIOContext = pSocket->createIoContext(IoAccept);
		bNewIOContext = true;
	}
	
	SOCKET sockClient = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (sockClient == INVALID_SOCKET)
	{
		if (bNewIOContext)
			pSocket->popIoContext();
		return false;
	}

	DWORD dwBytes;
	if (!(LPFN_ACCEPTEX(m_pfn_AcceptEx))(pSocket->socket, sockClient, pIOContext->wsaBuffer.buf,
		BufferSize - (sizeof(sockaddr_in) + 16) * 2, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		&dwBytes, (LPOVERLAPPED)pIOContext))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			if (bNewIOContext)
				pSocket->popIoContext();
			return false;
		}
	}
	
	pIOContext->sockClient = sockClient;

	return true;
}

bool CNetServer::postRecvRq(TSocketEx * pSocket, TPerIOContext * pIOContext)
{
	bool bNewIOContext = false;
	if (!pSocket)
		return false;
	if (!pIOContext)
	{
		pIOContext = pSocket->createIoContext(IoRecv);
		bNewIOContext = true;
	}

	DWORD dwBytes, dwFlags = 0;
	int nRet = WSARecv(pSocket->socket, &pIOContext->wsaBuffer, 1, &dwBytes, &dwFlags, (LPOVERLAPPED)pIOContext, nullptr);

	if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		if (bNewIOContext)
			pSocket->popIoContext();
		return false;
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

		}
	}
	delete param;
	return 0;
}

int CNetServer::doAccept(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize)
{
	if (!pSocket || pSocket->socket == INVALID_SOCKET)
		return -1;	//Fatal Error - Listen Socket Error
	if (!pIOContext || pIOContext->sockClient == INVALID_SOCKET)
		return -1;
	
	sockaddr_in *localAddr = nullptr, *remoteAddr = nullptr;
	int sizeLocal = sizeof(localAddr), sizeRemote = sizeof(remoteAddr);

	(LPFN_GETACCEPTEXSOCKADDRS(m_pfn_GetAcceptExSockAddrs))(pIOContext->wsaBuffer.buf,
		BufferSize - 2 * (sizeof(sockaddr_in) + 16),
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		(sockaddr **)&localAddr, &sizeLocal,
		(sockaddr **)&remoteAddr, &sizeRemote);

	TSocketEx * pClientSocket = new TSocketEx(pIOContext->sockClient, *remoteAddr, pSocket->TLS);

	//处理客户端发来的第一组数据
	receivedData(pClientSocket, pIOContext->wsaBuffer.buf, nDataSize);

	if (!postAcceptRq(pSocket, pIOContext))
		return -1;

	if (!CreateIoCompletionPort((HANDLE)pClientSocket->socket, m_hIOCP, (ULONG_PTR)pClientSocket, 0))
	{
		delete pClientSocket;
		return 1;	//Error - Client Socket Error
	}

	if (!postRecvRq(pClientSocket, nullptr))
	{
		delete pClientSocket;
		return 1;
	}

	EnterCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);
	m_vpClientSocks.insert(pClientSocket);
	LeaveCriticalSection((LPCRITICAL_SECTION)m_pcsClientSocks);

	return 0;	//Success
}

bool CNetServer::doRecv(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize)
{
	if (!pSocket || !pIOContext)
		return false;
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
	return postRecvRq(pSocket, pIOContext);
}

session_t CNetServer::createSession(const std::string& strClientIP, TSessionEx ** ppSession)
{
	session_t sessionID = generateSessionID();
	TSessionEx * cltSession = new TSessionEx(sessionID);
	if (!cltSession->session->startSession(strClientIP))
	{
		delete cltSession;
		return 0;
	}
	
	EnterCriticalSection((LPCRITICAL_SECTION)m_pcsSessionMap);
	m_mapSessions.insert(std::make_pair(sessionID, cltSession));
	addToCleanerQueue(cltSession);
	LeaveCriticalSection((LPCRITICAL_SECTION)m_pcsSessionMap);

	if (ppSession)
		*ppSession = cltSession;
	return sessionID;
}

session_t CNetServer::generateSessionID()
{
	session_t sessionID;
	unsigned int randomCode, checkSum = 0;
	m_rng->randomize((Botan::byte *)&randomCode, sizeof(randomCode));
	sessionID |= (1ULL << 36);
	sessionID |= ((session_t)randomCode << 4);
	for (int bit = 35; bit >= 4; bit--)
		checkSum += ((sessionID & (1ULL << bit)) > 0) * bit;
	checkSum ^= 0xF;
	checkSum &= 0xF;
	sessionID |= checkSum;
	return sessionID;
}

TSessionEx * CNetServer::getSession(session_t sessionID)
{
	if (!verifySessionID(sessionID))
		return nullptr;
	TSessionEx * cltSession = nullptr;
	EnterCriticalSection((LPCRITICAL_SECTION)m_pcsSessionMap);
	auto session = m_mapSessions.find(sessionID);
	if (session != m_mapSessions.end())
	{
		cltSession = (*session).second;

		//访问了会话，重新设置会话的最后访问时间
		removeFromCleanerQueue(cltSession);
		cltSession->timeLastAccess = time(nullptr);
		addToCleanerQueue(cltSession);
	}
	LeaveCriticalSection((LPCRITICAL_SECTION)m_pcsSessionMap);
	return cltSession;
}

bool CNetServer::verifySessionID(session_t sessionID)
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

void CNetServer::addToCleanerQueue(TSessionEx * session)
{
	if (!session)
		return;
	session->prev = m_qCleanerTail->prev;
	session->next = m_qCleanerTail->prev->next;
	m_qCleanerTail->prev->next = session;
	m_qCleanerTail->prev = session;
}

void CNetServer::removeFromCleanerQueue(TSessionEx * session)
{
	session->prev->next = session->next;
	session->next->prev = session->prev;
	session->prev = session->next = nullptr;
}

unsigned long WINAPI CNetServer::CleanerThread(void * pParam)
{
	CNetServer * pNetServer = (CNetServer *)pParam;
	HANDLE hExitEvent = pNetServer->m_hExitEvent;
	LPCRITICAL_SECTION csSessionMap = (LPCRITICAL_SECTION)pNetServer->m_pcsSessionMap;
	int nWaitTime = g_configSvr.nSessionTimeout * 500;	//清理等待时间
	int nMaxClean = 50;		//一次最多清理的会话数
	while (WaitForSingleObject(hExitEvent, nWaitTime) == WAIT_TIMEOUT)
	{
		int nCleanedSession = 0;
		time_t timeNow = time(nullptr);
		std::vector<TSessionEx *> vExpiredSession;
		TSessionEx * sessHead;

		EnterCriticalSection(csSessionMap);
		while (nCleanedSession < nMaxClean && (sessHead = pNetServer->m_qCleanerHead->next) != pNetServer->m_qCleanerTail)
		{
			vExpiredSession.push_back(sessHead);
			pNetServer->removeFromCleanerQueue(sessHead);
			pNetServer->m_mapSessions.erase(sessHead->idSession);
		}
		LeaveCriticalSection(csSessionMap);

		for (auto i = vExpiredSession.begin(); i != vExpiredSession.end(); ++i)
		{
			EnterCriticalSection(&(*i)->csSession);
			if((*i)->session)
				(*i)->session->stopSession();
			LeaveCriticalSection(&(*i)->csSession);
			delete (*i);
		}

		if (vExpiredSession.size() > 0)
		{
			nWaitTime = Cleaner_Short_Wait_Time;
			nMaxClean = nMaxClean * 3 / 2;			
		}
		else
		{
			nWaitTime = g_configSvr.nSessionTimeout * 500;
			nMaxClean = 50;
		}
	}
	return 0;
}

void CNetServer::receivedData(TSocketEx * pSocket, const char * pData, size_t nLen)
{
	
}

int CNetServer::parseRequest(TSocketEx * pSocket)
{

}