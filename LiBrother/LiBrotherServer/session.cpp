#include "config.h"
#include "session.h"

#include <cassert>
#include <json/json.h>
#include <string>
#include <vector>
#include <memory>

#include "function_interfaces.h"


CSession::CSession() :m_pClassFactory(nullptr) {}
bool CSession::startSession(ILibClassFactory * pClassFactory, const std::string& strClientIP)
{
	if (pClassFactory) {
		m_pClassFactory = pClassFactory;
		user_id = 0;
		return true;
	}
	return false;
}
bool CSession::stopSession()
{
	return true;
}

int CSession::getCurrentAuthLevel()
{
	IUserManager * tem_user_manager;
	m_pClassFactory->getUserManager(&tem_user_manager);
	IUser *tem_user;
	tem_user_manager->getUserByID(user_id, &tem_user);
	current_auth_level = tem_user->getAuthLevel();
	return current_auth_level;
}

int CSession::getCurrentReadLevel()
{
	IUserManager * tem_user_manager;
	m_pClassFactory->getUserManager(&tem_user_manager);
	IUser *tem_user;
	tem_user_manager->getUserByID(user_id, &tem_user);
	current_read_level = tem_user->getReadLevel();
	return current_read_level;
}


void CSession::writePermissionDenied(Json::Value& value)
{
	value["result"] = -1;
	value["err-code"] = 1;
	value["err-description"] = "Permission Denied";
}

void CSession::writeInterfaceError(Json::Value& value, IAbstract *pIface)
{
	assert(pIface);
	TErrInfo err = pIface->GetError();
	value["result"] = 0;
	value["err-code"] = err.code;
	value["err-description"] = err.description;
}

