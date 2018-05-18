/**
 * @file DefaultAllocator.cpp
 * @brief デフォルトアロケータ
 */
#include "Precompiled.h"

#include <nnfw/external/dlmalloc/malloc.h>
#include <nnfw/fnd/DefaultAllocator.h>

NS_FW_FND_BEGIN

class DefaultAllocator::Implement
{
public:
};

DefaultAllocator::DefaultAllocator()
    : mImplement(new DefaultAllocator::Implement())
{

}

DefaultAllocator::~DefaultAllocator()
{

}

bool DefaultAllocator::Initialize(const char* areaName, const size_t size)
{
    return true;
}

void DefaultAllocator::Finalize()
{

}

void* Alloc(const size_t size)
{
    return nullptr;
}

void Free(void* memory)
{

}

NS_FW_FND_END
