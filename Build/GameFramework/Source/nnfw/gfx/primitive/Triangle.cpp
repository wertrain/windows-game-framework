/**
 * @file Triangle.cpp
 * @brief 三角形を扱うクラス
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
    // 頂点データ構造体
    struct VertexData
    {
        f32 x;
        f32 y;
        f32 z;
        f32 tx;
        f32 ty;
    };

    // 頂点バッファ作成
    VertexData vertices[] = {
        {  0.0f,  0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
    };
    return Primitive::Create(device, context, reinterpret_cast<float*>(vertices), sizeof(VertexData), 3, nullptr);
}

NS_FW_GFX_END
