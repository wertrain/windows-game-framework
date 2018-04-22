/**
 * @file GraphicsBase.h
 * @brief 描画関連オブジェクトの基底クラス
 */
#ifndef GRAPHICS_GRAPHICSBASE_H_
#define GRAPHICS_GRAPHICSBASE_H_

#include <nnfw/core/ObjectBase.h>

NS_FW_GFX_BEGIN

/// <summary>
/// 描画関連オブジェクトの基底クラス
/// </summary>
class GraphicsBase : public NS_FW_SYS::ObjectBase
{
public:
    GraphicsBase() ;
    virtual ~GraphicsBase() = 0;
};

NS_FW_GFX_END

#endif // GRAPHICS_GRAPHICSBASE_H_
