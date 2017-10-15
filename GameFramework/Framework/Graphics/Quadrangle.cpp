#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../Common/Includes.h"
#include "../System/Includes.h"
#include "../Constants.h"

#include "Quadrangle.h"

namespace Framework {
namespace Graphics {

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
        float x;
        float y;
        float z;
        float tx;
        float ty;
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

} // namespace Graphics
} // namespace Framework
