#ifndef __HEAD_H__
#define __HEAD_H__
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

inline int MakeCommand(int main, int sub)
{
    return (main << 16) | (sub & 0x0000ffff);
}

struct Head
{
    int length;
    int cmd;
	int id;

    inline void MakeCommand(int main, int sub)
    {
        cmd = (main << 16) | (sub & 0x0000ffff);
    }

    inline int SubCommand()
    {
        return cmd & 0x0000ffff;
    }

    inline int MainCommand()
    {
        return cmd >> 16;
    }

    inline void* data()
    {
        return this + 1;
    }
};

#endif // __HEAD_H__
