#ifndef __KX_LOG_FILE_HANDLER_H__
#define __KX_LOG_FILE_HANDLER_H__

#include "ILogHandler.h"

class LogFileHandler : public ILogHandler
{
public:
    LogFileHandler();
    virtual ~LogFileHandler();

    virtual bool onLog(int level, const std::string& log);

    inline void setMaxFileCount(int count)
    {
        m_maxFileCount = count;
    }

    inline void setMaxFileSize(int size)
    {
        m_maxFileSize = size;
    }

    inline void setFilePath(const std::string& path)
    {
        m_filePath = path;
    }

    inline void setFileName(const std::string& fileName)
    {
        m_fileName = fileName;
    }

    inline void setFastModel(bool isfast)
    {
        m_fastMode = isfast;
    }

	inline void setNewFile(bool isNewFile)
	{
		m_newFile = isNewFile;
	}

private:

    bool writeLog(const std::string& log);

    bool writeFastLog(const std::string& log);

    bool openLogfile(const char* mode);

    void flush();

    bool checkNewFile(int addSize);

private:
    bool m_newFile;
    bool m_fastMode;
    FILE* m_curFile;
    int m_curFileCount;
    int m_maxFileCount;
    int m_curFileSize;
    int m_maxFileSize;
    std::string m_filePath;
    std::string m_fileName;
};



#endif //__KX_LOG_FILE_HANDLER_H__
