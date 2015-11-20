#include "config.h"
#include "netserver.h"
#include "factory.h"
#include "connection_pool.h"
#include "program_launcher.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <liblog.h>
#include <yaml-cpp/yaml.h>

MODULE_LOG_NAME("Main");

TConfig g_configSvr;
TPolicy g_configPolicy;

bool loadConfig(const std::string& strFile)
{
	std::ifstream fconfig;
	fconfig.open(strFile, std::ios::in);
	if (!fconfig.is_open())
		return false;

	try
	{
		std::string temp;
		YAML::Node doc = YAML::Load(fconfig);
		YAML::Node network = doc["network"];
		g_configSvr.strServerIP = network["host"].as<std::string>();
		g_configSvr.nPort = network["port"].as<int>();
		temp = network["tls"].as<std::string>();
		std::transform(temp.begin(), temp.end(), temp.begin(), toupper);
		if (temp == "ENABLED")
			g_configSvr.nTLS = 1;
		else if (temp == "FORCED")
			g_configSvr.nTLS = 2;
		else
			g_configSvr.nTLS = 0;
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
			if (mysql["dir"].IsDefined())
				g_configSvr.strMySQLDIR = mysql["dir"].as<std::string>();
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

bool initMySQL(CConnectionPool& connPool)
{
	std::string connURL = "tcp://";
	connURL += g_configSvr.strMySQLHost;
	connURL += ":";
	connURL += g_configSvr.nMySQLPort;
	return connPool.registerConnection(
		REGID_MYSQL_CONN, connURL, 
		g_configSvr.strUser, g_configSvr.strPWD, 
		g_configSvr.strSchema);
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
		return 1;
	}

	CProgramLauncher progLauncher;
	CConnectionPool connPool;

	int nMysqld = -1, nSearchd = -1;
	if (g_configSvr.nMySQLType == 1)
	{
		nMysqld = progLauncher.runProgram(
			g_configSvr.strPathMysqld, std::vector<std::string>(), g_configSvr.strMySQLDIR,
			CProgramLauncher::RunBackend);
		if (nMysqld < 0)
		{
			lprintf_e("Failed to execute mysqld");
			CloseLog();
			return 2;
		}
	}
	if (!initMySQL(connPool))
	{
		lprintf_e("Failed to init MySQL");
		CloseLog();
		return 3;
	}

	if (g_configSvr.nSphinxType == 1)
	{
		nSearchd = progLauncher.runProgram(
			g_configSvr.strPathSearchd, std::vector<std::string>(), g_configSvr.strSphinxDIR,
			CProgramLauncher::RunBackend);
		if (nSearchd < 0)
		{
			lprintf_w("Failed to execute searchd, will disable sphinx support");
		}
		else
		{
			//TODO: Launch Indexer
		}
	}
	
	CloseLog();
	return 0;
}