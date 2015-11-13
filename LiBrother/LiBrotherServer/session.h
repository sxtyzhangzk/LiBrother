#ifndef __SESSION_H__
#define __SESSION_H__

#include <string>

class CSession
{
public:
	CSession();
	bool startSession(const std::string& strClientIP);
	bool stopSession();
	void recvRequest(const std::string& strRequest, std::string& strRespond);

protected:
	std::string m_strClientIP;
};

#endif