/**
 * @file ModelBase.h
 * @brief 3D モデル基底クラス
 */
#ifndef GRAPHICS_MODEL_MODELBASE_H_
#define GRAPHICS_MODEL_MODELBASE_H_

#include <nnfw/common/Defines.h>
#include <nnfw/common/Types.h>

NS_FW_GFX_BEGIN

/// <summary>
/// 3D モデル基底クラス
/// </summary>
class ModelBase
{
public:
    ModelBase() ;
    virtual ~ModelBase();
};

NS_FW_GFX_END

#endif // GRAPHICS_MODEL_MODELBASE_H_
