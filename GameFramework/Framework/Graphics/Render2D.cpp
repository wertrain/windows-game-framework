#include <d3d11.h>
#include <atlbase.h>
#include <DirectXMath.h>
#include <memory>

#include "../Common/Includes.h"
#include "../System/Includes.h"
#include "../Constants.h"
#include "Texture.h"
#include "Render2D.h"

namespace Framework {
namespace Graphics {

// シェーダ定数バッファ
struct ConstBuffer
{
    Matrix44 mtxWorld;
};

const u32 sVertexNum = 4;

// 頂点データ構造体
struct VertexData
{
    Vector4 pos;
    Vector4 color;
    Vector4 uv;
}

sVertices[sVertexNum] = 
{
    { Vector4( 0.5f, -0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 0.0f) },
    { Vector4(-0.5f, -0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f) },
    { Vector4( 0.5f,  0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 0.0f, 0.0f) },
    { Vector4(-0.5f,  0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f) }
};

Render2D::Render2D()
    : mVertexShader(nullptr)
    , mPixelShader(nullptr)
    , mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mConstantBuffer(nullptr)
    , mVertexLayout(nullptr)
{

}

Render2D::~Render2D()
{

}

bool Render2D::Create(ID3D11Device* device)
{
    if (FAILED(CreateShader(device))) return false;
    if (FAILED(CreateBuffer(device))) return false;
    
    return true;
}

void Render2D::Destroy()
{
    if (mIndexBuffer != nullptr)
    {
        mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }
    if (mPixelShader != nullptr)
    {
        mPixelShader->Release();
        mPixelShader = nullptr;
    }
    if (mVertexShader != nullptr)
    {
        mVertexShader->Release();
        mVertexShader = nullptr;
    }
    if (mConstantBuffer != nullptr)
    {
        mConstantBuffer->Release();
        mConstantBuffer = nullptr;
    }
    if (mVertexLayout != nullptr)
    {
        mVertexLayout->Release();
        mVertexLayout = nullptr;
    }
}

HRESULT Render2D::CreateShader(ID3D11Device* device)
{
    // コンパイル済みバーテックスシェーダーファイルの読み込み
    Framework::System::File::Binary vsFile;
    if (!vsFile.Read(L"vs_2d.cso"))
    {
        return E_FAIL;
    }
    // コンパイル済みピクセルシェーダーファイルの読み込み
    Framework::System::File::Binary psFile;
    if (!psFile.Read(L"ps_2d.cso"))
    {
        return E_FAIL;
    }

    // 頂点シェーダー生成
    {
        HRESULT hr = device->CreateVertexShader(vsFile.Get(), vsFile.Size(), NULL, &mVertexShader);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // ピクセルシェーダー生成
    {
        HRESULT hr = device->CreatePixelShader(psFile.Get(), psFile.Size(), NULL, &mPixelShader);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // 入力レイアウト定義
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    u32 elem_num = ARRAYSIZE(layout);
    // 入力レイアウト作成
    // Create InputLayout VSのみに必要　どのような要素を持つか
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return hr;
    }
    
    return S_OK;
}

HRESULT Render2D::CreateBuffer(ID3D11Device* device)
{
    // 頂点バッファ
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(VertexData) * sVertexNum;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = sVertices;
        HRESULT hr = device->CreateBuffer(&bd, &InitData, &mVertexBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }

    //インデックスバッファ
    {
        UINT indices[sVertexNum] = { 1,0,3,2 };

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(UINT) * sVertexNum;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = indices;
        HRESULT hr = device->CreateBuffer(&bd, &InitData, &mIndexBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // 定数バッファ
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(ConstBuffer);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        HRESULT hr = device->CreateBuffer(&bd, nullptr, &mConstantBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }
    return S_OK;
}

void Render2D::Render(ID3D11DeviceContext* context, const f32 x, const f32 y, const f32 width, const f32 height)
{
    Render(context, x, y, width, height, nullptr);
}

void Render2D::Render(ID3D11DeviceContext* context, const f32 x, const f32 y, const f32 width, const f32 height, Texture* texture)
{
    const f32 wh = Constants::WIDTH * 0.5f;
    const f32 hh = Constants::HEIGHT * 0.5f;
    const f32 x1 = -1.0f + (x / wh);
    const f32 x2 = x1 + (width / wh);
    const f32 y1 =  1.0f - (y / hh);
    const f32 y2 = y1 - (height / hh);

    sVertices[0].pos = Vector4(x2, y1, 0.0f, 1.0f);
    sVertices[1].pos = Vector4(x1, y1, 0.0f, 1.0f);
    sVertices[2].pos = Vector4(x2, y2, 0.0f, 1.0f);
    sVertices[3].pos = Vector4(x1, y2, 0.0f, 1.0f);



    // 頂点バッファ内容更新
    context->UpdateSubresource(mVertexBuffer, 0, NULL, &sVertices, 0, 0);

    // 頂点バッファ
    u32 vb_slot = 0;
    ID3D11Buffer* vb[1] = { mVertexBuffer };
    u32 stride[1] = { sizeof(VertexData) };
    u32 offset[1] = { 0 };
    context->IASetVertexBuffers(vb_slot, 1, vb, stride, offset);

    // 入力レイアウト
    context->IASetInputLayout(mVertexLayout);

    // インデックスバッファ
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // プリミティブ形状
    context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // シェーダ
    context->VSSetShader(mVertexShader, nullptr, 0);
    context->PSSetShader(mPixelShader, nullptr, 0);

    //定数バッファ
    ConstBuffer cbuff;
    cbuff.mtxWorld = DirectX::XMMatrixIdentity();
    // シェーダーでは行列を転置してから渡す
    // 定数バッファ内容更新
    context->UpdateSubresource(mConstantBuffer, 0, NULL, &cbuff, 0, 0);

    // 定数バッファ
    u32 cb_slot = 0;
    ID3D11Buffer* cb[1] = { mConstantBuffer };
    context->VSSetConstantBuffers(cb_slot, 1, cb);
    context->PSSetConstantBuffers(cb_slot, 1, cb);

    u32 smp_slot = 0;
    ID3D11SamplerState* smp[1] = { const_cast<ID3D11SamplerState*>(texture->GetSamplerState()) };
    context->PSSetSamplers(smp_slot, 1, smp);

    // シェーダーリソースビュー（テクスチャ）
    u32 srv_slot = 0;
    ID3D11ShaderResourceView* srv[1] = { const_cast<ID3D11ShaderResourceView*>(texture->GetShaderResourceView()) };
    context->PSSetShaderResources(srv_slot, 1, srv);

    // ポリゴン描画
    context->DrawIndexed(sVertexNum, 0, 0);
}

} // namespace Graphics
} // namespace Framework
