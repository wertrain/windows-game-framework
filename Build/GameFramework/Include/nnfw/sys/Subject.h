/**
 * @file Subject.h
 * @brief �T�u�W�F�N�g�C���^�[�t�F�[�X
 */
#ifndef SYSTEM_SUBJECT_H_
#define SYSTEM_SUBJECT_H_

#include <nnfw/fnd/GenericPool.h>

NS_FW_SYS_BEGIN

class Observer;

/// <summary>
/// �T�u�W�F�N�g���N���X
/// </summary>
class SubjectBase
{
public:
    virtual ~SubjectBase() = 0;

protected:
    /// <summary>
    /// �I�u�U�[�o�m�[�h
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
/// �T�u�W�F�N�g�C���^�[�t�F�[�X
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
