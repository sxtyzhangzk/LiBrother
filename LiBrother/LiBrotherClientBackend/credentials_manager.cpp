#include "credentials_manager.h"


void CCredentialsManager::loadCA(const std::string& strPath)
{
	std::shared_ptr<Botan::Certificate_Store> cs(new Botan::Certificate_Store_In_Memory(strPath));
	m_vCerts.push_back(cs);
}

std::vector<Botan::Certificate_Store*> CCredentialsManager::trusted_certificate_authorities(
	const std::string& type, const std::string& hostname)
{
	std::vector<Botan::Certificate_Store*> vResult;
	for (auto cs : m_vCerts)
		vResult.push_back(cs.get());
	return vResult;
}