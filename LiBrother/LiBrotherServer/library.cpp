#include "library.h"
#include "sstream"
#include "book.h"
#include"config.h"
CLibrary::CLibrary(CConnectionPool *  DatabaseFile)
{
	m_pDatabase = DatabaseFile;
}
CLibrary::~CLibrary()
{

}
/*int CLibrary::queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop)			//Äã×Ô¼º¸ã¶¨°É
{
	IRecordset * BIRecordset;
	m_pDatabase->getTable("BookInfoDatabase", &BIRecordset);
	if (!BIRecordset)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	int flag = nTop-1;
	while (BIRecordset->findNext("name", Clike, strName) && flag)
		flag--;
	if (flag)
	{
		setError(InvalidParam, 12, "There's no so many books.");
		return false;
	}
	int count = 0;
	while (count<nCount && BIRecordset->findNext("name", Clike, strName))
	{
		CBook * ppBook = new CBook(m_pDatabase);
		TBookBasicInfo Basicinfo;
		Basicinfo.author = std::string(BIRecordset->getData("author"));
		Basicinfo.count = BIRecordset->getData("count");
		Basicinfo.id = BIRecordset->getData("id");
		Basicinfo.isbn = std::string(BIRecordset->getData("ISBN"));
		Basicinfo.name = std::string(BIRecordset->getData("name"));
		Basicinfo.publisher = std::string(BIRecordset->getData("publisher"));
		ppBook->setBasicInfo(Basicinfo);
		ppBook->setDescription(std::string(BIRecordset->getData("description")).c_str());
		ppBook->sign();
		vBooks.push_back(ppBook);
		count++;
	}
	BIRecordset->Release();
	return count;
}*/
bool CLibrary::queryById(int nID, IBook ** ppBook)
{
	if (!ppBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "SELECT * FROM BookInfoDatabase WHERE id = " << nID;
		stat->execute(str.str());
		std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
		TBookBasicInfo Basicinfo;
		Basicinfo.author = result->getString("author");
		Basicinfo.count = result->getInt("count");
		Basicinfo.bcount = result->getInt("bcount");
		Basicinfo.id = result->getInt("id");
		Basicinfo.isbn = result->getString("isbn");
		Basicinfo.name = result->getString("name");
		Basicinfo.publisher = result->getString("publisher");
		(*ppBook)->setBookReadLevel(result->getInt("ReadLevel"));
		(*ppBook)->setBasicInfo(Basicinfo);
		(*ppBook)->setDescription(result->getString("description").c_str());
		((CBook*)(*ppBook))->sign();
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CLibrary::queryByISBN(const char * strISBN, IBook ** ppBook)
{
	if (!strISBN || !ppBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	try
	{
		sql::Connection*  c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << "SELECT * FROM BookInfoDatabase WHERE isbn = " <<'\''<<strISBN<<'\'';
		stat->execute(str.str());
		std::shared_ptr<sql::ResultSet> result(stat->getResultSet());
		TBookBasicInfo Basicinfo;
		Basicinfo.author = result->getString("author");
		Basicinfo.count = result->getInt("count");
		Basicinfo.bcount = result->getInt("bcount");
		Basicinfo.id = result->getInt("id");
		Basicinfo.isbn = result->getString("isbn");
		Basicinfo.name = result->getString("name");
		Basicinfo.publisher = result->getString("publisher");
		(*ppBook)->setBookReadLevel(result->getInt("ReadLevel"));
		(*ppBook)->setBasicInfo(Basicinfo);
		(*ppBook)->setDescription(result->getString("description").c_str());
		((CBook*)(*ppBook))->sign();
		return true;
	}
	catch (sql::SQLException& e)
	{
		setError(DatabaseError, 9, (std::string("There is some wrong with our database.\n") + e.what()).c_str());
		return false;
	}
	return true;
}
bool CLibrary::insertBook(IBook * pBook)
{
	return ((CBook*)pBook)->insert();
}