#ifndef __UTILS_CLIENT_H__
#define __UTILS_CLIENT_H__

#define _TRY_SEND_REQUEST(request, response, falseret) \
	if(!sendRequest((request), (response))) \
	{ \
		setError(NetworkError, 1, "Failed to send request to server"); \
		return (falseret); \
	} 

#define _CHECK_PTR(ptr, falseret) \
	if(!ptr) \
	{ \
		setError(InvalidParam, -2, "Null Pointer"); \
		return (falseret); \
	}

#define _BEGIN_PARSE try

#define _END_PARSE(falseret) catch (std::exception& e) { setError(NetworkError, 2, "Failed to parse response"); return (falseret); }

#define _BEGIN_PARSE_RESPONSE(valueReq, valueRes, falseret) \
	{ \
		Json::FastWriter writer; \
		Json::Reader reader; \
		std::string strRequest, strResponse; \
		strRequest = writer.write(valueReq); \
		_TRY_SEND_REQUEST(strRequest, strResponse, falseret); \
		try { \
		reader.parse(strResponse, valueRes);

#define _END_PARSE_RESPONSE(falseret) } _END_PARSE(falseret) }


#define TRY_SEND_REQUEST(request, response) _TRY_SEND_REQUEST(request, response, false)

#define CHECK_PTR(ptr) _CHECK_PTR(ptr, false)

#define BEGIN_PARSE _BEGIN_PARSE

#define END_PARSE _END_PARSE(false)

#define BEGIN_PARSE_RESPONSE(valueReq, valueRes) _BEGIN_PARSE_RESPONSE(valueReq, valueRes, false)

#define END_PARSE_RESPONSE _END_PARSE_RESPONSE(false)

#endif