#ifndef UTILITY_H_
#define UTILITY_H_

namespace Framework {
namespace System {
namespace Utility {

/// <summary>
/// 0 �N���A�p���[�e�B���e�B
/// </summary>
/// �����ɂ����ׂ��Ȃ̂�
__forceinline void memset_zero(void* p, const size_t size)
{
    memset(p, 0, size);
}

} // namespace Utility
} // namespace System
} // namespace Framework

#endif // UTILITY_H_
