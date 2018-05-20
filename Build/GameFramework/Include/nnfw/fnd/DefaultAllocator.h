/**
 * @file DefaultAllocator.h
 * @brief デフォルトアロケータ
 */
#ifndef FOUNDATION_DEFAULTALLOCATOR_H_
#define FOUNDATION_DEFAULTALLOCATOR_H_

#include <nnfw/fnd/GenericPool.h>
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
    bool Initialize(const uint8_t id, const size_t size = DEFAULT_ALLOC_SIZE);
    void Finalize();

    void* Alloc(const size_t size) final;
    void* Alloc(const size_t size, const size_t align) final;
    void Free(void* memory) final;
    void* ReAlloc(void* memory, const size_t size);

private:
    static const size_t DEFAULT_ALLOC_SIZE = 1024 * 1024 * 64;
    static const size_t DEFAULT_ALIGNMENT_SIZE = 16;
    static const uint32_t DETECT_MEMORY_CORRUPTION_VALUE = 0xBAADF00D;

private:
    struct DebugBlock
    {
        uint32_t trap;
        uint8_t padding_1[4];
        void* memory;
        size_t size;
        uint8_t area;
        uint8_t padding_2[7];
    };
    static_assert(sizeof(DebugBlock) == 32, "sizeof(DebugBlock) == 32");

#ifndef NDEBUG
    void SetDebugBlock(void* memory, const size_t size);
#endif // #ifndef NDEBUG

private:
    class Implement;
    std::unique_ptr<Implement> mImplement;
};

/// <summary>
/// デフォルトアロケータマネージャー
/// </summary>
class DefaultAllocatorManager
{
public:
    /// メモリエリア
    enum MemoryArea : uint8_t
    {
        AREA_SYSTEM = 0,
        AREA_MAIN,
        AREA_DEBUG,
        AREA_NUM
    };
    /// エリアごとのメモリサイズ
    enum MemoryAreaSize
    {
        AREA_SIZE_SYSTEM = 1024 * 1024 * 32,
        AREA_SIZE_MAIN   = 1024 * 1024 * 256,
        AREA_SIZE_DEBUG  = 1024 * 1024 * 128,
        AREA_SIZE_NUM
    };

public:
    DefaultAllocatorManager();
    ~DefaultAllocatorManager();

    bool Initialize();
    void Finalize();

    DefaultAllocator* GetAllocator(const MemoryArea area);

private:
    DefaultAllocator mAllocator[AREA_NUM];
};

NS_FW_FND_END

#endif // FOUNDATION_DEFAULTALLOCATOR_H_
