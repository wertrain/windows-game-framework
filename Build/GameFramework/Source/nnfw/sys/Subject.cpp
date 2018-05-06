/**
* @file Subject.cpp
* @brief サブジェクトインターフェース
*/
#include "Precompiled.h"

#include <nnfw/sys/Observer.h>
#include <nnfw/sys/Subject.h>

NS_FW_SYS_BEGIN

NS_FW_FND::GenericPool<Subject::ObserverNode, 128> SubjectBase::mObserverNodePool;

SubjectBase::~SubjectBase()
{

}

Subject::~Subject()
{

}

void Subject::AddObserver(Observer* observer)
{
    auto node = mObserverNodePool.Get();
    assert(node);
    if (node == nullptr)
    {
        return;
    }

    node->SetObserver(observer);
    node->SetNext(nullptr);

    if (mHead == nullptr)
    {
        mHead = node;
    }
    else
    {
        auto p = mHead;
        while (p)
        {
            if (p->GetNext() == nullptr)
            {
                p->SetNext(node);
                break;
            }
            else
            {
                p = p->GetNext();
            }
        }
    }
}

void Subject::RemoveObserver(Observer* observer)
{
    if (mHead->GetObserver() == observer)
    {
        mHead = mHead->GetNext();
        return;
    }

    auto prev = mHead;
    auto current = mHead->GetNext();
    while (current)
    {
        if (current->GetObserver() == observer)
        {
            prev->SetNext(current->GetNext());
            break;
        }
        else
        {
            prev = current;
            current = current->GetNext();
        }
    }
}

void Subject::Notify(const uint32_t eventId)
{
    auto p = mHead;
    while (p)
    {
        p->GetObserver()->OnNotify(eventId);
        p = p->GetNext();
    }
}

NS_FW_SYS_END
