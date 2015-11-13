#include "book.h"
#include "netclient.h"
#include <json/json.h>
CBook::CBook():m_CBBI(nullptr){}
CBook::CBook(TBookBasicInfo *Info) : m_CBBI(Info) {}
CBook::~CBook() {
	if (m_CBBI) delete m_CBBI;
}
bool CBook::getBasicInfo(TBookBasicInfo& info)
{
	if (!m_CBBI) m_CBBI = new TBookBasicInfo;
	Json::Value value0;
	value0["command"] = "getBasicInfo";
	value0["bookid"] = m_Id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);//respnod contains 4 elements
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	m_CBBI->id = m_Id;
	m_CBBI->author = value["author"].asString();
	m_CBBI->count = value["count"].asInt();
	m_CBBI->isbn = value["isbn"].asString();
	m_CBBI->name = value["name"].asString();
	m_CBBI->publisher = value["publisher"].asString();
	if (!check(*m_CBBI))
	{
		setError(InvalidParam, 1, "This book is not valid.");
		return false;
	}
	info = *m_CBBI;
	return true;
}

bool CBook::getDescription(std::string& description)
{
	Json::Value value0;
	value0["command"] = "getDescription";
	value0["id"] = m_Id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);//respnod contains 1 elements
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	m_Description = value["description"].asString();
	description = m_Description;
	return true;
}

bool CBook::setBasicInfo(const TBookBasicInfo& info)
{
	if (!check(info))
	{
		setError(InvalidParam, 2, "The information is not valid.");
		return false;
	}
	if (info.id != m_CBBI->id)
	{
		setError(PermissionDenied, 3, "You have no access to the database.");
		return false;
	}
	*m_CBBI = info;
	Json::Value value0;
	value0["command"] = "setBasicInfo";
	value0["id"] = m_CBBI->id;
	value0["count"] = m_CBBI->count;
	value0["name"] = m_CBBI->name;
	value0["publisher"] = m_CBBI->publisher;
	value0["author"] = m_CBBI->author;
	value0["isbn"] = m_CBBI->isbn;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);//respnod contains result as key
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if(value["result"] == "1") return true;
	return false;
}

bool CBook::setDescription(const char * description)
{
	if (!description)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	m_Description = description;
	Json::Value value0;
	value0["command"] = "setDescription";
	value0["id"] = m_Id;
	value0["description"] = m_Description;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);//respnod contains result as key
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"] == "1") return true;
	return false;
}

bool CBook::deleteBook(int number) 
{
	//TODO: number表示删除图书的本数，将它传给服务端 ---[zk]
	Json::Value value0;
	value0["command"] = "deleteBook";
	value0["id"] = m_Id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	sendRequest(strRequest, strRespond);//respond contains result
	Json::Reader reader;
	Json::Value value;
	reader.parse(strRespond, value);
	if (value["result"] == "1") {
		Release();
		return true;
	}
	return false;
}

bool CBook::getBorrowInfo(std::vector<TBorrowInfo> &binfo)
{
	return true;
}

bool CBook::insert()
{
	return true;
}

bool CBook::check(TBookBasicInfo info_to_check)
{
	return true;
}