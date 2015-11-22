#ifndef __SERVER_UTILS_H__
#define __SERVER_UTILS_H__

#include <string>
#include <sstream>

template<typename T>
std::string type2str(T num)
{
	std::stringstream sstream;
	sstream << num;
	return sstream.str();
}

std::string str2sql(const std::string& str);)

#endif