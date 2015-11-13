#ifndef __LIBRARY_H_
#define __LIBRARY_H_

#include "function_interfaces.h"
#include "common_types.h"
#include<string>

class CLibrary :public ILibrary
{
	IMPLEMENT_INTERFACE
public:/*
	  ����ͼ�����ƽ��в���
	  ������	strName	[in]	ͼ������ƣ�����ģ�����ң��ؼ����ÿո�ֿ�
			vBooks	[out]	���ؼ�������ͼ��
			nCount	[in]	���ڷ�ҳ��һ�η���ͼ�������
			nTop	[in]	���ڷ�ҳ���Ӽ������ĵ�nTop���鿪ʼ����ͼ����Ϣ
	  ���أ�	��������ͼ��������-1��ʾ�������ִ���
	*/
	virtual int queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop) override;

	//����ͼ���ID�Ž��в���
	virtual bool queryById(int nID, IBook ** ppBook) override;

	//����ͼ���ISBN�Ž��в���
	virtual bool queryByISBN(const char * strISBN, IBook ** ppBook) override;

	//����һ��ͼ��
	virtual bool insertBook(IBook * pBook) override;
};

#endif