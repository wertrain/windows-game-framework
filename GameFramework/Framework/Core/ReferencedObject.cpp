/**
 * @file ReferencedObject.cpp
 * @brief 参照されるオブジェクトの基底クラス
 */
#include "ReferencedObject.h"

NS_FW_SYS_BEGIN

ReferencedObject::ReferencedObject()
    : mRefCounter(nullptr)
{
    mRefCounter = new ReferenceCounter();
}

ReferencedObject::ReferencedObject(const ReferencedObject& r)
    : mRefCounter(r.mRefCounter)
{
    if (mRefCounter) mRefCounter->Inc();
}

ReferencedObject::~ReferencedObject()
{
    Release();
}

ReferencedObject &ReferencedObject::operator=(const ReferencedObject &r)
{
    ReferenceCounter* rc = r.mRefCounter;
    if (rc)
    {
        Release();
        mRefCounter = rc;
    }
    return (*this);
}

int ReferencedObject::AddRef()
{
    if (mRefCounter)
        return mRefCounter->Inc();
    return 0;
}

int ReferencedObject::GetRef()
{
    if (mRefCounter)
        return mRefCounter->Get();
    return 0;
}

int ReferencedObject::Release()
{
    if (mRefCounter)
    {
        const int ref = mRefCounter->Dec();
        if (ref == 0)
        {
            delete mRefCounter;
        }
        mRefCounter = nullptr;
        return ref;
    }
    return 0;
}

NS_FW_SYS_END
