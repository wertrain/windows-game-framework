/**
 * @file Quadrangle.cpp
 * @brief �l�p�`�������N���X
 */
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/Constants.h>

#include <nnfw/gfx/primitive/Quadrangle.h>


NS_FW_GFX_BEGIN

//�V�F�[�_�萔�o�b�t�@
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
        {  0.5f,  0.5f, 0.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },
    };
    return Primitive::Create(device, context, reinterpret_cast<float*>(vertices), sizeof(VertexData), 4, L"image.png");
}

NS_FW_GFX_END
