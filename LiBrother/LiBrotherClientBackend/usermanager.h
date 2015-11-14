#ifndef __USERMANAGER_H_
#define __USERMANAGER_H_

#include "function_interfaces.h"
#include "common_types.h"
#include <string>

class CUserManager :public IUserManager
{
	IMPLEMENT_INTERFACE
public:	
	//�����û�ID��ȡ�û�
	virtual bool getUserByID(int nID, IUser ** ppUser) override;

	//�����û�����Email�������ַ�����ʽ��ID��ȡ�û�
	virtual bool getUserByName(const char * strName, IUser ** ppUser) override;

	//����һ���û�
	virtual bool insertUser(IUser * pUser) override;
};
#endif