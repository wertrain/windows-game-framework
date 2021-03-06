/**
 * @file ObjectBase.h
 * @brief フレームワークに関するオブジェクトすべての基底クラス
 */
#ifndef CORE_OBJECTBASE_H_
#define CORE_OBJECTBASE_H_

#include "../Common/Defines.h"
#include "../Common/Types.h"

NS_FW_SYS_BEGIN

/// <summary>
/// フレームワークに関するオブジェクトすべての基底クラス
/// </summary>
class ObjectBase
{
public:
    ObjectBase();
    virtual ~ObjectBase() = 0;
};

NS_FW_SYS_END

#endif // CORE_OBJECTBASE_H_
