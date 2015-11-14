#include "library.h"
#include "book.h"
#include "netclient.h"

#include <json/json.h>
#include<string>

int CLibrary::queryByName(const char * strName, IFvector& vBooks, int nCount, int nTop)
{
	std::string strRequest;
	std::string strRespond;
	Json::Value value0;
	value0["command"] = "queryByName";
	value0["name"] = strName;
	value0["nCount"] = nCount;
	value0["nTop"] = nTop;
	Json::FastWriter writer;
	strRequest = writer.write(value0);
	if (!sendRequest(strRequest, strRespond))//respnod[0] 1:ok 0:no book
	{
		setError(NetworkError, 0, "Network Error");
		return -1;
	}
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);

	int num = value[0].asInt();
	for (int i = 0; i < num; i++) {
		TBookBasicInfo *Info = new TBookBasicInfo;
		Json::Value bookinfo = value[i + 1];
		Info->id = bookinfo[0].asInt();
		Info->count = bookinfo[1].asInt();
		Info->name = bookinfo[2].asString();
		Info->author = bookinfo[3].asString();
		Info->isbn = bookinfo[4].asString();
		Info->publisher = bookinfo[5].asString();
		CBook *book = new CBook(Info);
		vBooks.push_back(book);
	}
	return 	num;
}

bool CLibrary::queryById(int nID, IBook ** ppBook)
{
	std::string strRequest;
	std::string strRespond;
	Json::Value value0;
	value0["command"] = "queryById";
	value0["nID"] = nID;
	Json::FastWriter writer;
	std::string str=writer.write(value0);
	sendRequest(strRequest, strRespond);//return ifok(1/0) and results 
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) {
		TBookBasicInfo *Info = new TBookBasicInfo;
		Info->id = value["id"].asInt();
		Info->count = value["count"].asInt();
		Info->name = value["name"].asString();
		Info->author = value["author"].asString();
		Info->isbn = value["isbn"].asString();
		Info->publisher = value["publisher"].asString();
		CBook *book = new CBook(Info);
		book->AddRef();
		*ppBook = book;
		return true;
	}
	else
	{
		setError(InvalidParam, 1, "Invalid ID");//jere w about 1
		return false;
	}
}

bool CLibrary::queryByISBN(const char * strISBN, IBook ** ppBook)
{
	std::string strRequest;
	std::string strRespond;
	Json::Value value0;
	value0["command"] = "queryByISBN";
	value0["strISBN"] = strISBN;
	Json::FastWriter writer;
	std::string str = writer.write(value0);
	sendRequest(strRequest, strRespond);//return ifok(1/0) and results 
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) {
		TBookBasicInfo *Info = new TBookBasicInfo;
		Info->id = value["id"].asInt();
		Info->count = value["count"].asInt();
		Info->name = value["name"].asString();
		Info->author = value["author"].asString();
		Info->isbn = value["isbn"].asString();
		Info->publisher = value["publisher"].asString();
		CBook *book = new CBook(Info);
		book->AddRef();
		*ppBook = book;
		return true;
	}
	else
	{
		setError(InvalidParam, 1, "Invalid strISBN");//jere w about 1
		return false;
	}
}

bool CLibrary::insertBook(IBook * pBook)
{
	CBook *book = dynamic_cast<CBook*>(pBook);
	Json::Value value0;
	value0["command"] = "insertBook";
	TBookBasicInfo tem_book_basic_info;
	book->getBasicInfo(tem_book_basic_info);
	value0["id"] = tem_book_basic_info.id;
	value0["count"] = tem_book_basic_info.count;
	value0["name"] = tem_book_basic_info.name;
	value0["author"] = tem_book_basic_info.author;
	value0["isbn"] = tem_book_basic_info.isbn;
	value0["publisher"] = tem_book_basic_info.publisher;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"].asInt() == 1) return true;
	return false;
}