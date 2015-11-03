#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <string>
#include <ctime>

//错误类型
enum ErrType
{
	None,
	InvalidParam, PermissionDenied,
	UnsupportedMethod, NetworkError, DatabaseError,
	Other
};

//描述一个错误
struct TErrInfo
{
	ErrType type;		//类型
	int code;			//错误代码
	const char * description;	//错误的描述[不使用时置为nullptr]
	TErrInfo(ErrType type = None, int code = 0, const char * description = nullptr) : type(type), code(code), description(description) {}
};

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
	int gender;			//性别
	std::string name;	//用户名
	std::string email;	//用户邮箱
};

//借阅信息
struct TBorrowInfo
{
	int userID;			//借阅的用户ID
	time_t borrowTime;	//借阅的时间
};

#endif