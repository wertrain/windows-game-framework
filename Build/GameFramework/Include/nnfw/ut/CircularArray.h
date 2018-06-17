/**
 * @file CircularArray.h
 * @brief 環状配列
 */
#ifndef UTILITY_CIRCULARARRAY_H_
#define UTILITY_CIRCULARARRAY_H_

NS_FW_UTIL_BEGIN

/// <summary>
/// 環状配列基底クラス
/// </summary>
template <typename T>
class CircularArrayInterface
{
public:
    CircularArrayInterface<T>::CircularArrayInterface()
        : mArray(nullptr)
        , mHeadIndex(0)
        , mTailIndex(0)
        , mCount(0)
    {

    }
    virtual ~CircularArrayInterface() {}

    T &operator[](const size_t i)
    {
        return mArray[(mHeadIndex + i) % mSize];
    }

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

    bool Enqueue(const T& value)
    {
        if (mCount >= mSize)
        {
            return false;
        }
        mArray[mTailIndex] = value;
        mTailIndex = (mTailIndex + 1) % mSize;
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
        mHeadIndex = (mHeadIndex + 1) % mSize;
        --mCount;
        return true;
    }

    bool IsEmpty()
    {
        return mCount == 0;
        // 下記の判定では判定、満タンと初期の区別がつかない
        //return mHeadIndex == mTailIndex;
    }

    size_t GetSize()
    {
        return mSize;
    }

protected:
    void Construct(T* p, const size_t size)
    {
        mArray = p;
        mSize = size;
    }

private:
    T* mArray;
    size_t mSize;
    uint32_t mHeadIndex;
    uint32_t mTailIndex;
    uint32_t mCount;
};

/// <summary>
/// 固定長環状配列
/// </summary>
template <typename T, size_t N>
class FixedCircularArray : public CircularArrayInterface<T>
{
public:
    FixedCircularArray() 
        : CircularArrayInterface()
    {
        Construct(mData, N);
    }
    ~FixedCircularArray() {}

private:
    T mData[N];
};

/// <summary>
/// 可変長メモリ環状配列
/// </summary>
template <typename T>
class CircularArray : public CircularArrayInterface<T>
{
public:
    CircularArray()
        : CircularArrayInterface()
        , mData(nullptr)
    {

    }
    CircularArray(const size_t size)
        : CircularArrayInterface()
        , mData(new T[size])
    {
        Construct(mData, size);
    }
    ~CircularArray() { Finalize(); }

    bool Initialize(const size_t size)
    {
        if (mData == nullptr)
        {
            mData = new T[size];
            Construct(mData, size);
            return true;
        }
        return false;
    }
    void Finalize()
    {
        delete [] mData;
        mData = nullptr;
    }

private:
    T* mData;
};

NS_FW_UTIL_END

#endif // UTILITY_CIRCULARARRAY_H_

