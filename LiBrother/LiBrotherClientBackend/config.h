#ifndef __CONFIG_H__
#define __CONFIG_H__

//�ͻ�����������

#include <string>

struct TConfig
{
	std::string strServer;
	int nPort;
	bool bTLS;
	bool bKeepAlive;
	int nBufferSize;
	bool bLog;
	std::string strLogFile;
	std::string strCAPath;
	bool bTLSVerifyHostname;
};

extern TConfig g_configClient;

#endif