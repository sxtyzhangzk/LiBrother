#ifndef __CLASS_FACTORY_CLIENT_H__
#define __CLASS_FACTORY_CLIENT_H__

#include "client_interfaces.h"

class CClassFactory : public IClassFactoryClient
{
	IMPLEMENT_INTERFACE
public:
	virtual bool createEmptyBook(IBook ** ppBook) override;
	virtual bool createEmptyUser(IUser ** ppUser) override;
	virtual bool getLibrary(ILibrary ** ppLibrary) override;
	virtual bool getUserManager(IUserManager ** ppManager) override;
	virtual bool getAuthManager(IAuthManager ** ppAuthManager) override;
};
#endif