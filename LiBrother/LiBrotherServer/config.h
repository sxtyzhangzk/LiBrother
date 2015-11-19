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
	std::string strLicense;					//�û�Э�������
	std::vector<TAuthorization> vAuthList;	//��ͬȨ�޵ȼ���Ӧ��Ȩ������
	int nDefaultUserAuthLevel;				//Ĭ�ϵ��û�Ȩ�޵ȼ�
	int nDefaultUserReadLevel;				//Ĭ�ϵ��û��Ķ�Ȩ��
	int nDefaultBookReadLevel;				//Ĭ�ϵ�ͼ���Ķ�Ȩ��
};

extern TConfig g_configSvr;
extern TPolicy g_configPolicy;

#endif