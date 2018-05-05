/**
 * @file ReferencedObject.h
 * @brief 参照されるオブジェクトの基底クラス
 */
#ifndef CORE_REFERENCEDOBJECT_H_
#define CORE_REFERENCEDOBJECT_H_

#include "../Common/Defines.h"
#include "../Common/Types.h"

NS_FW_SYS_BEGIN

/// <summary>
/// 参照されるオブジェクトの基底クラス
/// </summary>
class ReferencedObject
{
public:
    ReferencedObject();
    ReferencedObject(const ReferencedObject& r);
    virtual ~ReferencedObject();
    ReferencedObject &operator=(const ReferencedObject &r);

public:
    virtual int32_t AddRef();
    virtual int32_t GetRef();
    virtual int32_t Release();

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

#endif // CORE_REFERENCEDOBJECT_H_
