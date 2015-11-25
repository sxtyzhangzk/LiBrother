#include "user.h"
#include "usermanager.h"
#include "netclient.h"
#include "utils.h"
#include <json/json.h>

bool CUserManager::getUserByID(int nID, IUser ** ppUser)
{
	CHECK_PTR(ppUser);

	TUserBasicInfo info;
	CUser *pUser = new CUser(nID);
	if (!pUser->getBasicInfo(info))
	{
		transferError(pUser);
		delete pUser;
		return false;
	}

	pUser->AddRef();
	*ppUser = pUser;
	return true;
}

//利用用户名、Email、或者字符串形式的ID获取用户
bool CUserManager::getUserByName(const char * strName, IUser ** ppUser)
{
	CHECK_PTR(strName && ppUser);

	std::string strRequest, strResponse;
	Json::Value value0;
	value0["command"] = "usermanager_getUserByName";
	value0["name"] = strName;

	Json::FastWriter writer;
	strRequest = writer.write(value0);

	TRY_SEND_REQUEST(strRequest, strResponse);
	
	Json::Reader reader;
	Json::Value value;

	BEGIN_PARSE
	{
		reader.parse(strResponse, value);
		if (value["result"].asInt() == 1)
		{
			TUserBasicInfo Info;
			Info.id = value["id"].asInt();
			Info.gender = value["gender"].asInt();
			Info.name = value["name"].asString();
			Info.email = value["email"].asString();
			Info.num = value["book_num"].asInt();

			CUser *user = new CUser(Info.id);
			user->AddRef();
			*ppUser = user;
			return true;
		}
	}
	END_PARSE;

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CUserManager::insertUser(IUser * pUser)
{
	CHECK_PTR(pUser);
	CUser *user = dynamic_cast<CUser*>(pUser);
	if (user->m_strEPassword.empty() || !user->m_pBasicInfo)
	{
		setError(InvalidParam, -2, "User Info not completed");
		return false;
	}

	Json::Value value0;
	value0["command"] = "usermanager_insertUser";
	value0["name"] = user->m_pBasicInfo->name;
	value0["gender"] = user->m_pBasicInfo->gender;
	value0["email"] = user->m_pBasicInfo->email;

	Json::FastWriter writer;
	std::string strRequest, strResponse;
	strRequest = writer.write(value0);
	
	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	Json::Value value;

	BEGIN_PARSE
	{
		reader.parse(strResponse, value);
		if (value["result"].asInt() == 1)
			return true;
	}
	END_PARSE;

	setError(Other, -1, "Operation Error");
	return false;

}