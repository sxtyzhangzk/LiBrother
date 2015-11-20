#ifndef __CONNECTION_POOL_H__
#define __CONNECTION_POOL_H__

#include <string>
#include <map>
#include <vector>
#include <driver/mysql_public_iface.h>

class CConnectionPool
{
public:
	bool registerConnection(
		int regID,
		const std::string& strConn,
		const std::string& strUser,
		const std::string& strPWD,
		const std::string& strDB);
	sql::Connection * getConnection(int regID);
	void releaseConnection(sql::Connection * pConn);

protected:
	struct TSQLConn
	{
		std::string conn;
		std::string user;
		std::string pwd;
		std::string db;
		std::vector<sql::Connection *> pConn;
	};
	std::map<int, TSQLConn> m_mapConn;
};

#endif