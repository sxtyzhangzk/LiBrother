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

MODULE_LOG_NAME("LibLog");

static bool g_bCopytoScreen = false;
static FILE * g_pFile = nullptr;

//获取当前时间
tm GetTime();

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
	return true;
}

void lprintf_(const char * strStatus, const char * strModuleName, const char * strFormat, ...)
{
	va_list pArgs;
	tm tmNow = GetTime();
	if (g_pFile)
	{
		fprintf(g_pFile, "[%d-%02d-%02d %02d:%02d:%02d] ",
			tmNow.tm_year + 1900,
			tmNow.tm_mon,
			tmNow.tm_mday,
			tmNow.tm_hour,
			tmNow.tm_min,
			tmNow.tm_sec);
		if (strStatus)
			fprintf(g_pFile, "[%s] ", strStatus);
		if (strModuleName)
			fprintf(g_pFile, "[%s] ", strModuleName);
	}
	if (g_bCopytoScreen)
	{
		printf("[%d-%02d-%02d %02d:%02d:%02d] ",
			tmNow.tm_year + 1900,
			tmNow.tm_mon,
			tmNow.tm_mday,
			tmNow.tm_hour,
			tmNow.tm_min,
			tmNow.tm_sec);
		if (strStatus)
			printf("[%s] ", strStatus);
		if (strModuleName)
			printf("[%s] ", strModuleName);
	}
	va_start(pArgs, strFormat);
	if (g_pFile)
		vfprintf(g_pFile, strFormat, pArgs);
	if (g_bCopytoScreen)
		vprintf(strFormat, pArgs);
	va_end(pArgs);
}

void CloseLog()
{
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