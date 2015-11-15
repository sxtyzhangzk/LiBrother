#ifndef __FUNCTION_INTERFACES_H__
#define __FUNCTION_INTERFACES_H__

#include "common_types.h"
#include "common_interfaces.h"

/*
  �����ͻ��˺ͷ�������ͨ�õĽӿ�
*/

/*
  ͼ��ӿ�
  ������ͼ��
*/
interface IBook : public IAbstract
{
	//��ȡͼ��Ļ�����Ϣ
	virtual bool getBasicInfo(TBookBasicInfo& info) = 0;

	//��ȡͼ�������
	virtual bool getDescription(std::string& description) = 0;

	//�趨ͼ��Ļ�����Ϣ
	virtual bool setBasicInfo(const TBookBasicInfo& info) = 0;

	//�趨ͼ�������
	virtual bool setDescription(const char * description) = 0;

	/*
	 ɾ��ͼ��
	 ������number	[in]	ɾ����ͼ��ı�����-1��ʾȫ��ɾ��
	*/
	virtual bool deleteBook(int number) = 0;

	//��ȡͼ��Ľ�����Ϣ
	virtual bool getBorrowInfo(std::vector<TBorrowInfo> &binfo) = 0;

	//���������
	virtual bool insert() = 0;
};

/*
  ͼ��ݽӿ�
  ����ͼ����е�����ͼ��
*/
interface ILibrary : public IAbstract
{
	/*
	  ����ͼ�����ƽ��в���
	  ������	strName	[in]	ͼ������ƣ�����ģ�����ң��ؼ����ÿո�ֿ�
			vBooks	[out]	���ؼ�������ͼ��
			nCount	[in]	���ڷ�ҳ��һ�η���ͼ�������
			nTop	[in]	���ڷ�ҳ���Ӽ������ĵ�nTop���鿪ʼ����ͼ����Ϣ
	  ���أ�	��������ͼ��������-1��ʾ�������ִ���
	*/
	virtual int queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop) = 0;

	//����ͼ���ID�Ž��в���
	virtual bool queryById(int nID, IBook ** ppBook) = 0;

	//����ͼ���ISBN�Ž��в���
	virtual bool queryByISBN(const char * strISBN, IBook ** ppBook) = 0;

	//����һ��ͼ��
	virtual bool insertBook(IBook * pBook) = 0;
};

/*
  �û��ӿ�
  �������û�
*/
interface IUser : public IAbstract
{
	//��ȡ�û��Ļ�����Ϣ
	virtual bool getBasicInfo(TUserBasicInfo& info) = 0;

	//�趨�û��Ļ�����Ϣ
	virtual bool setBasicInfo(const TUserBasicInfo& info) = 0;

	/*
	  ��֤�û�������
	  ������	strPWD	[in]	�û������룬����˵���ʱ����ɢ�к�����룬�ͻ���ǰ�˵���ʱ������������
	*/
	virtual bool verifyPassword(const char * strPWD) = 0;

	/*
	  �趨�û�������
	  ��������Ա�ͷ����ʹ�ã��û������޸�������ʹ��IAuthManager
	  ������	strPWD	[in]	�û������룬����˵���ʱ����ɢ�к�����룬�ͻ���ǰ�˵���ʱ������������
	*/
	virtual bool setPassword(const char * strPWD) = 0;

	//��ȡ�Ѿ����ĵ�ͼ��
	virtual bool getBorrowedBooks(std::vector<TBorrowInfo> &binfo) = 0;

	//����һ��ͼ��
	virtual bool borrowBook(IBook * pBook) = 0;

	//�黹һ��ͼ��
	virtual bool returnBook(IBook * pBook) = 0;

	//ɾ�����û�
	virtual bool deleteUser() = 0;

	//�����˽���
	virtual bool insert() = 0;
};

/*
  �û��������ӿ�
  ����ȫ�����û�
*/
interface IUserManager : public IAbstract
{
	//�����û�ID��ȡ�û�
	virtual bool getUserByID(int nID, IUser ** ppUser) = 0;

	//�����û�����Email�������ַ�����ʽ��ID��ȡ�û�
	virtual bool getUserByName(const char * strName, IUser ** ppUser) = 0;

	//����һ���û�
	virtual bool insertUser(IUser * pUser) = 0;
};

/*
  �๤��
  ���ڴ��������ӿڵ�ʵ��
*/
interface ILibClassFactory : public IAbstract
{
	//����һ���յ�IBook��������insertBook�Ȳ���
	virtual bool createEmptyBook(IBook ** ppBook) = 0;

	//����һ���յ�IUser��������insertUser�Ȳ���
	virtual bool createEmptyUser(IUser ** ppUser) = 0;

	//��ȡILibrary����
	virtual bool getLibrary(ILibrary ** ppLibrary) = 0;

	//��ȡIUserManager����
	virtual bool getUserManager(IUserManager ** ppManager) = 0;
};

#endif