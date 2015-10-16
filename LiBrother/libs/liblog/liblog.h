#ifndef __LIBLOG_H__
#define __LIBLOG_H__

/*
	模块名称：	liblog
	功能：		记录日志
*/

//定义日志中显示的模块名称
#define MODULE_LOG_NAME(s) static const char * g_strModuleName = s
//输出一条信息
#define lprintf(format, ...) lprintf_("INFO", g_strModuleName, format, __VA_ARGS__)
//输出一条错误
#define lprintf_e(format, ...) lprintf_("ERROR", g_strModuleName, format, __VA_ARGS__)
//输出一条警告
#define lprintf_w(format, ...) lprintf_("WARNING", g_strModuleName, format, __VA_ARGS__)

//初始化日志文件
bool InitLog(const char * strFile, bool bAppend = true, bool bCopytoScreen = true);
//记录日志，格式同printf，请使用lprintf宏调用此函数
void lprintf_(const char * strStatus, const char * strModuleName, const char * strFormat, ...);
//关闭日志文件
void CloseLog();

#endif