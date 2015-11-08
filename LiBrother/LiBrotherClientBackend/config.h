#ifndef __CONFIG_H__
#define __CONFIG_H__

//客户端设置内容

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
};

extern TConfig g_configClient;

#endif