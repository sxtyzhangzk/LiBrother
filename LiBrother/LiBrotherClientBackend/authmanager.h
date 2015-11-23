#ifndef __AUTHMANAGER_H_
#define __AUTHMANAGER_H_

#include "client_interfaces.h"
#include "common_types.h"

class CUser;
class CAuthManager :public IAuthManager
{
	IMPLEMENT_INTERFACE
public:
	CAuthManager();
	~CAuthManager();

	virtual bool Login(const char * strUser, const char * strPWD) override;

	virtual bool Logout() override;

	virtual bool Register(IUser * pUser) override;

	virtual bool changePassword(const char * strOldPWD, const char * strPWD) override;

	virtual int getLoginStatus() override;

	virtual bool getCurrentUser(IUser ** ppUser) override;

	virtual int getAuthLevel() override;

	virtual bool getAuthItems(TAuthorization& auth) override;

	virtual bool getLicense(std::string& strLicense) override;

public:
	static std::string encryptPassword(const char * strPWD);
	
protected:
	CUser * m_pCurrentUser;
};

#endif