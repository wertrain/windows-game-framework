#include "../Common/Includes.h"
#include "../System/Includes.h"
#include <d3d11.h>

#include "Triangle.h"

namespace Framework {
namespace Graphics {

Triangle::Triangle()
{

}

Triangle::~Triangle()
{

}

bool Triangle::Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // ポリゴン頂点構造体
    struct Vertex3D {
        float pos[3];	//x-y-z
        float col[4];	//r-g-b-a
    };

    const int TYOUTEN = 3;	//ポリゴンの頂点数

    // 頂点データ(三角ポリゴン1枚)
    Vertex3D hVectorData[TYOUTEN] = {
        { { +0.0f, +0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
        { { +0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
        { { -0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } }
    };

    // 頂点レイアウト
    // ５番目のパラメータは先頭からのバイト数なので，COLORにはPOSITIONのfloat型4バイト×3を記述
    D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // 頂点バッファ作成
    D3D11_BUFFER_DESC hBufferDesc;
    hBufferDesc.ByteWidth = sizeof(Vertex3D) * TYOUTEN;
    hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    hBufferDesc.CPUAccessFlags = 0;
    hBufferDesc.MiscFlags = 0;
    hBufferDesc.StructureByteStride = sizeof(float);

    D3D11_SUBRESOURCE_DATA hSubResourceData;
    hSubResourceData.pSysMem = hVectorData;
    hSubResourceData.SysMemPitch = 0;
    hSubResourceData.SysMemSlicePitch = 0;

    ID3D11Buffer* hpBuffer;
    if (FAILED(device->CreateBuffer(&hBufferDesc, &hSubResourceData, &hpBuffer))) {
        return false;
    }
     
    // その頂点バッファをコンテキストに設定
    UINT hStrides = sizeof(Vertex3D);
    UINT hOffsets = 0;
    context->IASetVertexBuffers(0, 1, &hpBuffer, &hStrides, &hOffsets);

    // プリミティブ(ポリゴンの形状)をコンテキストに設定
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    size_t vs_buffer_size;
    void* vs_buffer = Framework::System::File::Read("vs_triangle.cso", vs_buffer_size);

    size_t ps_buffer_size;
    void* ps_buffer = Framework::System::File::Read("ps_triangle.cso", ps_buffer_size);

    // 頂点レイアウト作成
    ID3D11InputLayout* hpInputLayout = NULL;
    if (FAILED(device->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), vs_buffer, vs_buffer_size, &hpInputLayout))) {
        return false;
    }

    // 頂点レイアウトをコンテキストに設定
    context->IASetInputLayout(hpInputLayout);

    // 頂点シェーダー生成
    ID3D11VertexShader* hpVertexShader;
    if (FAILED(device->CreateVertexShader(vs_buffer, vs_buffer_size, NULL, &hpVertexShader))) {
        return false;
    }

    // 頂点シェーダーをコンテキストに設定
    context->VSSetShader(hpVertexShader, NULL, 0);

    // ピクセルシェーダー生成
    ID3D11PixelShader* hpPixelShader;
    if (FAILED(device->CreatePixelShader(ps_buffer, ps_buffer_size, NULL, &hpPixelShader))) {
        return false;
    }

    // ピクセルシェーダーをコンテキストに設定
    context->PSSetShader(hpPixelShader, NULL, 0);

    return true;
}

} // namespace Graphics
} // namespace Framework
