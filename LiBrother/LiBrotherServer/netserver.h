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

//#define FORCE_ASIO
#if defined(_WIN32) && !defined(FORCE_ASIO)
#include <vector>

typedef void * handle_t;				//WinAPI HANDLE
#else

#include <thread>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>
using namespace boost::asio;

#endif

#include <set>
#include <map>
#include <utility>

struct ILibClassFactory;
struct TSocketEx;
struct TPerIOContext;
class CSessionManager;
class CCredentialsManager;
namespace Botan
{
	class RandomNumberGenerator;
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

	//��������
	bool sendData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//������յ�������
	void receivedData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//��ͻ��˷�����Ӧ
	void sendResponse(TSocketEx * pSocket, const std::string& strResponse);

	//��ʼ��TLS
	bool initTLS();

	//����һ��TLS�Ự
	bool createTLSSession(TSocketEx * pSocket);
	
	//������յ�������
	void recvRawData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//����׽��ּ���������״̬
	void setSocketClose(TSocketEx * pSocket);

#if defined(_WIN32) && !defined(FORCE_ASIO)
	//����һ��ָ���˿��ϵ��׽���
	TSocketEx * createListenSocket(int nPort);

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

	//��ɶ˿� - ����Send����
	bool doSend(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);
#else
	//�½�һ���׽���
	void createNewSocket(bool bTLS);

	//������ɵ�Accept����
	void doAccept(TSocketEx * pSocket, boost::system::error_code errcode);

	//������ɵ�Recv����
	void doRecv(TSocketEx * pSocket, size_t nLen, boost::system::error_code errcode);

	//������ɵ�Send����
	void doSend(TSocketEx * pSocket, size_t nLen, boost::system::error_code errcode);

	//IO Service�����߳�
	void workerThread();
#endif

protected:
	CSessionManager * m_pSManager;						//�Ự������
	std::set<TSocketEx *> m_vpClientSocks;				//�����׽���
	Botan::RandomNumberGenerator * m_rng;				//Botan - �����������

	CCredentialsManager * m_pCredManager;				//TLS ƾ�ݹ�����
	Botan::TLS::Session_Manager_In_Memory * m_pTLSSM;	//TLS �Ự������
	Botan::TLS::Policy * m_pPolicy;						//TLS ����

#if defined(_WIN32) && !defined(FORCE_ASIO)
	TSocketEx * m_psockListen;			//�����׽���
	TSocketEx * m_psockListenTLS;		//�����׽���(TLS)
	handle_t m_hIOCP;					//��ɶ˿ڵľ��
	std::vector<handle_t> m_vhThreads;	//�����߳��б�
	void * m_pfn_AcceptEx;				//Winsock - AcceptEx������ָ��
	void * m_pfn_GetAcceptExSockAddrs;	//Winsock - GetAcceptExSockAddrs������ָ��
	void * m_pcsClientSocks;			//����m_vpClientSocks���ٽ���
#else
	io_service m_ioService;									//ASIO IO Service
	ip::tcp::acceptor * m_pacceptorMain;					//�����׽���
	ip::tcp::acceptor * m_pacceptorTLS;						//�����׽���(TLS)
	std::mutex m_mutexClientSocks;							//����m_vpClientSocks�Ļ�����
	std::vector<std::shared_ptr<std::thread> > m_vThreads;	//�����߳��б�
#endif
};

#endif