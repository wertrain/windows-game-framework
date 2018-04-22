/**
 * @file Quadrangle.cpp
 * @brief 四角形を扱うクラス
 */
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/Constants.h>

#include <nnfw/gfx/primitive/Quadrangle.h>


NS_FW_GFX_BEGIN

//シェーダ定数バッファ
struct ConstBuffer
{
    Matrix44 mtxProj;
    Matrix44 mtxView;
    Matrix44 mtxWorld;
    Vector4 Diffuse;
};

Quadrangle::Quadrangle()
    : Primitive()
{

}

Quadrangle::~Quadrangle()
{

}

bool Quadrangle::Create(ID3D11Device* device, ID3D11DeviceContext* context)
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
        {  0.5f,  0.5f, 0.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },
    };
    return Primitive::Create(device, context, reinterpret_cast<float*>(vertices), sizeof(VertexData), 4, L"image.png");
}

NS_FW_GFX_END
