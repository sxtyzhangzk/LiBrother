#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include "function_interfaces.h"
#include "common_types.h"
#include <memory>
#include <driver/mysql_public_iface.h>

class CConnectionPool;
class CBook;
class CLibrary : public ILibrary
{
	IMPLEMENT_INTERFACE
public:
	CLibrary(CConnectionPool * DatabaseFile);
	virtual int queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop) override;
	virtual bool queryById(int nID, IBook ** ppBook) override;
	virtual bool queryByISBN(const char * strISBN, IBook ** ppBook) override;
	virtual bool insertBook(IBook * pBook) override;
	~CLibrary();

protected:
	void readBookInfo(std::shared_ptr<sql::ResultSet> pResultSet, CBook * pBook);

protected:
	CConnectionPool * m_pDatabase;
};
#endif
