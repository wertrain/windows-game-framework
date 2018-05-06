/**
 * @file Observer.h
 * @brief オブザーバインターフェース
 */
#ifndef SYSTEM_OBSERVER_H_
#define SYSTEM_OBSERVER_H_

NS_FW_SYS_BEGIN

/// <summary>
/// オブザーバインターフェース
/// </summary>
class Observer
{
public:
    virtual ~Observer() = 0;
    virtual void OnNotify(const uint32_t eventId) = 0;
};

NS_FW_SYS_END

#endif // SYSTEM_OBSERVER_H_
