#ifndef __FUNCTION_INTERFACES_H__
#define __FUNCTION_INTERFACES_H__

#include "common_types.h"
#include "common_interfaces.h"

/*
  声明客户端和服务器端通用的接口
*/

/*
  图书接口
  管理单本图书
*/
interface IBook : public IAbstract
{
	//获取图书的基本信息
	virtual bool getBasicInfo(TBookBasicInfo& info) = 0;

	//获取图书的描述
	virtual bool getDescription(std::string& description) = 0;

	//设定图书的基本信息
	virtual bool setBasicInfo(const TBookBasicInfo& info) = 0;

	//设定图书的描述
	virtual bool setDescription(const char * description) = 0;

	/*
	 删除图书
	 参数：number	[in]	删除此图书的本数，-1表示全部删除
	*/
	virtual bool deleteBook(int number) = 0;

	//获取图书的借阅信息
	virtual bool getBorrowInfo(std::vector<TBorrowInfo> &binfo) = 0;

	//乡下书进城
	virtual bool insert() = 0;
};

/*
  图书馆接口
  管理图书馆中的所有图书
*/
interface ILibrary : public IAbstract
{
	/*
	  利用图书名称进行查找
	  参数：	strName	[in]	图书的名称，允许模糊查找，关键词用空格分开
			vBooks	[out]	返回检索到的图书
			nCount	[in]	用于分页：一次返回图书的数量
			nTop	[in]	用于分页：从检索到的第nTop本书开始返回图书信息
	  返回：	检索到的图书数量，-1表示操作出现错误
	*/
	virtual int queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop) = 0;

	//利用图书的ID号进行查找
	virtual bool queryById(int nID, IBook ** ppBook) = 0;

	//利用图书的ISBN号进行查找
	virtual bool queryByISBN(const char * strISBN, IBook ** ppBook) = 0;

	//插入一本图书
	virtual bool insertBook(IBook * pBook) = 0;
};

/*
  用户接口
  管理单个用户
*/
interface IUser : public IAbstract
{
	//获取用户的基本信息
	virtual bool getBasicInfo(TUserBasicInfo& info) = 0;

	//设定用户的基本信息
	virtual bool setBasicInfo(const TUserBasicInfo& info) = 0;

	/*
	  验证用户的密码
	  参数：	strPWD	[in]	用户的密码，服务端调用时传入散列后的密码，客户端前端调用时传入明文密码
	*/
	virtual bool verifyPassword(const char * strPWD) = 0;

	/*
	  设定用户的密码
	  仅供管理员和服务端使用，用户自行修改密码请使用IAuthManager
	  参数：	strPWD	[in]	用户的密码，服务端调用时传入散列后的密码，客户端前端调用时传入明文密码
	*/
	virtual bool setPassword(const char * strPWD) = 0;

	//获取已经借阅的图书
	virtual bool getBorrowedBooks(std::vector<TBorrowInfo> &binfo) = 0;

	//借阅一本图书
	virtual bool borrowBook(IBook * pBook) = 0;

	//归还一本图书
	virtual bool returnBook(IBook * pBook) = 0;

	//删除此用户
	virtual bool deleteUser() = 0;

	//乡下人进城
	virtual bool insert() = 0;
};

/*
  用户管理器接口
  管理全部的用户
*/
interface IUserManager : public IAbstract
{
	//利用用户ID获取用户
	virtual bool getUserByID(int nID, IUser ** ppUser) = 0;

	//利用用户名、Email、或者字符串形式的ID获取用户
	virtual bool getUserByName(const char * strName, IUser ** ppUser) = 0;

	//插入一个用户
	virtual bool insertUser(IUser * pUser) = 0;
};

/*
  类工厂
  用于创建其他接口的实例
*/
interface ILibClassFactory : public IAbstract
{
	//创建一个空的IBook对象，用于insertBook等操作
	virtual bool createEmptyBook(IBook ** ppBook) = 0;

	//创建一个空的IUser对象，用于insertUser等操作
	virtual bool createEmptyUser(IUser ** ppUser) = 0;

	//获取ILibrary对象
	virtual bool getLibrary(ILibrary ** ppLibrary) = 0;

	//获取IUserManager对象
	virtual bool getUserManager(IUserManager ** ppManager) = 0;
};

#endif