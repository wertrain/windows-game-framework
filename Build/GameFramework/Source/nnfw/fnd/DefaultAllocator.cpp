/**
 * @file DefaultAllocator.cpp
 * @brief デフォルトアロケータ
 */
#include "Precompiled.h"


#include <nnfw/external/dlmalloc/malloc.h>
#include <nnfw/fnd/DefaultAllocator.h>

#ifndef NDEBUG
namespace
{
    static NS_FW::dictonary<void*, size_t> sInnerMemoryMap;
    static NS_FW::mutex sMutex;
}
#endif // #ifndef NDEBUG

NS_FW_FND_BEGIN

// あまり意味のない Pimpl イディオム

/// DefaultAllocator
/// メモリエリア管理実装
class DefaultAllocator::Implement
{
public:
    Implement()
        : mId(0)
        , mMSpace(nullptr)
        , mSize(0)
    {

    }

    ~Implement()
    {

    }

    bool Initialize(const uint8_t id, const size_t size)
    {
        mId = id;
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

    const uint8_t GetId()
    {
        return mId;
    }

private:
    uint8_t mId;
    uint8_t padding[7];
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

bool DefaultAllocator::Initialize(const uint8_t id, const size_t size)
{
    return mImplement->Initialize(id, size);
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
    const size_t dbgblockSize =
#ifdef NDEBUG
        0;
#else
        sizeof(DefaultAllocator::DebugBlock);
#endif
    void* memory = mspace_memalign(mImplement->GetMSpace(), align, size + dbgblockSize);
    assert(memory);

#ifndef NDEBUG
    SetDebugBlock(memory, size);
#endif
    return memory;
}

void DefaultAllocator::Free(void* memory)
{
#ifndef NDEBUG
    {
        NS_FW::ScopedLock<NS_FW::mutex> lock(sMutex);

        auto it = sInnerMemoryMap.find(memory);
        if (it != sInnerMemoryMap.end())
        {
            const size_t size = sInnerMemoryMap[memory];
            DebugBlock* block = static_cast<DebugBlock*>(memory) + size;
            // メモリ破壊検出
            assert(block->trap == DETECT_MEMORY_CORRUPTION_VALUE);
            
            // メモリを指定値で埋める
            memset(memory, 0xFE, size + sizeof(DefaultAllocator::DebugBlock));

            sInnerMemoryMap.erase(it);
        }
    }
#endif
    mspace_free(mImplement->GetMSpace(), memory);
}

void* DefaultAllocator::ReAlloc(void* memory, const size_t size)
{
    const size_t dbgblockSize =
#ifdef NDEBUG
        0;
#else
        sizeof(DefaultAllocator::DebugBlock);
#endif
    void* newMemory = mspace_realloc(mImplement->GetMSpace(), memory, size + dbgblockSize);
    assert(newMemory);

#ifndef NDEBUG
    SetDebugBlock(newMemory, size);
#endif
    return newMemory;
}

#ifndef NDEBUG
void DefaultAllocator::SetDebugBlock(void* memory, const size_t size)
{
    const size_t dbgblockSize = sizeof(DefaultAllocator::DebugBlock);
    // メモリを指定値で埋める
    memset(memory, 0xC1, size);

    DebugBlock* block = static_cast<DebugBlock*>(memory) + size;
    memset(block, 0, sizeof(DebugBlock));
    block->trap = DETECT_MEMORY_CORRUPTION_VALUE;
    block->size = size;
    block->memory = memory;
    block->area = mImplement->GetId();

    sInnerMemoryMap[memory] = size;
}
#endif

//-----------------------------------------------------------------------------

DefaultAllocatorManager::DefaultAllocatorManager()
    : mAllocator()
{

}

DefaultAllocatorManager::~DefaultAllocatorManager()
{

}

bool DefaultAllocatorManager::Initialize()
{
    const char* areaName[MemoryArea::AREA_NUM] =
    {
        "AREA_SYSTEM",
        "AREA_MAIN",
        "AREA_DEBUG"
    };

    for (int i = 0; i < MemoryArea::AREA_NUM; ++i)
    {
        if (!mAllocator[i].Initialize(static_cast<MemoryArea>(i), static_cast<MemoryAreaSize>(i)))
        {
            return false;
        }
    }
    return true;
}

void DefaultAllocatorManager::Finalize()
{
    for (int i = 0; i < MemoryArea::AREA_NUM; ++i)
    {
        mAllocator[i].Finalize();
    }
}

DefaultAllocator* DefaultAllocatorManager::GetAllocator(const MemoryArea area)
{
    assert(area < MemoryArea::AREA_NUM);
    return &mAllocator[area];
}

//-----------------------------------------------------------------------------

#ifndef NDEBUG

#include <nnfw/common/Debug.h>

bool DefaultAllocatorManager::CheckMemoryLeak()
{
    if (sInnerMemoryMap.empty())
    {
        return false;
    }
    return true;
}

bool DefaultAllocatorManager::CheckMemoryCorruption()
{
    for each (auto it in sInnerMemoryMap)
    {
        DefaultAllocator::DebugBlock* block = static_cast<DefaultAllocator::DebugBlock*>(it.first) + it.second;

        if (block->trap != DefaultAllocator::DETECT_MEMORY_CORRUPTION_VALUE)
        {
            return true;
        }
    }
    return false;
}

#endif // #ifndef NDEBUG

NS_FW_FND_END
