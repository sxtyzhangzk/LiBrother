#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include "function_interfaces.h"
#include "common_types.h"
interface IDatabase;
class CLibrary : public ILibrary
{
	IMPLEMENT_INTERFACE
public:
	CLibrary(IDatabase * DatabaseFile);
	virtual int queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop) override;
	virtual bool queryById(int nID, IBook ** ppBook) override;
	virtual bool queryByISBN(const char * strISBN, IBook ** ppBook) override;
	virtual bool insertBook(IBook * pBook) override;
	~CLibrary();
protected:
	IDatabase * m_pDatabase;
};
#endif
