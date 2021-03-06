/************
 * liblog
 * Author: zk
 * Created at: 2015/10/16
 ************/

#include "liblog.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <ctime>

#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#endif

MODULE_LOG_NAME("LibLog");

//单条日志的长度上限
const int nLogBufferSize = 512;
//日志头长度上限
const int nLogHeaderSize = 64;

static bool g_bCopytoScreen = false;
static FILE * g_pFile = nullptr;
static bool g_bInited = false;

static std::mutex mtxLog;

//获取当前时间
tm GetTime();
//生成日志头，格式：[时间] [日志类别] [模块名称]
void GetLogHeader(char * pDest, const size_t nBufferSize, const char * strStatus, const char * strModuleName, const tm& tmNow);
//输出日志，nSize == -1表示不指定长度
void printLog(const char * strContent, int nSize = -1);

void setConsoleColor(const char * strStatus);

bool InitLog(const char * strFile, bool bAppend, bool bCopytoScreen)
{
	g_bCopytoScreen = bCopytoScreen;
	if (strFile)
	{
#if _MSC_VER >= 1500
		if (fopen_s(&g_pFile, strFile, bAppend ? "a" : "w") != 0)
			return false;
#else
		g_pFile = fopen(strFile, bAppend ? "a" : "w");
		if (!g_pFile)
			return false;
#endif
	}
	g_bInited = true;
	return true;
}

void lprintf_(const char * strStatus, const char * strModuleName, const char * strFormat, ...)
{
	if (!g_bInited)
		return;
	va_list pArgs;
	tm tmNow = GetTime();
	char strLogHeader[nLogHeaderSize];
	GetLogHeader(strLogHeader, sizeof(strLogHeader), strStatus, strModuleName, tmNow);

	char strBuffer[nLogBufferSize];
	size_t nLength;
	va_start(pArgs, strFormat);
	vsnprintf(strBuffer, sizeof(strBuffer), strFormat, pArgs);
	va_end(pArgs);
	nLength = strnlen(strBuffer, sizeof(strBuffer));

	mtxLog.lock();
	setConsoleColor(strStatus);
	//每换一行输出日志头
	for (int i = 0, j = 0; i < nLength; i++)
		if (strBuffer[i] == '\n' || i == nLength - 1)
		{
			printLog(strLogHeader);
			printLog(strBuffer + j, i - j + 1);
			j = i + 1;
		}
	if (strBuffer[nLength - 1] != '\n')
		printLog("\n");
	setConsoleColor(nullptr);
	mtxLog.unlock();
}

void CloseLog()
{
	g_bInited = false;
	if (g_pFile)
	{
		fclose(g_pFile);
		g_pFile = nullptr;
	}
	g_bCopytoScreen = false;
}

tm GetTime()
{
	time_t tNow;
	time(&tNow);
	tm tmNow;
#if _MSC_VER >= 1500
	localtime_s(&tmNow, &tNow);
#else
	tmNow = *localtime(&tNow);
#endif
	return tmNow;
}

void GetLogHeader(char * pDest, const size_t nBufferSize, const char * strStatus, const char * strModuleName, const tm& tmNow)
{
	if (!pDest)
		return;
	snprintf(pDest, nBufferSize, "[%d-%02d-%02d %02d:%02d:%02d] [%s] [%s] ",
		tmNow.tm_year + 1900, tmNow.tm_mon, tmNow.tm_mday,
		tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec,
		strStatus, strModuleName);
}

void printLog(const char * strContent, int nSize)
{
	if (nSize < 0)
	{
		if (g_pFile)
			fprintf(g_pFile, "%s", strContent);
		if (g_bCopytoScreen)
			printf("%s", strContent);
	}
	else
	{
		if (g_pFile)
			fprintf(g_pFile, "%.*s", nSize, strContent);
		if (g_bCopytoScreen)
			printf("%.*s", nSize, strContent);
	}
}

void setConsoleColor(const char * strContent)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
		return;
	if (!strContent)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	else if (strcmp(strContent, "WARNING") == 0)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else if (strcmp(strContent, "ERROR") == 0)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	else if(strcmp(strContent, "DEBUG") == 0)
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
	if (!strContent)
		printf("\x1b[m");
	else if (strcmp(strContent, "WARNING") == 0)
		printf("\x1b[33m\x1b[1m");
	else if (strcmp(strContent, "ERROR") == 0)
		printf("\x1b[31m\x1b[1m");
	else if (strcmp(strContent, "DEBUG") == 0)
		printf("\x1b[32m\x1b[1m");
#endif
}