#ifndef __USER_H_
#define __USER_H_

#include "function_interfaces.h"
#include "common_types.h"
#include<string>

class CUser :public IUser
{
	IMPLEMENT_INTERFACE
public:	
	CUser(TUserBasicInfo *basic_info0 = nullptr);
	~CUser();
	//��ȡ�û��Ļ�����Ϣ
	virtual bool getBasicInfo(TUserBasicInfo& info) override;

	//�趨�û��Ļ�����Ϣ
	virtual bool setBasicInfo(const TUserBasicInfo& info) override;

	/*
	��֤�û�������
	������	strPWD	[in]	�û������룬����˵���ʱ����ɢ�к�����룬�ͻ���ǰ�˵���ʱ������������
	*/
	virtual bool verifyPassword(const char * strPWD) override;

	/*
	�趨�û�������
	��������Ա�ͷ����ʹ�ã��û������޸�������ʹ��IAuthManager
	������	strPWD	[in]	�û������룬����˵���ʱ����ɢ�к�����룬�ͻ���ǰ�˵���ʱ������������
	*/
	virtual bool setPassword(const char * strPWD) override;

	//��ȡ�Ѿ����ĵ�ͼ��
	virtual bool getBorrowedBooks(std::vector<TBorrowInfo> &binfo) override;

	//����һ��ͼ��
	virtual bool borrowBook(IBook * pBook) override;

	//�黹һ��ͼ��
	virtual bool returnBook(IBook * pBook) override;

	//ɾ�����û�
	virtual bool deleteUser() override;
protected:
	TUserBasicInfo *basic_info;
};

#endif