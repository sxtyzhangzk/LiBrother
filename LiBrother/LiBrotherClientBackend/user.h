#ifndef __USER_H_
#define __USER_H_

#include "function_interfaces.h"
#include "common_types.h"
#include<string>

class CUser :public IUser
{
	IMPLEMENT_INTERFACE
public:	
	CUser(TUserBasicInfo *basic_info0 = nullptr);
	~CUser();
	//获取用户的基本信息
	virtual bool getBasicInfo(TUserBasicInfo& info) override;

	//设定用户的基本信息
	virtual bool setBasicInfo(const TUserBasicInfo& info) override;

	/*
	验证用户的密码
	参数：	strPWD	[in]	用户的密码，服务端调用时传入散列后的密码，客户端前端调用时传入明文密码
	*/
	virtual bool verifyPassword(const char * strPWD) override;

	/*
	设定用户的密码
	仅供管理员和服务端使用，用户自行修改密码请使用IAuthManager
	参数：	strPWD	[in]	用户的密码，服务端调用时传入散列后的密码，客户端前端调用时传入明文密码
	*/
	virtual bool setPassword(const char * strPWD) override;

	//获取已经借阅的图书
	virtual bool getBorrowedBooks(std::vector<TBorrowInfo> &binfo) override;

	//借阅一本图书
	virtual bool borrowBook(IBook * pBook) override;

	//归还一本图书
	virtual bool returnBook(IBook * pBook) override;

	//删除此用户
	virtual bool deleteUser() override;
protected:
	TUserBasicInfo *basic_info;
};

#endif