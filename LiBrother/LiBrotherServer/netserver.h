#ifndef __NET_SERVER_H__
#define __NET_SERVER_H__

//服务端网络模块

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

	//初始化服务器
	bool initNetServer();

	//启动服务器
	bool startServer(ILibClassFactory * pClassFactory);

	//停止服务器
	bool stopServer();			

protected:
	//初始化监听套接字
	bool initListenSocket();

	//发送数据
	bool sendData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//处理接收到的数据
	void receivedData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//向客户端发送响应
	void sendResponse(TSocketEx * pSocket, const std::string& strResponse);

	//初始化TLS
	bool initTLS();

	//建立一个TLS会话
	bool createTLSSession(TSocketEx * pSocket);
	
	//处理刚收到的数据
	void recvRawData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//标记套接字即将结束的状态
	void setSocketClose(TSocketEx * pSocket);

#if defined(_WIN32) && !defined(FORCE_ASIO)
	//创建一个指定端口上的套接字
	TSocketEx * createListenSocket(int nPort);

	//初始化完成端口
	bool initIOCP();

	//完成端口 - 工作线程
	static unsigned long __stdcall WorkerThread(void * pParam);

	//完成端口 - 向完成端口投递Accept请求
	bool postAcceptRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//完成端口 - 向完成端口投递Recv请求
	bool postRecvRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	bool postSendRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//完成端口 - 处理Accept请求
	int doAccept(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);

	//完成端口 - 处理Recv请求
	bool doRecv(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);

	//完成端口 - 处理Send请求
	bool doSend(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);
#else
	//新建一个套接字
	void createNewSocket(bool bTLS);

	//处理完成的Accept操作
	void doAccept(TSocketEx * pSocket, boost::system::error_code errcode);

	//处理完成的Recv操作
	void doRecv(TSocketEx * pSocket, size_t nLen, boost::system::error_code errcode);

	//处理完成的Send操作
	void doSend(TSocketEx * pSocket, size_t nLen, boost::system::error_code errcode);

	//IO Service工作线程
	void workerThread();
#endif

protected:
	CSessionManager * m_pSManager;						//会话管理器
	std::set<TSocketEx *> m_vpClientSocks;				//连接套接字
	Botan::RandomNumberGenerator * m_rng;				//Botan - 随机数生成器

	CCredentialsManager * m_pCredManager;				//TLS 凭据管理器
	Botan::TLS::Session_Manager_In_Memory * m_pTLSSM;	//TLS 会话管理器
	Botan::TLS::Policy * m_pPolicy;						//TLS 配置

#if defined(_WIN32) && !defined(FORCE_ASIO)
	TSocketEx * m_psockListen;			//监听套接字
	TSocketEx * m_psockListenTLS;		//监听套接字(TLS)
	handle_t m_hIOCP;					//完成端口的句柄
	std::vector<handle_t> m_vhThreads;	//工作线程列表
	void * m_pfn_AcceptEx;				//Winsock - AcceptEx函数的指针
	void * m_pfn_GetAcceptExSockAddrs;	//Winsock - GetAcceptExSockAddrs函数的指针
	void * m_pcsClientSocks;			//保护m_vpClientSocks的临界区
#else
	io_service m_ioService;									//ASIO IO Service
	ip::tcp::acceptor * m_pacceptorMain;					//监听套接字
	ip::tcp::acceptor * m_pacceptorTLS;						//监听套接字(TLS)
	std::mutex m_mutexClientSocks;							//保护m_vpClientSocks的互斥器
	std::vector<std::shared_ptr<std::thread> > m_vThreads;	//工作线程列表
#endif
};

#endif