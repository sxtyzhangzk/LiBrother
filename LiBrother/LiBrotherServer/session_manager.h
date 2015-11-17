#ifndef __SESSION_MANAGER_H__
#define __SESSION_MANAGER_H__

//会话管理器模块

#include <string>
#include <map>
#include <thread>
#include <mutex>

typedef unsigned long long session_t;	//会话ID类型

struct ILibClassFactory;
struct TSessionEx;
class CSession;

namespace Botan { class RandomNumberGenerator; };

class CSessionManager
{
public:
	CSessionManager();
	~CSessionManager();

	//启动服务器
	bool startServer(ILibClassFactory * pFactory);

	//停止服务器
	bool stopServer();

	//处理一条收到的请求
	bool recvRequest(const std::string& strClientIP, const std::string& strRequest, std::string& strResponse);

protected:
	//创建一个会话 并返回会话ID
	session_t createSession(const std::string& strClientIP, TSessionEx ** ppSession = nullptr);

	//生成一个会话ID
	session_t generateSessionID();

	//通过ID获取会话对象
	TSessionEx * getSession(session_t sessionID);

	//验证会话ID是否合法
	bool verifySessionID(session_t sessionID);

	//清理线程函数
	void cleanerThread();

protected:
	ILibClassFactory * m_pClassFactory;					//类工厂对象，传给CSession用
	Botan::RandomNumberGenerator * m_rng;				//随机数发生器
	TSessionEx *m_qCleanerHead, *m_qCleanerTail;		//清理过期会话使用的队列
	std::map<session_t, TSessionEx *> m_mapSessions;	//会话ID -> 会话对象映射表

	std::timed_mutex m_mutexExitSignal;		//通知清理线程退出的信号
	std::mutex m_mutexMap;					//保护m_mapSessions和清理队列的互斥器
	std::thread * m_pthreadCleaner;			//清理线程

private:
	//将一个会话对象添加进清理队列
	void addToCleanerQueue(TSessionEx * session);

	//将一个会话对象从清理队列中删除
	void removeFromCleanerQueue(TSessionEx * session);
};

#endif