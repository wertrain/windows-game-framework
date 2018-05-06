/**
 * @file Subject.h
 * @brief サブジェクトインターフェース
 */
#ifndef SYSTEM_SUBJECT_H_
#define SYSTEM_SUBJECT_H_

#include <nnfw/fnd/GenericPool.h>

NS_FW_SYS_BEGIN

class Observer;

/// <summary>
/// サブジェクト基底クラス
/// </summary>
class SubjectBase
{
public:
    virtual ~SubjectBase() = 0;

protected:
    /// <summary>
    /// オブザーバノード
    /// </summary>
    struct ObserverNode
    {
    private:
        Observer* mObserver;
        ObserverNode* mNext;

    public:
        Observer* GetObserver() { return mObserver; }
        ObserverNode* GetNext() { return mNext; }
        void SetObserver(Observer* observer) { mObserver = observer; }
        void SetNext(ObserverNode* next) { mNext = next; }

        ObserverNode()
            : mObserver(nullptr)
            , mNext(nullptr)
        {

        }
    };

    static NS_FW_FND::GenericPool<ObserverNode, 128> mObserverNodePool;
};

/// <summary>
/// サブジェクトインターフェース
/// </summary>
class Subject : public SubjectBase
{
public:
    Subject() 
        : mHead(nullptr)
    {

    };
    virtual ~Subject() = 0;

    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);
    void Notify(const uint32_t eventId);

private:
    ObserverNode* mHead;
};

NS_FW_SYS_END

#endif // SYSTEM_SUBJECT_H_
