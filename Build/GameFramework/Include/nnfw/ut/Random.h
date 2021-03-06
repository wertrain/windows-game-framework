/**
 * @file Random.h
 * @brief ランダム
 */
#ifndef UTILITY_RANDOM_H_
#define UTILITY_RANDOM_H_

#include <nnfw/common/Defines.h>
#include <nnfw/external/MersenneTwister/MersenneTwister.h>

NS_FW_UTIL_BEGIN

/// <summary>
/// 乱数生成
/// </summary>
class Random
{
public:
    Random();
    Random(unsigned long ulSeed);
    Random(unsigned long aulInitKey[], int iKeyLength);
    ~Random();

    float NextFloat();
    double NextDouble();
    uint32_t NextUInt();
    int32_t Random::GetRange(const int32_t min, const int32_t max);
    uint32_t Random::GetRange(const uint32_t min, const uint32_t max);

private:
    CMersenneTwister mImplement;
};

NS_FW_UTIL_END

#endif // UTILITY_RANDOM_H_
