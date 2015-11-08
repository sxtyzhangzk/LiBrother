#include "config.h"
#include "class_factory_client.h"
#include "netclient.h"

#include <liblog.h>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

MODULE_LOG_NAME("Main");

TConfig g_configClient;
static CClassFactory * g_mainFactory = nullptr;

//TODO: 实现CClassFactory

bool CClassFactory::createEmptyBook(IBook ** ppBook)
{
	//TODO: 创建(new)一个CBook *对象并赋给*ppBook
}
bool CClassFactory::createEmptyUser(IUser ** ppUser)
{
	//TODO: 创建(new)一个CUser *对象并赋给*ppUser
}

bool initBackend(int argc, char * argv[], int& retcode)
{
	std::string strConfig = "client.yaml";
	if (argc > 1)
		strConfig = argv[1];
	std::ifstream fconfig;
	fconfig.open(strConfig, std::ios::in);
	if (!fconfig.is_open())
	{
		retcode = 1;
		return false;
	}

	//读取配置文件
	try
	{
		YAML::Node doc = YAML::Load(fconfig);
		YAML::Node netConfig = doc["Network"];
		g_configClient.strServer = netConfig["server"].as<std::string>();
		g_configClient.nPort = netConfig["port"].as<int>();
		g_configClient.bTLS = netConfig["tls"].as<bool>();
		g_configClient.bKeepAlive = netConfig["keepalive"].as<bool>();

		YAML::Node generalConfig = doc["general"];
		g_configClient.bLog = generalConfig["log"].as<bool>();
		if (g_configClient.bLog)
			g_configClient.strLogFile = generalConfig["log"].as<std::string>();
	}
	catch (YAML::Exception e)
	{
		fconfig.close();
		retcode = 2;
		return false;
	}
	fconfig.close();

	if (g_configClient.bLog)
	{
		InitLog(g_configClient.strLogFile.c_str(), true, false);
	}
	lprintf("Client Started.");

	if (!initNetClient())
	{
		lprintf_e("Client Aborted - Failed to init NetClient");
		CloseLog();
		retcode = 3;
		return false;
	}

	g_mainFactory = new CClassFactory;
	g_mainFactory->AddRef();

	retcode = 0;
	return true;
}

void cleanupBackend()
{
	if(g_mainFactory)
		delete g_mainFactory;

	lprintf("Client Stopped.");

	if (g_configClient.bLog)
		CloseLog();
	cleanupNetClient();
}

bool getClassFactory(IClassFactoryClient ** ppFactory)
{
	if (!ppFactory)
		return false;
	if (!g_mainFactory)
	{
		*ppFactory = nullptr;
		return false;
	}
	g_mainFactory->AddRef();
	*ppFactory = (IClassFactoryClient *)g_mainFactory;
	return true;
}