#include "user.h"
#include "netclient.h"
#include <json/json.h>
#include "book.h"

CUser::CUser(TUserBasicInfo *basic_info0 = nullptr) :basic_info(basic_info0) {if(basic_info) id=basic_info->id};
CUser::~CUser()
{
	if (basic_info) delete basic_info;
}

//��ȡ�û��Ļ�����Ϣ
bool CUser::getBasicInfo(TUserBasicInfo& info)
{
	info = *basic_info;
}

bool CUser::setBasicInfo(const TUserBasicInfo& info)
{
	Json::Value value0;
	value0["command"] = "setUserBasicInfo";
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
��֤�û�������
������	strPWD	[in]	�û������룬����˵���ʱ����ɢ�к�����룬�ͻ���ǰ�˵���ʱ������������
*/
bool CUser::verifyPassword(const char * strPWD)
{
	Json::Value value0;
	value0["command"] = "verifyUserPassword";
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
�趨�û�������
��������Ա�ͷ����ʹ�ã��û������޸�������ʹ��IAuthManager
������	strPWD	[in]	�û������룬����˵���ʱ����ɢ�к�����룬�ͻ���ǰ�˵���ʱ������������
*/
bool CUser::setPassword(const char * strPWD)
{
	Json::Value value0;
	value0["command"] = "setUserPassword";//may require revision
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
	return 1;
}

bool CUser::borrowBook(IBook * pBook)
{
	CBook *book=dynamic_cast <CBook*>(pBook);
	Json::Value value0;
	value0["command"] = "borrowBook";//may require revision
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
	value0["command"] = "returnBook";//may require revision
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
	value0["command"] = "deleteUser";
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