#include "library.h"
#include "sstream"
#include "magicdb.h"
#include "book.h"

CLibrary::CLibrary(IDatabase * DatabaseFile)
{
	m_pDatabase = DatabaseFile;
}
CLibrary::~CLibrary()
{

}
int CLibrary::queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop)
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
}
bool CLibrary::queryById(int nID, IBook ** ppBook)
{
	if (!ppBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	IRecordset * BIRecordset=nullptr;
	std::stringstream str;
	str << "SELECT * FROM BookInfoDatabase WHERE id=" << nID;
	m_pDatabase->executeSQL(str.str().c_str(), &BIRecordset);
	if (!BIRecordset)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	TBookBasicInfo Basicinfo;
	Basicinfo.author = std::string(BIRecordset->getData("author"));
	Basicinfo.count= BIRecordset->getData("count");
	Basicinfo.id = BIRecordset->getData("id");
	Basicinfo.isbn = std::string(BIRecordset->getData("ISBN"));
	Basicinfo.name= std::string(BIRecordset->getData("name"));
	Basicinfo.publisher= std::string(BIRecordset->getData("publisher"));
	(*ppBook)->setBasicInfo(Basicinfo);
	(*ppBook)->setDescription(std::string(BIRecordset->getData("description")).c_str());
	((CBook*)(*ppBook))->sign();
	BIRecordset->Release();
	return true;
}
bool CLibrary::queryByISBN(const char * strISBN, IBook ** ppBook)
{
	if (!strISBN || !ppBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	IRecordset * BIRecordset=nullptr;
	std::stringstream str;
	str << "SELECT * FROM BookInfoDatabase WHERE ISBN=" << '"'<<strISBN<<'"';
	m_pDatabase->executeSQL(str.str().c_str(), &BIRecordset);
	if (!BIRecordset)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	TBookBasicInfo Basicinfo;
	Basicinfo.author = std::string(BIRecordset->getData("author"));
	Basicinfo.count = BIRecordset->getData("count");
	Basicinfo.id = BIRecordset->getData("id");
	Basicinfo.isbn = std::string(BIRecordset->getData("ISBN"));
	Basicinfo.name = std::string(BIRecordset->getData("name"));
	Basicinfo.publisher = std::string(BIRecordset->getData("publisher"));
	(*ppBook)->setBasicInfo(Basicinfo);
	(*ppBook)->setDescription(std::string(BIRecordset->getData("description")).c_str());
	((CBook*)(*ppBook))->sign();
	BIRecordset->Release();
	return true;
}
bool CLibrary::insertBook(IBook * pBook)
{
	return ((CBook*)pBook)->insert();
}