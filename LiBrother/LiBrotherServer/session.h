#ifndef __SESSION_H__
#define __SESSION_H__

#include "common_types.h"
#include <string>
#include <json/json.h>

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
	static void writePermissionDenied(Json::Value& value);
	static void writeInterfaceError(Json::Value& value, IAbstract *pIface);
	static void releaseInterface(IAbstract *pIface);

protected:
	ILibClassFactory * m_pClassFactory;
	std::string m_strClientIP;
	int user_id;
	int current_read_level;
	int current_auth_level;
};

#endif