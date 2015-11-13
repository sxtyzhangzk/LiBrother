#ifndef __NET_SERVER_H__
#define __NET_SERVER_H__

//���������ģ��

#if defined(_WIN64)
typedef unsigned long long socket_t;
#elif defined(_WIN32)
typedef unsigned int socket_t;
#else
typedef int socket_t;
const socket_t INVALID_SOCKET = 0;
#endif

#ifdef _WIN32
#include <vector>

typedef void * handle_t;				//WinAPI HANDLE
#endif

#include <set>
#include <map>
#include <utility>


typedef unsigned long long session_t;	//�ỰID����

struct TSocketEx;
struct TPerIOContext;
struct TSessionEx;
class CSession;
namespace Botan { class RandomNumberGenerator; };

class CNetServer
{
public:
	CNetServer();
	~CNetServer();

	//��ʼ��������
	bool initNetServer();

	//����������
	bool startServer();

	//ֹͣ������
	bool stopServer();			

protected:
	//��ʼ�������׽���
	bool initListenSocket();

	//����һ��ָ���˿��ϵ��׽���
	TSocketEx * createListenSocket(int nPort);

	//������յ�������
	void receivedData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//�����յ�������
	int parseRequest(TSocketEx * pSocket);

	//����һ���Ự �����ػỰID
	session_t createSession(const std::string& strClientIP, TSessionEx ** ppSession = nullptr);

	//����һ���ỰID
	session_t generateSessionID();

	//ͨ��ID��ȡ�Ự����
	TSessionEx * getSession(session_t sessionID);

	//��֤�ỰID�Ƿ�Ϸ�
	bool verifySessionID(session_t sessionID);

#ifdef _WIN32
	//��ʼ����ɶ˿�
	bool initIOCP();

	//��ɶ˿� - �����߳�
	static unsigned long __stdcall WorkerThread(void * pParam);

	//�Ự�����߳�
	static unsigned long __stdcall CleanerThread(void * pParam);

	//��ɶ˿� - ����ɶ˿�Ͷ��Accept����
	bool postAcceptRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//��ɶ˿� - ����ɶ˿�Ͷ��Recv����
	bool postRecvRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//��ɶ˿� - ����Accept����
	int doAccept(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);

	//��ɶ˿� - ����Recv����
	bool doRecv(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);
#endif

protected:
	TSocketEx * m_psockListen;							//�����׽���
	TSocketEx * m_psockListenTLS;						//�����׽���(TLS)
	std::set<TSocketEx *> m_vpClientSocks;				//�����׽���
	std::map<session_t, TSessionEx *> m_mapSessions;	//�ỰID -> �Ự����ӳ���
	Botan::RandomNumberGenerator * m_rng;				//Botan - �����������

	TSessionEx *m_qCleanerHead, *m_qCleanerTail;		//������ڻỰʹ�õĶ���

#ifdef _WIN32
	handle_t m_hIOCP;					//��ɶ˿ڵľ��
	handle_t m_hThreadCleaner;			//�����̵߳ľ��
	handle_t m_hExitEvent;				//֪ͨ�߳��˳����¼����
	std::vector<handle_t> m_vhThreads;	//�����߳��б�
	void * m_pfn_AcceptEx;				//Winsock - AcceptEx������ָ��
	void * m_pfn_GetAcceptExSockAddrs;	//Winsock - GetAcceptExSockAddrs������ָ��
	void * m_pcsClientSocks;			//����m_vpClientSocks���ٽ���
	void * m_pcsSessionMap;				//����m_mapSessions��������е��ٽ���
#endif

private:
	//��һ���Ự������ӽ��������
	void addToCleanerQueue(TSessionEx * session);

	//��һ���Ự��������������ɾ��
	void removeFromCleanerQueue(TSessionEx * session);
};

#endif