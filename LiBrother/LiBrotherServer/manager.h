#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "function_interfaces.h"
#include "common_types.h"
#include"connection_pool.h"
#include <driver/mysql_public_iface.h>

class CManager :public IUserManager
{
	IMPLEMENT_INTERFACE
public:
	CManager(CConnectionPool * DatabaseFile);
	virtual bool getUserByID(int nID, IUser ** ppUser) override;
	virtual bool getUserByName(const char * strName, IUser ** ppUser) override;
	virtual bool insertUser(IUser * pUser) override;
	bool verify(const char* strName,const char* strEmail);
	~CManager();
protected:
	CConnectionPool * m_pDatabase;
};
#endif
