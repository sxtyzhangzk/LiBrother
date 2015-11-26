#include "config.h"
#include "class_factory_client.h"
#include "netclient.h"
#include "utils.h"

#include <thread>
#include <liblog.h>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
#include "book.h"
#include "user.h"
#include "authmanager.h"
#include "library.h"
#include "usermanager.h"
MODULE_LOG_NAME("Main");

TConfig g_configClient;
static CClassFactory * g_mainFactory = nullptr;

//TODO: 实现CClassFactory

CClassFactory::CClassFactory() :
	m_pLibrary(nullptr), m_pUserManager(nullptr), m_pAuthManager(nullptr)
{
	lprintf("CONSTRUCT");
}

bool CClassFactory::createEmptyBook(IBook ** ppBook)
{
	CHECK_PTR(ppBook);

	CBook *book = new CBook;
	book->AddRef();
	*ppBook = book;
	return true;
}
bool CClassFactory::createEmptyUser(IUser ** ppUser)
{
	CHECK_PTR(ppUser);
	
	CUser *user = new CUser;
	user->AddRef();
	*ppUser = user;
	return true;
}

bool CClassFactory::getLibrary(ILibrary ** ppLibrary)
{
	CHECK_PTR(ppLibrary);

	if (!m_pLibrary)
	{
		m_pLibrary = new CLibrary;
		m_pLibrary->AddRef();
	}
	m_pLibrary->AddRef();
	*ppLibrary = m_pLibrary;
	return true;
}
bool CClassFactory::getUserManager(IUserManager ** ppManager)
{
	CHECK_PTR(ppManager);

	if (!m_pUserManager)
	{
		m_pUserManager = new CUserManager;
		m_pUserManager->AddRef();
	}
	m_pUserManager->AddRef();
	*ppManager = m_pUserManager;
	return true;
}
bool CClassFactory::getAuthManager(IAuthManager ** ppAuthManager)
{
	CHECK_PTR(ppAuthManager);

	if (!m_pAuthManager)
	{
		m_pAuthManager = new CAuthManager;
		m_pAuthManager->AddRef();
	}
	m_pAuthManager->AddRef();
	*ppAuthManager = m_pAuthManager;
	return true;
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
		YAML::Node netConfig = doc["network"];
		g_configClient.strServer = netConfig["server"].as<std::string>();
		g_configClient.nPort = netConfig["port"].as<int>();
		g_configClient.bTLS = netConfig["tls"].as<bool>();
		if (g_configClient.bTLS)
		{
			g_configClient.strCAPath = netConfig["ca-certs"].as<std::string>();
			g_configClient.bTLSVerifyHostname = netConfig["verify-hostname"].as<bool>();
		}
		g_configClient.bKeepAlive = netConfig["keepalive"].as<bool>();
		g_configClient.nBufferSize = netConfig["buffersize"].as<int>();

		YAML::Node generalConfig = doc["general"];
		g_configClient.bLog = generalConfig["log"].as<bool>();
		if (g_configClient.bLog)
			g_configClient.strLogFile = generalConfig["logfile"].as<std::string>();
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

	lprintf("HELLO");
	g_mainFactory->AddRef();

	lprintf("GO");

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