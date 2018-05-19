/**
 * @file Allocator.h
 * @brief アロケータインターフェース
 */
#ifndef FOUNDATION_ALLOCATOR_H_
#define FOUNDATION_ALLOCATOR_H_

NS_FW_FND_BEGIN

/// <summary>
/// アロケータ基底クラス
/// </summary>
class Allocator
{
public:
    virtual void* Alloc(const size_t size) = 0;
    virtual void* Alloc(const size_t size, const size_t align) = 0;
    virtual void Free(void* memory) = 0;
};

NS_FW_FND_END

#endif // FOUNDATION_ALLOCATOR_H_
