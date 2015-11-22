#ifndef __CONNECTION_POOL_H__
#define __CONNECTION_POOL_H__

#include <string>
#include <set>
#include <map>
#include <driver/mysql_public_iface.h>

#define MYSQL_CONN_RELEASER std::bind(&CConnectionPool::releaseConnection, m_pDatabase, REGID_MYSQL_CONN, std::placeholders::_1)

class CConnectionPool
{
public:
	CConnectionPool();
	~CConnectionPool();
	bool registerConnection(
		int regID,
		const std::string& strHost,
		const std::string& strUser,
		const std::string& strPWD,
		const std::string& strDB);
	sql::Connection * getConnection(int regID);
	void releaseConnection(int regID, sql::Connection * pConn);

protected:
	struct TSQLConn
	{
		std::string host;
		std::string user;
		std::string pwd;
		std::string db;
		std::set<sql::Connection *> pConn;
		std::set<sql::Connection *> pConnInUse;
	};

	bool createConnection(TSQLConn& sqlConn);

protected:
	std::map<int, TSQLConn> m_mapConn;
	sql::Driver * m_pDriver;
};

#endif