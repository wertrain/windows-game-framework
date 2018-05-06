/**
 * @file GenericPool.h
 * @brief îƒópÉvÅ[Éã
 */
#ifndef FOUNDATION_GENERICPOOL_H_
#define FOUNDATION_GENERICPOOL_H_

NS_FW_FND_BEGIN

template <typename T, int N>
class GenericPool
{
public:
    GenericPool<T, N>::GenericPool()
        : mPool()
        , mInUse()
    {
        for (int i = 0; i < N; ++i)
            mInUse[i] = false;
    }

    T* Get()
    {
        for (int i = 0; i < N; ++i)
        {
            if (!mInUse[i])
            {
                mInUse[i] = true;
                return &mPool[i];
            }
        }
        return nullptr;
    }

    void Remove(T* value)
    {
        for (int i = 0; i < N; ++i)
        {
            if (&mPool[i] == value)
            {
                mInUse[i] = false;
                break;
            }
        }
    }

private:
    T mPool[N];
    bool mInUse[N];
};

NS_FW_FND_END

#endif // FOUNDATION_GENERICPOOL_H_
