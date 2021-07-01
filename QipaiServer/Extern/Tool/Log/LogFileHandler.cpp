#include "LogFileHandler.h"

LogFileHandler::LogFileHandler()
:m_fastMode(true),
m_curFile(NULL),
m_curFileCount(0),
m_maxFileCount(1),
m_curFileSize(0),
m_maxFileSize(100 * 1024 * 1024),
m_filePath(""),
m_fileName("debuglog")
{

}

LogFileHandler::~LogFileHandler()
{
    flush();
}

bool LogFileHandler::onLog(int level, const std::string& log)
{
    if (m_fastMode)
        return writeFastLog(log);
    else
        return writeLog(log);
}

bool LogFileHandler::writeLog(const std::string& log)
{
    if ((m_newFile && openLogfile("w+")) || (!m_newFile && openLogfile("a+")))
    {
        int ret = (int)fwrite(log.c_str(), sizeof(char), log.length(), m_curFile);
        fwrite("\n", 1, 1, m_curFile);
        fclose(m_curFile);
        m_curFile = NULL;
        if (ret < 0)
			return false;
        else
        {
            checkNewFile(ret);
            return true;
        }
    }
    return false;
}

bool LogFileHandler::writeFastLog(const std::string& log)
{
    if (NULL == m_curFile || m_newFile)
		if (!openLogfile("w+"))        // 新的日志文件
            return false;

    int ret = (int)fwrite(log.c_str(), sizeof(char), log.length(), m_curFile);
    fwrite("\n", 1, 1, m_curFile);
    if (ret < 0)
    {
        fclose(m_curFile);
        m_curFile = NULL;
        return false;
    }

    if (checkNewFile(ret))
    {
        fclose(m_curFile);
        m_curFile = NULL;
    }

    return true;
}

bool LogFileHandler::openLogfile(const char* mode)
{
    flush();

    if (m_newFile)
    {
        m_curFileSize = 0;
        m_newFile = false;
    }

   std::string fullPath = m_filePath + m_fileName +/* toolToStr(m_curFileCount) + */".log";
    m_curFile = fopen(fullPath.c_str(), mode);
    if (NULL == m_curFile)
    {
        return false;
    }

    return true;
}

void LogFileHandler::flush()
{
    if (NULL != m_curFile)
    {
        fclose(m_curFile);
        m_curFile = NULL;
    }
}

bool LogFileHandler::checkNewFile(int addSize)
{
    m_curFileSize += addSize;
    if (0 != m_maxFileSize
        && m_curFileSize >= m_maxFileSize)
    {
        m_curFileCount = (m_curFileCount + 1) % m_maxFileCount;
        m_newFile = true;
        return true;
    }

    return false;
}


