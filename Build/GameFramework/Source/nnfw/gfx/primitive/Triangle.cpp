/**
 * @file Triangle.cpp
 * @brief �O�p�`�������N���X
 */
#include "Precompiled.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/Constants.h>

#include <nnfw/gfx/primitive/Triangle.h>

NS_FW_GFX_BEGIN

Triangle::Triangle()
    : Primitive()
{

}

Triangle::~Triangle()
{

}

bool Triangle::Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // ���_�f�[�^�\����
    struct VertexData
    {
        f32 x;
        f32 y;
        f32 z;
        f32 tx;
        f32 ty;
    };

    // ���_�o�b�t�@�쐬
    VertexData vertices[] = {
        {  0.0f,  0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
    };
    return Primitive::Create(device, context, reinterpret_cast<float*>(vertices), sizeof(VertexData), 3, nullptr);
}

NS_FW_GFX_END
