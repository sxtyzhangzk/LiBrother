#include "user.h"
#include "netclient.h"
#include <json/json.h>
#include "book.h"

CUser::CUser(TUserBasicInfo *basic_info0) :basic_info(basic_info0) {}
CUser::~CUser()
{
	if (basic_info) delete basic_info;
}

//获取用户的基本信息
bool CUser::getBasicInfo(TUserBasicInfo& info)
{
	if (basic_info) {
		info = *basic_info;
		return true;
	}
	return false;
	
}

bool CUser::setBasicInfo(const TUserBasicInfo& info)
{
	Json::Value value0;
	value0["command"] = "user_setBasicInfo";
	value0["id"] = basic_info->id;
	value0["gender"] = basic_info->gender;
	value0["name"] = basic_info->name;
	value0["email"] = basic_info->email;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) return true;
	return false;
}

/*
验证用户的密码
参数：	strPWD	[in]	用户的密码，服务端调用时传入散列后的密码，客户端前端调用时传入明文密码
*/
bool CUser::verifyPassword(const char * strPWD)
{
	Json::Value value0;
	value0["command"] = "user_verifyPassword";
	value0["id"] = basic_info->id;
	value0["password"] = strPWD;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) return true;
	return false;
}

/*
设定用户的密码
仅供管理员和服务端使用，用户自行修改密码请使用IAuthManager
参数：	strPWD	[in]	用户的密码，服务端调用时传入散列后的密码，客户端前端调用时传入明文密码
*/
bool CUser::setPassword(const char * strPWD)
{
	Json::Value value0;
	value0["command"] = "user_setPassword";//may require revision
	value0["id"] = basic_info->id;
	//encryption
	value0["password"] = strPWD;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) return true;
	return false;
}

bool CUser::getBorrowedBooks(std::vector<TBorrowInfo> &binfo)//to be implemented
{
	Json::Value value0;
	value0["command"] = "user_getBorrowedBooks";
	value0["id"] = basic_info->id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	int num = value[0].asInt();
	if (num>0) {
		TBorrowInfo borrow_info;
		for (int i = 1; i <= num; i++) {
			Json::Value tem_value = value[i];
			borrow_info.bookID = tem_value[1].asInt();
			borrow_info.borrowTime = tem_value[2].asInt64();
			borrow_info.flag = tem_value[3].asBool();
			binfo.push_back(borrow_info);
		}
		return true;
	}
	return false;
}

bool CUser::borrowBook(IBook * pBook)
{
	CBook *book=dynamic_cast <CBook*>(pBook);
	Json::Value value0;
	value0["command"] = "user_borrowBook";//may require revision
	value0["userid"] = basic_info->id;
	TBookBasicInfo tem_basic_info;
	book->getBasicInfo(tem_basic_info);
	value0["bookid"] = tem_basic_info.id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) return true;
	return false;
}

bool CUser::returnBook(IBook * pBook)
{
	CBook *book = dynamic_cast <CBook*>(pBook);
	Json::Value value0;
	value0["command"] = "user_returnBook";//may require revision
	value0["userid"] = basic_info->id;
	TBookBasicInfo tem_basic_info;
	book->getBasicInfo(tem_basic_info);
	value0["bookid"] = tem_basic_info.id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) return true;
	return false;
}

bool CUser::deleteUser()
{
	Json::Value value0;
	value0["command"] = "user_deleteUser";
	value0["id"] = id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1)  return true;
	return false;
}