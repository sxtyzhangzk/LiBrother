#ifndef __NET_CLIENT_H__
#define __NET_CLIENT_H__

//����ģ��

#include <string>

//��ʼ������ģ��
bool initNetClient();
//��������ģ��
void cleanupNetClient();
//���������������
bool sendRequest(const std::string& strRequest, std::string& strRespond);

#endif