void CSession::recvRequest(const std::string& strRequest, std::string& strResponse)
{
	getCurrentAuthLevel();
	getCurrentReadLevel();
	Json::Reader reader;
	Json::Value value0;
	reader.parse(strRequest,value0);
	std::string request = value0["command"].asString();
	Json::FastWriter writer;
	Json::Value value;

	if (request == "book_getDescription") {
		if (!g_configPolicy.vAuthList[current_auth_level].auth_GetBookInfo) {
			value["result"] = "PermissionDenied";
			return;
		}
		int tem_id=value0["id"].asInt();
		auto_iface<ILibrary> library;
		m_pClassFactory->getLibrary(&library);
		auto_iface<IBook> book;
		library->queryById(tem_id,&book);
		std::string tem_description;
		if (book->getDescription(tem_description)) {
			value["description"] = tem_description;
			value["result"] = '1';
			strResponse = writer.write(value);
		}
		else value["result"] = "DatabaseError";
		return;
	}
		
	if (request == "book_setBasicInfo") {
		if (!g_configPolicy.vAuthList[current_auth_level].auth_SetBookInfo) {
			value["result"] = "PermissionDenied";
			return;
		}
		TBookBasicInfo book_basic_info;
		book_basic_info.id = value0["id"].asInt();
		book_basic_info.count = value0["count"].asInt();
		book_basic_info.name = value0["name"].asString();
		book_basic_info.publisher = value0["publisher"].asString();
		book_basic_info.author = value0["author"].asString();
		book_basic_info.isbn = value0["isbn"].asString();
		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(book_basic_info.id, &book);
		if (book->setBasicInfo(book_basic_info))  value["result"] = '1';
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}
		
	if (request == "book_setDescription") {
		if (!g_configPolicy.vAuthList[current_auth_level].auth_SetBookInfo) {
			value["result"] = "PermissionDenied";
			return;
		}
		int tem_id = value0["id"].asInt();
		 std::string tem_description = value0["description"].asString();
		
		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		if (book->setDescription(tem_description.c_str())) value["result"] = '1';
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}

	if (request == "book_deleteBook") {
		if (!g_configPolicy.vAuthList[current_auth_level].auth_DeleteBook) {
			value["result"] = "PermissionDenied";
			return;
		}
		int tem_id = value0["id"].asInt();
		int tem_num = value0["number"].asInt();

		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		if (book->deleteBook(tem_num)) value["result"] = '1';
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}
	if (request == "book_getBorrowInfo") {
		if (!g_configPolicy.vAuthList[current_auth_level].auth_GetBookInfo) {
			value["result"] = "PermissionDenied";
			return;
		}
		int tem_id = value0["id"].asInt();
		ILibrary  *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		std::vector<TBorrowInfo> tem_binfo;
		book->getBorrowInfo(tem_binfo);
		Json::FastWriter writer;
		int num_of_records = tem_binfo.size();
		value[0] = num_of_records;
		for (int i = 0; i < num_of_records; i++) {
			Json::Value borinfoVal;
			borinfoVal["bookID"] = tem_binfo[i].bookID;
			borinfoVal["userID"] = tem_binfo[i].userID;
			borinfoVal["flag"] = tem_binfo[i].flag;
			borinfoVal["borrowTime"] = tem_binfo[i].borrowTime;
			value[i + 1] = borinfoVal;
		}
		return;
	}

	if (request == "book_getBookReadLevel")
	{
		if (!g_configPolicy.vAuthList[current_auth_level].auth_GetBookInfo) {
			value["result"] = "PermissionDenied";
			return;
		}
		int tem_id = value0["id"].asInt();
		int tem_num = value0["number"].asInt();

		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		if (book->getBookReadLevel() != -1) {
			value["read_level"] = book->getBookReadLevel();
			value["result"] = '1';
		}
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}

	if (request == "book_setBookReadLevel")
	{
		if (!g_configPolicy.vAuthList[current_auth_level].auth_SetReadLevel) {
			value["result"] = "PermissionDenied";
			return;
		}
		int tem_id = value0["id"].asInt();
		int read_level0 = value0["read_level"].asInt();

		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		if (book->setBookReadLevel(read_level0)) value["result"] = '1';
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}
	if (request == "library_queryByName") {
		std::string tem_name = value0["name"].asString();
		int nCount = value0["nCount"].asInt();
		int nTop = value0["nTop"].asInt();
		ILibrary  *library;
		m_pClassFactory->getLibrary(&library);
		IFvector book;
		int num=library->queryByName(tem_name.c_str(), book, nCount, nTop);
		Json::Value value;
		value[0] = num;
		for (int i = 0; i < num; i++)
		{
			TBookBasicInfo info;
			((IBook*)book[i])->getBasicInfo(info);
			Json::Value bookVal;
			bookVal["id"] = info.id;
			bookVal["count"] = info.count;
			bookVal["name"] = info.name;
			bookVal["author"] = info.author;
			bookVal["isbn"] = info.isbn;
			bookVal["publisher"] = info.publisher;
			value[i+1] = bookVal;
		}
		strResponse = writer.write(value);
		return;
	}

	if (request == "library_queryById") {
		int tem_id = value0["id"].asInt();
		ILibrary  *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		if (library->queryById(tem_id, &book)) {
			TBookBasicInfo tem_book_basic_info;
			book->getBasicInfo(tem_book_basic_info);
			value["id"] = tem_book_basic_info.id;
			value["count"] = tem_book_basic_info.count;
			value["name"] = tem_book_basic_info.name;
			value["author"] = tem_book_basic_info.author;
			value["isbn"] = tem_book_basic_info.isbn;
			value["publisher"] = tem_book_basic_info.publisher;
			Json::FastWriter writer;
			strResponse = writer.write(value);
			value["result"] = '1';
		}
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}
	
	if (request == "library_queryByISBN") {
		std::string tem_ISBN = value0["isbn"].asString();
		ILibrary  *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		if (library->queryByISBN(tem_ISBN.c_str(), &book)) {
			TBookBasicInfo tem_book_basic_info;
			book->getBasicInfo(tem_book_basic_info);
			value["id"] = tem_book_basic_info.id;
			value["count"] = tem_book_basic_info.count;
			value["name"] = tem_book_basic_info.name;
			value["author"] = tem_book_basic_info.author;
			value["isbn"] = tem_book_basic_info.isbn;
			value["publisher"] = tem_book_basic_info.publisher;
			value["result"] = '1';
			Json::FastWriter writer;
			strResponse = writer.write(value);
		}
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}

	if (request == "library_insertBook") {
		TBookBasicInfo book_basic_info;
		book_basic_info.id = value0["id"].asInt();
		book_basic_info.count = value0["count"].asInt();
		book_basic_info.name = value0["name"].asString();
		book_basic_info.publisher = value0["publisher"].asString();
		book_basic_info.author = value0["author"].asString();
		book_basic_info.isbn = value0["isbn"].asString();
		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		m_pClassFactory->createEmptyBook(&book);
		book->setBasicInfo(book_basic_info);
		if (library->insertBook(book))  value["result"] = '1';
		else value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}

	TAuthorization auth = { 0 };
	if (user_id)
		auth = g_configPolicy.vAuthList[current_auth_level];

	if (request == "usermanager_getUserByName")
	{
		if (!auth.auth_GetUserInfo)
			writePermissionDenied(value);
		else
		{

			std::string tem_name = value0["name"].asString();
			
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;
			if (usermanager->getUserByName(tem_name.c_str(), &user))
			{
				TUserBasicInfo tem_user_basic_info;
				user->getBasicInfo(tem_user_basic_info);
				value["id"] = tem_user_basic_info.id;
				value["gender"] = tem_user_basic_info.gender;
				value["name"] = tem_user_basic_info.name;
				value["email"] = tem_user_basic_info.email;
				value["book_num"] = tem_user_basic_info.num;

				value["result"] = 1;
			}
			else
				writeInterfaceError(value, usermanager.abs_iface());
		}
		strResponse = writer.write(value);
		return;
	}

	if (request == "usermanager_insertUser")
	{
		if (!auth.auth_InsertUser)
			writePermissionDenied(value);
		else
		{
			TUserBasicInfo tem_user_basic_info;
			tem_user_basic_info.gender = value0["gender"].asInt();
			tem_user_basic_info.name = value0["name"].asString();
			tem_user_basic_info.email = value0["email"].asString();

			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;
			m_pClassFactory->createEmptyUser(&user);
			user->setBasicInfo(tem_user_basic_info);
			if (usermanager->insertUser(user))
				value["result"] = 1;
			else
				writeInterfaceError(value, usermanager.abs_iface());
		}
		strResponse = writer.write(value);
		return;
	}

	if (request == "user_getBasicInfo")
	{
		int tem_id = value0["id"].asInt();
		if (tem_id != user_id && !auth.auth_GetUserInfo)
			writePermissionDenied(value);
		else
		{
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;

			if (usermanager->getUserByID(tem_id, &user))
			{
				TUserBasicInfo tem_user_basic_info;
				user->getBasicInfo(tem_user_basic_info);
				value["id"] = tem_user_basic_info.id;
				value["gender"] = tem_user_basic_info.gender;
				value["name"] = tem_user_basic_info.name;
				value["email"] = tem_user_basic_info.email;
				value["result"] = 1;
			}
			else
				writeInterfaceError(value, usermanager.abs_iface());
		}
		strResponse = writer.write(value);
		return;
	}
	
	if (request == "user_setBasicInfo")
	{
		int req_id = value0["id"].asInt();
		if (req_id != user_id && !auth.auth_SetUserInfo)
			writePermissionDenied(value);
		else
		{
			TUserBasicInfo tem_user_basic_info;
			tem_user_basic_info.gender = value0["gender"].asInt();
			tem_user_basic_info.name = value0["name"].asString();
			tem_user_basic_info.email = value0["email"].asString();

			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			IUser *user;
			usermanager->getUserByID(tem_user_basic_info.id, &user);
			if (user->setBasicInfo(tem_user_basic_info)) value["result"] = '1';
			else value["result"] = "DatabaseError";
		}
		strResponse = writer.write(value);
		return;
	}

	
	if (request == "user_verifyPassword") {
		value["result"] = 0;
		strResponse = writer.write(value);
		return;
	}

	if (request == "user_setPassword") {
		int tem_id = value0["id"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			std::string newPWD = value0["password"].asString();
			const char *pnewPWD = newPWD.c_str();
			if (user->setPassword(pnewPWD)) {
				value["result"] = 1;
				strResponse = writer.write(value);
				return;
			}
			value["result"] = 0;
		}
		strResponse = writer.write(value);
		return;
	}

	if (request == "user_getBorrowedBooks") {
		int tem_id = value0["id"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			ILibrary  *library;
			m_pClassFactory->getLibrary(&library);
			std::vector<TBorrowInfo> tem_binfo;
			user->getBorrowedBooks(tem_binfo);
			int num_of_records = tem_binfo.size();
			value[0] = num_of_records;
			Json::Value borinfoVal;
			for (int i = 0; i < num_of_records; i++) {
				borinfoVal[1] = tem_binfo[i].bookID;
				borinfoVal[2] = tem_binfo[i].borrowTime;
				borinfoVal[3] = tem_binfo[i].flag;
				value[i + 1] = borinfoVal;
			}
			value[0] = 0;
		}
		strResponse = writer.write(value);
		return;
	}

	if (request == "user_borrowBook") {
		int tem_id = value0["userid"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			ILibrary  *library;
			m_pClassFactory->getLibrary(&library);
			IBook *book;
			if (library->queryById(value["bookid"].asInt(), &book)) {
				if (user->borrowBook(book)) {
					value["result"] = 1;
					strResponse = writer.write(value);
					return;
				}
			}
		}
		value["result"] = 0;
		strResponse = writer.write(value);
		return;
	}

	if (request == "user_returnBook") {
		int tem_id = value0["userid"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			ILibrary  *library;
			m_pClassFactory->getLibrary(&library);
			IBook *book;
			if (library->queryById(value["bookid"].asInt(), &book)) {
				if (user->returnBook(book)) {
					value["result"] = 1;
					strResponse = writer.write(value);
					return;
				}
			}
		}
		value["result"] = 0;
		strResponse = writer.write(value);
		return;
	}

	if (request == "user_deleteUser") {
		int tem_id = value0["id"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			value["result"] = 1;
			user->deleteUser();
			strResponse = writer.write(value);
			return;
		}
		value["result"] = 0;
		strResponse = writer.write(value);
		return;
	}


	if (request == "authmanager_Login") {
		std::string tem_name = value0["userid"].asString();
		std::string tem_password = value0["usepassword"].asString();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByName(tem_name.c_str(), &user)) {
			TUserBasicInfo tem_user_basic_info;
			user->getBasicInfo(tem_user_basic_info);
			const char *pwd = tem_password.c_str();
			if (user->verifyPassword(pwd)) {
				/*if (tem_user_basic_info->LoginStatus) {
					value["result"] = "Multipled log in";				//验证重复登入
					strResponse = writer.write(value);
					return;
				}*/
				value["result"] = 1;
				value["name"] = tem_user_basic_info.name;
				value["email"] = tem_user_basic_info.email;
				value["id"] = tem_user_basic_info.id;
				value["gender"] = tem_user_basic_info.gender;
				strResponse = writer.write(value);
				return;
			}
		}
		value["result"] = "DatabaseError";
		strResponse = writer.write(value);
		return;
	}

	if (request == "authmanager_Logout")
	{
		user_id = 0;
		value["result"] = 1;
		strResponse = writer.write(value);
		return;
	}

	if (request == "authmanager_Register") {
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		std::string tem_name = value0["name"].asString();
		if (usermanager->getUserByName(tem_name.c_str(), &user)) {
			value["result"] = 0;
			strResponse = writer.write(value);
			return;
		}
		std::string tem_email = value0["rmail"].asString();
		if (usermanager->getUserByName(tem_email.c_str(), &user)) {
			value["result"] = -1;
			strResponse = writer.write(value);
			return;
		}
		int newid = 1;
		while (usermanager->getUserByID(newid, &user)) ++newid;
		m_pClassFactory->createEmptyUser(&user);
		TUserBasicInfo info;
		info.id = newid;
		info.email = tem_email;
		info.name = tem_name;
		info.gender = value["gender"].asInt();
		user->setBasicInfo(info);
		value["result"] = 1;
		value["id"] = newid;
		strResponse = writer.write(value);
		return;
	}

	if (request == "authmanager_changePassword") {
		int tem_id = value0["id"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			std::string oldPWD = value0["oldPWD"].asString();
			const char *poldPWD = oldPWD.c_str();
			if (user->verifyPassword(poldPWD)) {
				std::string newPWD = value0["newPWD"].asString();
				const char *pnewPWD = newPWD.c_str();
				if (user->setPassword(pnewPWD)) {
					value["result"] = 1;
					strResponse = writer.write(value);
					return;
				}
				value["result"] = -2;
				strResponse = writer.write(value);
				return;
			}
		}
		value["result"] = -1;
		strResponse = writer.write(value);
		return;
	}

	if (request == "authmanager_getCurrentUser") {
		int tem_id = value0["id"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			TUserBasicInfo Info;
			user->getBasicInfo(Info);
			value["result"] = 1;
			value["email"] = Info.email;
			value["gender"] = Info.gender;
			value["id"] = Info.id;
			value["name"] = Info.name;
			strResponse = writer.write(value);
			return;
		}
		value["result"] = 0;
		strResponse = writer.write(value);
		return;
	}

	if (request == "authmanager_getAuthLevel") {
		int tem_id = value0["id"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id, &user)) {
			int AuthLevel = user->getAuthLevel();
			if (AuthLevel == -1) {
				value["result"] = -1;
				strResponse = writer.write(value);
				return;
			}
			value["result"] = 1;
			value["AuthLevel"] = AuthLevel;
			strResponse = writer.write(value);
			return;
		}
		value["result"] = -1;
		strResponse = writer.write(value);
		return;
	}
}