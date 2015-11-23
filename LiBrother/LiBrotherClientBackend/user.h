#ifndef __USER_H_
#define __USER_H_

#include "function_interfaces.h"
#include "common_types.h"
#include <string>

class CAuthManager;
class CUserManager;
class CUser :public IUser
{
	IMPLEMENT_INTERFACE
public:	
	friend CAuthManager;
	friend CUserManager;
	CUser();
	CUser(int nID);
	CUser(const TUserBasicInfo& info);
	~CUser();

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

protected:
	TUserBasicInfo *m_pBasicInfo;
	std::string m_strEPassword;
	int m_nID;
	int m_nAuthLevel, m_nReadLevel;
};

#endif