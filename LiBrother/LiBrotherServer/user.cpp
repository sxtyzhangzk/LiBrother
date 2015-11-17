#include"user.h"
#include"sstream"
#include"magicdb.h"
#include"book.h"
#include<time.h>
#include "config.h"

CUser::CUser(IDatabase * DatabaseFile)
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
	return true;
}
bool CUser::bcheck(TBorrowInfo info_to_check)
{
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
		IRecordset * UIRecordset;
		std::stringstream str;
		str << "SELECT * FROM UserInfoDatabase WHERE userID=" << m_Id;
		m_pDatabase->executeSQL(str.str().c_str(), &UIRecordset);
		UIRecordset->setData("id", m_CUBI.id);
		UIRecordset->setData("gender", m_CUBI.gender);
		UIRecordset->setData("email", m_CUBI.email);
		UIRecordset->setData("name", m_CUBI.name);
		UIRecordset->updateDatabase();	
	}
	return true;
}
bool CUser::verifyPassword(const char * strPWD)
{
	return true;
}
bool CUser::setPassword(const char * strPWD)
{
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
	IRecordset * URecordset;
	std::stringstream str;
	str << "SELECT * FROM BorrowDatabase WHERE userID=" << m_Id;
	m_pDatabase->executeSQL(str.str().c_str(), &URecordset);
	if (URecordset->getSize() == -1)	//判断记录集是否为空
	{
		setError(InvalidParam, 8, "The user has no borrow information.");
		return false;	//为空，返回false
	}
	do
	{
		TBorrowInfo Info;
		Info.userID = m_Id;
		Info.bookID = URecordset->getData("bookID");
		Info.borrowTime = URecordset->getData("borrowTime");
		Info.flag = URecordset->getData("flag");	//从数据库获取一个借阅信息
		if (!bcheck(Info))	//判断得到的信息是否合法
		{
			setError(DatabaseError, 9, "There is some wrong with our database.");
			return false;	//不合法，认为数据库异常，返回false
		}
		binfo.push_back(Info);
	} while (URecordset->nextRecord());	//合法，塞进容器并移向下一条
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
	TBookBasicInfo CBBI;
	if (!((CBook*)pBook)->getBasicInfo(CBBI))
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;
	}
	 if (!CBBI.count)
	 {
		 setError(Other, 12580, "This book has been borrowed.");
		 return false;
	 }
	 IRecordset * UIRecordset;
	 m_pDatabase->getTable("BorrowDatabase", &UIRecordset);
	 UIRecordset->addNew();
	 UIRecordset->setData("userID", m_Id);
	 UIRecordset->setData("bookID", CBBI.id);
	 UIRecordset->setData("borrowTime",time(0));
	 UIRecordset->setData("flag", 0);
	 ((CBook*)pBook)->deleteBook(1);
	 return 0;
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
	TBookBasicInfo CBBI;
	if (!((CBook*)pBook)->getBasicInfo(CBBI))
	{
		setError(InvalidParam, 12345, "This user is not valid.");
		return false;
	}
	IRecordset * UIRecordset;
	m_pDatabase->getTable("BorrowDatabase", &UIRecordset);
	UIRecordset->addNew();
	UIRecordset->setData("userID", m_Id);
	UIRecordset->setData("bookID", CBBI.id);
	UIRecordset->setData("borrowTime", time(0));
	UIRecordset->setData("flag", 1);
	((CBook*)pBook)->deleteBook(-1);
	return true;
}
bool CUser::deleteUser()
{
	if (!is_from_Database)
	{
		setError(UnsupportedMethod, 4008517, "The user does not exist in the database");
		return false;
	}
	std::stringstream str;
	str << "DELETE FROM UserInfoDatabse WHERE userID=" << m_Id;
	m_pDatabase->executeSQL(str.str().c_str(), nullptr);
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
	IRecordset * UIRecordset;
	IRecordset * temp;
	m_pDatabase->getTable("UserInfoDatabase", &UIRecordset);
	UIRecordset->addNew();
	if (!m_pDatabase->executeSQL("SELECT MAX(id) FROM UserInfoDatabase", &temp))
	{
		setError(DatabaseError, 9, "There is some wrong with our database.");
		return false;
	}
	m_Id = (int)(temp->getData("id"))+ 1;
	UIRecordset->setData("id", m_Id);
	UIRecordset->setData("gender", m_CUBI.gender);
	UIRecordset->setData("email", m_CUBI.email);
	UIRecordset->setData("name", m_CUBI.name);
	UIRecordset->setData("password", m_password);
	UIRecordset->setData("AuthLevel", g_configPolicy.nDefaultUserAuthLevel);
	UIRecordset->setData("ReadLevel", g_configPolicy.nDefaultUserReadLevel);
	UIRecordset->updateDatabase();
	return true;
}
int CUser::getAuthLevel()
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM UserinfoDatabase WHERE userID=" << m_Id;
	if (!m_pDatabase->executeSQL(str.str().c_str(), &BRecordset))
	{
		setError(DatabaseError, 9, "There is some wrong with our database.");
		return false;
	}
	return BRecordset->getData("AuthLevel");
}
int CUser::getReadLevel()
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This user is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM UserinfoDatabase WHERE userID=" << m_Id;
	if (!m_pDatabase->executeSQL(str.str().c_str(), &BRecordset))
	{
		setError(DatabaseError, 9, "There is some wrong with our database.");
		return false;
	}
	return BRecordset->getData("ReadLevel");
}
bool CUser::setAuthLevel(int nAuthLevel)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	if (nAuthLevel == -1) return false;
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM UserInfoDatabase WHERE userID=" << m_Id;
	if (!m_pDatabase->executeSQL(str.str().c_str(), &BRecordset))
	{
		setError(DatabaseError, 9, "There is some wrong with our database.");
		return false;
	}
	BRecordset->setData("AuthLevel", nAuthLevel);
	return true;
}
bool CUser::setReadLevel(int nReadLevel)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	if (nReadLevel == -1) return false;
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM UserInfoDatabase WHERE userID=" << m_Id;
	if (!m_pDatabase->executeSQL(str.str().c_str(), &BRecordset))
	{
		setError(DatabaseError, 9, "There is some wrong with our database.");
		return false;
	}
	BRecordset->setData("ReadLevel", nReadLevel);
	return true;
}