#ifndef __NET_CLIENT_H__
#define __NET_CLIENT_H__

//网络模块

#include <string>

//初始化网络模块
bool initNetClient();
//结束网络模块
void cleanupNetClient();
//向服务器发送请求
bool sendRequest(const std::string& strRequest, std::string& strRespond);

#endif