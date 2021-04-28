#ifndef __HEAD_H__
#define __HEAD_H__

inline int MakeCommand(int main, int sub)
{
    return (main << 16) | (sub & 0x0000ffff);
}

struct Head
{
    int length;
    int cmd;
	int uid;

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
