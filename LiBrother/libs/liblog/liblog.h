#ifndef __LIBLOG_H__
#define __LIBLOG_H__

/*
	ģ�����ƣ�	liblog
	���ܣ�		��¼��־
*/

//������־����ʾ��ģ������
#define MODULE_LOG_NAME(s) static const char * g_strModuleName = s
//���һ����Ϣ
#define lprintf(format, ...) lprintf_("INFO", g_strModuleName, format, __VA_ARGS__)
//���һ������
#define lprintf_e(format, ...) lprintf_("ERROR", g_strModuleName, format, __VA_ARGS__)
//���һ������
#define lprintf_w(format, ...) lprintf_("WARNING", g_strModuleName, format, __VA_ARGS__)

//��ʼ����־�ļ�
bool InitLog(const char * strFile, bool bAppend = true, bool bCopytoScreen = true);
//��¼��־����ʽͬprintf����ʹ��lprintf����ô˺���
void lprintf_(const char * strStatus, const char * strModuleName, const char * strFormat, ...);
//�ر���־�ļ�
void CloseLog();

#endif