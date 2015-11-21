#ifndef __USER_H__
#define __USER_H__
#include "function_interfaces.h"
#include "common_types.h"
#include"connection_pool.h"
#include <driver/mysql_public_iface.h>
class CUser:public IUser
{
	IMPLEMENT_INTERFACE
public:
	CUser(CConnectionPool * DatabaseFile);
	virtual bool getBasicInfo(TUserBasicInfo& info) override;
	virtual bool setBasicInfo(const TUserBasicInfo& info) override;
	virtual bool verifyPassword(const char * strPWD) override;
	virtual bool setPassword(const char * strPWD) override;
	virtual bool getBorrowedBooks(std::vector<TBorrowInfo> &binfo) override;
	virtual bool borrowBook(IBook * pBook) override;
	virtual bool returnBook(IBook * pBook) override;
	virtual bool deleteUser() override; 
	virtual int getAuthLevel() override;
	virtual int getReadLevel() override;
	virtual bool setAuthLevel(int nAuthLevel) override;
	virtual bool setReadLevel(int nReadLevel) override;
	bool sign();
	bool check(TUserBasicInfo info_to_check);
	bool bcheck(TBorrowInfo info_to_check);
	bool insert();
	bool straightsetpassword(const char * strPWD);
	~CUser();
protected:
	CConnectionPool * m_pDatabase;
	TUserBasicInfo m_CUBI;
	int m_Id;
	std::string m_password;
	bool is_from_Database;
};

#endif
