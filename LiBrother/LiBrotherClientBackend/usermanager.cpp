#include "user.h"
#include "usermanager.h"
#include "netclient.h"
#include <json/json.h>

bool CUserManager::getUserByID(int nID, IUser ** ppUser)
{
	std::string strRequest;
	std::string strRespond;
	Json::Value value0;
	value0["command"] = "getUserById";
	value0["nID"] = nID;
	Json::FastWriter writer;
	std::string str = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) {
		TUserBasicInfo *Info = new TUserBasicInfo;
		Info->id = value0["id"].asInt();
		Info->gender = value0["gender"].asInt();
		Info->name = value0["name"].asString();
		Info->email = value0["email"].asString();
		CUser *user = new CUser(Info);
		user->AddRef();
		*ppUser = user;
		return true;
	}
	return false;
}

//利用用户名、Email、或者字符串形式的ID获取用户
bool CUserManager::getUserByName(const char * strName, IUser ** ppUser)
{
	std::string strRequest;
	std::string strRespond;
	Json::Value value0;
	value0["command"] = "getUserByName";
	value0["strName"] = strName;
	Json::FastWriter writer;
	std::string str = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) {
		TUserBasicInfo *Info = new TUserBasicInfo;
		Info->id = value0["id"].asInt();
		Info->gender = value0["gender"].asInt();
		Info->name = value0["name"].asString();
		Info->email = value0["email"].asString();
		CUser *user = new CUser(Info);
		user->AddRef();
		*ppUser = user;
		return true;
	}
	return false;
}

bool CUserManager::insertUser(IUser * pUser)
{
	CUser *user = dynamic_cast<CUser*>(pUser);
	Json::Value value0;
	value0["command"] = "insertUser";
	TUserBasicInfo tem_user_basic_info;
	user->getBasicInfo(tem_user_basic_info);
	value0["id"] = tem_user_basic_info.id;
	value0["gender"] = tem_user_basic_info.gender;
	value0["name"] = tem_user_basic_info.name;
	value0["email"] = tem_user_basic_info.email;
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