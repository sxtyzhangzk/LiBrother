#include "factory.h"
#include "book.h"
#include "library.h"
#include "user.h"
#include "magicdb.h"
#include "manager.h"
#include"connection_pool.h"
#include"config.h"
Cfactory::Cfactory(CConnectionPool * DatabaseFile) : m_Database(DatabaseFile)
{
}
bool Cfactory :: createEmptyBook(IBook ** ppBook)
{
	CBook * pBook = new CBook(m_Database);
	pBook->AddRef();
	*ppBook = (IBook *)pBook;
	return true;
}
bool Cfactory :: createEmptyUser(IUser ** ppUser)
{
	CUser * pUser = new CUser(m_Database);
	pUser->AddRef();
	*ppUser = (IUser *)pUser;
	return true;
}
bool Cfactory :: getLibrary(ILibrary ** ppLibrary)
{
	CLibrary * pLibrary = new CLibrary(m_Database);
	pLibrary->AddRef();
	*ppLibrary = (ILibrary *)pLibrary;
	return true;
}
bool Cfactory :: getUserManager(IUserManager ** ppManager)
{
	CManager * pManager = new CManager(m_Database);
	pManager->AddRef();
	*ppManager = (IUserManager *)pManager;
	return true;
}
