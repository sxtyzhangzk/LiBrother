#ifndef __SESSION_H__
#define __SESSION_H__

#include <string>

struct ILibClassFactory;
class CSession
{
public:
	CSession();
	bool startSession(ILibClassFactory * pClassFactory, const std::string& strClientIP);
	bool stopSession();
	void recvRequest(const std::string& strRequest, std::string& strResponse);

protected:
	ILibClassFactory * m_pClassFactory;
	std::string m_strClientIP;
};

#endif