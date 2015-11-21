#ifndef __BOOK_H__
#define __BOOK_H__

#include "function_interfaces.h"
#include "common_types.h"
#include"connection_pool.h"
#include <driver/mysql_public_iface.h>
class CBook : public IBook
{
	IMPLEMENT_INTERFACE
public:
	CBook(CConnectionPool * DatabaseFile);
	virtual bool getBasicInfo(TBookBasicInfo& info) override;
	virtual bool getDescription(std::string& description) override;
	virtual bool setBasicInfo(const TBookBasicInfo& info) override;
	virtual bool setDescription(const char * description) override;
	virtual bool deleteBook(int number) override;
	virtual bool getBorrowInfo(std::vector<TBorrowInfo> &binfo) override; 
	virtual int getBookReadLevel() override;
	virtual bool setBookReadLevel(int nReadLevel) override;
	bool insert();
	bool sign();
	bool check(TBookBasicInfo info_to_check);
	bool bcheck(TBorrowInfo info_to_check);
	bool borrow(int number);
	~CBook();
protected:
	CConnectionPool * m_pDatabase;
	TBookBasicInfo m_CBBI;
	std::string m_Description;
	int is_from_Database;
	int m_Id;
};

#endif