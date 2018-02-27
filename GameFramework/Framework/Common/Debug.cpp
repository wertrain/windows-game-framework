/**
 * @file Debug.cpp
 * @brief デバッグ関連定義
 */
#include <stdio.h> 
#include <stdarg.h>
#include <stdlib.h>
#ifdef _WIN64
#include <Windows.h>
#endif // _WIN64

#include "Debug.h"

#ifdef _DEBUG
#ifdef _WIN64

void DebugPrintf(wchar_t* pszFormat, ...)
{
    va_list argp;
    wchar_t pszBuf[256];
    va_start(argp, pszFormat);

    vswprintf(pszBuf, sizeof(pszBuf), pszFormat, argp);
    va_end(argp);
    OutputDebugStringW(pszBuf);
}

void DebugPrintf(char* pszFormat, ...)
{
    va_list argp;
    char pszBuf[256];
    va_start(argp, pszFormat);

    vsprintf_s(pszBuf, sizeof(pszBuf), pszFormat, argp);
    va_end(argp);
    OutputDebugStringA(pszBuf);
}

#endif // _WIN64
#endif // DEBUG 
