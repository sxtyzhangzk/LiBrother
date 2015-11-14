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
}
bool CBook::check(TBookBasicInfo info_to_check)
{
	return true;
}
bool CBook::bcheck(TBorrowInfo info_to_check)
{
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
		IRecordset * BIRecordset;
		std::stringstream str;
		str << "SELECT * FROM BookInfoDatabase WHERE bookID=" << m_Id;
		m_pDatabase->executeSQL(str.str().c_str(), &BIRecordset);
		BIRecordset->setData("count", m_CBBI.count);
		BIRecordset->setData("name", m_CBBI.name);
		BIRecordset->setData("author", m_CBBI.author);
		BIRecordset->setData("publisher", m_CBBI.publisher);
		BIRecordset->setData("ISBN", m_CBBI.isbn);
		BIRecordset->updateDatabase();	//��ֵ����
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
		IRecordset * BIRecordset;
		std::stringstream str;
		str << "SELECT discription FROM BookInfoDatabase WHERE bookID=" << m_Id;
		m_pDatabase->executeSQL(str.str().c_str(), &BIRecordset);
		BIRecordset->setData("discription", m_Description);
		BIRecordset->updateDatabase();	//��ֵ����
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
	if (m_CBBI.count < number)	//�ж�ɾȥ���鱾��Ŀ�Ƿ�Ϸ�
	{
		setError(InvalidParam, 6, "The number of this book is not enough.");
		return false;	//ɾ��̫�࣬����false
	}
	m_CBBI.count -= number;
	IRecordset * BIRecordset;
	std::stringstream str;
	if (m_CBBI.count)
	{
		str << "SELECT * FROM BookInfoDatabase WHERE bookID=" << m_Id;
		m_pDatabase->executeSQL(str.str().c_str(), &BIRecordset);
		BIRecordset->setData("count", m_CBBI.count);
		BIRecordset->updateDatabase();	//��ֵ����
		return true;
	}
	else
	{
		str << "DELETE FROM BookInfoDatabse WHERE bookID=" << m_Id;
		m_pDatabase->executeSQL(str.str().c_str(), nullptr);
		return true;
	}
}
bool CBook::getBorrowInfo(std::vector<TBorrowInfo> &binfo)
{
	if (!is_from_Database)	//�ж��Ƿ��������ݿ�
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;	//�����������ݿ���飬���ɽ��ģ�����false
	}
	binfo.clear();
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM BorrowDatabase WHERE bookID=" << m_Id;
	m_pDatabase->executeSQL(str.str().c_str(),&BRecordset);
	if (BRecordset->getSize() == -1)	//�жϼ�¼���Ƿ�Ϊ��
	{
		setError(InvalidParam, 8, "The book has no borrow information.");
		return false;	//Ϊ�գ�����false
	}
	do
	{
		TBorrowInfo Info;
		Info.bookID = m_Id;
		Info.userID = BRecordset->getData("userID");
		Info.borrowTime = BRecordset->getData("borrowTime");
		Info.flag = BRecordset->getData("flag");	//�����ݿ��ȡһ��������Ϣ
		if (!bcheck(Info))	//�жϵõ�����Ϣ�Ƿ�Ϸ�
		{
			setError(DatabaseError, 9, "There is some wrong with our database.");
			return false;	//���Ϸ�����Ϊ���ݿ��쳣������false
		}
		binfo.push_back(Info);
	} while (BRecordset->nextRecord());	//�Ϸ�������������������һ��
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
	IRecordset * BIRecordset;
	m_pDatabase->getTable("BookInfoDatabase", &BIRecordset);
	BIRecordset->addNew();
	m_Id = m_pDatabase->executeSQL("SELECT MAX(id) FROM BookInfoDatabase", nullptr);
	BIRecordset->setData("id", m_Id);
	BIRecordset->setData("count", m_CBBI.count);
	BIRecordset->setData("name", m_CBBI.name);
	BIRecordset->setData("author", m_CBBI.author);
	BIRecordset->setData("publisher", m_CBBI.publisher);
	BIRecordset->setData("ISBN", m_CBBI.isbn);
	BIRecordset->setData("discription", m_Description);
	BIRecordset->updateDatabase();	//��ֵ����
	return true;
}

