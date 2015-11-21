#ifndef __CLIENT_INTERFACES_H__
#define __CLIENT_INTERFACES_H__

#include "function_interfaces.h"

//�����ͻ��˺���ṩ�Ľӿںͺ���

/*
  ��Ȩ������
  �ṩ��¼��ע�������Ȩ�޵ȹ���
*/
interface IAuthManager : public IAbstract
{
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

	//��ȡ��ǰ��Ȩ������
	virtual bool getAuthItems(TAuthorization& auth) = 0;

	//��ȡ�û�Э�������
	virtual bool getLicense(std::string& strLicense) = 0;
};

/*
  ���ڿͻ��˵��๤����չ
*/
interface IClassFactoryClient : public ILibClassFactory
{
	//��ȡһ��IAuthManager�ӿڶ���
	virtual bool getAuthManager(IAuthManager ** ppAuthManager) = 0;
};

//��ȡIClassFactoryClient�ӿڶ������ڴ����������еĶ���
bool getClassFactory(IClassFactoryClient ** ppFactory);

/*
  ��ʼ���ͻ��˺��
  ������	argc, argv	[in]	main�����Ĳ���
		retcode		[out]	��������ִ��ʱ��main�����ķ���ֵ
  ���أ��ͻ����Ƿ�Ӧ�ü���ִ�У���Ϊfalse����ֱ���˳�����
*/
bool initBackend(int argc, char * argv[], int& retcode);
//�����ͻ��ˣ��ͻ����˳�ʱִ��
void cleanupBackend();

#endif