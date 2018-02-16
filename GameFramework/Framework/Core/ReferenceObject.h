/**
 * @file ReferenceObject.h
 * @brief 参照されるオブジェクトの基底クラス
 */
#ifndef CORE_REFERENCEOBJECT_H_
#define CORE_REFERENCEOBJECT_H_

#include "../Common/Defines.h"
#include "../Common/Types.h"

NS_FW_SYS_BEGIN

/// <summary>
/// 参照されるオブジェクトの基底クラス
/// </summary>
class ReferenceObject
{
public:
    ReferenceObject();
    ReferenceObject(const ReferenceObject& r);
    virtual ~ReferenceObject();
    ReferenceObject &operator=(const ReferenceObject &r);

public:
    virtual s32 AddRef();
    virtual s32 GetRef();
    virtual s32 Release();

private:
    struct ReferenceCounter {
        ReferenceCounter() : mRefCount(1) {}
        ReferenceCounter(const ReferenceCounter& r) : mRefCount(r.mRefCount) {}
        int Inc(void) { return ++mRefCount; }
        int Dec(void) { return --mRefCount; }
        int Get(void) { return mRefCount; }
        int mRefCount;
    };
    ReferenceCounter*  mRefCounter;
};

NS_FW_SYS_END

#endif // CORE_REFERENCEOBJECT_H_
