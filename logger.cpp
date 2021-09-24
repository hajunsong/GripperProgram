#include "logger.h"

Logger::Logger(const string &fileName)
{
    m_showDate = true;
    if (!fileName.empty())
    {
        file = fopen(fileName.c_str(), "a+");
    }
}

void Logger::write(const string &value)
{
    string text = value;
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);

    if(file != nullptr)
    {
        if(m_showDate)
        {
            fprintf(file, "%04d-%02d-%02d-%02d-%02d-%02d\t%s\n",
                tstruct.tm_year + 1900, tstruct.tm_mon + 1, tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min, tstruct.tm_sec, text.c_str());
            fflush(file);
        }
        else
        {
            fprintf(file, "%s\n", text.c_str());
            fflush(file);
        }
    }
}

void Logger::setShowDateTime(bool value)
{
    m_showDate = value;
}

Logger::~Logger()
{
    if (file != nullptr)
    {
        fclose(file);
    }
}
