#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "function_interfaces.h"
#include "common_types.h"

interface IDatabase;
class CManager :public IUserManager
{
	IMPLEMENT_INTERFACE
public:
	CManager(IDatabase * DatabaseFile);
	virtual bool getUserByID(int nID, IUser ** ppUser) override;
	virtual bool getUserByName(const char * strName, IUser ** ppUser) override;
	virtual bool insertUser(IUser * pUser) override;
	~CManager();
protected:
	IDatabase * m_pDatabase;
};
#endif
