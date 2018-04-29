/**
 * @file Singleton.h
 * @brief シングルトンユーティリティ
 */
#ifndef UTILITY_SINGLETON_H_
#define UTILITY_SINGLETON_H_

#include <memory>
#include <nnfw/common/Defines.h>

NS_FW_UTIL_BEGIN

/// <summary>
/// シングルトンテンプレートクラス
/// </summary>
template <class T>
class Singleton
{
public:
    static T& GetInstance()
    {
        static typename T::singleton_pointer_type instance(T::CreateInstance());
        return GetReference(instance);
    }

private:
    typedef std::unique_ptr<T> singleton_pointer_type;
    inline static T *CreateInstance() { return new T(); }
    inline static T &GetReference(const singleton_pointer_type &ptr) { return *ptr; }

protected:
    Singleton() {}

private:
    Singleton(const Singleton &) = delete;
    Singleton& operator=(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton& operator=(Singleton &&) = delete;
};

NS_FW_UTIL_END

#endif // #define UTILITY_SINGLETON_H_
