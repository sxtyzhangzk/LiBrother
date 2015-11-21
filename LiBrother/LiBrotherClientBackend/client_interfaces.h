#ifndef __CLIENT_INTERFACES_H__
#define __CLIENT_INTERFACES_H__

#include "function_interfaces.h"

//声明客户端后端提供的接口和函数

/*
  授权管理器
  提供登录、注销、检查权限等功能
*/
interface IAuthManager : public IAbstract
{
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

	//获取当前的权限内容
	virtual bool getAuthItems(TAuthorization& auth) = 0;

	//获取用户协议的内容
	virtual bool getLicense(std::string& strLicense) = 0;
};

/*
  用于客户端的类工厂扩展
*/
interface IClassFactoryClient : public ILibClassFactory
{
	//获取一个IAuthManager接口对象
	virtual bool getAuthManager(IAuthManager ** ppAuthManager) = 0;
};

//获取IClassFactoryClient接口对象，用于创建其他所有的对象
bool getClassFactory(IClassFactoryClient ** ppFactory);

/*
  初始化客户端后端
  参数：	argc, argv	[in]	main函数的参数
		retcode		[out]	当不继续执行时，main函数的返回值
  返回：客户端是否应该继续执行，若为false，则直接退出程序
*/
bool initBackend(int argc, char * argv[], int& retcode);
//结束客户端，客户端退出时执行
void cleanupBackend();

#endif