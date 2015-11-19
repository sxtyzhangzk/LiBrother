#ifndef __SESSION_H__
#define __SESSION_H__

#include <string>
#include "common_types.h"
struct ILibClassFactory;
class CSession
{
public:
	CSession();
	bool startSession(ILibClassFactory * pClassFactory, const std::string& strClientIP);
	bool stopSession();
	void recvRequest(const std::string& strRequest, std::string& strResponse);
	int getCurrentAuthLevel();
	int getCurrentReadLevel();

protected:
	ILibClassFactory * m_pClassFactory;
	std::string m_strClientIP;
	int user_id;
	int current_read_level;
	int current_auth_level;
};

#endif