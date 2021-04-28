#ifndef __MODEL_DEF_H__
#define __MODEL_DEF_H__

#include <string>
#include <sstream>

class ModelKey
{
public:
	static std::string UsrKey(int uid) 
	{
        return MakeKey(uid, "usr-");
	}

private:
    static std::string MakeKey(int num, std::string prefix)
    {
        std::string key;
        std::stringstream ss;
        ss << num;
        ss >> key;

        return prefix + key;
    }
};

#endif //__MODEL_DEF_H__
