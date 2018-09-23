/**
 * @file Log.h
 * @brief ���O�֘A���[�e�B���e�B
 */
#ifndef UTILITY_LOG_H_
#define UTILITY_LOG_H_

#include <nnfw/common/Defines.h>

NS_FW_UTIL_BEGIN

/// <summary>
/// ���O�o�͊Ǘ��N���X
/// </summary>
class Log
{
public:
    enum Type : uint8_t
    {
        TYPE_BEGIN   = 0,
        TYPE_SYSTEM  = 1 << 0,
        TYPE_INFO    = 1 << 1,
        TYPE_CAUTION = 1 << 2,
        TYPE_DANGER  = 1 << 3,
        TYPE_USER    = TYPE_INFO | TYPE_CAUTION | TYPE_DANGER,
        TYPE_ALL     = 0xFF,
        TYPE_END
    };

public:
    Log();
    ~Log();

private:
    Type mType;
};

NS_FW_UTIL_END

#endif // UTILITY_LOG_H_
