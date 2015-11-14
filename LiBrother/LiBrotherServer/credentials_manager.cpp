#include "credentials_manager.h"

#include <cstdio>

CCredentialsManager::CCredentialsManager(Botan::RandomNumberGenerator * rng) :
	m_rng(rng)
{
}

void CCredentialsManager::loadCertificate(const std::string& strCert, const std::string& strKey, const std::string& strPassphrase)
{
	TServerCert cert;
	cert.pKey.reset(Botan::PKCS8::load_key(strKey, *m_rng, strPassphrase));
	Botan::DataSource_Stream dsStream(strCert);
	while (!dsStream.end_of_data())
		cert.certChain.push_back(Botan::X509_Certificate(dsStream));
	m_vCerts.push_back(cert);
}

std::vector<Botan::X509_Certificate> CCredentialsManager::cert_chain(
	const std::vector<std::string>& algos, const std::string& type, const std::string& hostname)
{
	for (auto i = m_vCerts.begin(); i != m_vCerts.end(); ++i)
	{
		if (std::find(algos.begin(), algos.end(), i->pKey->algo_name()) == algos.end())
			continue;
		if(hostname != "" && !i->certChain[0].matches_dns_name(hostname))
			continue;
		return i->certChain;
	}
	return std::vector<Botan::X509_Certificate>();
}

Botan::Private_Key* CCredentialsManager::private_key_for(
	const Botan::X509_Certificate& cert, const std::string& type, const std::string& context)
{
	for (auto i = m_vCerts.begin(); i != m_vCerts.end(); ++i)
	{
		if (i->certChain[0] == cert)
			return i->pKey.get();
	}
	return nullptr;
}