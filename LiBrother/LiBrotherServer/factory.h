#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "function_interfaces.h"

class Cfactory : public ILibClassFactory
{
	IMPLEMENT_INTERFACE
public:
	Cfactory(IDatabase * DatabaseFile);
	virtual bool createEmptyBook(IBook ** ppBook) override;
	virtual bool createEmptyUser(IUser ** ppUser) override;
	virtual bool getLibrary(ILibrary ** ppLibrary) override;
	virtual bool getUserManager(IUserManager ** ppManager) override;
protected:
	IDatabase * m_Database;
};
#endif