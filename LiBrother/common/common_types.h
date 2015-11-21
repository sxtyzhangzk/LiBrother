#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <string>
#include <ctime>

//图书的基本信息
struct TBookBasicInfo
{
	int id;		//图书ID
	int count;	//在馆数量
	std::string name;	//图书名称
	std::string author;	//图书作者
	std::string isbn;	//ISBN号
	std::string publisher;	//出版社
};

//用户的基本信息
struct TUserBasicInfo
{
	bool LoginStatus;   //True=登入,False=未登入
	int id;				//用户ID
	int gender;			//性别 male:0
	std::string name;	//用户名
	std::string email;	//用户邮箱
};

//借阅信息
struct TBorrowInfo
{
	int bookID;			//借阅的书ID
	int userID;			//借阅的用户ID
	time_t borrowTime;	//借阅的时间
	bool flag;		//表示是借出还是还入，0表示借出，1表示还入
};

struct TAuthorization
{
	bool auth_Login;			//登录
	bool auth_GetUserInfo;		//获取其他用户的信息
	bool auth_SetUserInfo;		//设置其他用户的信息（备注：无权设置比自己权限等级高的用户的信息）
	bool auth_ChangePassword;	//更改其他用户的密码（备注：无权修改比自己权限等级高的用户的密码）
	bool auth_DeleteUser;		//删除一个用户
	bool auth_InsertUser;		//新增一个用户
	bool auth_BorrowBook;		//借阅图书
	bool auth_ReturnBook;		//归还图书
	bool auth_GetBookInfo;		//获取图书的信息
	bool auth_SetBookInfo;		//设置图书的信息
	bool auth_InsertBook;		//插入图书
	bool auth_DeleteBook;		//删除图书
	bool auth_SetAuthLevel;		//更改其他用户权限等级（备注：不得更改自身的权限，不得更改高于自己权限等级的用户）
	bool auth_SetReadLevel;		//更改其他用户的阅读权限
};

#endif