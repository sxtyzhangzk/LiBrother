#ifndef __AUTHMANAGER_H_
#define __AUTHMANAGER_H_

#include "client_interfaces.h"
#include "common_types.h"

class CAuthManager :public IAuthManager
{
public:
	//用户登录
	//参数：strUser 登录表示符，允许用户名/邮箱/用户ID; strPWD 用户密码
	virtual bool Login(const char * strUser, const char * strPWD) = 0;

	//用户注销
	virtual bool Logout() = 0;

	//注册用户
	//参数：pUser 一个IUser接口对象，表示用户的相关信息
	virtual bool Register(IUser * pUser) = 0;

	//修改密码
	//参数：strOldPWD 原密码; strPWD 新密码
	virtual bool changePassword(const char * strOldPWD, const char * strPWD) = 0;

	//获取登录状态
	//返回：1表示已登录，0表示未登录，-1表示操作出现错误
	virtual int getLoginStatus() = 0;

	//获取当前登录用户的IUser对象
	//参数：ppUser，一个指向IUser双重指针
	virtual bool getCurrentUser(IUser ** ppUser) = 0;

	//获取当前的权限等级
	//返回：权限等级，-1表示操作出现错误
	virtual int getAuthLevel() = 0;
private:
	TUserBasicInfo* current_user
};

#endif