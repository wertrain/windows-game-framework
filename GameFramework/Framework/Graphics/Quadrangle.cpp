#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../Common/Includes.h"
#include "../System/Includes.h"
#include "../Constants.h"

#include "Quadrangle.h"

namespace Framework {
namespace Graphics {

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
    // 頂点バッファ作成
    Primitive::VertexData vertices[] = {
        {  0.5f,  0.5f, 0.0f },
        { -0.5f,  0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
    };
    return Primitive::Create(device, context, vertices, 4, nullptr);
}

} // namespace Graphics
} // namespace Framework
