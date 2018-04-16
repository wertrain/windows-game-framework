/**
 * @file Stopwatch.cpp
 * @brief ���Ԍv���p���[�e�B���e�B
 */
#include <memory>
#include <chrono>
#include "../Common/Defines.h"
#include "Stopwatch.h"

NS_FW_UTIL_BEGIN

Stopwatch::Stopwatch()
    : mStart()
{

}

void Stopwatch::Start()
{
    mStart = std::chrono::system_clock::now();
}

__int64 Stopwatch::GetElapsed()
{
    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
}

NS_FW_UTIL_END
