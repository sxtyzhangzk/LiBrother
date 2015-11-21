#include"user.h"
#include"sstream"
#include"book.h"
#include<time.h>
#include "config.h"
#include<botan\bcrypt.h>
#include<botan\auto_rng.h>
CUser::CUser(CConnectionPool * DatabaseFile)
{
	m_pDatabase = DatabaseFile;
	m_Id = -1;
	is_from_Database = 0;
	m_CUBI.id = -1;
	m_CUBI.gender = -1;
}
CUser::~CUser()
{

}
bool CUser::check(TUserBasicInfo info_to_check)
{
	if (info_to_check.id == -1 || info_to_check.gender==-1) return false;
	if (info_to_check.email.empty() || info_to_check.name.empty()) return false;
	return true;
}
bool CUser::bcheck(TBorrowInfo info_to_check)
{
	if (info_to_check.bookID == -1 || info_to_check.userID == -1) return false;
	return true;
}
bool CUser::getBasicInfo(TUserBasicInfo& info)
{
	if (!check(m_CUBI) || !is_from_Database)	
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;	
	}
	info = m_CUBI;	
	return true;
}
bool CUser::setBasicInfo(const TUserBasicInfo& info)
{
	if (!check(info))
	{
		setError(InvalidParam, 2, "The information is not valid.");
		return false;	
	}
	if (is_from_Database && info.id != m_CUBI.id)	
	{
		setError(PermissionDenied, 3, "You have no access to the database.");
		return false;	
	}
	m_CUBI = info;	
	if (is_from_Database)
	{
		try
		{
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "UPDATE UserInfoDatabase SET name = " << '\'' << info.name << '\'' << ", " <<
				"email = " << '\'' << info.email << '\'' << ", " <<
				"gender = " << info.gender <<
				" WHERE id = " << m_Id;
			stat->execute(str.str());
		}
		catch (sql::SQLException& e)
		{
			setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
			return false;
		}
	}
	return true;
}
bool CUser::verifyPassword(const char * strPWD)
{
	if (!strPWD)
	{
		setError(InvalidParam, 4, "The pointer is null.");
		return false;
	}
	if (!is_from_Database)
	{
		setError(InvalidParam, 2, "This user is not valid.");
		return false;
	}
	std::string password;
	password = m_password;
	return Botan::check_bcrypt(strPWD, password);
}
bool CUser::setPassword(const char * strPWD)
{
	if (!strPWD)
	{
		setError(InvalidParam, 4, "The pointer is null.");
		return false;
	}
	Botan::AutoSeeded_RNG rng;
	m_password = Botan::generate_bcrypt(strPWD, rng, 12);
	if (is_from_Database)
	{
		try
		{
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "UPDATE UserInfoDatabase SET password = " << '\'' << m_password << '\'' << " WHERE userID=" << m_Id;
			stat->execute(str.str());
			return true;
		}
		catch (sql::SQLException& e)
		{
			setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
			return false;
		}
	}
	return true;
}
bool CUser::getBorrowedBooks(std::vector<TBorrowInfo> &binfo)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	binfo.clear();
	sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
	std::shared_ptr<sql::Statement> stat(c->createStatement());
	std::stringstream str;
	str << "SELECT * FROM BorrowDatabase WHERE userID=" << m_Id;
	stat->execute(str.str());
	std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
	try
	{
		while (result->next())
		{
			TBorrowInfo Info;
			Info.userID = m_Id;
			Info.bookID = result->getInt("bookID");
			Info.borrowTime = result->getInt64("borrowTime");
			Info.flag = result->getBoolean("flag");	//从数据库获取一个借阅信息
			if (!bcheck(Info))	//判断得到的信息是否合法
			{
				setError(DatabaseError, 9, "There is some wrong with our database.");
				return false;	//不合法，认为数据库异常，返回false
			}
			binfo.push_back(Info);
		}	//合法，塞进容器并移向下一条
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CUser::borrowBook(IBook * pBook)
{
	if (!pBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	if (!is_from_Database)
	{
		setError(PermissionDenied, 1258011, "The user has no permission.");
		return false;
	}
	if (((CBook*)pBook)->borrow(1))
	{
		try
		{
			TBookBasicInfo info;
			((CBook*)pBook)->getBasicInfo(info);
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "INSERT INTO BorrowDatabase(id) VALUES ("<<info.id << " ," << m_Id << " ," << time(0) << " ," << 0<<")";
			stat->execute(str.str());
			return true;
		}
		catch (sql::SQLException& e)
		{
			((CBook*)pBook)->borrow(-1);
			setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
			return false;
		}
	}
	return true;
}
bool CUser::returnBook(IBook * pBook)
{
	if (!pBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	if (!is_from_Database)
	{
		setError(PermissionDenied, 1258011, "The user has no permission.");
		return false;
	}
	if (((CBook*)pBook)->borrow(-1))
	{
		try
		{
			TBookBasicInfo info;
			((CBook*)pBook)->getBasicInfo(info);
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "INSERT INTO BorrowDatabase(id) VALUES (" << info.id << " ," << m_Id << " ," << time(0) << " ," << 1 << ")";
			stat->execute(str.str());
			return true;
		}
		catch (sql::SQLException& e)
		{
			((CBook*)pBook)->borrow(1);
			setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
			return false;
		}
	}
	return true;
}
bool CUser::deleteUser()
{
	if (!is_from_Database)
	{
		setError(UnsupportedMethod, 4008517, "The user does not exist in the database");
		return false;
	}
	try 
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "DELETE FROM UserInfoDatabse WHERE userID=" << m_Id;
		stat->execute(str.str());
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CUser::sign()
{
	if (!check(m_CUBI))
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;
	}
	is_from_Database = 1;
	m_Id = m_CUBI.id;
	return true;
}
bool CUser::insert()
{
	if (is_from_Database)
	{
		setError(InvalidParam, 11111, "Only new users can be inserted.");
		return false;
	}
	if (!check(m_CUBI))
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "INSERT INTO UserInfoDatabase(id) VALUES (null)";
		stat->execute(str.str());
		sign();
		setBasicInfo(m_CUBI);
		setAuthLevel(g_configPolicy.nDefaultUserAuthLevel);
		setReadLevel(g_configPolicy.nDefaultUserReadLevel);
		str << "UPDATE UserInfoDatabase password = " << '\'' << m_password << '\'' << " WHERE id=" << m_Id;
		stat->execute(str.str());
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
int CUser::getAuthLevel()
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "SELECT AuthLevel FROM UserInfoDatabase WHERE userID = " << m_Id;
		stat->execute(str.str());
		std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
		return result->getInt("AuthLevel");
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
int CUser::getReadLevel()
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "SELECT ReadLevel FROM UserInfoDatabase WHERE userID = " << m_Id;
		stat->execute(str.str());
		std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
		return result->getInt("ReadLevel");
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CUser::setAuthLevel(int nAuthLevel)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "UPDATE FROM UserInfoDatabase AuthLevel = "<<nAuthLevel<<" WHERE userID = " << m_Id;
		stat->execute(str.str());
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CUser::setReadLevel(int nReadLevel)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "UPDATE FROM UserInfoDatabase ReadLevel = " << nReadLevel << " WHERE userID = " << m_Id;
		stat->execute(str.str());
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CUser::straightsetpassword(const char * strPWD)
{
	m_password = strPWD;
	return true;
}