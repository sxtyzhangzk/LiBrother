#include "book.h"

#include <magicdb.h>
CBook::CBook(IDatabase * DatabaseFile)
{
	m_pDatabase = DatabaseFile;
	m_Id = -1;
	is_from_Database = 0;
}
bool CBook::getBasicInfo(TBookBasicInfo& info)
{
	if (!check(m_CBBI))
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;
	}
	info = m_CBBI;
	return true;
}
bool CBook::getDescription(std::string& description)
{
	description = m_Description;
	return true;
}
bool CBook::setBasicInfo(const TBookBasicInfo& info)
{
	if (!check(info))
	{
		setError(InvalidParam, 2, "The information is not valid.");
		return false;
	}
	if (is_from_Database && info.id != m_CBBI.id)
	{
		setError(PermissionDenied, 3 , "You have no access to the database.");
		return false;
	}
	m_CBBI = info;
	return true;
}
bool CBook::setDescription(const char * description)
{
	if (!description)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	m_Description = description;
	return true;
}
bool CBook::deleteBook()
{
	if (!is_from_Database)
	{
		setError(UnsupportedMethod, 5, "This book do not exist in the database.");
		return false;
	}
	Release();
	return true;
}
bool CBook::getBorrowInfo(std::vector<TBorrowInfo> binfo)
{

}