#include "book.h"
#include "sstream"
#include "config.h"

// 顺序：id name author publisher ISBN count
CBook::CBook(CConnectionPool * DatabaseFile)
{
	m_pDatabase = DatabaseFile;
	m_Id = -1;
	is_from_Database = 0;
	m_CBBI.id = -1;
	m_CBBI.count = 0;
	m_CBBI.bcount = 0;
}
CBook::~CBook()
{
}
bool CBook::check(TBookBasicInfo info_to_check)
{
	if (info_to_check.id == -1 || info_to_check.count < 1) return false;
	if (info_to_check.isbn.empty() || info_to_check.name.empty() || info_to_check.publisher.empty() || info_to_check.author.empty()) return false;
	return true;
}
bool CBook::bcheck(TBorrowInfo info_to_check)
{
	if (info_to_check.bookID == -1 || info_to_check.userID == -1) return false;
	return true;
}
bool CBook::getBasicInfo(TBookBasicInfo& info)
{
	if (!check(m_CBBI) || !is_from_Database)	//检查书本基本信息是否合法以及是否来自数据库
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//不合法，返回false
	}
	info = m_CBBI;	//合法，将书本基本信息赋给info
	return true;
}
bool CBook::getDescription(std::string& description)
{
	description = m_Description;	//根本不能不合法，直接赋值
	return true;
}
bool CBook::setBasicInfo(const TBookBasicInfo& info)	
{
	if (!check(info))	//判断给予的基本信息是否合法
	{
		setError(InvalidParam, 2, "The information is not valid.");
		return false;	//不合法，返回false
	}
	if (is_from_Database && info.id != m_CBBI.id)	//判断是否在尝试修改数据库内的书本ID
	{
		setError(PermissionDenied, 3 , "You have no access to the database.");
		return false;	//在作死，返回false
	}
	m_CBBI = info;	//操作合法，将info赋给书本基本信息
	if (is_from_Database)
	{
		try
		{
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "UPDATE BookInfoDatabase SET name = "<<'\''<<info.name<<'\''<<", "<<
												"author = "<<'\''<<info.author<<'\''<< ", " <<
												"publisher = " << '\'' << info.publisher << '\'' << ", " <<
												"ISBN = " << '\'' << info.isbn << '\'' << ", " <<
												"count = "<<info.count<<", "<<
												"bcount = "<<info.bcount<<
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
bool CBook::setDescription(const char * description)
{
	if (!description)	//判断指针是否为空
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;	//为空，返回false
	}
	m_Description = description;	//不为空，将description赋给书的介绍
	if (is_from_Database)
	{
		try
		{
			sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
			std::shared_ptr<sql::Statement> stat(c->createStatement());
			std::stringstream str;
			str << "UPDATE BookInfoDatabase SET description = " << '\'' << description << '\'' << " WHERE id=" << m_Id;
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
bool CBook::deleteBook(int number)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(UnsupportedMethod, 5, "This book do not exist in the database.");
		return false;	//不是来自数据库的书，不可删除，返回false
	}
	if ((m_CBBI.count -m_CBBI.bcount)< number)	//判断删去的书本数目是否合法
	{
		setError(InvalidParam, 6, "The number of this book is not enough.");
		return false;	//删的太多，返回false
	}
	m_CBBI.count -= number;
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		if (m_CBBI.count)
		{
			str << "UPDATE BookInfoDatabase SET count=" << m_CBBI.count << " WHERE id=" << m_Id;
			stat->execute(str.str());
			return true;
		}
		else
		{
			str << "DELETE FROM BookInfoDatabase where id=" << m_Id;
			stat->execute(str.str());
			return true;
		}
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CBook::getBorrowInfo(std::vector<TBorrowInfo> &binfo)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	binfo.clear();
	sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
	std::shared_ptr<sql::Statement> stat(c->createStatement());
	std::stringstream str;
	str << "SELECT * FROM BorrowDatabase WHERE bookID=" << m_Id;
	stat->execute(str.str());
	std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
	try
	{
		while (result->next())
		{
			TBorrowInfo Info;
			Info.bookID = m_Id;
			Info.userID = result->getInt("userID");
			Info.borrowTime =result->getInt64("borrowTime");
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
int CBook::getBookReadLevel()
{
	int r;
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
		str << "SELECT ReadLevel FROM BookInfoDatabase WHERE bookID=" << m_Id;
		stat->execute(str.str());
	
		std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
		r = result->getInt("Readlevel");
		return r;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CBook::setBookReadLevel(int nReadLevel)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	if (nReadLevel == -1) return false;
	try 
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "UPDATE BookInfoDatabase SET ReadLevel = " << nReadLevel << " WHERE bookID=" << m_Id;
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

bool CBook::insert()
{
	if (is_from_Database)
	{
		setError(InvalidParam, 10, "Only new books can be inserted.");
		return false;
	}
	if (!check(m_CBBI))
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "INSERT INTO BookInfoDatabase(id) VALUES (null)";
		sign();
		setBasicInfo(m_CBBI);
		setDescription(m_Description.c_str());
		setBookReadLevel(g_configPolicy.nDefaultBookReadLevel);
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CBook::sign()
{
	if (!check(m_CBBI))
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;
	}
	is_from_Database = 1;
	m_Id = m_CBBI.id;
	return true;
}
bool CBook::borrow(int number)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(UnsupportedMethod, 5, "This book do not exist in the database.");
		return false;	//不是来自数据库的书，不可删除，返回false
	}
	if (m_CBBI.count < (m_CBBI.bcount+number) || (m_CBBI.bcount+number)<0)	//判断删去的书本数目是否合法
	{
		setError(InvalidParam, 6, "The number of this book is not enough.");
		return false;	//删的太多，返回false
	}
	m_CBBI.bcount += number;
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "UPDATE BookInfoDatabase SET bcount = " << m_CBBI.bcount << " WHERE id=" << m_Id;
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