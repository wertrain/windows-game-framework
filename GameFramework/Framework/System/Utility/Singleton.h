#ifndef UTILITY_SINGLETON_H_
#define UTILITY_SINGLETON_H_

#include <memory>
#include "../../Common/Defines.h"

NS_FW_UTIL_BEGIN

template <class T>
class Singleton
{
public:
    static T& singleton()
    {
        static typename T::singleton_pointer_type s_singleton(T::createInstance());

        return GetReference(s_singleton);
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
