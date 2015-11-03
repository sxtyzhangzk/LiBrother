#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <string>
#include <ctime>

//��������
enum ErrType
{
	None,
	InvalidParam, PermissionDenied,
	UnsupportedMethod, NetworkError, DatabaseError,
	Other
};

//����һ������
struct TErrInfo
{
	ErrType type;		//����
	int code;			//�������
	const char * description;	//���������[��ʹ��ʱ��Ϊnullptr]
	TErrInfo(ErrType type = None, int code = 0, const char * description = nullptr) : type(type), code(code), description(description) {}
};

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
	int id;				//�û�ID
	int gender;			//�Ա�
	std::string name;	//�û���
	std::string email;	//�û�����
};

//������Ϣ
struct TBorrowInfo
{
	int userID;			//���ĵ��û�ID
	time_t borrowTime;	//���ĵ�ʱ��
};

#endif