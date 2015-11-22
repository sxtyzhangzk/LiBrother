#ifndef __UTILS_CLIENT_H__
#define __UTILS_CLIENT_H__

#define TRY_SEND_REQUEST(request, response) \
	if(!sendRequest((request), (response))) \
	{ \
		setError(NetworkError, 1, "Failed to send request to server"); \
		return false; \
	} 

#define CHECK_PTR(ptr) \
	if(!ptr) \
	{ \
		setError(InvalidParam, -2, "Null Pointer"); \
		return false; \
	}

#define BEGIN_PARSE try

#define END_PARSE catch (std::exception& e) { setError(NetworkError, 2, "Failed to parse response"); return false; }

#define BEGIN_PARSE_RESPONSE(valueReq, valueRes) \
	{ \
		Json::FastWriter writer; \
		Json::Reader reader; \
		std::string strRequest, strResponse; \
		TRY_SEND_REQUEST(strRequest, strResponse); \
		try { \
		reader.parse(strResponse, valueRes);

#define END_PARSE_RESPONSE } END_PARSE }

#endif