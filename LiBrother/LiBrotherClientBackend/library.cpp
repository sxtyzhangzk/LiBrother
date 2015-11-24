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
	value0["command"] = "library_queryByName";
	value0["name"] = strName;
	value0["nCount"] = nCount;
	value0["nTop"] = nTop;
	Json::FastWriter writer;
	strRequest = writer.write(value0);
	if (!sendRequest(strRequest, strRespond))
	{
		setError(NetworkError, 0, "Network Error");
		return -1;
	}
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);

	int num = value[0].asInt();
	for (int i = 0; i < num; i++) {
		TBookBasicInfo Info;
		Json::Value bookinfo = value[i + 1];
		Info.id = bookinfo["id"].asInt();
		Info.count = bookinfo["count"].asInt();
		Info.name = bookinfo["name"].asString();
		Info.author = bookinfo["author"].asString();
		Info.isbn = bookinfo["isbn"].asString();
		Info.publisher = bookinfo["publisher"].asString();;
		CBook *book = new CBook;
		TBookBasicInfo *pInfo = new TBookBasicInfo(Info);
		book->m_CBBI = pInfo;
		book->id = Info.id;
		//book->setBasicInfo(Info);
		vBooks.push_back(book);
	}
	return 	num;
}

bool CLibrary::queryById(int nID, IBook ** ppBook)
{
	std::string strRequest;
	std::string strRespond;
	Json::Value value0;
	value0["command"] = "library_queryById";
	value0["id"] = nID;
	Json::FastWriter writer;
	strRequest = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		if (value["result"].asString() == "1") {
			TBookBasicInfo *Info = new TBookBasicInfo;
			Info->id = value["id"].asInt();
			Info->count = value["count"].asInt();
			Info->name = value["name"].asString();
			Info->author = value["author"].asString();
			Info->isbn = value["isbn"].asString();
			Info->publisher = value["publisher"].asString();
			CBook *book = new CBook();
			//book->setBasicInfo(*Info);
			book->m_CBBI = Info;
			book->id = Info->id;
			book->AddRef();
			*ppBook = book;
			return true;
		}
		else
		{
			if (value["result"].asString() == "DatabaseError")
				setError(DatabaseError, 0, "database_error");
			else setError(InvalidParam, 1, "Invalid ID");
			return false;
		}
	}
	else setError(NetworkError, 0, "network_error");
	return false;
}

bool CLibrary::queryByISBN(const char * strISBN, IBook ** ppBook)
{
	std::string strRequest;
	std::string strRespond;
	Json::Value value0;
	value0["command"] = "library_queryByISBN";
	value0["isbn"] = strISBN;
	Json::FastWriter writer;
	std::string str = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		if (value["result"].asString() == "1") {
			TBookBasicInfo *Info = new TBookBasicInfo;
			Info->id = value["id"].asInt();
			Info->count = value["count"].asInt();
			Info->name = value["name"].asString();
			Info->author = value["author"].asString();
			Info->isbn = value["isbn"].asString();
			Info->publisher = value["publisher"].asString();
			CBook *book = new CBook();
			book->m_CBBI = Info;
			book->id = Info->id;
			book->AddRef();
			*ppBook = book;
			return true;
		}
		else
		{
			if (value["result"].asString() == "DatabaseError")
				setError(DatabaseError, 0, "database_error");
			else setError(InvalidParam, 1, "Invalid strISBN");//jere w about 1
			return false;
		}
	}
	else setError(NetworkError, 0, "network_error");
	return false;
}

bool CLibrary::insertBook(IBook * pBook)
{
	//CBook *book = dynamic_cast<CBook*>(pBook);
	Json::Value value0;
	value0["command"] = "library_insertBook";
	TBookBasicInfo tem_book_basic_info;
	pBook->getBasicInfo(tem_book_basic_info);
	value0["count"] = tem_book_basic_info.count;
	value0["name"] = tem_book_basic_info.name;
	value0["author"] = tem_book_basic_info.author;
	value0["isbn"] = tem_book_basic_info.isbn;
	value0["publisher"] = tem_book_basic_info.publisher;
	value0["description"] = ((CBook*)pBook)->m_Description;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		if (value["result"].asString() == "1") return true;
		else
		{
			if (value["result"].asString() == "DatabaseError")
				setError(DatabaseError, 0, "database_error");
			else setError(InvalidParam, 0, "Invalid strISBN");
			return false;
		}
	}
	else setError(NetworkError, 0, "network_error");
	return false;
}