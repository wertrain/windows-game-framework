/**
 * @file Primitive.cpp
 * @brief 単純なポリゴンを表す基底クラス
 */
#include "Precompiled.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/Constants.h>
#include <nnfw/external/DirectXTex/WICTextureLoader/WICTextureLoader.h>

#include <nnfw/gfx/primitive/Primitive.h>

NS_FW_GFX_BEGIN

// シェーダ定数バッファ
struct ConstBuffer
{
    Matrix44 mtxProj;
    Matrix44 mtxView;
    Matrix44 mtxWorld;
    Vector4 Diffuse;
};

Primitive::Primitive()
    : mVertexLayout(nullptr)
    , mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mCBuffer(nullptr)
    , mRsState(nullptr)
    , mDsState(nullptr)
    , mBdState(nullptr)
    , mVertexShader(nullptr)
    , mPixelShader(nullptr)
    , mVertexNum(0)
    , mVertexDataSize(0)
    , mTexture(nullptr)
    , mShaderResView(nullptr)
    , mSampler(nullptr)
{

}

Primitive::~Primitive()
{

}

bool Primitive::Create(
    ID3D11Device* device, ID3D11DeviceContext* context,
    const float* vertices, const uint32_t vertex_data_size, const uint32_t vertex_num,
    const wchar_t* texture_filename
)
{
    NNFW_UNUSED(context);

    const bool use_texture = texture_filename != nullptr;

    mVertexDataSize = vertex_data_size;
    mVertexNum = vertex_num;

    NS_FW_SYS::Binary vsFile;
    NS_FW_SYS::Binary psFile;

    if (use_texture)
    {
        // コンパイル済みバーテックスシェーダーファイルの読み込み
        if (!vsFile.Read(L"vs_textured_primitive.cso"))
        {
            return false;
        }
        // コンパイル済みピクセルシェーダーファイルの読み込み
        if (!psFile.Read(L"ps_textured_primitive.cso"))
        {
            return false;
        }
    }
    else
    {
        // コンパイル済みバーテックスシェーダーファイルの読み込み
        if (!vsFile.Read(L"vs_primitive.cso"))
        {
            return false;
        }
        // コンパイル済みピクセルシェーダーファイルの読み込み
        if (!psFile.Read(L"ps_primitive.cso"))
        {
            return false;
        }
    }

    // 頂点シェーダー生成
    if (FAILED(device->CreateVertexShader(vsFile.Get(), vsFile.Size(), NULL, &mVertexShader)))
    {
        return false;
    }

    // ピクセルシェーダー生成
    if (FAILED(device->CreatePixelShader(psFile.Get(), psFile.Size(), NULL, &mPixelShader)))
    {
        return false;
    }

    // 入力レイアウト定義
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT elem_num = use_texture ? ARRAYSIZE(layout) : 1;

    // 入力レイアウト作成
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return false;
    }

    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = mVertexDataSize * mVertexNum;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = vertices;
        hr = device->CreateBuffer(&bd, &InitData, &mVertexBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }

    //インデックスバッファ
    UINT* indices = new UINT[mVertexNum];
    for (uint32_t i = 0; i < mVertexNum; ++i)
        indices[i] = i;
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(int) * mVertexNum;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = indices;
        hr = device->CreateBuffer(&bd, &InitData, &mIndexBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }
    delete[] indices;

    if (use_texture)
    {
        // テクスチャ作成
        hr = DirectX::CreateWICTextureFromFile(device, texture_filename, &mTexture, &mShaderResView);
        if (FAILED(hr)) {
            return hr;
        }

        // サンプラー作成
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&sampDesc, &mSampler);
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
        hr = device->CreateBuffer(&bd, nullptr, &mCBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // ラスタライザステート
    CD3D11_DEFAULT default_state;
    CD3D11_RASTERIZER_DESC rsdesc(default_state);
    rsdesc.CullMode = D3D11_CULL_NONE;
    hr = device->CreateRasterizerState(&rsdesc, &mRsState);
    if (FAILED(hr))
    {
        return hr;
    }

    // デプスステンシルステート
    CD3D11_DEPTH_STENCIL_DESC dsdesc(default_state);
    hr = device->CreateDepthStencilState(&dsdesc, &mDsState);
    if (FAILED(hr)) {
        return hr;
    }

    // ブレンドステート
    CD3D11_BLEND_DESC bddesc(default_state);
    hr = device->CreateBlendState(&bddesc, &mBdState);
    if (FAILED(hr)) {
        return hr;
    }

    return true;
}

void Primitive::Destroy()
{
    SafeRelease(mSampler);
    SafeRelease(mShaderResView);
    SafeRelease(mTexture);
    SafeRelease(mPixelShader);
    SafeRelease(mVertexShader);

    SafeRelease(mBdState);
    SafeRelease(mDsState);
    SafeRelease(mRsState);

    SafeRelease(mCBuffer);
    SafeRelease(mIndexBuffer);
    SafeRelease(mVertexBuffer);
    SafeRelease(mVertexLayout);
}

void Primitive::Render(ID3D11DeviceContext* context)
{
    // 頂点バッファ
    uint32_t vb_slot = 0;
    ID3D11Buffer* vb[1] = { mVertexBuffer };
    uint32_t stride[1] = { mVertexDataSize };
    uint32_t offset[1] = { 0 };
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


    // サンプラー
    if (mTexture)
    {
        uint32_t smp_slot = 0;
        ID3D11SamplerState* smp[1] = { mSampler };
        context->PSSetSamplers(smp_slot, 1, smp);

        // シェーダーリソースビュー（テクスチャ）
        uint32_t srv_slot = 0;
        ID3D11ShaderResourceView* srv[1] = { mShaderResView };
        context->PSSetShaderResources(srv_slot, 1, srv);
    }

    //定数バッファ
    ConstBuffer cbuff;

    // プロジェクション行列
    float aspect = NS_FW_CONST::WIDTH / NS_FW_CONST::HEIGHT;//アスペクト比
    float min_z = 0.01f;
    float max_z = 1000.0f;
    float fov = DirectX::XM_PIDIV4;//画角
    cbuff.mtxProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, min_z, max_z));

    // カメラ行列
    DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
    DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cbuff.mtxView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));
    float RotateY = 0.0f;
    cbuff.mtxWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(RotateY));
    cbuff.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1);
    // シェーダーでは行列を転置してから渡す

    // 定数バッファ内容更新
    context->UpdateSubresource(mCBuffer, 0, NULL, &cbuff, 0, 0);

    // 定数バッファ
    uint32_t cb_slot = 0;
    ID3D11Buffer* cb[1] = { mCBuffer };
    context->VSSetConstantBuffers(cb_slot, 1, cb);
    context->PSSetConstantBuffers(cb_slot, 1, cb);

    // ラスタライザステート
    context->RSSetState(mRsState);

    // デプスステンシルステート
    context->OMSetDepthStencilState(mDsState, 0);

    // ブレンドステート
    context->OMSetBlendState(mBdState, NULL, 0xfffffff);

    // ポリゴン描画
    context->DrawIndexed(mVertexNum, 0, 0);
}

NS_FW_GFX_END
