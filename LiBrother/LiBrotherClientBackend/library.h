#ifndef __LIBRARY_H_
#define __LIBRARY_H_

#include "function_interfaces.h"
#include "common_types.h"
#include<string>

class CLibrary :public ILibrary
{
	IMPLEMENT_INTERFACE
public:/*
	  利用图书名称进行查找
	  参数：	strName	[in]	图书的名称，允许模糊查找，关键词用空格分开
			vBooks	[out]	返回检索到的图书
			nCount	[in]	用于分页：一次返回图书的数量
			nTop	[in]	用于分页：从检索到的第nTop本书开始返回图书信息
	  返回：	检索到的图书数量，-1表示操作出现错误
	*/
	virtual int queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop) override;

	//利用图书的ID号进行查找
	virtual bool queryById(int nID, IBook ** ppBook) override;

	//利用图书的ISBN号进行查找
	virtual bool queryByISBN(const char * strISBN, IBook ** ppBook) override;

	//插入一本图书
	virtual bool insertBook(IBook * pBook) override;
};

#endif