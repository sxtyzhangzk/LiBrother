#include "connection_pool.h"

#include <liblog.h>

MODULE_LOG_NAME("ConnectionPool");

CConnectionPool::CConnectionPool()
{
	m_pDriver = sql::mysql::get_driver_instance();
}

CConnectionPool::~CConnectionPool()
{
	bool flag = false;
	for (auto sqlConn : m_mapConn)
	{
		for (auto conn : sqlConn.second.pConn)
			delete conn;
		for (auto conn : sqlConn.second.pConnInUse)
		{
			delete conn;
			flag = true;
		}
	}
	if (!flag)
		lprintf_w("Some connections are not released.");
}

bool CConnectionPool::registerConnection(
	int regID,
	const std::string& strHost, const std::string& strUser, const std::string& strPWD, const std::string& strDB)
{
	TSQLConn sqlConn;
	sqlConn.host = strHost;
	sqlConn.user = strUser;
	sqlConn.pwd = strPWD;
	sqlConn.db = strDB;
	if (!createConnection(sqlConn))
		return false;
	m_mapConn.insert(std::make_pair(regID, sqlConn));
	return true;
}

sql::Connection * CConnectionPool::getConnection(int regID)
{
	auto sqlConn = m_mapConn.find(regID);
	if (sqlConn == m_mapConn.end())
		return nullptr;
	if (sqlConn->second.pConn.empty())
	{
		if (!createConnection(sqlConn->second))
			return nullptr;
	}
	auto iter_conn = sqlConn->second.pConn.begin();
	sql::Connection * conn = *iter_conn;
	if (conn->isClosed())
		conn->reconnect();
	sqlConn->second.pConn.erase(iter_conn);
	sqlConn->second.pConnInUse.insert(conn);
	return conn;
}

void CConnectionPool::releaseConnection(int nID, sql::Connection * pConn)
{
	auto sqlConn = m_mapConn.find(nID);
	if (sqlConn == m_mapConn.end())
		return;
	auto iter_conn = sqlConn->second.pConnInUse.find(pConn);
	if (iter_conn == sqlConn->second.pConnInUse.end())
		return;
	sqlConn->second.pConn.insert(*iter_conn);
	sqlConn->second.pConnInUse.erase(iter_conn);
}

bool CConnectionPool::createConnection(TSQLConn& sqlConn)
{
	sql::Connection * conn = nullptr;
	try
	{
		conn = m_pDriver->connect(sqlConn.host, sqlConn.user, sqlConn.pwd);
		if(sqlConn.db != "")
			conn->setSchema(sqlConn.db);
		sqlConn.pConn.insert(conn);
	}
	catch (sql::SQLException& e)
	{
		if (conn)
			delete conn;
		lprintf_e("Cannot Create MySQL Connection: %s", e.what());
		return false;
	}
	return true;
}