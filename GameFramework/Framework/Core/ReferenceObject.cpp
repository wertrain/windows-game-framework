#include "ReferenceObject.h"

NS_FW_SYS_BEGIN

ReferenceObject::ReferenceObject()
    : mRefCounter(nullptr)
{
    mRefCounter = new ReferenceCounter();
}

ReferenceObject::ReferenceObject(const ReferenceObject& r)
    : mRefCounter(r.mRefCounter)
{
    if (mRefCounter) mRefCounter->Inc();
}

ReferenceObject::~ReferenceObject()
{
    Release();
}

ReferenceObject &ReferenceObject::operator=(const ReferenceObject &r)
{
    ReferenceCounter* rc = r.mRefCounter;
    if (rc)
    {
        Release();
        mRefCounter = rc;
    }
    return (*this);
}

int ReferenceObject::AddRef()
{
    if (mRefCounter)
        return mRefCounter->Inc();
    return 0;
}

int ReferenceObject::GetRef()
{
    if (mRefCounter)
        return mRefCounter->Get();
    return 0;
}

int ReferenceObject::Release()
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
