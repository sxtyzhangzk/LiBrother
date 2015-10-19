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
#include <string>
#include <sstream>
#include <iomanip>
using std::string;
using std::stringstream;
using std::setw;
using std::setfill;

MODULE_LOG_NAME("LibLog");

//������־�ĳ�������
const int nLogBufferSize = 512;

static bool g_bCopytoScreen = false;
static FILE * g_pFile = nullptr;

//��ȡ��ǰʱ��
tm GetTime();
//������־ͷ����ʽ��[ʱ��] [��־���] [ģ������]
string GetLogHeader(const char * strStatus, const char * strModuleName, const tm& tmNow);
//�����־��nSize == -1��ʾ��ָ������
void printLog(const char * strContent, int nSize = -1);

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
	string strLogHeader = GetLogHeader(strStatus, strModuleName, tmNow);

	char strBuffer[nLogBufferSize];
	size_t nLength;
	va_start(pArgs, strFormat);
	vsnprintf(strBuffer, sizeof(strBuffer), strFormat, pArgs);
	va_end(pArgs);
	nLength = strnlen(strBuffer, sizeof(strBuffer));

	//ÿ��һ�������־ͷ
	for (int i = 0, j = 0; i < nLength; i++)
		if (strBuffer[i] == '\n' || i == nLength - 1)
		{
			printLog(strLogHeader.c_str());
			printLog(strBuffer + j, i - j + 1);
			j = i + 1;
		}
	if (strBuffer[nLength - 1] != '\n')
		printLog("\n");
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

string GetLogHeader(const char * strStatus, const char * strModuleName, const tm& tmNow)
{
	stringstream ss;

	ss << "["
		<< tmNow.tm_year + 1900 << "-"
		<< setw(2) << setfill('0') << tmNow.tm_mon << "-"
		<< setw(2) << setfill('0') << tmNow.tm_mday << " "
		<< setw(2) << setfill('0') << tmNow.tm_hour << ":"
		<< setw(2) << setfill('0') << tmNow.tm_min << ":"
		<< setw(2) << setfill('0') << tmNow.tm_sec
		<< "] [" << strStatus << "] [" << strModuleName << "] ";

	return ss.str();
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