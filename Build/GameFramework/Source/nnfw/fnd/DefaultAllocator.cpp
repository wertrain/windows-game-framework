/**
 * @file DefaultAllocator.cpp
 * @brief デフォルトアロケータ
 */
#include "Precompiled.h"

#include <nnfw/fnd/DefaultAllocator.h>

NS_FW_FND_BEGIN

DefaultAllocator::DefaultAllocator()
{

}

DefaultAllocator::~DefaultAllocator()
{

}

bool DefaultAllocator::Initialize()
{
    return true;
}

void DefaultAllocator::Finalize()
{

}

void* Alloc(const size_t size)
{

}

void Free(void* memory)
{

}

NS_FW_FND_END
