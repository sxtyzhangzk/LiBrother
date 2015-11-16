#ifndef __AUTHMANAGER_H_
#define __AUTHMANAGER_H_

#include "client_interfaces.h"
#include "common_types.h"

class CAuthManager :public IAuthManager
{
public:
	//�û���¼
	//������strUser ��¼��ʾ���������û���/����/�û�ID; strPWD �û�����
	virtual bool Login(const char * strUser, const char * strPWD) = 0;

	//�û�ע��
	virtual bool Logout() = 0;

	//ע���û�
	//������pUser һ��IUser�ӿڶ��󣬱�ʾ�û��������Ϣ
	virtual bool Register(IUser * pUser) = 0;

	//�޸�����
	//������strOldPWD ԭ����; strPWD ������
	virtual bool changePassword(const char * strOldPWD, const char * strPWD) = 0;

	//��ȡ��¼״̬
	//���أ�1��ʾ�ѵ�¼��0��ʾδ��¼��-1��ʾ�������ִ���
	virtual int getLoginStatus() = 0;

	//��ȡ��ǰ��¼�û���IUser����
	//������ppUser��һ��ָ��IUser˫��ָ��
	virtual bool getCurrentUser(IUser ** ppUser) = 0;

	//��ȡ��ǰ��Ȩ�޵ȼ�
	//���أ�Ȩ�޵ȼ���-1��ʾ�������ִ���
	virtual int getAuthLevel() = 0;
private:
	TUserBasicInfo* current_user
};

#endif