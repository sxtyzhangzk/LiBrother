#include "book.h"
#include "netclient.h"
#include <json/json.h>
CBook::CBook() : m_CBBI(nullptr),id(0),read_level(0) {}
CBook::~CBook() {
	if (m_CBBI) delete m_CBBI;
}
bool CBook::getBasicInfo(TBookBasicInfo& info)
{
	if (m_CBBI) {
		info = *m_CBBI;
		return true;
	}
	else setError(Other, 0, "uninitiated book");
	return false;
}

bool CBook::getDescription(std::string& description)
{
	Json::Value value0;
	value0["command"] = "book_getDescription";
	value0["id"] = id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		if (value["result"].asString()=="1") {
			m_Description = value["description"].asString();
			description = m_Description;
			return true;
		}
		else {
			if (value["result"].asString() == "PermissionDenied")
				setError(PermissionDenied, 0, "permission_denied");
			if (value["result"].asString() == "DatabaseError")
				setError(DatabaseError, 0, "database_error");
		}
	}
	else setError(NetworkError, 0, "network down");
	return false;
}

bool CBook::setBasicInfo(const TBookBasicInfo& info)
{
	if (!m_CBBI)
		m_CBBI = new TBookBasicInfo(info);
	else
		*m_CBBI = info;
	if (id)
	{
		//id = info.id;
		Json::Value value0;
		value0["command"] = "book_setBasicInfo";
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
		if (sendRequest(strRequest, strRespond)) {
			Json::Reader reader;
			Json::Value value;
			reader.parse(strRespond, value);
			if (value["result"].asString() == "1") return true;
			else {
				if (value["result"].asString() == "PermissionDenied")
					setError(PermissionDenied, 0, "permission_denied");
				if (value["result"].asString() == "DatabaseError")
					setError(DatabaseError, 0, "database_error");
			}
		}
		else setError(NetworkError, 0, "network down");
		return false;
	}
	return true;
}

bool CBook::setDescription(const char * description)
{
	if (!description)
	{
		setError(InvalidParam, 4, "The pointer is NULL.");
		return false;
	}
	if (id)
	{
		;
		Json::Value value0;
		value0["command"] = "book_setDescription";
		value0["id"] = id;
		value0["description"] = description;
		Json::FastWriter writer;
		std::string strRequest;
		std::string strRespond;
		strRequest = writer.write(value0);
		if (sendRequest(strRequest, strRespond)) {
			Json::Reader reader;
			Json::Value value;
			reader.parse(strRespond, value);
			if (value["result"].asString() == "1")
			{
				m_Description = description;
				return true;
			}
			else {
				if (value["result"].asString() == "PermissionDenied")
					setError(PermissionDenied, 0, "permission_denied");
				if (value["result"].asString() == "DatabaseError")
					setError(DatabaseError, 0, "database_error");
			}
		}
		else setError(NetworkError, 0, "network down");
		return false;
	}
	else
		m_Description = description;
	return true;
}

bool CBook::deleteBook(int number) 
{
	Json::Value value0;
	value0["command"] = "book_deleteBook";
	value0["id"] = id;
	value0["number"] = number;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		if (value["result"].asString() == "1") return true;
		else {
			if (value["result"].asString() == "PermissionDenied")
				setError(PermissionDenied, 0, "permission_denied");
			if (value["result"].asString() == "DatabaseError")
				setError(DatabaseError, 0, "database_error");
		}
	}
	else setError(NetworkError, 0, "network down");
	return false;
}

bool CBook::getBorrowInfo(std::vector<TBorrowInfo> &binfo)
{
	Json::Value value0;
	value0["command"] = "book_getBorrowInfo";
	value0["id"] = id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		int num = value[0].asInt();
		if (num>0) {
			for (int i = 1; i <= num; i++) {
				Json::Value tem_value = value[i];
				TBorrowInfo borrow_info;
				borrow_info.bookID = tem_value["bookID"].asInt();
				borrow_info.userID = tem_value["userID"].asInt();
				borrow_info.borrowTime = tem_value["borrowTime"].asInt64();
				borrow_info.flag = tem_value["flag"].asBool();
				binfo.push_back(borrow_info);
			}
			return true;
		}
	}
	else setError(NetworkError, 0, "network down");
	return false;
}

int CBook::getBookReadLevel()
{
	Json::Value value0;
	value0["command"] = "book_getBookReadLevel";
	value0["id"] = id;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		if (value["result"].asString() == "1") {
			read_level = value["read_level"].asInt();
			return read_level;
		}
		else {
			if (value["result"].asString() == "PermissionDenied")
				setError(PermissionDenied, 0, "permission_denied");
			if (value["result"].asString() == "DatabaseError")
				setError(DatabaseError, 0, "database_error");
		}
	}
	else setError(NetworkError, 0, "network down");
	return false;
}
bool CBook::setBookReadLevel(int nReadLevel)
{
	Json::Value value0;
	value0["command"] = "book_setBookReadLevel";
	value0["id"] = id;
	value0["read_level"] = nReadLevel;
	Json::FastWriter writer;
	std::string strRequest;
	std::string strRespond;
	strRequest = writer.write(value0);
	if (sendRequest(strRequest, strRespond)) {
		Json::Reader reader;
		Json::Value value;
		reader.parse(strRespond, value);
		if (value["result"].asString() == "1") {
			read_level = nReadLevel;
			return true;
		}
		else {
			if (value["result"].asString() == "PermissionDenied")
				setError(PermissionDenied, 0, "permission_denied");
			if (value["result"].asString() == "DatabaseError")
				setError(DatabaseError, 0, "database_error");
		}
	}
	else setError(NetworkError, 0, "network down");
	return false;
}