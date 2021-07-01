#ifndef UrlEncode_h__
#define UrlEncode_h__
#include <string>
#include <assert.h>


class UrlEncode
{
public:
	static std::string Encode(const std::string& str);
	static std::string Decode(const std::string& str);
};



#endif
