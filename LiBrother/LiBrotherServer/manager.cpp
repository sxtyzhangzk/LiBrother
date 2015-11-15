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
	IRecordset * URecordset;
	std::stringstream str;
	str << "SELECT * FROM UserInfoDatabase WHERE id=" << nID;
	if (!m_pDatabase->executeSQL(str.str().c_str(), &URecordset))
	{
		setError(InvalidParam, 142857, "No this user.");
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
	return true;
}
bool CManager::insertUser(IUser * pUser)
{
	return pUser->insert();
}
bool getUserByName(const char * strName, IUser ** ppUser)
{
	return true;
}