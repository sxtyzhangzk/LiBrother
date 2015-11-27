#include "config.h"
#include "netserver.h"
#include "factory.h"
#include "connection_pool.h"
#include "program_launcher.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <signal.h>

#include <mutex>
#include <condition_variable>

#include <liblog.h>
#include <yaml-cpp/yaml.h>

MODULE_LOG_NAME("Main");

TConfig g_configSvr;
TPolicy g_configPolicy;

std::mutex mtxExitEvent;
std::condition_variable cvExitEvent;
bool bExitEvent;


//加载默认配置
void loadDefaultConfig()
{
	g_configSvr.nTLS = 0;
	g_configSvr.nThreadPerCPU = 2;
	g_configSvr.nSessionTimeout = 300;
	g_configSvr.strMySQLHost = "localhost";
	g_configSvr.nMySQLPort = 3306;
	g_configSvr.strSchema = "librother";
	g_configSvr.strMySQLDIR = "./mysql/";
	g_configSvr.nSphinxType = 0;
	g_configSvr.strMySQLHost = "localhost";
	g_configSvr.nSphinxPort = 9306;
	g_configSvr.strSphinxDIR = "./sphinx/";
	g_configSvr.nUpdateInterval = 300000;
#ifdef _WIN32
	g_configSvr.strPathSearchd = "searchd.exe";
	g_configSvr.strPathIndexer = "indexer.exe";
	g_configSvr.strPathMysqld = "mysqld.exe";
	g_configSvr.strPathMysqlAdmin = "mysqladmin.exe";
#else
	g_configSvr.strPathSearchd = "searchd";
	g_configSvr.strPathIndexer = "indexer";
	g_configSvr.strPathMysqld = "mysqld";
	g_configSvr.strPathMysqlAdmin = "mysqladmin";
#endif
	g_configPolicy.nDefaultUserReadLevel = 10;
	g_configPolicy.nDefaultBookReadLevel = 10;
	g_configPolicy.nDefaultUserAuthLevel = 0;
	g_configPolicy.vAuthList.push_back({ 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 });
	g_configPolicy.vAuthList.push_back({ 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1 });
	g_configPolicy.vAuthList.push_back({ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 });
	g_configPolicy.nDefaultBorrowLimit = 6;
}

