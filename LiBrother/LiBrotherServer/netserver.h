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

#ifdef _WIN32
#include <vector>

typedef void * handle_t;				//WinAPI HANDLE
#endif

#include <set>
#include <map>
#include <utility>


typedef unsigned long long session_t;	//会话ID类型

struct ILibClassFactory;
struct TSocketEx;
struct TPerIOContext;
struct TSessionEx;
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

	//创建一个指定端口上的套接字
	TSocketEx * createListenSocket(int nPort);

	//发送数据，返回将要发送的字节数
	size_t sendData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//处理接收到的数据
	void receivedData(TSocketEx * pSocket, const char * pData, size_t nLen);

	//解析收到的数据
	int parseRequest(TSocketEx * pSocket);

	//创建一个会话 并返回会话ID
	session_t createSession(const std::string& strClientIP, TSessionEx ** ppSession = nullptr);

	//生成一个会话ID
	session_t generateSessionID();

	//通过ID获取会话对象
	TSessionEx * getSession(session_t sessionID);

	//验证会话ID是否合法
	bool verifySessionID(session_t sessionID);

	//初始化TLS
	bool initTLS();

	bool createTLSSession(TSocketEx * pSocket);

	void TLSRecvData(TSocketEx * pSocket, const char * pData, size_t nLen);

#ifdef _WIN32
	//初始化完成端口
	bool initIOCP();

	//完成端口 - 工作线程
	static unsigned long __stdcall WorkerThread(void * pParam);

	//会话清理线程
	static unsigned long __stdcall CleanerThread(void * pParam);

	//完成端口 - 向完成端口投递Accept请求
	bool postAcceptRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//完成端口 - 向完成端口投递Recv请求
	bool postRecvRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	bool postSendRq(TSocketEx * pSocket, TPerIOContext * pIOContext);

	//完成端口 - 处理Accept请求
	int doAccept(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);

	//完成端口 - 处理Recv请求
	bool doRecv(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);

	bool doSend(TSocketEx * pSocket, TPerIOContext * pIOContext, size_t nDataSize);
#endif

protected:
	ILibClassFactory * m_pClassFactory;
	TSocketEx * m_psockListen;							//监听套接字
	TSocketEx * m_psockListenTLS;						//监听套接字(TLS)
	std::set<TSocketEx *> m_vpClientSocks;				//连接套接字
	std::map<session_t, TSessionEx *> m_mapSessions;	//会话ID -> 会话对象映射表
	Botan::RandomNumberGenerator * m_rng;				//Botan - 随机数生成器

	TSessionEx *m_qCleanerHead, *m_qCleanerTail;		//清理过期会话使用的队列

	CCredentialsManager * m_pCredManager;
	Botan::TLS::Session_Manager_In_Memory * m_pTLSSM;
	Botan::TLS::Policy * m_pPolicy;

#ifdef _WIN32
	handle_t m_hIOCP;					//完成端口的句柄
	handle_t m_hThreadCleaner;			//清理线程的句柄
	handle_t m_hExitEvent;				//通知线程退出的事件句柄
	std::vector<handle_t> m_vhThreads;	//工作线程列表
	void * m_pfn_AcceptEx;				//Winsock - AcceptEx函数的指针
	void * m_pfn_GetAcceptExSockAddrs;	//Winsock - GetAcceptExSockAddrs函数的指针
	void * m_pcsClientSocks;			//保护m_vpClientSocks的临界区
	void * m_pcsSessionMap;				//保护m_mapSessions及清理队列的临界区
#endif

private:
	//将一个会话对象添加进清理队列
	void addToCleanerQueue(TSessionEx * session);

	//将一个会话对象从清理队列中删除
	void removeFromCleanerQueue(TSessionEx * session);
};

#endif