#ifndef __NET_CLIENT_H__
#define __NET_CLIENT_H__

#include <string>

bool initNetClient();
void cleanupNetClient();
bool sendRequest(const std::string& strRequest, std::string& strRespond);

#endif