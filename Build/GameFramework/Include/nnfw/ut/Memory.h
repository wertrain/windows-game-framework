/**
 * @file Memory.h
 * @brief メモリ関連ユーティリティ
 */
#ifndef UTILITY_MEMORY_H_
#define UTILITY_MEMORY_H_

#include <memory>
#include <nnfw/common/Defines.h>

NS_FW_UTIL_BEGIN

/// <summary>
/// 0 クリア用ユーティリティ
/// </summary>
__forceinline void memset_zero(void* p, const size_t size)
{
    memset(p, 0, size);
}

NS_FW_UTIL_END

#endif // UTILITY_MEMORY_H_
