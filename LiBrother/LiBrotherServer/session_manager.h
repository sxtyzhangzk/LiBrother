#ifndef __SESSION_MANAGER_H__
#define __SESSION_MANAGER_H__

//�Ự������ģ��

#include <string>
#include <map>
#include <thread>
#include <mutex>

typedef unsigned long long session_t;	//�ỰID����

struct ILibClassFactory;
struct TSessionEx;
class CSession;

namespace Botan { class RandomNumberGenerator; };

class CSessionManager
{
public:
	CSessionManager();
	~CSessionManager();

	//����������
	bool startServer(ILibClassFactory * pFactory);

	//ֹͣ������
	bool stopServer();

	//����һ���յ�������
	bool recvRequest(const std::string& strClientIP, const std::string& strRequest, std::string& strResponse);

protected:
	//����һ���Ự �����ػỰID
	session_t createSession(const std::string& strClientIP, TSessionEx ** ppSession = nullptr);

	//����һ���ỰID
	session_t generateSessionID();

	//ͨ��ID��ȡ�Ự����
	TSessionEx * getSession(session_t sessionID);

	//��֤�ỰID�Ƿ�Ϸ�
	bool verifySessionID(session_t sessionID);

	//�����̺߳���
	void cleanerThread();

protected:
	ILibClassFactory * m_pClassFactory;					//�๤�����󣬴���CSession��
	Botan::RandomNumberGenerator * m_rng;				//�����������
	TSessionEx *m_qCleanerHead, *m_qCleanerTail;		//������ڻỰʹ�õĶ���
	std::map<session_t, TSessionEx *> m_mapSessions;	//�ỰID -> �Ự����ӳ���

	std::timed_mutex m_mutexExitSignal;		//֪ͨ�����߳��˳����ź�
	std::mutex m_mutexMap;					//����m_mapSessions��������еĻ�����
	std::thread * m_pthreadCleaner;			//�����߳�

private:
	//��һ���Ự������ӽ��������
	void addToCleanerQueue(TSessionEx * session);

	//��һ���Ự��������������ɾ��
	void removeFromCleanerQueue(TSessionEx * session);
};

#endif