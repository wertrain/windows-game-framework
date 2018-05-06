/**
 * @file CircularArray.h
 * @brief 環状配列
 */
#ifndef UTILITY_CIRCULARARRAY_H_
#define UTILITY_CIRCULARARRAY_H_

NS_FW_UTIL_BEGIN

/// <summary>
/// 環状配列
/// </summary>
template <typename T, int N>
class CircularArray
{
public:
    CircularArray<T, N>::CircularArray()
        : mArray()
        , mHeadIndex(0)
        , mTailIndex(0)
        , mCount(0)
    {

    }
    ~CircularArray() {};

    void Reset()
    {
        mHeadIndex = 0;
        mTailIndex = 0;
        mCount = 0;
    }

    void Fill(const T value)
    {
        for (uint32_t i = 0; i < N; ++i)
        {
            mArray[i] = value;
        }
    }

    bool Enqueue(const T value)
    {
        if (mCount >= N)
        {
            return false;
        }
        mArray[mTailIndex] = value;
        mTailIndex = (mTailIndex + 1) % N;
        ++mCount;
        return true;
    }

    bool Dequeue(T& value)
    {
        if (mCount <= 0)
        {
            return false;
        }
        value = mArray[mHeadIndex];
        mHeadIndex = (mHeadIndex + 1) % N;
        --mCount;
        return true;
    }

    bool IsEmpty()
    {
        return mCount == 0;
        // 下記の判定では判定、満タンと初期の区別がつかない
        //return mHeadIndex == mTailIndex;
    }

private:
    T mArray[N];
    uint32_t mHeadIndex;
    uint32_t mTailIndex;
    uint32_t mCount;
};

NS_FW_UTIL_END

#endif // UTILITY_CIRCULARARRAY_H_

