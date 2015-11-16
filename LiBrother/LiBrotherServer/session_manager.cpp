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

const int Cleaner_Short_Wait_Time = 1000;	//�����̵߳Ķ̵ȴ�ʱ��

//�Ự��չ����
struct TSessionEx
{
	session_t idSession;		//�ỰID
	CSession * session;			//�Ự����
	std::string strIP;			//�ͻ���IP��ַ
	time_t timeLastAccess;		//�����ʻỰ��ʱ��
	std::mutex mutexSession;	//����session�Ļ�����
	TSessionEx *prev, *next;	//��������е�ǰ��Ԫ��
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

	//��ʼ�������߳�
	m_mutexExitSignal.lock();
	m_pthreadCleaner = new std::thread(&CSessionManager::cleanerThread, this);
}

bool CSessionManager::stopServer()
{
	//�ȴ������߳̽���
	m_mutexExitSignal.unlock();
	m_pthreadCleaner->join();
	delete m_pthreadCleaner;
	m_pthreadCleaner = nullptr;

	//�رջ��ڴ�״̬�ĻỰ
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

	//�õ���һ������
	std::getline(ss, cmd);
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), toupper);
	if (cmd == "KEEP-ALIVE")
	{
		//��һ��ָʾ�������ӣ������������ڵڶ���
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

		//�Ѿ��������ȡ����
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
		//�����µĻỰ
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

		//�����˻Ự���������ûỰ��������ʱ��
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
	int nWaitTime = g_configSvr.nSessionTimeout * 500;	//����ȴ�ʱ��
	int nMaxClean = 50;		//һ���������ĻỰ��
	while (!m_mutexExitSignal.try_lock_for(std::chrono::milliseconds(nWaitTime)))
	{
		int nCleanedSession = 0;
		time_t timeNow = time(nullptr);
		std::vector<TSessionEx *> vExpiredSession;
		TSessionEx * sessHead;

		m_mutexMap.lock();
		while (nCleanedSession < nMaxClean && (sessHead = m_qCleanerHead->next) != m_qCleanerTail)
		{
			//�ȰѴ�����ĻỰ������
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
			//�����и���ĻỰҪ�����´ζ���һ��
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