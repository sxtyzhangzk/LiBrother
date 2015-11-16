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

struct ILibClassFactory;
struct TSocketEx;
struct TPerIOContext;
class CSessionManager;
class CSession;
class CCredentialsManager;
namespace Botan
{
	class RandomNumberGenerator;
	//class Basic_Credentials_Manager;
	namespace TLS
	{
		class Policy;
		class Session_Manager_In_Memory;
	};
};

class CNetServer
{
	friend struct TSocketEx;
public:
	CNetServer();
	~CNetServer();

	//��ʼ��������
	bool initNetServer();

	//����������
	bool startServer(ILibClassFactory * pClassFactory);

	//ֹͣ������
	bool stopServer();			

protected:
	//��ʼ�������׽���
	bool initListenSocket();

	//����һ��ָ���˿��ϵ��׽���
	TSocketEx * createListenSocket(int nPort);

	//�������ݣ����ؽ�Ҫ���͵��ֽ���
	bool sendData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//������յ�������
	void receivedData(TSocketEx * pSocket, const char * pData, size_t nLen);

	void sendResponse(TSocketEx * pSocket, const std::string& strResponse);

	//��ʼ��TLS
	bool initTLS();

	bool createTLSSession(TSocketEx * pSocket);

	void TLSRecvData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//bool isReadyToClose(TSocketEx * pSocket);

#ifdef _WIN32
	//��ʼ����ɶ˿�
	bool initIOCP();

	//��ɶ˿� - �����߳�
	static unsigned long __stdcall WorkerThread(void * pParam);

	//��ɶ˿� - ����ɶ˿�Ͷ��Accept����
	bool postAcceptRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//��ɶ˿� - ����ɶ˿�Ͷ��Recv����
	bool postRecvRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	bool postSendRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//��ɶ˿� - ����Accept����
	int doAccept(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);

	//��ɶ˿� - ����Recv����
	bool doRecv(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);

	bool doSend(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);
#endif

protected:
	CSessionManager * m_pSManager;
	TSocketEx * m_psockListen;							//�����׽���
	TSocketEx * m_psockListenTLS;						//�����׽���(TLS)
	std::set<TSocketEx *> m_vpClientSocks;				//�����׽���
	
	Botan::RandomNumberGenerator * m_rng;				//Botan - �����������

	CCredentialsManager * m_pCredManager;
	Botan::TLS::Session_Manager_In_Memory * m_pTLSSM;
	Botan::TLS::Policy * m_pPolicy;

#ifdef _WIN32
	handle_t m_hIOCP;					//��ɶ˿ڵľ��
	std::vector<handle_t> m_vhThreads;	//�����߳��б�
	void * m_pfn_AcceptEx;				//Winsock - AcceptEx������ָ��
	void * m_pfn_GetAcceptExSockAddrs;	//Winsock - GetAcceptExSockAddrs������ָ��
	void * m_pcsClientSocks;			//����m_vpClientSocks���ٽ���
#endif
};

#endif