//加载配置文件
bool loadConfig(const std::string& strFile)
{
	std::ifstream fconfig;
	fconfig.open(strFile, std::ios::in);
	if (!fconfig.is_open())
		return false;

	loadDefaultConfig();
	try
	{
		std::string temp;
		YAML::Node doc = YAML::Load(fconfig);
		YAML::Node network = doc["network"];
		g_configSvr.strServerIP = network["host"].as<std::string>();
		g_configSvr.nPort = network["port"].as<int>();
		if (network["tls"].IsDefined())
		{
			temp = network["tls"].as<std::string>();
			std::transform(temp.begin(), temp.end(), temp.begin(), toupper);
			if (temp == "ENABLED")
				g_configSvr.nTLS = 1;
			else if (temp == "FORCED")
				g_configSvr.nTLS = 2;
			else
				g_configSvr.nTLS = 0;
		}
		if (g_configSvr.nTLS)
		{
			g_configSvr.nTLSPort = network["tls-port"].as<int>();

			YAML::Node secure = doc["secure"];
			for (int i = 0; i < secure.size(); i++)
			{
				TCertFile cert;
				cert.strCert = secure[i]["cert"].as<std::string>();
				cert.strKey = secure[i]["key"].as<std::string>();
				cert.strPassphrase = secure[i]["passphrase"].as<std::string>();
				g_configSvr.vCerts.push_back(cert);
			}
		}

		YAML::Node performance = doc["performance"];
		if (performance.IsDefined())
		{
			if (performance["thread-per-cpu"].IsDefined())
				g_configSvr.nThreadPerCPU = performance["thread-per-cpu"].as<int>();
			if (performance["session-timeout"].IsDefined())
				g_configSvr.nSessionTimeout = performance["session-timeout"].as<int>();
		}

		YAML::Node policy = doc["policy"];
		if (policy.IsDefined())
		{
			if (policy["license-file"].IsDefined())
			{
				std::ifstream flicense;
				flicense.open(policy["license-file"].as<std::string>());
				if (flicense.is_open())
				{
					std::stringstream sstream;
					sstream << flicense.rdbuf();
					g_configPolicy.strLicense = sstream.str();
					flicense.close();
				}
			}
			if (policy["borrow-limit"].IsDefined())
				g_configPolicy.nDefaultBorrowLimit = policy["borrow-limit"].as<int>();
			//TODO: Other Policy Config
		}

		YAML::Node mysql = doc["mysql"];
		temp = mysql["type"].as<std::string>();
		std::transform(temp.begin(), temp.end(), temp.begin(), toupper);
		if (temp == "LOCAL")
			g_configSvr.nMySQLType = 1;
		else if (temp == "REMOTE")
			g_configSvr.nMySQLType = 2;
		else
			return false;
		g_configSvr.strUser = mysql["user"].as<std::string>();
		g_configSvr.strPWD = mysql["password"].as<std::string>();
		if (mysql["host"].IsDefined())
			g_configSvr.strMySQLHost = mysql["host"].as<std::string>();
		if (mysql["port"].IsDefined())
			g_configSvr.nMySQLPort = mysql["port"].as<int>();
		if (mysql["schema"].IsDefined())
			g_configSvr.strSchema = mysql["schema"].as<std::string>();
		if (g_configSvr.nMySQLType == 1)
		{
			if (mysql["mysqld-bin"].IsDefined())
				g_configSvr.strPathMysqld = mysql["mysqld-bin"].as<std::string>();
			if (mysql["mysqladmin-bin"].IsDefined())
				g_configSvr.strPathMysqlAdmin = mysql["mysqladmin-bin"].as<std::string>();
			if (mysql["dir"].IsDefined())
				g_configSvr.strMySQLDIR = mysql["dir"].as<std::string>();
			if (mysql["mysqld-param"].IsDefined())
				g_configSvr.strMysqldParam = mysql["mysqld-param"].as<std::string>();
		}

		YAML::Node sphinx = doc["sphinx"];
		if (sphinx.IsDefined())
		{
			temp = sphinx["type"].as<std::string>();
			std::transform(temp.begin(), temp.end(), temp.begin(), toupper);
			if (temp == "LOCAL")
				g_configSvr.nSphinxType = 1;
			else if (temp == "REMOTE")
				g_configSvr.nSphinxType = 2;
			else
				g_configSvr.nSphinxType = 0;
			if (sphinx["host"].IsDefined())
				g_configSvr.strSphinxHost = sphinx["host"].as<std::string>();
			if (sphinx["port"].IsDefined())
				g_configSvr.nSphinxPort = sphinx["port"].as<int>();
			if (g_configSvr.nSphinxType == 1)
			{
				if (sphinx["searchd-bin"].IsDefined())
					g_configSvr.strPathSearchd = sphinx["searchd-bin"].as<std::string>();
				if (sphinx["indexer-bin"].IsDefined())
					g_configSvr.strPathIndexer = sphinx["indexer-bin"].as<std::string>();
				if (sphinx["dir"].IsDefined())
					g_configSvr.strSphinxDIR = sphinx["dir"].as<std::string>();
				if (sphinx["upd-interval"].IsDefined())
					g_configSvr.nUpdateInterval = sphinx["upd-interval"].as<int>();
				if (sphinx["searchd-param"].IsDefined())
					g_configSvr.strSearchdParam = sphinx["searchd-param"].as<std::string>();
				if (sphinx["indexer-param"].IsDefined())
					g_configSvr.strIndexerParam = sphinx["indexer-param"].as<std::string>();
			}
		}
	}
	catch (std::exception& e)
	{
		lprintf_e("An error occurred while parsing config file: \n%s", e.what());
		return false;
	}
	return true;
}

//初始化MySQL连接
bool initMySQL(CConnectionPool& connPool)
{
	std::string connURL = "tcp://";
	connURL += g_configSvr.strMySQLHost;
	connURL += ":";
	connURL += type2str(g_configSvr.nMySQLPort);
	return connPool.registerConnection(
		REGID_MYSQL_CONN, connURL, 
		g_configSvr.strUser, g_configSvr.strPWD, 
		g_configSvr.strSchema);
}

//初始化SphinxQL连接
bool initSphinx(CConnectionPool& connPool)
{
	std::string connURL = "tcp://";
	connURL += g_configSvr.strSphinxHost;
	connURL += ":";
	connURL += type2str(g_configSvr.nSphinxPort);
	return connPool.registerConnection(
		REGID_SPHINX_CONN, connURL, "librother", "", "");
}

//停止Sphinx服务器
void stopSphinx(CProgramLauncher& progLauncher, int nSearchd, int nIndexer)
{
	if(nIndexer >= 0)
		progLauncher.stopProgram(nIndexer, -1, false);
	if (nSearchd >= 0)
	{
		std::vector<std::string> vecArgs;
		vecArgs.push_back("--stop");
		progLauncher.runProgram(
			g_configSvr.strPathSearchd, vecArgs, g_configSvr.strSphinxDIR,
			CProgramLauncher::RunWait);
		progLauncher.stopProgram(nSearchd, 30000, true);
	}
}

//停止MySQL服务器
void stopMysql(CProgramLauncher& progLauncher, int nMysqld)
{
	if (nMysqld < 0)
		return;
	std::vector<std::string> vecArgs;
	vecArgs.push_back(std::string("--host=") + g_configSvr.strMySQLHost);
	vecArgs.push_back(std::string("--port=") + type2str(g_configSvr.nMySQLPort));
	vecArgs.push_back(std::string("--user=") + g_configSvr.strUser);
	vecArgs.push_back(std::string("--password=") + g_configSvr.strPWD);
	vecArgs.push_back("shutdown");
	progLauncher.runProgram(
		g_configSvr.strPathMysqlAdmin, vecArgs, g_configSvr.strMySQLDIR,
		CProgramLauncher::RunWait);
	progLauncher.stopProgram(nMysqld, 30000, true);
}

void signalHandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "SIGINT received" << std::endl;
		std::unique_lock<std::mutex> lock(mtxExitEvent);
		bExitEvent = true;
		cvExitEvent.notify_all();
	}
}

//等待服务结束命令
void waitForSigint()
{
	signal(SIGINT, signalHandler);
	std::unique_lock<std::mutex> lock(mtxExitEvent);
	if (!bExitEvent)
		cvExitEvent.wait(lock);
}

int main(int argc, char * argv[])
{
	//TODO: parse cmdline
	InitLog("server.log");
	lprintf("Initializing LiBrother Server.");
	if (!loadConfig("server.yaml"))
	{
		lprintf_e("Failed to load config");
		CloseLog();
		return 1;	//加载配置文件失败
	}

	CProgramLauncher progLauncher;
	CConnectionPool connPool;

	int nMysqld = -1, nSearchd = -1, nIndexer = -1;
	if (g_configSvr.nMySQLType == 1)
	{
		std::vector<std::string> vec;
		if (!g_configSvr.strMysqldParam.empty())
			vec.push_back(g_configSvr.strMysqldParam);
		nMysqld = progLauncher.runProgram(
			g_configSvr.strPathMysqld, vec, g_configSvr.strMySQLDIR,
			CProgramLauncher::RunBackend);
		if (nMysqld < 0)
		{
			lprintf_e("Failed to execute mysqld");
			CloseLog();
			return 2;	//启动本地MySQL服务器失败
		}
	}
	if (!initMySQL(connPool))
	{
		lprintf_e("Failed to init MySQL");
		stopMysql(progLauncher, nMysqld);
		CloseLog();
		return 3;	//连接MySQL服务器失败
	}

	if (g_configSvr.nSphinxType == 1)
	{
		std::vector<std::string> vecArgs;
		vecArgs.push_back("book");
		if (!g_configSvr.strIndexerParam.empty())
			vecArgs.push_back(g_configSvr.strIndexerParam);
		progLauncher.runProgram(
			g_configSvr.strPathIndexer, vecArgs, g_configSvr.strSphinxDIR,
			CProgramLauncher::RunWait);

		vecArgs.clear();
		if (!g_configSvr.strSearchdParam.empty())
			vecArgs.push_back(g_configSvr.strSearchdParam);

		nSearchd = progLauncher.runProgram(
			g_configSvr.strPathSearchd, vecArgs, g_configSvr.strSphinxDIR,
			CProgramLauncher::RunBackend);
		if (nSearchd < 0)
		{
			lprintf_w("Failed to execute searchd, will disable sphinx support");
			g_configSvr.nSphinxType = 0;
		}
		else
		{
			//TODO: Launch Indexer
			vecArgs.clear();
			vecArgs.push_back("book_delta");
			vecArgs.push_back("--rotate");
			if (!g_configSvr.strIndexerParam.empty())
				vecArgs.push_back(g_configSvr.strIndexerParam);
			nIndexer = progLauncher.runProgram(
				g_configSvr.strPathIndexer, vecArgs, g_configSvr.strSphinxDIR, 
				CProgramLauncher::RunAsTask, g_configSvr.nUpdateInterval);
			if (nIndexer < 0)
			{
				lprintf_w("Failed to add task Indexer");
			}
		}
	}
	if (g_configSvr.nSphinxType)
	{
		if (!initSphinx(connPool))
		{
			lprintf_w("Failed to init Sphinx");
			stopSphinx(progLauncher, nSearchd, nIndexer);
			nSearchd = nIndexer = -1;
		}
	}

	CNetServer netServer;
	if (!netServer.initNetServer())
	{
		lprintf_e("Failed to init NetServer");
		stopSphinx(progLauncher, nSearchd, nIndexer);
		stopMysql(progLauncher, nMysqld);
		CloseLog();
		return 4;	//初始化网络服务器失败
	}
	Cfactory *pClassFactory = new Cfactory(&connPool);
	pClassFactory->AddRef();
	if (!netServer.startServer(dynamic_cast<ILibClassFactory*>(pClassFactory)))
	{
		lprintf_e("Failed to start NetServer");
		delete pClassFactory;
		stopSphinx(progLauncher, nSearchd, nIndexer);
		stopMysql(progLauncher, nMysqld);
		CloseLog();
		return 5;	//启动网络服务器失败
	}
	lprintf("Server started successfully.");

	waitForSigint();

	lprintf("Stopping server");
	netServer.stopServer();
	delete pClassFactory;
	stopSphinx(progLauncher, nSearchd, nIndexer);
	stopMysql(progLauncher, nMysqld);
	lprintf("Server stopped");

	CloseLog();
	return 0;
}