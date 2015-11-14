#include "library.h"
#include "sstream"
#include "magicdb.h"
#include "book.h"
//#include "book.h"

Clibrary::Clibrary(IDatabase * DatabaseFile)
{
	m_pDatabase = DatabaseFile;
}
Clibrary::~Clibrary()
{

}
int Clibrary::queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop)
{
	IRecordset * BRecordset;
	if (!m_pDatabase->getTable("BookInfoDatabase", &BRecordset))
	{
		setError(DatabaseError, 9, "There is some wrong with our database.");
		return false;
	}
	int flag = nTop-1;
	while (BRecordset->findNext("name", Clike, strName) && flag)
		flag--;
	if (flag)
	{
		setError(InvalidParam, 12, "There's no so many books.");
		return false;
	}
	int count = 0;
	while (count<nCount && BRecordset->findNext("name", Clike, strName))
	{
		CBook * ppBook = new CBook(m_pDatabase);
		TBookBasicInfo Basicinfo;
		Basicinfo.author = std::string(BRecordset->getData("author"));
		Basicinfo.count = BRecordset->getData("count");
		Basicinfo.id = BRecordset->getData("id");
		Basicinfo.isbn = std::string(BRecordset->getData("ISBN"));
		Basicinfo.name = std::string(BRecordset->getData("name"));
		Basicinfo.publisher = std::string(BRecordset->getData("publisher"));
		ppBook->setBasicInfo(Basicinfo);
		ppBook->setDescription(std::string(BRecordset->getData("description")).c_str());
		ppBook->sign();
		vBooks.push_back(ppBook);
		count++;
	}
	return count;
}
bool Clibrary::queryById(int nID, IBook ** ppBook)
{
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM BookInfoDatabase WHERE id=" << nID;
	if (!m_pDatabase->executeSQL(str.str().c_str(), &BRecordset))
	{
		setError(InvalidParam, 11, "No this book.");
		return false;
	}
	TBookBasicInfo Basicinfo;
	Basicinfo.author = std::string(BRecordset->getData("author"));
	Basicinfo.count= BRecordset->getData("count");
	Basicinfo.id = BRecordset->getData("id");
	Basicinfo.isbn = std::string(BRecordset->getData("ISBN"));
	Basicinfo.name= std::string(BRecordset->getData("name"));
	Basicinfo.publisher= std::string(BRecordset->getData("publisher"));
	(*ppBook)->setBasicInfo(Basicinfo);
	(*ppBook)->setDescription(std::string(BRecordset->getData("description")).c_str());
	(*ppBook)->sign();
	return true;
}
bool Clibrary::queryByISBN(const char * strISBN, IBook ** ppBook)
{
	IRecordset * BRecordset;
	std::stringstream str;
	str << "SELECT * FROM BookInfoDatabase WHERE ISBN=" << '"'<<strISBN<<'"';
	if (!m_pDatabase->executeSQL(str.str().c_str(), &BRecordset))
	{
		setError(InvalidParam, 11, "No this book.");
		return false;
	}
	TBookBasicInfo Basicinfo;
	Basicinfo.author = std::string(BRecordset->getData("author"));
	Basicinfo.count = BRecordset->getData("count");
	Basicinfo.id = BRecordset->getData("id");
	Basicinfo.isbn = std::string(BRecordset->getData("ISBN"));
	Basicinfo.name = std::string(BRecordset->getData("name"));
	Basicinfo.publisher = std::string(BRecordset->getData("publisher"));
	(*ppBook)->setBasicInfo(Basicinfo);
	(*ppBook)->setDescription(std::string(BRecordset->getData("description")).c_str());
	(*ppBook)->sign();
	return true;
}
bool Clibrary::insertBook(IBook * pBook)
{
	return pBook->insert();
}