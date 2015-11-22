#include "utils.h"

std::string str2sql(const std::string& str)
{
	std::string result;
	size_t length = str.length();
	for (int i = 0; i < length; i++)
	{
		if (str[i] == '\'')
			result += "\\\'";
		else if (str[i] == '\"')
			result += "\\\"";
		else if (str[i] == '\\')
			result += "\\\\";
		else
			result += str[i];
	}
	return result;
}