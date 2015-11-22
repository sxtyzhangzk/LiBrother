#include "user.h"
#include "netclient.h"
#include "authmanager.h"
#include "book.h"
#include "utils.h"

#include <json/json.h>

CUser::CUser() : 
	m_pBasicInfo(nullptr),
	m_nID(0),
	m_nAuthLevel(-1), m_nReadLevel(-1)
{
}

CUser::CUser(int nID) :
	m_pBasicInfo(nullptr),
	m_nID(nID),
	m_nAuthLevel(-1), m_nReadLevel(-1)
{
}

CUser::CUser(const TUserBasicInfo& info) :
	m_nAuthLevel(-1), m_nReadLevel(-1)
{
	m_nID = info.id;
	m_pBasicInfo = new TUserBasicInfo;
	*m_pBasicInfo = info;
}

CUser::~CUser()
{
	if (m_pBasicInfo)
		delete m_pBasicInfo;
}

bool CUser::getBasicInfo(TUserBasicInfo& info)
{
	if (m_pBasicInfo)
	{
		info = *m_pBasicInfo;
		return true;
	}
	if (!m_nID)
	{
		setError(Other, -3, "Basic Info not set");
		return false;
	}

	Json::Value valueReq, valueRes;
	valueReq["command"] = "user_getBasicInfo";
	valueReq["id"] = m_nID;
	
	Json::FastWriter writer;
	std::string strRequest, strResponse;
	strRequest = writer.write(valueReq);

	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	try
	{
		TUserBasicInfo info;
		reader.parse(strResponse, valueRes);
		if (valueRes["result"].asInt() == 1)
		{
			info.id = m_nID;
			info.name = valueRes["name"].asString();
			info.email = valueRes["email"].asString();
			info.gender = valueRes["gender"].asInt();
			info.num = valueRes["book_num"].asInt();
			m_pBasicInfo = new TUserBasicInfo;
			*m_pBasicInfo = info;
			return true;
		}
	}
	catch (std::exception& e)
	{
		setError(NetworkError, 2, "Failed to parse response");
		return false;
	}

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CUser::setBasicInfo(const TUserBasicInfo& info)
{
	if (!m_pBasicInfo)
		m_pBasicInfo = new TUserBasicInfo;
	*m_pBasicInfo = info;
	m_pBasicInfo->id = m_nID;
	if (!m_nID)
		return true;

	Json::Value value0;
	value0["command"] = "user_setBasicInfo";
	value0["id"] = m_pBasicInfo->id;
	value0["gender"] = m_pBasicInfo->gender;
	value0["name"] = m_pBasicInfo->name;
	value0["email"] = m_pBasicInfo->email;

	Json::FastWriter writer;
	std::string strRequest, strResponse;
	strRequest = writer.write(value0);

	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	Json::Value value;

	int result;

	try
	{
		reader.parse(strResponse, value);
		result = value["result"].asInt();
	}
	catch (std::exception& e)
	{
		setError(NetworkError, 2, "Failed to parse response");
		return false;
	}

	if (result == 1)
		return true;

	setError(Other, 3, "Operation Failed");
	return false;
}

bool CUser::verifyPassword(const char * strPWD)
{
	setError(UnsupportedMethod, -1, "Unsupported Method verifyPassword");
	return false;
}

bool CUser::setPassword(const char * strPWD)
{
	m_strEPassword = CAuthManager::encryptPassword(strPWD);
	if (!m_nID)
		return true;
	
	Json::Value value0;
	value0["command"] = "user_setPassword";//may require revision
	value0["id"] = m_nID;
	value0["password"] = m_strEPassword;

	Json::FastWriter writer;
	std::string strRequest, strResponse;
	strRequest = writer.write(value0);

	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	Json::Value value;
	int result;

	try
	{
		reader.parse(strResponse, value);
		result = value["result"].asInt();
	}
	catch (std::exception& e)
	{
		setError(NetworkError, 2, "Failed to parse response");
		return false;
	}

	if (result == 1)
		return true;

	setError(Other, 3, "Operation Failed");
	return false;
}

bool CUser::getBorrowedBooks(std::vector<TBorrowInfo> &binfo)//to be implemented
{
	if (!m_nID)
	{
		setError(InvalidParam, -2, "Cannot getBorrowedBook from a temp object");
		return false;
	}

	Json::Value value0;
	value0["command"] = "user_getBorrowedBooks";
	value0["id"] = m_nID;

	Json::FastWriter writer;
	std::string strRequest;
	std::string strResponse;
	strRequest = writer.write(value0);

	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	Json::Value value;

	try
	{
		reader.parse(strResponse, value);
		int num = value[0].asInt();
		if (num > 0)
		{
			TBorrowInfo borrow_info;
			for (int i = 1; i <= num; i++)
			{
				Json::Value tem_value = value[i];
				borrow_info.bookID = tem_value[1].asInt();
				borrow_info.borrowTime = tem_value[2].asInt64();
				borrow_info.flag = tem_value[3].asBool();
				binfo.push_back(borrow_info);
			}
		}
		else
		{
			setError(Other, -1, "Operation Failed");
			return false;
		}
	}
	catch (std::exception& e)
	{
		setError(NetworkError, 2, "Failed to parse response");
		return false;
	}

	return true;
}

bool CUser::borrowBook(IBook * pBook)
{
	CHECK_PTR(pBook);
	CBook *book = dynamic_cast<CBook*>(pBook);
	if (!m_nID || !book->id)
	{
		setError(InvalidParam, -2, "Cannot borrowBook from a temp object");
		return false;
	}

	Json::Value value0;
	value0["command"] = "user_borrowBook";//may require revision
	value0["userid"] = m_nID;
	value0["bookid"] = book->id;

	Json::FastWriter writer;
	std::string strRequest, strResponse;
	strRequest = writer.write(value0);

	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	Json::Value value;

	try
	{
		reader.parse(strResponse, value);
		if (value["result"].asInt() == 1)
			return true;
	}
	catch (std::exception& e)
	{
		setError(NetworkError, 2, "Failed to parse response");
		return false;
	}

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CUser::returnBook(IBook * pBook)
{
	CHECK_PTR(pBook);
	CBook *book = dynamic_cast<CBook*>(pBook);
	if (!m_nID || !book->id)
	{
		setError(InvalidParam, -2, "Cannot returnBook from a temp object");
		return false;
	}

	Json::Value value0;
	value0["command"] = "user_returnBook";//may require revision
	value0["userid"] = m_nID;
	value0["bookid"] = book->id;

	Json::FastWriter writer;
	std::string strRequest, strResponse;
	strRequest = writer.write(value0);

	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	Json::Value value;

	try
	{
		reader.parse(strResponse, value);
		if (value["result"].asInt() == 1)
			return true;
	}
	catch (std::exception& e)
	{
		setError(NetworkError, 2, "Failed to parse response");
		return false;
	}

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CUser::deleteUser()
{
	if (!m_nID)
	{
		setError(InvalidParam, -2, "Cannot deleteUser from a temp object");
		return false;
	}

	Json::Value value0;
	value0["command"] = "user_deleteUser";
	value0["id"] = m_nID;

	Json::FastWriter writer;
	std::string strRequest, strResponse;
	strRequest = writer.write(value0);

	TRY_SEND_REQUEST(strRequest, strResponse);

	Json::Reader reader;
	Json::Value value;

	try
	{
		reader.parse(strResponse, value);
		if (value["result"].asInt() == 1) 
			return true;
	}
	catch (std::exception& e)
	{
		setError(NetworkError, 2, "Failed to parse response");
		return false;
	}

	setError(Other, -1, "Operation Failed");
	return false;
}

int CUser::getAuthLevel()
{
	if (!m_nID)
	{
		setError(InvalidParam, -2, "Cannot getAuthLevel from a temp object");
		return -1;
	}
	if (m_nAuthLevel >= 0)
		return m_nAuthLevel;

	Json::Value valueReq, valueRes;
	valueReq["command"] = "user_getAuthLevel";
	valueReq["id"] = m_nID;

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		int authLevel = valueRes["AuthLevel"].asInt();
		if (authLevel >= 0)
		{
			m_nAuthLevel = authLevel;
			return authLevel;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return -1;
}

int CUser::getReadLevel()
{
	if (!m_nID)
	{
		setError(InvalidParam, -2, "Cannot getReadLevel from a temp object");
		return -1;
	}
	if (m_nReadLevel >= 0)
		return m_nReadLevel;

	Json::Value valueReq, valueRes;
	valueReq["command"] = "user_getReadLevel";
	valueReq["id"] = m_nID;

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		int readLevel = valueRes["ReadLevel"].asInt();
		if (readLevel >= 0)
		{
			m_nReadLevel = readLevel;
			return readLevel;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return -1;
}

bool CUser::setAuthLevel(int nAuthLevel)
{
	if (nAuthLevel < 0)
	{
		setError(InvalidParam, -2, "AuthLevel must be positive");
		return false;
	}
	if (!m_nID)
	{
		setError(InvalidParam, -2, "Cannot setAuthLevel to a temp object");
		return false;
	}

	Json::Value valueReq, valueRes;
	valueReq["command"] = "user_setAuthLevel";
	valueReq["id"] = m_nID;
	valueReq["AuthLevel"] = nAuthLevel;

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		if (valueRes["result"] == 1)
		{
			m_nAuthLevel = nAuthLevel;
			return true;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return false;
}

bool CUser::setReadLevel(int nReadLevel)
{
	if (nReadLevel < 0)
	{
		setError(InvalidParam, -2, "ReadLevel must be positive");
		return false;
	}
	if (!m_nID)
	{
		setError(InvalidParam, -2, "Cannot setReadLevel to a temp object");
		return false;
	}

	Json::Value valueReq, valueRes;
	valueReq["command"] = "user_setReadLevel";
	valueReq["id"] = m_nID;
	valueReq["ReadLevel"] = nReadLevel;

	BEGIN_PARSE_RESPONSE(valueReq, valueRes)
	{
		if (valueRes["result"] == 1)
		{
			m_nReadLevel = nReadLevel;
			return true;
		}
	}
	END_PARSE_RESPONSE;

	setError(Other, -1, "Operation Failed");
	return false;
}