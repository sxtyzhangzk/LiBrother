#ifndef __CREDENTIALS_MANAGER_SERVER_H__
#define __CREDENTIALS_MANAGER_SERVER_H__

#include <string>
#include <vector>
#include <memory>
#include <botan/botan.h>
#include <botan/credentials_manager.h>
#include <botan/pkcs8.h>

class CCredentialsManager : public Botan::Credentials_Manager
{
public:
	CCredentialsManager(Botan::RandomNumberGenerator * rng);
	void loadCertificate(const std::string& strCert, const std::string& strKey, const std::string& strPassphrase);

	virtual std::vector<Botan::X509_Certificate> cert_chain(const std::vector<std::string>& algos, const std::string& type, const std::string& hostname) override;
	virtual Botan::Private_Key* private_key_for(const Botan::X509_Certificate& cert, const std::string& type, const std::string& context) override;
protected:
	struct TServerCert
	{
		std::vector<Botan::X509_Certificate> certChain;
		std::shared_ptr<Botan::Private_Key> pKey;
	};
	std::vector<TServerCert> m_vCerts;
	Botan::RandomNumberGenerator * m_rng;
};

#endif