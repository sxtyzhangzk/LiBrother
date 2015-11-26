#ifndef __CREDENTIALS_MANAGER_CLIENT_H__
#define __CREDENTIALS_MANAGER_CLIENT_H__

#include <string>
#include <vector>
#include <memory>
#include <botan/credentials_manager.h>

class CCredentialsManager : public Botan::Credentials_Manager
{
public:
	void loadCA(const std::string& strPath);

	virtual std::vector<Botan::Certificate_Store*> trusted_certificate_authorities(const std::string& type, const std::string& hostname) override;

protected:
	std::vector<std::shared_ptr<Botan::Certificate_Store> > m_vCerts;
};

#endif