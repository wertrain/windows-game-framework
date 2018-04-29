/**
 * @file Memory.h
 * @brief �������֘A���[�e�B���e�B
 */
#ifndef UTILITY_MEMORY_H_
#define UTILITY_MEMORY_H_

#include <memory>
#include <nnfw/common/Defines.h>

NS_FW_UTIL_BEGIN

/// <summary>
/// 0 �N���A�p���[�e�B���e�B
/// </summary>
__forceinline void memset_zero(void* p, const size_t size)
{
    memset(p, 0, size);
}

NS_FW_UTIL_END

#endif // UTILITY_MEMORY_H_
