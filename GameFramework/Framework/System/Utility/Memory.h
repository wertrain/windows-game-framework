#ifndef UTILITY_H_
#define UTILITY_H_

#include <memory>
#include "../../Common/Defines.h"

NS_FW_UTIL_BEGIN

/// <summary>
/// 0 クリア用ユーティリティ
/// </summary>
/// ここにおくべきなのか
__forceinline void memset_zero(void* p, const size_t size)
{
    memset(p, 0, size);
}

NS_FW_UTIL_END

#endif // UTILITY_H_
