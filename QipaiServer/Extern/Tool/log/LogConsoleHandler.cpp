#include "LogConsoleHandler.h"

bool LogConsoleHandler::onLog(int level, const std::string& log)
{
    printf("%s\n", log.c_str());
    return true;
}



