/**
 * @file CircularArray.h
 * @brief ŠÂó”z—ñ
 */
#ifndef UTILITY_CIRCULARARRAY_H_
#define UTILITY_CIRCULARARRAY_H_

NS_FW_UTIL_BEGIN

/// <summary>
/// ŠÂó”z—ñ
/// </summary>
template <typename T, int N>
class CircularArray
{
public:
    CircularArray<T, N>::CircularArray()
        : mArray()
        , mHeadIndex(0)
        , mTailIndex(0)
    {

    }
    ~CircularArray() {};

    void Clear(const T value)
    {
        for (uint32_t i = 0; i < N; ++i)
        {
            mArray[i] = value;
        }
    }

    bool Enqueue(const T value)
    {
        if ((mTailIndex + 1) % N == mHeadIndex)
        {
            return false;
        }
        mArray[mTailIndex] = value;
        mTailIndex = (mTailIndex + 1) % N;
        return true;
    }

    T Dequeue()
    {
        const int head = mHeadIndex;
        mHeadIndex = (mHeadIndex + 1) % N;
        return mArray[head];
    }

    bool IsEmpty()
    {
        return mHeadIndex == mTailIndex;
    }

private:
    T mArray[N];
    uint32_t mHeadIndex;
    uint32_t mTailIndex;
};

NS_FW_UTIL_END

#endif // UTILITY_CIRCULARARRAY_H_

