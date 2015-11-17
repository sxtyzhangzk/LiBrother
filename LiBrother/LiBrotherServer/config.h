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
	std::string strLicense;					//用户协议的内容
	std::vector<TAuthorization> vAuthList;	//不同权限等级对应的权限内容
	int nDefaultUserAuthLevel;				//默认的用户权限等级
	int nDefaultUserReadLevel;				//默认的用户阅读权限
	int nDefaultBookReadLevel;				//默认的图书阅读权限
};

extern TConfig g_configSvr;
extern TPolicy g_configPolicy;

#endif