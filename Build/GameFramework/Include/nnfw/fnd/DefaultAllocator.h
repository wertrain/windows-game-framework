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
    bool Initialize(const char* areaName, const size_t size = DEFAULT_ALLOC_SIZE);
    void Finalize();
    void* Alloc(const size_t size);
    void Free(void* memory);

private:
    static const size_t DEFAULT_ALLOC_SIZE = 1024 * 1024 * 64;

private:
    class Implement;
    std::unique_ptr<Implement> mImplement;
};

NS_FW_FND_END

#endif // FOUNDATION_DEFAULTALLOCATOR_H_
