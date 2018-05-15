/**
 * @file DefaultAllocator.h
 * @brief デフォルトアロケータ
 */
#ifndef FOUNDATION_DEFAULTALLOCATOR_H_
#define FOUNDATION_DEFAULTALLOCATOR_H_

#include <nnfw/fnd/Allocator.h>

NS_FW_FND_BEGIN

/// <summary>
/// デフォルトアロケータ
/// </summary>
class DefaultAllocator : public Allocator
{
public:
    DefaultAllocator();
    ~DefaultAllocator();
    bool Initialize();
    void Finalize();
    void* Alloc(const size_t size);
    void Free(void* memory);

private:

};

NS_FW_FND_END

#endif // FOUNDATION_DEFAULTALLOCATOR_H_
