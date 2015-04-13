#include "tool.h"
#include "math.h"

void Tool::Warning(const QString& text)
{
    QMessageBox::warning(0, "Warning", text);
}

bool Tool::isPrimeNumber(const ulonglong& number, ulonglong& divNumber)
{
    ulonglong temp = sqrt(number) + 1;
    for (ulonglong i = 2; i < temp; ++i)
    {
        if (number % i == 0)
        {
            divNumber = i;
            return false;
        }
    }

    return true;
}
