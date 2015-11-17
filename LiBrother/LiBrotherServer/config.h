#ifndef __CONFIG_SERVER_H__
#define __CONFIG_SERVER_H__

#include <common_types.h>
#include <string>
#include <vector>
#include <utility>

struct TCertFile
{
	std::string strCert;
	std::string strKey;
	std::string strPassphrase;
};
struct TConfig
{
	//Network
	std::string strServerIP;
	int nPort;
	int nTLS;		//0 Disabled, 1 Enabled, 2 Forced
	int nTLSPort;

	//Secure
	std::vector<TCertFile> vCerts;

	//Performance
	int nThreadPerCPU;
	int nSessionTimeout;
};
struct TPolicy
{
	std::string strLicense;
	std::vector<TAuthorization> vAuthList;
};

extern TConfig g_configSvr;
extern TPolicy g_configPolicy;

#endif