#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <string>
#include <ctime>

//ͼ��Ļ�����Ϣ
struct TBookBasicInfo
{
	int id;		//ͼ��ID
	int count;	//�ڹ�����
	std::string name;	//ͼ������
	std::string author;	//ͼ������
	std::string isbn;	//ISBN��
	std::string publisher;	//������
};

//�û��Ļ�����Ϣ
struct TUserBasicInfo
{
	bool LoginStatus;   //True=����,False=δ����
	int id;				//�û�ID
	int gender;			//�Ա� male:0
	std::string name;	//�û���
	std::string email;	//�û�����
};

//������Ϣ
struct TBorrowInfo
{
	int bookID;			//���ĵ���ID
	int userID;			//���ĵ��û�ID
	time_t borrowTime;	//���ĵ�ʱ��
	bool flag;		//��ʾ�ǽ�����ǻ��룬0��ʾ�����1��ʾ����
};

struct TAuthorization
{
	bool auth_Login;			//��¼
	bool auth_GetUserInfo;		//��ȡ�����û�����Ϣ
	bool auth_SetUserInfo;		//���������û�����Ϣ����ע����Ȩ���ñ��Լ�Ȩ�޵ȼ��ߵ��û�����Ϣ��
	bool auth_ChangePassword;	//���������û������루��ע����Ȩ�޸ı��Լ�Ȩ�޵ȼ��ߵ��û������룩
	bool auth_DeleteUser;		//ɾ��һ���û�
	bool auth_InsertUser;		//����һ���û�
	bool auth_BorrowBook;		//����ͼ��
	bool auth_ReturnBook;		//�黹ͼ��
	bool auth_GetBookInfo;		//��ȡͼ�����Ϣ
	bool auth_SetBookInfo;		//����ͼ�����Ϣ
	bool auth_InsertBook;		//����ͼ��
	bool auth_DeleteBook;		//ɾ��ͼ��
	bool auth_SetAuthLevel;		//���������û�Ȩ�޵ȼ�����ע�����ø��������Ȩ�ޣ����ø��ĸ����Լ�Ȩ�޵ȼ����û���
	bool auth_SetReadLevel;		//���������û����Ķ�Ȩ��
};

#endif