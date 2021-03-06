/**
 * @file Random.cpp
 * @brief �����_��
 */
#include "Precompiled.h"

#include <nnfw/ut/Random.h>

NS_FW_UTIL_BEGIN

Random::Random()
    : mImplement()
{

}

Random::Random(unsigned long ulSeed)
    : mImplement()
{
    mImplement.init_genrand(ulSeed);
}

Random::Random(unsigned long aulInitKey[], int iKeyLength)
    : mImplement()
{
    mImplement.init_by_array(aulInitKey, iKeyLength);
}

Random::~Random()
{

}

float Random::NextFloat()
{
    return static_cast<float>(mImplement.genrand_real1());
}

double Random::NextDouble()
{
    return mImplement.genrand_real1();
}

uint32_t Random::NextUInt()
{
    return mImplement.genrand_uint32();
}

int32_t Random::Random::GetRange(const int32_t min, const int32_t max)
{
    return mImplement.genrand_uint32() % (max - min + 1) + min;
}

uint32_t Random::Random::GetRange(const uint32_t min, const uint32_t max)
{
    return mImplement.genrand_uint32() % (max - min + 1) + min;
}

NS_FW_UTIL_END
