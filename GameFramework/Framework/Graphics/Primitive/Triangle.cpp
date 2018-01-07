#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../../Common/Includes.h"
#include "../../System/Includes.h"
#include "../../Constants.h"

#include "Triangle.h"

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
