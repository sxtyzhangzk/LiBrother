#ifndef __USERMANAGER_H_
#define __USERMANAGER_H_

#include "function_interfaces.h"
#include "common_types.h"
#include <string>

class CUserManager :public IUserManager
{
	IMPLEMENT_INTERFACE
public:	
	//利用用户ID获取用户
	virtual bool getUserByID(int nID, IUser ** ppUser) override;

	//利用用户名、Email、或者字符串形式的ID获取用户
	virtual bool getUserByName(const char * strName, IUser ** ppUser) override;

	//插入一个用户
	virtual bool insertUser(IUser * pUser) override;
};
#endif