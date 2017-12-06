#include <tchar.h>
#include <stdlib.h>
#include <windows.h>

#include "Debug.h"

void DebugPrintf(wchar_t* pszFormat, ...)
{
#ifdef _DEBUG
#ifdef _WIN64

    va_list argp;
    TCHAR pszBuf[256];
    va_start(argp, pszFormat);

    _vstprintf(pszBuf, sizeof(pszBuf), pszFormat, argp);
    va_end(argp);
    OutputDebugString(pszBuf);

#endif // _WIN64
#endif // DEBUG 
}


