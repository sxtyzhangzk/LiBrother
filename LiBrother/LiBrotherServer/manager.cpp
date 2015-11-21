#include"manager.h"
#include"user.h"
#include<sstream>
#include"config.h"

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
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "SELECT * FROM UserInfoDatabase WHERE id = " << nID;
		stat->execute(str.str());
		std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
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
bool CManager::insertUser(IUser * pUser)
{
	return ((CUser*)pUser)->insert();
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
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "SELECT * FROM UserInfoDatabase WHERE email = " << '\''<<strName<<'\'';
			stat->execute(str.str());
			std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
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
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "SELECT * FROM UserInfoDatabase WHERE name = " << '\'' << strName << '\'';
			stat->execute(str.str());
			std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
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