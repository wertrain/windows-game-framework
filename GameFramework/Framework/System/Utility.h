#ifndef UTILITY_H_
#define UTILITY_H_

namespace Framework {
namespace System {
namespace Utility {

/**
 * 0 �N���A�p���[�e�B���e�B
 * �����ɂ����ׂ��Ȃ̂��B
 */
__forceinline void memset_zero(void* p, const size_t size)
{
    memset(p, 0, size);
}

} // namespace Utility
} // namespace System
} // namespace Framework

#endif // UTILITY_H_
