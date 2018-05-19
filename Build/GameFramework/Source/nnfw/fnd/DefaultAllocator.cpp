/**
 * @file DefaultAllocator.cpp
 * @brief デフォルトアロケータ
 */
#include "Precompiled.h"

#include <nnfw/external/dlmalloc/malloc.h>
#include <nnfw/fnd/DefaultAllocator.h>

NS_FW_FND_BEGIN

// あまり意味のない Pimpl イディオム

/// DefaultAllocator
/// メモリエリア管理実装
class DefaultAllocator::Implement
{
public:
    Implement()
        : mAreaName()
        , mMSpace(nullptr)
        , mSize(0)
    {

    }

    ~Implement()
    {

    }

    bool Initialize(const char* areaName, const size_t size)
    {
        const size_t nameSize = (sizeof((mAreaName)) / sizeof((mAreaName)[0]));
        strcpy_s(mAreaName, nameSize, areaName);

        mMSpace = create_mspace(size, 0);
        assert(mMSpace);
        mSize = size;
        return mMSpace != nullptr;
    }

    void Finalize()
    {
        if (mMSpace)
        {
            destroy_mspace(mMSpace);
            mMSpace = nullptr;
        }
    }

    mspace GetMSpace()
    {
        return mMSpace;
    }

private:
    char mAreaName[32];
    mspace mMSpace;
    size_t mSize;
};

//-----------------------------------------------------------------------------

DefaultAllocator::DefaultAllocator()
    : mImplement(new DefaultAllocator::Implement())
{

}

DefaultAllocator::~DefaultAllocator()
{

}

bool DefaultAllocator::Initialize(const char* areaName, const size_t size)
{
    return mImplement->Initialize(areaName, size);
}

void DefaultAllocator::Finalize()
{
    mImplement->Finalize();
}

void* DefaultAllocator::Alloc(const size_t size)
{
    return Alloc(size, DEFAULT_ALIGNMENT_SIZE);
}

void* DefaultAllocator::Alloc(const size_t size, const size_t align)
{
    return mspace_memalign(mImplement->GetMSpace(), align, size);
}

void DefaultAllocator::Free(void* memory)
{
    mspace_free(mImplement->GetMSpace(), memory);
}

void* DefaultAllocator::ReAlloc(void* memory, const size_t size)
{
    return mspace_realloc(mImplement->GetMSpace(), memory, size);
}

NS_FW_FND_END
