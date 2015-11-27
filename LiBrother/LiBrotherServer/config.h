#ifndef __CONFIG_SERVER_H__
#define __CONFIG_SERVER_H__

#include <common_types.h>
#include <string>
#include <vector>
#include <utility>

const int REGID_MYSQL_CONN = 0;
const int REGID_SPHINX_CONN = 1;

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
	int nSessionTimeout;	//seconds

	//MySQL
	int nMySQLType;	//1 Local, 2 Remote
	std::string strMySQLHost;
	int nMySQLPort;
	std::string strUser;
	std::string strPWD;
	std::string strSchema;
	std::string strMySQLDIR;
	std::string strPathMysqld;
	std::string strPathMysqlAdmin;

	//Sphinx
	int nSphinxType;	//0 Disabled, 1 Local, 2 Remote
	std::string strSphinxHost;
	int nSphinxPort;
	std::string strSphinxDIR;
	std::string strPathSearchd;
	std::string strPathIndexer;
	int nUpdateInterval;
};
struct TPolicy
{
	std::string strLicense;					//�û�Э�������
	std::vector<TAuthorization> vAuthList;	//��ͬȨ�޵ȼ���Ӧ��Ȩ������
	int nDefaultUserAuthLevel;				//Ĭ�ϵ��û�Ȩ�޵ȼ�
	int nDefaultUserReadLevel;				//Ĭ�ϵ��û��Ķ�Ȩ��
	int nDefaultBookReadLevel;				//Ĭ�ϵ�ͼ���Ķ�Ȩ��
	int nDefaultBorrowLimit;
};

extern TConfig g_configSvr;
extern TPolicy g_configPolicy;

#endif