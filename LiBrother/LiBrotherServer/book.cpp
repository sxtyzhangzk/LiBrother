#include "book.h"
#include "sstream"
#include "magicdb.h"
CBook::CBook(IDatabase * DatabaseFile)
{
	m_pDatabase = DatabaseFile;
	m_Id = -1;
	is_from_Database = 0;
	m_CBBI.id = -1;
	m_CBBI.count = 0;
}
CBook::~CBook()
{
	Release();
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
bool CBook::setBasicInfo(const TBookBasicInfo& info)	//还未完成
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
	return true;
}
bool CBook::setDescription(const char * description)	//还未完成
{
	if (!description)	//判断指针是否为空
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;	//为空，返回false
	}
	m_Description = description;	//不为空，将description赋给书的介绍
	return true;
}
bool CBook::deleteBook(int number)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(UnsupportedMethod, 5, "This book do not exist in the database.");
		return false;	//不是来自数据库的书，不可删除，返回false
	}
	if (m_CBBI.count < number)	//判断删去的书本数目是否合法
	{
		setError(InvalidParam, 6, "The number of this book is not enough.");
		return false;	//删的太多，返回false
	}
	m_CBBI.count -= number;
	IRecordset * BIRecordset;
	std::stringstream str;
	str << "SELECT * FROM BookInfoDatabase WHERE bookID=" << m_Id;
	m_pDatabase->executeSQL(str.str().c_str(), &BIRecordset);
	BIRecordset->setData("count", m_CBBI.count);
	BIRecordset->updateDatabase();	//赋值操作
	return true;
}
bool CBook::getBorrowInfo(std::vector<TBorrowInfo> &binfo)
{
	if (!is_from_Database)	//判断是否来自数据库
	{
		setError(InvalidParam, 7, "This book is not in the library.");
		return false;	//不是来自数据库的书，不可借阅，返回false
	}
	binfo.clear();
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM BorrowDatabase WHERE bookID=" << m_Id;
	m_pDatabase->executeSQL(str.str().c_str(),&BRecordset);
	if (BRecordset->getSize() == -1)	//判断记录集是否为空
	{
		setError(InvalidParam, 8, "The book has no borrow information.");
		return false;	//为空，返回false
	}
	do
	{
		TBorrowInfo Info;
		Info.bookID = m_Id;
		Info.userID = BRecordset->getData("userID");
		Info.borrowTime = BRecordset->getData("borrowTime");
		Info.flag = BRecordset->getData("flag");	//从数据库获取一个借阅信息
		if (!bcheck(Info))	//判断得到的信息是否合法
		{
			setError(DatabaseError, 9, "There is some wrong with our database.");
			return false;	//不合法，认为数据库异常，返回false
		}
		binfo.push_back(Info);
	} while (BRecordset->nextRecord());	//合法，塞进容器并移向下一条
	return true;
}
