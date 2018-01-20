#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_

#ifdef _DEBUG

void DebugPrintf(wchar_t* pszFormat, ...);
void DebugPrintf(char* pszFormat, ...);

#define DEBUG_MESSAGE_W DebugPrintf
#define DEBUG_MESSAGE_A DebugPrintf
#define DEBUG_MESSAGE DebugPrintf

#else

#define DEBUG_MESSAGE_W ((void)0)
#define DEBUG_MESSAGE_A ((void)0)
#define DEBUG_MESSAGE ((void)0)

#endif // _DEBUG

#endif // COMMON_DEBUG_H_
