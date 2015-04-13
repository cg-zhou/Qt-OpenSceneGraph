#ifndef TOOL_H
#define TOOL_H

#include <QString>
#include <QMessageBox>

typedef unsigned long long ulonglong;

class Tool
{
public:
    static void Warning(const QString& text);

    static bool isPrimeNumber(const ulonglong& number, ulonglong& divNumber);

};

#endif // TOOL_H
