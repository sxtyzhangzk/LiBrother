#include "session.h"
#include<json/json.h>
#include<string>

#include "function_interfaces.h"
#include "common_types.h"
CSession::CSession() :m_pClassFactory(nullptr) {}
bool CSession::startSession(ILibClassFactory * pClassFactory, const std::string& strClientIP)
{
	if (pClassFactory) {
		m_pClassFactory = pClassFactory;
		return true;
	}
	return false;
}
bool CSession::stopSession()
{
	return true;
}
void CSession::recvRequest(const std::string& strRequest, std::string& strResponse)
{
	Json::Reader reader;
	Json::Value value0;
	reader.parse(strRequest,value0);
	std::string request = value0["command"].asString;
	Json::FastWriter writer;
	Json::Value value;

	if (request == "book_getDescription") {
		int tem_id=value0["id"].asInt();
		ILibrary * library;
		m_pClassFactory->getLibrary(&library);
		IBook * book;
		library->queryById(tem_id,&book);
		std::string tem_description;
		if (book->getDescription(tem_description)) {
			value["description"] = tem_description;
			strResponse = writer.write(value);
			value["result"] = 1;
		}
		else value["result"] = 0;
	}
		
	if (request == "book_setBasicInfo") {
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
		if (book->setBasicInfo(book_basic_info))  value["result"] = 1;
		else value["result"] = 0;
		strResponse = writer.write(value);
	}
		
	if (request == "book_setDescription") {
		int tem_id = value0["id"].asInt();
		 std::string tem_description = value0["description"].asString();
		
		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		if (book->setDescription(tem_description.c_str())) value["result"] = 1;
		else value["result"] = 0;
		strResponse = writer.write(value);
	}

	if (request == "book_deleteBook") {
		int tem_id = value0["id"].asInt();
		int tem_num = value0["number"].asInt();

		ILibrary *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		if (book->deleteBook(tem_num)) value["result"] = 1;
		else value["result"] = 0;
		strResponse = writer.write(value);
	}
	if (request == "book_getBorrowInfo") {
		int tem_id = value0["id"].asInt();
		ILibrary  *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryById(tem_id, &book);
		std::vector<TBorrowInfo> tem_binfo;
		book->getBorrowInfo(tem_binfo);
		Json::FastWriter writer;
		int num_of_records = tem_binfo.size();
		value["result"] = num_of_records;
		if (num_of_records > 0) Json::Value * tem_value = new Json::Value[num_of_records];
		for (int i = 0; i < num_of_records; i++) {
			
		}
	}
	if (request == "library_queryByName") {
		std::string tem_name = value0["name"].asString();
		ILibrary  *library;
		m_pClassFactory->getLibrary(&library);
		IBook *book;
		library->queryByName(tem_name,)//to be implemented
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
			Json::FastWriter writer;
			strResponse = writer.write(value);
			value["result"] = 1;
		}
		else value["result"] = 0;
		strResponse = writer.write(value);
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
			Json::FastWriter writer;
			strResponse = writer.write(value);
			value["result"] = 1;
		}
		else value["result"] = 0;
		strResponse = writer.write(value);
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
		if (library->insertBook(book))  value["result"] = 1;
		else value["result"] = 0;
		strResponse = writer.write(value);
	}

	if (request == "usermanager_getUserById") {
		int tem_id = value0["id"].asInt();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByID(tem_id,&user)) {
			TUserBasicInfo tem_user_basic_info;
			user->getBasicInfo(tem_user_basic_info);
			value["id"] = tem_user_basic_info.id;
			value["gender"] = tem_user_basic_info.gender;
			value["name"] = tem_user_basic_info.name;
			value["email"] = tem_user_basic_info.email;
			Json::FastWriter writer;
			strResponse = writer.write(value);
			value["result"] = 1;
		}
		else value["result"] = 0;
		strResponse = writer.write(value);
	}

	if (request == "usermanager_getUserByName") {
		std::string tem_name = value0["name"].asString();
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		if (usermanager->getUserByName(tem_name.c_str(),&user)) {
			TUserBasicInfo tem_user_basic_info;
			user->getBasicInfo(tem_user_basic_info);
			value["id"] = tem_user_basic_info.id;
			value["gender"] = tem_user_basic_info.gender;
			value["name"] = tem_user_basic_info.name;
			value["email"] = tem_user_basic_info.email;
			Json::FastWriter writer;
			strResponse = writer.write(value);
			value["result"] = 1;
		}
		else value["result"] = 0;
		strResponse = writer.write(value);
	}

	if (request == "usermanager_insertUser") {
		TUserBasicInfo tem_user_basic_info;
		tem_user_basic_info.id = value0["id"].asInt();
		tem_user_basic_info.gender = value0["gender"].asInt();
		tem_user_basic_info.name = value0["name"].asString();
		tem_user_basic_info.email = value0["email"].asString();
		
		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		m_pClassFactory->createEmptyUser(&user);
		user->setBasicInfo(tem_user_basic_info);
		if (usermanager->insertUser(user))  value["result"] = 1;
		else value["result"] = 0;
		strResponse = writer.write(value);
	}
	
	if (request == "user_setBasicInfo") {
		TUserBasicInfo tem_user_basic_info;
		tem_user_basic_info.id = value0["id"].asInt();
		tem_user_basic_info.gender = value0["gender"].asInt();
		tem_user_basic_info.name = value0["name"].asString();
		tem_user_basic_info.email = value0["email"].asString();

		IUserManager *usermanager;
		m_pClassFactory->getUserManager(&usermanager);
		IUser *user;
		usermanager->getUserByID(tem_user_basic_info.id, &user);
		if(user->setBasicInfo(tem_user_basic_info)) value["result"]=1;
		else value["result"] = 0;
		strResponse = writer.write(value);
	}
	if (request == "user_verifyPassword")
		getDescription(strResponse);
	if (request == "user_setPassword")
		getDescription(strResponse);
	if (request == "user_borrowBook")
		getDescription(strResponse);
	if (request == "user_returnBook")
		getDescription(strResponse);
	if (request == "user_deleteUser")
		getDescription(strResponse);
}