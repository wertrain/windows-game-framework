/**
 * @file Quadrangle.h
 * @brief 四角形を扱うクラス
 */
#ifndef GRAPHICS_PRIMITIVE_QUADRANGLE_H_
#define GRAPHICS_PRIMITIVE_QUADRANGLE_H_

#include "Primitive.h"

NS_FW_GFX_BEGIN

/// <summary>
/// 四角形を扱うクラス
/// </summary>
class Quadrangle : public Primitive
{
public:
    Quadrangle() ;
    virtual ~Quadrangle();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
};

NS_FW_GFX_END

#endif // GRAPHICS_PRIMITIVE_QUADRANGLE_H_
