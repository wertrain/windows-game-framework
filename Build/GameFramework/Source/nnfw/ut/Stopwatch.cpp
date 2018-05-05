/**
 * @file Stopwatch.cpp
 * @brief 時間計測用ユーティリティ
 */
#include "Precompiled.h"

#include <memory>
#include <chrono>

#include <nnfw/ut/Stopwatch.h>

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
