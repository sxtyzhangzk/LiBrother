#include "config.h"
#include "library.h"
#include "book.h"
#include "connection_pool.h"

#include <sstream>
#include <memory>

#include <liblog.h>

MODULE_LOG_NAME("CLibrary");

//TODO: SQL Injection Defense
//TODO: Read Book Description (In CBook)

static const char * strSelectSQL = 
	"Select id, name, author, publisher, ISBN, count, bcount, ReadLevel From BookBasicInfo ";

CLibrary::CLibrary(CConnectionPool *  DatabaseFile)
{
	m_pDatabase = DatabaseFile;
}
CLibrary::~CLibrary()
{

}

int CLibrary::queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop)
{
	if (!strName)
	{
		setError(InvalidParam, 4, "Null Pointer");
		return false;
	}

	int ret = 0;

	sql::Connection * pConn = m_pDatabase->getConnection(REGID_MYSQL_CONN);
	sql::Connection * pConnSphinx = nullptr;
	if (g_configSvr.nSphinxType)
		pConnSphinx = m_pDatabase->getConnection(REGID_SPHINX_CONN);

	std::stringstream strSQL;
	CBook * pBook = new CBook(m_pDatabase);
	try
	{
		std::shared_ptr<sql::Statement> pStat(pConn->createStatement());
		
		strSQL << strSelectSQL << "Where name='" << strName << "'";
		std::shared_ptr<sql::ResultSet> pResult(pStat->executeQuery(strSQL.str()));
		while (pResult->next())
		{
			readBookInfo(pResult, pBook);
			pBook->sign();
			vBooks.push_back(pBook);
			ret++;
			pBook = new CBook(m_pDatabase);
		}

		if (pConnSphinx)
		{
			std::shared_ptr<sql::Statement> pStatSphinx(pConnSphinx->createStatement());
			strSQL.clear();
			strSQL << "Select * From book Where Match('" << strName << "')";
			std::shared_ptr<sql::ResultSet> pResultSphinx(pStatSphinx->executeQuery(strSQL.str()));
			while (pResult->next())
			{
				int nID = pResultSphinx->getInt("id");
				strSQL.clear();
				strSQL << strSelectSQL << "Where id=" << nID;
				std::shared_ptr<sql::ResultSet> pResultID(pStat->executeQuery(strSQL.str()));
				if (pResultID->next())
				{
					readBookInfo(pResult, pBook);
					pBook->sign();
					vBooks.push_back(pBook);
					ret++;
					pBook = new CBook(m_pDatabase);
				}
			}
		}
	}
	catch (sql::SQLException& e)
	{
		std::string strError = std::string("An error occurred while searching books: ") + e.what();
		setError(DatabaseError, 9, strError.c_str());
		ret = -1;
		lprintf_e("%s", strError.c_str());
	}

	lprintf("queryByName called, name='%s', got %d result(s).", strName, ret);
	
	delete pBook;
	if (pConn)
		m_pDatabase->releaseConnection(REGID_MYSQL_CONN, pConn);
	if (pConnSphinx)
		m_pDatabase->releaseConnection(REGID_SPHINX_CONN, pConnSphinx);
	return ret;
}

bool CLibrary::queryById(int nID, IBook ** ppBook)
{
	if (!ppBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	bool ret = false;

	sql::Connection *c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
	CBook *pBook = new CBook(m_pDatabase);

	try
	{
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << strSelectSQL << "WHERE id = " << nID;
		std::shared_ptr<sql::ResultSet> result(stat->executeQuery(str.str()));
		if (result->next())
		{
			readBookInfo(result, pBook);
			pBook->sign();
			pBook->AddRef();
			*ppBook = pBook;
			pBook = nullptr;
			ret = true;
		}
	}
	catch (sql::SQLException& e)
	{
		std::string strError = std::string("An error occurred while searching books by id: ") + e.what();
		setError(DatabaseError, 9, strError.c_str());
		ret = false;
		lprintf_e("%s", strError.c_str());
	}

	lprintf("queryByID called, id=%d, got %d result(s)", nID, ret);

	if (pBook)
		delete pBook;
	m_pDatabase->releaseConnection(REGID_MYSQL_CONN, c);
	return ret;
}

bool CLibrary::queryByISBN(const char * strISBN, IBook ** ppBook)
{
	if (!strISBN || !ppBook)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	
	bool ret = false;
	sql::Connection *c = m_pDatabase->getConnection(REGID_MYSQL_CONN);
	CBook *pBook = new CBook(m_pDatabase);

	try
	{
		std::shared_ptr<sql::Statement> stat(c->createStatement());
		std::stringstream str;
		str << strSelectSQL << "WHERE isbn='" << strISBN << "'";
		std::shared_ptr<sql::ResultSet> result(stat->executeQuery(str.str()));
		if (result->next())
		{
			readBookInfo(result, pBook);
			pBook->sign();
			pBook->AddRef();
			*ppBook = pBook;
			pBook = nullptr;
			ret = true;
		}
	}
	catch (sql::SQLException& e)
	{
		std::string strError = std::string("An error occurred while searching by ISBN: ") + e.what();
		setError(DatabaseError, 9, strError.c_str());
		ret = false;
		lprintf_e("%s", strError);
	}

	lprintf("queryByISBN called, isbn='%s', got %d results", strISBN, ret);

	if (pBook)
		delete pBook;
	m_pDatabase->releaseConnection(REGID_MYSQL_CONN, c);
	return true;
}

bool CLibrary::insertBook(IBook * pBook)
{
	return ((CBook*)pBook)->insert();
}

void CLibrary::readBookInfo(std::shared_ptr<sql::ResultSet> pResultSet, CBook * pBook)
{
	TBookBasicInfo binfo;
	binfo.id = pResultSet->getInt("id");
	binfo.name = pResultSet->getString("name");
	binfo.isbn = pResultSet->getString("isbn");
	binfo.author = pResultSet->getString("author");
	binfo.publisher = pResultSet->getString("publisher");
	binfo.count = pResultSet->getInt("count");
	binfo.bcount = pResultSet->getInt("bcount");
	pBook->setBasicInfo(binfo);
	pBook->setBookReadLevel(pResultSet->getInt("ReadLevel"));
}