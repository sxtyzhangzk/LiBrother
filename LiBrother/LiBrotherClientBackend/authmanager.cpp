#include "authmanager.h"
#include "user.h"
#include "netclient.h"
#include "utils.h"

#include <json/json.h>
#include <botan/botan.h>
#include <botan/filters.h>

CAuthManager::CAuthManager() : 
	m_pCurrentUser(nullptr)
{
}

CAuthManager::~CAuthManager()
{
	if (m_pCurrentUser)
		m_pCurrentUser->Release();
}

int CAuthManager::getLoginStatus()
{	
	if (!m_pCurrentUser)
		return 0;
	return 1;
}

bool CAuthManager::Login(const char * strUser, const char * strPWD)
{
	CHECK_PTR(strUser && strPWD);
	if (m_pCurrentUser)
		Logout();

	Json::Value valueReq, valueRes;
	valueReq["command"] = "authmanager_Login";
	valueReq["username"] = strUser;
	valueReq["password"] = encryptPassword(strPWD);

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		if (valueRes["result"] == 1)
		{
			int nUserID = valueRes["id"].asInt();
			m_pCurrentUser = new CUser(nUserID);
			m_pCurrentUser->AddRef();
			return true;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CAuthManager::Logout()
{
	if (!m_pCurrentUser)
	{
		setError(InvalidParam, 4, "You have not logged in!");
		return false;
	}

	Json::Value valueReq, valueRes;
	valueReq["command"] = "authmanager_Logout";

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		if (valueRes["result"] == 1)
		{
			m_pCurrentUser->Release();
			m_pCurrentUser = nullptr;
			return true;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return false;
}

bool  CAuthManager::Register(IUser * pUser)
{
	CHECK_PTR(pUser);
	CUser *user = dynamic_cast<CUser*>(pUser);
	if (!user->m_pBasicInfo || user->m_strEPassword.empty())
	{
		setError(InvalidParam, -2, "Incomplete User Info");
		return false;
	}

	Json::Value valueReq, valueRes;
	valueReq["command"] = "authmanager_Register";
	valueReq["name"] = user->m_pBasicInfo->name;
	valueReq["email"] = user->m_pBasicInfo->email;
	valueReq["gender"] = user->m_pBasicInfo->gender;
	valueReq["password"] = user->m_strEPassword;

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		int result = valueRes["result"].asInt();
		if (result == 1)
			return true;
		if (valueRes["result"].asInt() == -1)
		{
			setError(InvalidParam, 4, "Your email has been registered");
			return false;
		}
		if (valueRes["result"].asInt() == 0)
		{
			setError(InvalidParam, 4, "Your name has been registered");
			return false;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CAuthManager::changePassword(const char * strOldPWD, const char * strPWD)
{
	CHECK_PTR(strOldPWD && strPWD);
	if (!m_pCurrentUser)
	{
		setError(InvalidParam, 4, "You have not logged in");
		return false;
	}

	Json::Value valueReq, valueRes;
	valueReq["command"] = "authmanager_changePassword";
	valueReq["id"] = m_pCurrentUser->m_nID;
	valueReq["oldPWD"] = encryptPassword(strOldPWD);
	valueReq["newPWD"] = encryptPassword(strPWD);
	
	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		int result = valueRes["result"].asInt();
		if (result == -2)
		{
			setError(InvalidParam, 4, "Wrong old password");
			return false;
		}
		if (result == 1)
			return true;
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CAuthManager::getCurrentUser(IUser **ppUser)
{
	CHECK_PTR(ppUser);
	if (!m_pCurrentUser)
	{
		setError(InvalidParam, 4, "You have not logged in!");
		return false;
	}
	
	m_pCurrentUser->AddRef();
	*ppUser = m_pCurrentUser;
	return true;
}

int CAuthManager::getAuthLevel()
{
	if (!m_pCurrentUser)
	{
		setError(InvalidParam, 4, "You have not logged in!");
		return -1;
	}
	int result = m_pCurrentUser->getAuthLevel();
	if (result == -1)
	{
		transferError(m_pCurrentUser);
		return -1;
	}
	return result;
}

bool CAuthManager::getAuthItems(TAuthorization& auth)
{
	if (!m_pCurrentUser)
	{
		setError(InvalidParam, 4, "You have not logged in");
		return false;
	}

	Json::Value valueReq, valueRes;
	valueReq["command"] = "authmanager_getAuthItems";

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		if (valueRes["result"] == 1)
		{
			bool *pAuth = (bool *)&auth;
			int size = valueRes["size"].asInt();
			if (size * sizeof(bool) != sizeof(TAuthorization))
			{
				setError(Other, -3, "Internal Error");
				return false;
			}
			for (int i = 0; i < size; i++)
				pAuth[i] = valueRes["data"][i].asBool();
			return true;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CAuthManager::getLicense(std::string& strLicense)
{
	Json::Value valueReq, valueRes;
	valueReq["command"] = "authmanager_getLicense";

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		if (valueRes["result"] != 1)
		{
			setError(Other, -1, "Operation Failed");
			return false;
		}
		strLicense = valueRes["license"].asString();
	}
	END_PARSE_RESPONSE;

	return true;
}

std::string CAuthManager::encryptPassword(const char * strPWD)
{
	Botan::Pipe pipe(new Botan::Hash_Filter("SHA-256"), new Botan::Hex_Encoder);
	pipe.process_msg(strPWD);
	return pipe.read_all_as_string();
}