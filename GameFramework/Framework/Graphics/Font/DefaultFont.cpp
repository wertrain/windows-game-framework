#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../../Common/Includes.h"
#include "../../System/Includes.h"
#include "../../Constants.h"
#include "../../External/DirectXTex/WICTextureLoader/WICTextureLoader.h"

#include "DefaultFont.h"

namespace Framework {
namespace Graphics {

// スクリーン中の一行・一列あたり文字数定義
static const s32 FONT_X_NUM = 36;
static const s32 FONT_Y_NUM = 24;
static const s32 FONT_WIDTH = Constants::WIDTH / FONT_X_NUM;
static const s32 FONT_HEIGHT = Constants::HEIGHT / FONT_Y_NUM;
static const s32 INSTANCE_NUM = FONT_X_NUM * FONT_Y_NUM;

struct InstancingPos 
{
    Vector4 pos;
};
static_assert(sizeof(InstancingPos) == 16, "sizeof InstancingPos == 16");

DefaultFont::DefaultFont()
    : mVertexLayout(nullptr)
    , mVertexBuffer(nullptr)
    , mInstancingVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
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

DefaultFont::~DefaultFont()
{

}

bool DefaultFont::Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // 頂点データ構造体
    struct VertexData
    {
        Vector4 pos;
        Vector4 color;
        Vector4 uv;
    };
    static_assert(sizeof(VertexData) == (4 * 4 * 3), "sizeof VertexData == (4 * 4 * 3)");

    const f32 pw = 2.0f / FONT_X_NUM, ph = 2.0f / FONT_Y_NUM;
    VertexData vertices[] =
    {
        { Vector4( pw, -ph, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 0.0f) },
        { Vector4(-pw, -ph, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f) },
        { Vector4( pw,  ph, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 0.0f, 0.0f) },
        { Vector4(-pw,  ph, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f) }
    };

    mVertexDataSize = sizeof(VertexData);
    mVertexNum = 4;

    Framework::System::File::Binary vsFile;
    Framework::System::File::Binary psFile;

    // コンパイル済みバーテックスシェーダーファイルの読み込み
    if (!vsFile.Read(L"vs_2d_instancing.cso"))
    {
        return false;
    }
    // コンパイル済みピクセルシェーダーファイルの読み込み
    if (!psFile.Read(L"ps_2d_instancing.cso"))
    {
        return false;
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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // 入力アセンブラにジオメトリ処理用の行列を追加設定する
        { "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    const UINT elem_num = ARRAYSIZE(layout);

    // 入力レイアウト作成
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return false;
    }

    // バーテックスバッファ
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

    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(InstancingPos) * INSTANCE_NUM;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = device->CreateBuffer(&bd, nullptr, &mInstancingVertexBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // インデックスバッファ
    UINT indices[] = { 1, 0, 3, 2 };
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(UINT) * mVertexNum;
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

    {
        // テクスチャ作成
        hr = DirectX::CreateWICTextureFromFile(device, TEXT("usa.png"), &mTexture, &mShaderResView);
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

void DefaultFont::Destroy()
{
    if (mSampler)
    {
        mSampler->Release();
        mSampler = nullptr;
    }

    if (mShaderResView)
    {
        mShaderResView->Release();
        mShaderResView = nullptr;
    }

    if (mTexture)
    {
        mTexture->Release();
        mTexture = nullptr;
    }

    if (mPixelShader)
    {
        mPixelShader->Release();
        mPixelShader = nullptr;
    }
    if (mVertexShader)
    {
        mVertexShader->Release();
        mVertexShader = nullptr;
    }
    if (mBdState)
    {
        mBdState->Release();
        mBdState = nullptr;
    }
    if (mDsState)
    {
        mDsState->Release();
        mDsState = nullptr;
    }
    if (mRsState)
    {
        mRsState->Release();
        mRsState = nullptr;
    }
    if (mIndexBuffer)
    {
        mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }
    if (mVertexBuffer)
    {
        mVertexBuffer->Release();
        mVertexBuffer = nullptr;
    }
    if (mVertexLayout)
    {
        mVertexLayout->Release();
        mVertexLayout = nullptr;
    }
}

void DefaultFont::Render(ID3D11DeviceContext* context)
{
    // 頂点バッファ
    u32 vb_slot = 0;
    ID3D11Buffer* vb[2] = { mVertexBuffer, mInstancingVertexBuffer };
    u32 stride[2] = { mVertexDataSize, sizeof(InstancingPos) };
    u32 offset[2] = { 0, 0 };
    context->IASetVertexBuffers(vb_slot, ARRAYSIZE(vb), vb, stride, offset);

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
        u32 smp_slot = 0;
        ID3D11SamplerState* smp[1] = { mSampler };
        context->PSSetSamplers(smp_slot, 1, smp);

        // シェーダーリソースビュー（テクスチャ）
        u32 srv_slot = 0;
        ID3D11ShaderResourceView* srv[1] = { mShaderResView };
        context->PSSetShaderResources(srv_slot, 1, srv);
    }

    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        HRESULT hr = context->Map(mInstancingVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return;
        InstancingPos* instancing = (InstancingPos*)(mappedResource.pData);
        
        const f32 pw = 2.0f / FONT_X_NUM, ph = 2.0f / FONT_Y_NUM;
        for (s32 y = 0; y < FONT_Y_NUM; ++y)
        {
            for (s32 x = 0; x < FONT_X_NUM; ++x)
            {
                s32 index = y * FONT_X_NUM + x;
                instancing[index].pos.x = -1.0f + static_cast<f32>(x * pw);
                instancing[index].pos.y = -1.0f + static_cast<f32>(y * ph);
                instancing[index].pos.z = instancing[index].pos.w = 0.0f;
            }
        }
        context->Unmap(mInstancingVertexBuffer, 0);
    }

    // ラスタライザステート
    context->RSSetState(mRsState);

    // デプスステンシルステート
    context->OMSetDepthStencilState(mDsState, 0);

    // ブレンドステート
    context->OMSetBlendState(mBdState, NULL, 0xfffffff);

    // ポリゴン描画
    context->DrawIndexedInstanced(mVertexNum, INSTANCE_NUM, 0, 0, 0);
}

} // namespace Graphics
} // namespace Framework
