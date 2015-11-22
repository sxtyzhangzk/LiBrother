#include "authmanager.h"
#include "netclient.h"
#include <json/json.h>
#include <botan/botan.h>
#include <botan/filters.h>

CAuthManager::CAuthManager(TUserBasicInfo* current_user) : current_user(nullptr) {}
CAuthManager::~CAuthManager() {
	if (current_user) delete current_user;
}

int CAuthManager::getLoginStatus()
{	
	if (!current_user) return -1;
	if (current_user->LoginStatus) return 1;
	else return 0;
}

bool CAuthManager::Login(const  char * strUser, const char * strPWD)
{
	if (current_user->LoginStatus) {
		setError(InvalidParam, 4, "You have already logged in!");
		return false;
	}

	Json::Value value0;
	value0["command"] = "authmanager_Login";
	value0["userid"] = strUser;
	value0["usepassword"] = encryptPassword(strPWD);
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) {
		current_user->LoginStatus = true;
		current_user->name = value["name"].asString();
		current_user->email = value["email"].asString();
		current_user->id = value["id"].asInt();
		current_user->gender = value["gender"].asInt();
		return true;
	}
	return false;
}

bool CAuthManager::Logout()
{
	if (!current_user->LoginStatus) {
		setError(InvalidParam, 4, "You have not logged in!");
		return false;
	}
	Json::Value value0;
	value0["command"] = "authmanager_Logout";
	value0["id"] = current_user->id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) {
		current_user->LoginStatus = false;
		return true;
	}
	return false;
}

bool  CAuthManager::Register(IUser * pUser)
{
	TUserBasicInfo *info = new TUserBasicInfo();
	pUser->setBasicInfo(*info);
	Json::Value value0;
	value0["command"] = "authmanager_Register";
	value0["name"] = info->name;
	value0["email"] = info->email;
	value0["gender"] = info->gender;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == -1) {
		setError(InvalidParam, 4, "Your email has been registered");
		delete info;
		return false;
	}
	if (value["result"].asInt() == 0) {
		setError(InvalidParam, 4, "Your name has been registered");
		delete info;
		return false;
	}
	info->id=value["id"].asInt();
	info->LoginStatus = true;
	current_user = info;
	delete info;
	return true;
}

bool CAuthManager::changePassword(const char * strOldPWD, const char * strPWD) {
	Json::Value value0;
	value0["command"] = "authmanager_changePassword";
	value0["id"] = current_user->id;
	value0["oldPWD"] = strOldPWD;
	value0["newPWD"] = strPWD;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == -2) {
		setError(InvalidParam, 4, "Wrong old password");
		return false;
	}
	if (value["result"].asInt() == 1) {
		return true;
	}
	return false;
}

bool CAuthManager::getCurrentUser(IUser **ppUser)
{
	if (!current_user->LoginStatus) {
		setError(InvalidParam, 4, "You have not logged in!");
		return false;
	}
	Json::Value value0;
	value0["command"] = "authmanager_getCurrentUser";
	value0["id"] = current_user->id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) {
		TUserBasicInfo *Info = new TUserBasicInfo;
		Info->email = value["email"].asString();
		Info->gender = value["gender"].asInt();
		Info->id = value["id"].asInt();
		Info->name = value["name"].asString();
		Info->LoginStatus = true;
		IUser *user = new IUser(Info);
		*ppUser = user;
		delete Info;
		return true;
	}
	return false;
}

int CAuthManager::getAuthLevel()
{
	if (!current_user->LoginStatus) {
		setError(InvalidParam, 4, "You have not logged in!");
		return -1;
	}
	Json::Value value0;
	value0["command"] = "authmanager_getAuthLevel";
	value0["id"] = current_user->id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) return value["AuthLevel"].asInt();
	return -1;
}

std::string CAuthManager::encryptPassword(const char * strPWD)
{
	std::string strEPWD;
	Botan::Pipe pipe(new Botan::Hash_Filter("SHA-256"), new Botan::Hex_Encoder);
	pipe.process_msg(strPWD);
	strEPWD = pipe.read_all_as_string();
}