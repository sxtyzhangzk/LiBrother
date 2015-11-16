#include "config.h"
#include "session_manager.h"
#include "session.h"

#include <cassert>
#include <ctime>
#include <chrono>
#include <sstream>

#include <liblog.h>
#include <botan/auto_rng.h>

MODULE_LOG_NAME("SessionManager");

const int Cleaner_Short_Wait_Time = 1000;	//清理线程的短等待时间

//会话扩展类型
struct TSessionEx
{
	session_t idSession;		//会话ID
	CSession * session;			//会话对象
	std::string strIP;			//客户端IP地址
	time_t timeLastAccess;		//最后访问会话的时间
	std::mutex mutexSession;	//保护session的互斥器
	TSessionEx *prev, *next;	//清理队列中的前后元素
	TSessionEx() : prev(nullptr), next(nullptr) {}
	TSessionEx(session_t sessionID) : idSession(sessionID), prev(nullptr), next(nullptr)
	{
		session = new CSession;
	}
	~TSessionEx()
	{
		if (session)
			delete session;
	}
};

CSessionManager::CSessionManager() :
	m_pthreadCleaner(nullptr)
{
	m_rng = new Botan::AutoSeeded_RNG;
	m_qCleanerHead = new TSessionEx;
	m_qCleanerTail = new TSessionEx;
	m_qCleanerHead->next = m_qCleanerTail;
	m_qCleanerTail->prev = m_qCleanerHead;
}

CSessionManager::~CSessionManager()
{
	delete m_rng;
	delete m_qCleanerHead;
	delete m_qCleanerTail;
}

bool CSessionManager::startServer(ILibClassFactory * pFactory)
{
	if (!pFactory)
		return false;
	m_pClassFactory = pFactory;

	//初始化清理线程
	m_mutexExitSignal.lock();
	m_pthreadCleaner = new std::thread(&CSessionManager::cleanerThread, this);
}

bool CSessionManager::stopServer()
{
	//等待清理线程结束
	m_mutexExitSignal.unlock();
	m_pthreadCleaner->join();
	delete m_pthreadCleaner;
	m_pthreadCleaner = nullptr;

	//关闭还在打开状态的会话
	for (auto session : m_mapSessions)
	{
		session.second->session->stopSession();
		delete session.second;
	}
	return true;
}

bool CSessionManager::recvRequest(const std::string& strClientIP, const std::string& strRequest, std::string& strResponse)
{
	strResponse.clear();

	bool bKeepAlive = true;
	session_t sessionID;
	TSessionEx * session;
	std::string cmd;
	std::stringstream ss(strRequest);

	//得到第一行命令
	std::getline(ss, cmd);
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), toupper);
	if (cmd == "KEEP-ALIVE")
	{
		//第一行指示保持连接，真正的命令在第二行
		std::getline(ss, cmd);
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), toupper);
	}
	else
		bKeepAlive = false;
	if (cmd == "DATA")
	{
		ss >> sessionID;
		std::string request, response;
		size_t now = ss.tellg();

		//把具体的内容取出来
		request.assign(strRequest, now);
		session = getSession(sessionID);
		if (!session || session->strIP != strClientIP)
		{
			strResponse = "INVALID_SESSION";
		}
		else
		{
			session->session->recvRequest(request, response);
			strResponse = "OK\n" + response;
		}
	}
	else if (cmd == "HELLO")
	{
		//创建新的会话
		sessionID = createSession(strClientIP, nullptr);
		strResponse = sessionID;
	}
	return bKeepAlive;
}

session_t CSessionManager::createSession(const std::string& strClientIP, TSessionEx ** ppSession)
{
	session_t sessionID = generateSessionID();
	TSessionEx * cltSession = new TSessionEx(sessionID);
	cltSession->strIP = strClientIP;
	cltSession->timeLastAccess = time(nullptr);

	if (!cltSession->session->startSession(m_pClassFactory, strClientIP))
	{
		delete cltSession;
		return 0;
	}

	m_mutexMap.lock();
	m_mapSessions.insert(std::make_pair(sessionID, cltSession));
	addToCleanerQueue(cltSession);
	m_mutexMap.unlock();

	if (ppSession)
		*ppSession = cltSession;
	return sessionID;
}

session_t CSessionManager::generateSessionID()
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

TSessionEx * CSessionManager::getSession(session_t sessionID)
{
	if (!verifySessionID(sessionID))
		return nullptr;
	TSessionEx * cltSession = nullptr;

	m_mutexMap.lock();
	auto session = m_mapSessions.find(sessionID);
	if (session != m_mapSessions.end())
	{
		cltSession = (*session).second;

		//访问了会话，重新设置会话的最后访问时间
		removeFromCleanerQueue(cltSession);
		cltSession->timeLastAccess = time(nullptr);
		addToCleanerQueue(cltSession);
	}
	m_mutexMap.unlock();

	return cltSession;
}

bool CSessionManager::verifySessionID(session_t sessionID)
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

void CSessionManager::addToCleanerQueue(TSessionEx * session)
{
	assert(session);
	session->prev = m_qCleanerTail->prev;
	session->next = m_qCleanerTail;
	m_qCleanerTail->prev->next = session;
	m_qCleanerTail->prev = session;
}

void CSessionManager::removeFromCleanerQueue(TSessionEx * session)
{
	assert(session);
	session->prev->next = session->next;
	session->next->prev = session->prev;
	session->prev = session->next = nullptr;
}

void CSessionManager::cleanerThread()
{
	int nWaitTime = g_configSvr.nSessionTimeout * 500;	//清理等待时间
	int nMaxClean = 50;		//一次最多清理的会话数
	while (!m_mutexExitSignal.try_lock_for(std::chrono::milliseconds(nWaitTime)))
	{
		int nCleanedSession = 0;
		time_t timeNow = time(nullptr);
		std::vector<TSessionEx *> vExpiredSession;
		TSessionEx * sessHead;

		m_mutexMap.lock();
		while (nCleanedSession < nMaxClean && (sessHead = m_qCleanerHead->next) != m_qCleanerTail)
		{
			//先把待清理的会话存起来
			vExpiredSession.push_back(sessHead);
			removeFromCleanerQueue(sessHead);
			m_mapSessions.erase(sessHead->idSession);
		}
		m_mutexMap.unlock();

		for (auto i = vExpiredSession.begin(); i != vExpiredSession.end(); ++i)
		{
			(*i)->mutexSession.lock();
			if ((*i)->session)
				(*i)->session->stopSession();
			(*i)->mutexSession.unlock();
			delete (*i);
		}

		if (vExpiredSession.size() > 0)
		{
			//可能有更多的会话要清理，下次多清一点
			nWaitTime = Cleaner_Short_Wait_Time;
			nMaxClean = nMaxClean * 3 / 2;
		}
		else
		{
			nWaitTime = g_configSvr.nSessionTimeout * 500;
			nMaxClean = 50;
		}
	}
}