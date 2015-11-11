#include "factory.h"
#include "book.h"
#include "magicdb.h"
Cfactory :: Cfactory(IDatabase * DatabaseFile) : m_Database(DatabaseFile)
{
}
bool Cfactory :: createEmptyBook(IBook ** ppBook)
{
	CBook * pBook = new CBook(m_Database);
	pBook->AddRef();
	*ppBook = (IBook *)pBook;
}
bool Cfactory :: createEmptyUser(IUser ** ppUser)
{
	CUser * pUser = new CUser(m_Database);
	pUser->AddRef();
	*ppUser = (IBook *)pUser;
}
bool Cfactory :: getLibrary(ILibrary ** ppLibrary)
{
	CLibrary * pLibrary = new CLibrary(m_Database);
	pLibrary->AddRef();
	*ppLibrary = (ILibrary *)pLibrary;
}
bool Cfactory :: getUserManager(IUserManager ** ppManager)
{
	CManager * pManager = new CManager(m_Database);
	pManager->AddRef();
	*ppManager = (IUserManager *)pManager;
}
