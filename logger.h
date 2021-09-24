#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
using namespace std;

class Logger
{
public:
    Logger(const string &fileName);
    ~Logger();
    void setShowDateTime(bool value);
    void write(const string &value);

private:
    FILE *file;
    bool m_showDate;
};

#endif // LOGGER_H
