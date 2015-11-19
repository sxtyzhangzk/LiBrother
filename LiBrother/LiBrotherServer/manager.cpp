#include"manager.h"
#include"magicdb.h"
#include"user.h"
#include<sstream>

CManager::CManager(IDatabase * DatabaseFile)
{
	m_pDatabase=DatabaseFile;
}
CManager::~CManager()
{

}
bool CManager::getUserByID(int nID, IUser ** ppUser)
{
	if (!ppUser)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	IRecordset * URecordset=nullptr;
	std::stringstream str;
	str << "SELECT * FROM UserInfoDatabase WHERE id=" << nID;
	m_pDatabase->executeSQL(str.str().c_str(), &URecordset);
	if (!URecordset)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	TUserBasicInfo BasicInfo;
	BasicInfo.email = std::string(URecordset->getData("email"));
	BasicInfo.name = std::string(URecordset->getData("name"));
	BasicInfo.gender = URecordset->getData("gender");
	BasicInfo.id = URecordset->getData("id");
	(*ppUser)->setBasicInfo(BasicInfo);
	(*ppUser)->setPassword(std::string(URecordset->getData("password")).c_str());
	((CUser*)(*ppUser))->sign();
	URecordset->Release();
	return true;
}
bool CManager::insertUser(IUser * pUser)
{
	return ((CUser*)pUser)->insert();
}
bool CManager::getUserByName(const char * strName, IUser ** ppUser)
{
	int type=0;
	for (int i = 0; i < strlen(strName); i++)
	{
		if (strName[i] == '@')
		{
			type = 1;
			break;
		}
		else if ((strName[i] <= 'z' && strName[i] >= 'a') || (strName[i] <= 'Z' && strName[i] >= 'A') || strName[i] == '_')
			type = 2;
		else if (strName[i] <= '9' && strName[i] >= '0') {}
		else
		{
			setError(InvalidParam, -1, "Invalid string.");
			return false;
		}
	}
	if (!type)
	{
		int nId;
		std::stringstream ss;
		ss << strName;
		ss >> nId;
		return getUserByID(nId, ppUser);
	}
	if (type == 1)
	{
		if (!ppUser)
		{
			setError(InvalidParam, 4, "The pointer is NULL.");
			return false;
		}
		IRecordset * URecordset;
		std::stringstream str;
		str << "SELECT * FROM UserInfoDatabase WHERE email=" <<'"'<<strName<<'"';
		m_pDatabase->executeSQL(str.str().c_str(), &URecordset);
		if (!URecordset)
		{
			setError(InvalidParam, 4, "The pointer is NULL.");
			return false;
		}
		TUserBasicInfo BasicInfo;
		BasicInfo.email = std::string(URecordset->getData("email"));
		BasicInfo.name = std::string(URecordset->getData("name"));
		BasicInfo.gender = URecordset->getData("gender");
		BasicInfo.id = URecordset->getData("id");
		(*ppUser)->setBasicInfo(BasicInfo);
		(*ppUser)->setPassword(std::string(URecordset->getData("password")).c_str());
		((CUser*)(*ppUser))->sign();
		URecordset->Release();
		return true;
	}
	if (type == 2)
	{
		if (!ppUser)
		{
			setError(InvalidParam, 4, "The pointer is NULL.");
			return false;
		}
		IRecordset * URecordset;
		std::stringstream str;
		str << "SELECT * FROM UserInfoDatabase WHERE name=" <<'"'<< strName<<'"';
		m_pDatabase->executeSQL(str.str().c_str(), &URecordset);
		if (!URecordset)
		{
			setError(InvalidParam, 4, "The pointer is NULL.");
			return false;
		}
		TUserBasicInfo BasicInfo;
		BasicInfo.email = std::string(URecordset->getData("email"));
		BasicInfo.name = std::string(URecordset->getData("name"));
		BasicInfo.gender = URecordset->getData("gender");
		BasicInfo.id = URecordset->getData("id");
		(*ppUser)->setBasicInfo(BasicInfo);
		(*ppUser)->setPassword(std::string(URecordset->getData("password")).c_str());
		((CUser*)(*ppUser))->sign();
		URecordset->Release();
		return true;
	}
	return false;
}