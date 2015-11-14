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

#endif