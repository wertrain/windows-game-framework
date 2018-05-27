/**
 * @file Random.cpp
 * @brief ƒ‰ƒ“ƒ_ƒ€
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

double Random::NextDouble()
{
    return mImplement.genrand_real1();
}

uint32_t Random::NextUInt()
{
    return mImplement.genrand_uint32();
}

NS_FW_UTIL_END
