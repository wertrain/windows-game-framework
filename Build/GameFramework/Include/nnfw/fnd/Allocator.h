/**
 * @file Allocator.h
 * @brief �A���P�[�^�C���^�[�t�F�[�X
 */
#ifndef FOUNDATION_ALLOCATOR_H_
#define FOUNDATION_ALLOCATOR_H_

NS_FW_FND_BEGIN

class Allocator
{
public:
    virtual void* Alloc(const size_t size) = 0;
    virtual void Free(void* memory) = 0;
};

NS_FW_FND_END

#endif // FOUNDATION_ALLOCATOR_H_
