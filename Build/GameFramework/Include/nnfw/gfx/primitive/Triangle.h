/**
 * @file Triangle.h
 * @brief �O�p�`�������N���X
 */
#ifndef GRAPHICS_PRIMITIVE_TRIANGLE_H_
#define GRAPHICS_PRIMITIVE_TRIANGLE_H_

#include <nnfw/gfx/primitive/Primitive.h>

NS_FW_GFX_BEGIN

/// <summary>
/// �O�p�`�������N���X
/// </summary>
class Triangle : public Primitive
{
public:
    Triangle() ;
    virtual ~Triangle();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
};

NS_FW_GFX_END

#endif // GRAPHICS_PRIMITIVE_TRIANGLE_H_
