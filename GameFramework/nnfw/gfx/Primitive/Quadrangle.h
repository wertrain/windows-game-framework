/**
 * @file Quadrangle.h
 * @brief �l�p�`�������N���X
 */
#ifndef GRAPHICS_PRIMITIVE_QUADRANGLE_H_
#define GRAPHICS_PRIMITIVE_QUADRANGLE_H_

#include "Primitive.h"

NS_FW_GFX_BEGIN

/// <summary>
/// �l�p�`�������N���X
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
