#include"manager.h"
#include"user.h"
#include<sstream>
#include"config.h"
#include"utils.h"
#include <liblog.h>

MODULE_LOG_NAME("CManager");

CManager::CManager(CConnectionPool * DatabaseFile)
{
	m_pDatabase=DatabaseFile;
}
CManager::~CManager()
{

}
bool CManager::getUserByID(int nID, IUser ** ppUser)
{
	if (!ppUser)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	IUser* n = new CUser(m_pDatabase);
	try
	{
		std::shared_ptr<sql::Connection>  c(m_pDatabase->getConnection(REGID_MYSQL_CONN),MYSQL_CONN_RELEASER);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "SELECT * FROM UserInfoDatabase WHERE id = " << nID;
		stat->execute(str.str());
		std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
		result->next();
		TUserBasicInfo Basicinfo;
		Basicinfo.email = result->getString("email");
		Basicinfo.gender = result->getInt("gender");
		Basicinfo.id = result->getInt("id");
		Basicinfo.name = result->getString("name");
		n->setReadLevel(result->getInt("ReadLevel"));
		n->setAuthLevel(result->getInt("AuthLevel"));
		n->setBasicInfo(Basicinfo);
		((CUser*)n)->straightsetpassword(result->getString("password").c_str());
		((CUser*)n)->sign();
		(*ppUser) = n;
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CManager::insertUser(IUser * pUser)
{
	TUserBasicInfo info;
	pUser->getBasicInfo(info);
	if (verify(info.name.c_str(), info.email.c_str()))
	{
		if (!((CUser*)pUser)->insert())
		{
			transferError(pUser);
			return false;
		}
		return true;
	}
	else return false;
}
bool CManager::getUserByName(const char * strName, IUser ** ppUser)
{
	int type=0;
	for (int i = 0; i < strlen(strName); i++)
	{
		if (strName[i] == '@')
		{
			type = 1;
			break;
		}
		else if ((strName[i] <= 'z' && strName[i] >= 'a') || (strName[i] <= 'Z' && strName[i] >= 'A') || strName[i] == '_')
			type = 2;
		else if (strName[i] <= '9' && strName[i] >= '0') {}
		else
		{
			setError(InvalidParam, -1, "Invalid string.");
			return false;
		}
	}
	if (!type)
	{
		int nId;
		std::stringstream ss;
		ss << strName;
		ss >> nId;
		return getUserByID(nId, ppUser);
	}
	if (type == 1)
	{
		if (!ppUser)
		{
			setError(InvalidParam, 4, "The pointer is NULL.");
			return false;
		}
		
		try
		{
			std::shared_ptr<sql::Connection>  c(m_pDatabase->getConnection(REGID_MYSQL_CONN), MYSQL_CONN_RELEASER);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "SELECT * FROM UserInfoDatabase WHERE email = " << '\'' << str2sql(strName) << '\'';
			stat->execute(str.str());
			std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
			result->next();
			TUserBasicInfo Basicinfo;
			Basicinfo.email = result->getString("email");
			Basicinfo.gender = result->getInt("gender");
			Basicinfo.id = result->getInt("id");
			Basicinfo.name = result->getString("name");
			(*ppUser)->setReadLevel(result->getInt("ReadLevel"));
			(*ppUser)->setBasicInfo(Basicinfo);
			((CUser*)(*ppUser))->straightsetpassword(result->getString("password").c_str());
			((CUser*)(*ppUser))->sign();
			return true;
		}
		catch (sql::SQLException& e)
		{
			setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
			return false;
		}
		return true;
	}
	if (type == 2)
	{
		if (!ppUser)
		{
			setError(InvalidParam, 4, "The pointer is NULL.");
			return false;
		}
		try
		{
			std::shared_ptr<sql::Connection>  c(m_pDatabase->getConnection(REGID_MYSQL_CONN),MYSQL_CONN_RELEASER);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "SELECT * FROM UserInfoDatabase WHERE name = " << '\'' << str2sql(strName) << '\'';
			stat->execute(str.str());
			std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
			result->next();
			TUserBasicInfo Basicinfo;
			Basicinfo.email = result->getString("email");
			Basicinfo.gender = result->getInt("gender");
			Basicinfo.id = result->getInt("id");
			Basicinfo.name = result->getString("name");
			(*ppUser)->setReadLevel(result->getInt("ReadLevel"));
			(*ppUser)->setBasicInfo(Basicinfo);
			((CUser*)(*ppUser))->straightsetpassword(result->getString("password").c_str());
			((CUser*)(*ppUser))->sign();
			return true;
		}
		catch (sql::SQLException& e)
		{
			setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
			return false;
		}
	}
	return false;
}
bool CManager::verify(const char* strName,const char * strEmail)
{
	if (!strName || !strEmail)
		return false;
	try
	{
		std::shared_ptr<sql::Connection>  c(m_pDatabase->getConnection(REGID_MYSQL_CONN), MYSQL_CONN_RELEASER);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str1;
		str1 << "SELECT * FROM UserInfoDatabase WHERE name = " << '\'' << str2sql(strName) << '\'';
		stat->execute(str1.str());
		std::shared_ptr<sql::ResultSet> result1(stat->getResultSet());
		int flag = 1;
		if (result1->next()) flag = 0;
		std::stringstream str2;
		str2 << "SELECT * FROM UserInfoDatabase WHERE email = " << '\'' << str2sql(strEmail) << '\'';
		stat->execute(str2.str());
		std::shared_ptr<sql::ResultSet> result2(stat->getResultSet());
		if (result2->next()) flag = 0;
		if (flag)
			return true;
		else
		{
			setError(InvalidParam, 99, "Repeated");
			return false;
		}
	}
	catch (sql::SQLException& e)
	{
		lprintf("An error occurred while verifying: %s", e.what());
		return false;
	}
	return false;
}