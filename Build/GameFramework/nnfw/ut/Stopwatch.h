/**
 * @file Stopwatch.h
 * @brief ���Ԍv���p���[�e�B���e�B
 */
#ifndef UTILITY_STOPWATCH_H_
#define UTILITY_STOPWATCH_H_

NS_FW_UTIL_BEGIN

/// <summary>
/// ���Ԍv���p���[�e�B���e�B
/// </summary>
class Stopwatch
{
public:
    Stopwatch();

    void Start();
    __int64 GetElapsed();

private:
    std::chrono::system_clock::time_point mStart;
};

NS_FW_UTIL_END

#endif // UTILITY_STOPWATCH_H_
