#include "config.h"
#include "session.h"

#include <liblog.h>
#include <cassert>
#include <json/json.h>
#include <string>
#include <vector>
#include <memory>

#include "function_interfaces.h"

MODULE_LOG_NAME("Session");

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
	auto_iface<IUserManager> tem_user_manager;
	m_pClassFactory->getUserManager(&tem_user_manager);
	auto_iface<IUser> tem_user;
	if (!tem_user_manager->getUserByID(user_id, &tem_user))
		return 0;
	current_auth_level = tem_user->getAuthLevel();
	return current_auth_level;
}

int CSession::getCurrentReadLevel()
{
	auto_iface<IUserManager>  tem_user_manager;
	m_pClassFactory->getUserManager(&tem_user_manager);
	auto_iface<IUser>tem_user;
	if (!tem_user_manager->getUserByID(user_id, &tem_user))
		return 0;
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
#ifdef _DEBUG
	value["err-description"] = err.description;
#else
	value["err-description"] = "Operation Failed";
#endif
}

void CSession::recvRequest(const std::string& strRequest, std::string& strResponse)
{
	lprintf_d("RECV REQUEST %s", strRequest.c_str());

	try
	{
		if (user_id)
		{
			getCurrentAuthLevel();
			getCurrentReadLevel();
		}
		Json::Reader reader;
		Json::Value value0;
		reader.parse(strRequest, value0);
		std::string request = value0["command"].asString();
		Json::FastWriter writer;
		Json::Value value;

		lprintf("Recv Request, Command='%s', IP='%s'", request.c_str(), m_strClientIP.c_str());

		TAuthorization auth = { 0 };
		if (user_id)
			auth = g_configPolicy.vAuthList[current_auth_level];

		if (request == "book_getDescription") {
			if (!g_configPolicy.vAuthList[current_auth_level].auth_GetBookInfo) {
				value["result"] = "PermissionDenied";
				return;
			}
			int tem_id = value0["id"].asInt();
			auto_iface<ILibrary>  library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook>  book;
			if (library->queryById(tem_id, &book))
			{
				std::string tem_description;
				if (book->getDescription(tem_description)) {
					value["description"] = tem_description;
					value["result"] = "1";
				}
				else value["result"] = "DatabaseError";
			}
			else
				writeInterfaceError(value, library);
			strResponse = writer.write(value);
			return;
		}

		if (request == "book_setBasicInfo") {
			if (!user_id || !g_configPolicy.vAuthList[current_auth_level].auth_SetBookInfo) {
				value["result"] = "PermissionDenied";
				return;
			}
			TBookBasicInfo book_basic_info;
			
			auto_iface<ILibrary> library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook> book;
			if (library->queryById(value0["id"].asInt(), &book))
			{
				if (book->getBasicInfo(book_basic_info))
				{
					book_basic_info.count = value0["count"].asInt();
					book_basic_info.name = value0["name"].asString();
					book_basic_info.publisher = value0["publisher"].asString();
					book_basic_info.author = value0["author"].asString();
					book_basic_info.isbn = value0["isbn"].asString();
					if (book->setBasicInfo(book_basic_info))  value["result"] = "1";
					else value["result"] = "DatabaseError";
				}
				else
					writeInterfaceError(value, book);
			}
			else
				writeInterfaceError(value, book);
			strResponse = writer.write(value);
			return;
		}

		if (request == "book_setDescription") {
			if (!user_id || !g_configPolicy.vAuthList[current_auth_level].auth_SetBookInfo) {
				value["result"] = "PermissionDenied";
				return;
			}
			int tem_id = value0["id"].asInt();
			std::string tem_description = value0["description"].asString();

			auto_iface<ILibrary> library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook>book;
			if (library->queryById(tem_id, &book))
			{
				if (book->setDescription(tem_description.c_str())) value["result"] = "1";
				else value["result"] = "DatabaseError";
			}
			else
				writeInterfaceError(value, library);
			strResponse = writer.write(value);
			return;
		}

		if (request == "book_deleteBook") {
			if (!user_id || !g_configPolicy.vAuthList[current_auth_level].auth_DeleteBook) {
				value["result"] = "PermissionDenied";
				return;
			}
			int tem_id = value0["id"].asInt();
			int tem_num = value0["number"].asInt();

			auto_iface<ILibrary> library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook> book;
			if (library->queryById(tem_id, &book))
			{
				if (book->deleteBook(tem_num)) value["result"] = "1";
				else value["result"] = "DatabaseError";
			}
			else
				writeInterfaceError(value, library);
			strResponse = writer.write(value);
			return;
		}
		if (request == "book_getBorrowInfo") {
			if (!g_configPolicy.vAuthList[current_auth_level].auth_GetBookInfo) {
				value["result"] = "PermissionDenied";
				return;
			}
			int tem_id = value0["id"].asInt();
			auto_iface<ILibrary>  library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook> book;
			if (library->queryById(tem_id, &book))
			{
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
			}
			else
				writeInterfaceError(value, library);
			strResponse = writer.write(value);
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

			auto_iface<ILibrary> library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook> book;
			if (library->queryById(tem_id, &book))
			{
				if (book->getBookReadLevel() != -1) {
					value["read_level"] = book->getBookReadLevel();
					value["result"] = "1";
				}
				else value["result"] = "DatabaseError";
			}
			else
				writeInterfaceError(value, library);
			strResponse = writer.write(value);
			return;
		}

		if (request == "book_setBookReadLevel")
		{
			if (!user_id || !g_configPolicy.vAuthList[current_auth_level].auth_SetReadLevel) {
				value["result"] = "PermissionDenied";
				return;
			}
			int tem_id = value0["id"].asInt();
			int read_level0 = value0["read_level"].asInt();

			auto_iface<ILibrary> library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook> book;
			if (library->queryById(tem_id, &book))
			{
				if (book->setBookReadLevel(read_level0)) value["result"] = "1";
				else value["result"] = "DatabaseError";
			}
			else
				writeInterfaceError(value, library);
			strResponse = writer.write(value);
			return;
		}
		if (request == "library_queryByName") {
			std::string tem_name = value0["name"].asString();
			int nCount = value0["nCount"].asInt();
			int nTop = value0["nTop"].asInt();
			auto_iface<ILibrary>  library;
			m_pClassFactory->getLibrary(&library);
			IFvector book;
			int num = library->queryByName(tem_name.c_str(), book, nCount, nTop);
			Json::Value value;
			value[0] = num;
			for (int i = 0; i < num; i++)
			{
				TBookBasicInfo info;
				((IBook*)book[i])->getBasicInfo(info);
				Json::Value bookVal;
				bookVal["id"] = info.id;
				bookVal["count"] = info.count;
				bookVal["bcount"] = info.bcount;
				bookVal["name"] = info.name;
				bookVal["author"] = info.author;
				bookVal["isbn"] = info.isbn;
				bookVal["publisher"] = info.publisher;
				value[i + 1] = bookVal;
			}
			strResponse = writer.write(value);
			return;
		}

		if (request == "library_queryById") {
			int tem_id = value0["id"].asInt();
			auto_iface<ILibrary> library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook> book;
			if (library->queryById(tem_id, &book)) {
				TBookBasicInfo tem_book_basic_info;
				book->getBasicInfo(tem_book_basic_info);
				value["id"] = tem_book_basic_info.id;
				value["count"] = tem_book_basic_info.count;
				value["bcount"] = tem_book_basic_info.bcount;
				value["name"] = tem_book_basic_info.name;
				value["author"] = tem_book_basic_info.author;
				value["isbn"] = tem_book_basic_info.isbn;
				value["publisher"] = tem_book_basic_info.publisher;
				Json::FastWriter writer;
				strResponse = writer.write(value);
				value["result"] = "1";
			}
			else value["result"] = "DatabaseError";
			strResponse = writer.write(value);
			return;
		}

		if (request == "library_queryByISBN") {
			std::string tem_ISBN = value0["isbn"].asString();
			auto_iface<ILibrary>  library;
			m_pClassFactory->getLibrary(&library);
			auto_iface<IBook> book;
			if (library->queryByISBN(tem_ISBN.c_str(), &book)) {
				TBookBasicInfo tem_book_basic_info;
				book->getBasicInfo(tem_book_basic_info);
				value["id"] = tem_book_basic_info.id;
				value["count"] = tem_book_basic_info.count;
				value["bcount"] = tem_book_basic_info.bcount;
				value["name"] = tem_book_basic_info.name;
				value["author"] = tem_book_basic_info.author;
				value["isbn"] = tem_book_basic_info.isbn;
				value["publisher"] = tem_book_basic_info.publisher;
				value["result"] = "1";
				Json::FastWriter writer;
				strResponse = writer.write(value);
			}
			else value["result"] = "DatabaseError";
			strResponse = writer.write(value);
			return;
		}

		if (request == "library_insertBook")
		{
			if (!user_id || !auth.auth_InsertBook)
				writePermissionDenied(value);
			else
			{
				TBookBasicInfo book_basic_info;
				book_basic_info.bcount = 0;
				book_basic_info.count = value0["count"].asInt();
				book_basic_info.name = value0["name"].asString();
				book_basic_info.publisher = value0["publisher"].asString();
				book_basic_info.author = value0["author"].asString();
				book_basic_info.isbn = value0["isbn"].asString();
				auto_iface<ILibrary> library;
				m_pClassFactory->getLibrary(&library);
				auto_iface<IBook> book;
				m_pClassFactory->createEmptyBook(&book);
				book->setBasicInfo(book_basic_info);
				book->setDescription(value0["description"].asCString());
				if (library->insertBook(book))
					value["result"] = 1;
				else
					writeInterfaceError(value, library);
			}
			strResponse = writer.write(value);
			return;
		}



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
			if (!user_id || (tem_id != user_id && !auth.auth_GetUserInfo))
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
			if (!user_id || (req_id != user_id && !auth.auth_SetUserInfo))
				writePermissionDenied(value);
			else
			{
				auto_iface<IUserManager> usermanager;
				m_pClassFactory->getUserManager(&usermanager);
				auto_iface<IUser> user;
				if (usermanager->getUserByID(req_id, &user))
				{
					TUserBasicInfo tem_user_basic_info;
					if (user->getBasicInfo(tem_user_basic_info))
					{
						tem_user_basic_info.gender = value0["gender"].asInt();
						tem_user_basic_info.email = value0["email"].asString();
						if (user->setBasicInfo(tem_user_basic_info))
							value["result"] = 1;
						else
							writeInterfaceError(value, user);
					}
					else
						writeInterfaceError(value, user);
				}
				else
					writeInterfaceError(value, usermanager);
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
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;
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

		if (request == "user_getBorrowedBooks")
		{
			int tem_id = value0["id"].asInt();
			if (!user_id || (tem_id != user_id && !auth.auth_GetUserInfo))
				writePermissionDenied(value);
			else
			{
				auto_iface<IUserManager> usermanager;
				m_pClassFactory->getUserManager(&usermanager);
				auto_iface<IUser> user;
				if (usermanager->getUserByID(tem_id, &user))
				{
					auto_iface<ILibrary>  library;
					m_pClassFactory->getLibrary(&library);

					std::vector<TBorrowInfo> tem_binfo;
					if (user->getBorrowedBooks(tem_binfo))
					{
						value["result"] = 1;
						Json::Value valueData;
						int num_of_records = tem_binfo.size();
						valueData[0] = num_of_records;
						Json::Value borinfoVal;
						for (int i = 0; i < num_of_records; i++) {
							borinfoVal[1] = tem_binfo[i].bookID;
							borinfoVal[2] = tem_binfo[i].borrowTime;
							borinfoVal[3] = tem_binfo[i].flag;
							valueData[i + 1] = borinfoVal;
						}
						value["data"] = valueData;
					}
					else
						writeInterfaceError(value, user.abs_iface());
				}
				else
					writeInterfaceError(value, usermanager.abs_iface());
			}
			strResponse = writer.write(value);
			return;
		}

		if (request == "user_borrowBook")
		{
			int tem_id = value0["userid"].asInt();
			if (!user_id || tem_id != user_id || !auth.auth_BorrowBook)
				writePermissionDenied(value);
			else
			{
				auto_iface<IUserManager> usermanager;
				m_pClassFactory->getUserManager(&usermanager);
				auto_iface<IUser> user;
				if (usermanager->getUserByID(tem_id, &user))
				{
					auto_iface<ILibrary> library;
					m_pClassFactory->getLibrary(&library);
					auto_iface<IBook> book;
					if (library->queryById(value0["bookid"].asInt(), &book))
					{
						if (user->borrowBook(book))
						{
							value["result"] = 1;
						}
						else
							writeInterfaceError(value, user);
					}
					else
						writeInterfaceError(value, library);
				}
				else
					writeInterfaceError(value, usermanager);
			}
			strResponse = writer.write(value);
			return;
		}

		if (request == "user_returnBook")
		{
			int tem_id = value0["userid"].asInt();
			if (!user_id || tem_id != user_id || !auth.auth_BorrowBook)
				writePermissionDenied(value);
			else
			{
				auto_iface<IUserManager> usermanager;
				m_pClassFactory->getUserManager(&usermanager);
				auto_iface<IUser> user;
				if (usermanager->getUserByID(tem_id, &user))
				{
					auto_iface<ILibrary>  library;
					m_pClassFactory->getLibrary(&library);
					auto_iface<IBook> book;
					if (library->queryById(value0["bookid"].asInt(), &book))
					{
						if (user->returnBook(book))
						{
							value["result"] = 1;
						}
						else
							writeInterfaceError(value, user);
					}
					else
						writeInterfaceError(value, library);
				}
				else
					writeInterfaceError(value, usermanager);
			}
			strResponse = writer.write(value);
			return;
		}

		if (request == "user_deleteUser") {
			int tem_id = value0["id"].asInt();
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;
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

		if (request == "user_getAuthLevel")
		{
			int uID = value0["id"].asInt();
			if (!user_id || (uID != user_id && !auth.auth_GetUserInfo))
				writePermissionDenied(value);
			else
			{
				auto_iface<IUserManager> userManager;
				m_pClassFactory->getUserManager(&userManager);
				auto_iface<IUser> user;
				if (userManager->getUserByID(uID, &user))
				{
					int nAuthLevel = user->getAuthLevel();
					if (nAuthLevel >= 0)
					{
						value["result"] = 1;
						value["AuthLevel"] = nAuthLevel;
					}
					else
						writeInterfaceError(value, user);
				}
				else
					writeInterfaceError(value, userManager);
			}
			strResponse = writer.write(value);
			return;
		}

		if (request == "user_setAuthLevel")
		{
			if (!user_id || !auth.auth_SetAuthLevel)
				writePermissionDenied(value);
			else
			{
				auto_iface<IUserManager> userManager;
				m_pClassFactory->getUserManager(&userManager);
				auto_iface<IUser> user;
				if (userManager->getUserByID(value0["id"].asInt(), &user))
				{
					if (user->setAuthLevel(value0["AuthLevel"].asInt()))
						value["result"] = 1;
					else
						writeInterfaceError(value, user);
				}
				else
					writeInterfaceError(value, userManager);
			}
			strResponse = writer.write(value);
			return;
		}


		if (request == "authmanager_Login")
		{
			auto_iface<IUserManager> userManager;
			auto_iface<IUser> user;

			m_pClassFactory->getUserManager(&userManager);

			if (userManager->getUserByName(value0["username"].asCString(), &user))
			{
				if (!user->verifyPassword(value0["password"].asCString()))
				{
					lprintf("Failed to Login(Invalid Password), name='%s', IP='%s'", value0["username"].asCString(), m_strClientIP.c_str());
					value["result"] = 2;
				}
				else
				{
					TUserBasicInfo info;
					if (!user->getBasicInfo(info))
						value["result"] = 2;
					else
					{
						lprintf("Login Successfully, id=%d, name='%s', IP='%s'", info.id, info.name.c_str(), m_strClientIP.c_str());
						value["result"] = 1;
						value["id"] = info.id;
						user_id = info.id;
					}
				}
			}
			else
				writeInterfaceError(value, userManager);

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

		if (request == "authmanager_Register")
		{
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;

			m_pClassFactory->createEmptyUser(&user);
			TUserBasicInfo info;
			info.name = value0["name"].asString();
			info.email = value0["email"].asString();
			info.gender = value0["gender"].asInt();
			info.num = g_configPolicy.nDefaultBorrowLimit;
			user->setBasicInfo(info);
			user->setPassword(value0["password"].asString().c_str());

			if (!usermanager->insertUser(user))
				writeInterfaceError(value, usermanager);
			else
			{
				lprintf("Register Successfully, name='%s', IP='%s'", value0["name"].asCString(), m_strClientIP.c_str());
				value["result"] = 1;
			}

			strResponse = writer.write(value);
			return;
		}

		if (request == "authmanager_changePassword") {
			int tem_id = value0["id"].asInt();
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;
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
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;
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
			auto_iface<IUserManager> usermanager;
			m_pClassFactory->getUserManager(&usermanager);
			auto_iface<IUser> user;
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

		if (request == "authmanager_getLicense")
		{
			value["result"] = 1;
			value["license"] = g_configPolicy.strLicense;
			strResponse = writer.write(value);
			return;
		}
	}
	catch (std::exception& e)
	{
		lprintf_e("An error occurred while parsing request: %s", e.what());
	}
}
