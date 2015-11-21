#include "book.h"
#include "sstream"
#include "config.h"

// ˳��id name author publisher ISBN count
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
	if (!check(m_CBBI) || !is_from_Database)	//����鱾������Ϣ�Ƿ�Ϸ��Լ��Ƿ��������ݿ�
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//���Ϸ�������false
	}
	info = m_CBBI;	//�Ϸ������鱾������Ϣ����info
	return true;
}
bool CBook::getDescription(std::string& description)
{
	description = m_Description;	//�������ܲ��Ϸ���ֱ�Ӹ�ֵ
	return true;
}
bool CBook::setBasicInfo(const TBookBasicInfo& info)	
{
	if (!check(info))	//�жϸ���Ļ�����Ϣ�Ƿ�Ϸ�
	{
		setError(InvalidParam, 2, "The information is not valid.");
		return false;	//���Ϸ�������false
	}
	if (is_from_Database && info.id != m_CBBI.id)	//�ж��Ƿ��ڳ����޸����ݿ��ڵ��鱾ID
	{
		setError(PermissionDenied, 3 , "You have no access to the database.");
		return false;	//������������false
	}
	m_CBBI = info;	//�����Ϸ�����info�����鱾������Ϣ
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
	if (!description)	//�ж�ָ���Ƿ�Ϊ��
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;	//Ϊ�գ�����false
	}
	m_Description = description;	//��Ϊ�գ���description������Ľ���
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
	if (!is_from_Database)	//�ж��Ƿ��������ݿ�
	{
		setError(UnsupportedMethod, 5, "This book do not exist in the database.");
		return false;	//�����������ݿ���飬����ɾ��������false
	}
	if ((m_CBBI.count -m_CBBI.bcount)< number)	//�ж�ɾȥ���鱾��Ŀ�Ƿ�Ϸ�
	{
		setError(InvalidParam, 6, "The number of this book is not enough.");
		return false;	//ɾ��̫�࣬����false
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
	if (!is_from_Database)	//�ж��Ƿ��������ݿ�
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//�����������ݿ���飬���ɽ��ģ�����false
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
			Info.flag = result->getBoolean("flag");	//�����ݿ��ȡһ��������Ϣ
			if (!bcheck(Info))	//�жϵõ�����Ϣ�Ƿ�Ϸ�
			{
				setError(DatabaseError, 9, "There is some wrong with our database.");
				return false;	//���Ϸ�����Ϊ���ݿ��쳣������false
			}
			binfo.push_back(Info);
		}	//�Ϸ�������������������һ��
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
	if (!is_from_Database)	//�ж��Ƿ��������ݿ�
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//�����������ݿ���飬���ɽ��ģ�����false
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
	if (!is_from_Database)	//�ж��Ƿ��������ݿ�
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//�����������ݿ���飬���ɽ��ģ�����false
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
	if (!is_from_Database)	//�ж��Ƿ��������ݿ�
	{
		setError(UnsupportedMethod, 5, "This book do not exist in the database.");
		return false;	//�����������ݿ���飬����ɾ��������false
	}
	if (m_CBBI.count < (m_CBBI.bcount+number) || (m_CBBI.bcount+number)<0)	//�ж�ɾȥ���鱾��Ŀ�Ƿ�Ϸ�
	{
		setError(InvalidParam, 6, "The number of this book is not enough.");
		return false;	//ɾ��̫�࣬����false
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