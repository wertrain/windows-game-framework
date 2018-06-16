/**
 * @file Particle.cpp
 * @brief パーティクルを表すクラス
 */
#include "Precompiled.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <nnfw/Constants.h>
#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/ut/Memory.h>
#include <nnfw/ut/Random.h>
#include <nnfw/external/DirectXTex/WICTextureLoader/WICTextureLoader.h>
#include <nnfw/gfx/effect/Particle.h>

NS_FW_GFX_BEGIN

// シェーダ定数バッファ
struct ConstBuffer
{
    Matrix44 mtxProj;
    Matrix44 mtxView;
    Matrix44 mtxWorld;
    Vector4 Diffuse;
};

// インスタンス一枚あたりのサイズ
static const float PLANE_SIZE = 1.0f;
static const float UV_WIDTH = 1.0f;
static const float UV_HEIGHT = 1.0f;

struct InstancingPos
{
    Vector4 pos;
};
static_assert(sizeof(InstancingPos) == 16, "sizeof InstancingPos == 16");

struct InstancingColor
{
    Vector4 color;
};
static_assert(sizeof(InstancingColor) == 16, "sizeof InstancingColor == 16");

static const uint32_t instancingBufferSize[] = 
{
    sizeof(InstancingPos),
    sizeof(InstancingColor)
};

Particles::Particles()
    : mVertexLayout(nullptr)
    , mVertexBuffer(nullptr)
    , mInstancingBuffer()
    , mIndexBuffer(nullptr)
    , mBdState(nullptr)
    , mConstBuffer(nullptr)
    , mVertexShader(nullptr)
    , mPixelShader(nullptr)
    , mVertexNum(0)
    , mVertexDataSize(0)
    , mTexture(nullptr)
    , mShaderResView(nullptr)
    , mSampler(nullptr)
    , mDepthStencilState(nullptr)
    , mInstanceNum(0)
    , mParticles()
{
    ZeroMemory(mInstancingBuffer, sizeof(mInstancingBuffer));
}

Particles::~Particles()
{

}

bool Particles::Create(ID3D11Device* device, ID3D11DeviceContext* context, const uint32_t instanceNum)
{
    NNFW_UNUSED(context);

    // 頂点データ構造体
    struct VertexData
    {
        float pos[3];
        float uv[2];
    };
    static_assert(sizeof(VertexData) == (4 * 3 + 4 * 2), "sizeof VertexData == (4 * 3 + 4 * 2)");

    VertexData vertices[] =
    {
        // UV は上下左右反転
        { { 0.0f,       0.0f,       0.0f }, { 0.0f,     UV_HEIGHT } },
        { { PLANE_SIZE, 0.0f,       0.0f }, { UV_WIDTH, UV_HEIGHT } },
        { { 0.0f,       PLANE_SIZE, 0.0f }, { 0.0f,     0.0f      } },
        { { PLANE_SIZE, PLANE_SIZE, 0.0f }, { UV_WIDTH, 0.0f      } }
    };
    // z 座標にはサイズの半分の値を入れておく
    vertices[0].pos[2] = vertices[1].pos[2] = vertices[2].pos[2] = vertices[3].pos[2] = PLANE_SIZE * 0.5f;

    mVertexDataSize = sizeof(VertexData);
    mVertexNum = 4;

#if BUILTIN_PARTICLE_SHADER
    // 頂点シェーダー生成
    if (FAILED(device->CreateVertexShader(vs_particle_simple_cso, vs_particle_simple_len, NULL, &mVertexShader)))
    {
        return false;
    }

    // ピクセルシェーダー生成
    if (FAILED(device->CreatePixelShader(ps_particle_simple_cso, ps_particle_simple_cso_len, NULL, &mPixelShader)))
    {
        return false;
    }
#else
    NS_FW_SYS::Binary vsFile;
    NS_FW_SYS::Binary psFile;

    // コンパイル済みバーテックスシェーダーファイルの読み込み
    if (!vsFile.Read(L"vs_particle_simple.cso"))
    {
        return false;
    }
    // コンパイル済みピクセルシェーダーファイルの読み込み
    if (!psFile.Read(L"ps_particle_simple.cso"))
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
#endif

    // 入力レイアウト定義
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // 入力アセンブラにジオメトリ処理用の行列を追加設定する
        { "IPOSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "ICOLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2,  0, D3D11_INPUT_PER_INSTANCE_DATA, 2 },
        //{ "IPOSITION", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        //{ "IPOSITION", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    const UINT elem_num = ARRAYSIZE(layout);

    // 入力レイアウト作成
#if BUILTIN_PARTICLE_SHADER
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vs_particle_simple_cso, vs_particle_simple_cso_len, &mVertexLayout);
#else
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
#endif
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
        if (FAILED(hr))
        {
            return false;
        }
    }

    // インスタンシング描画用バッファ
    for (int i = 0; i < InstancingBufferType::Num; ++i)
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = instancingBufferSize[i] * instanceNum;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = device->CreateBuffer(&bd, nullptr, &mInstancingBuffer[i]);
        if (FAILED(hr))
        {
            return false;
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
        if (FAILED(hr))
        {
            return false;
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
        hr = device->CreateBuffer(&bd, nullptr, &mConstBuffer);
        if (FAILED(hr)) {
            return false;
        }
    }

    {
        std::wstring wpath = L"particle4u.png";

        // テクスチャ作成
        hr = DirectX::CreateWICTextureFromFile(device, context, wpath.c_str(), &mTexture, &mShaderResView);
        if (FAILED(hr))
        {
            return false;
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
        if (FAILED(hr))
        {
            return false;
        }
    }

    // ブレンドステート
    CD3D11_DEFAULT default_state;
    CD3D11_BLEND_DESC bddesc(default_state);
    ZeroMemory(&bddesc, sizeof(D3D11_BLEND_DESC));
    bddesc.AlphaToCoverageEnable = FALSE;
    bddesc.IndependentBlendEnable = FALSE;
    bddesc.RenderTarget[0].BlendEnable = TRUE;
    bddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bddesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bddesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bddesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bddesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bddesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device->CreateBlendState(&bddesc, &mBdState);
    if (FAILED(hr))
    {
        return false;
    }

    // デプス無効
    D3D11_DEPTH_STENCIL_DESC ddsDesc;
    ::ZeroMemory(&ddsDesc, sizeof(ddsDesc));
    ddsDesc.DepthEnable = FALSE;
    ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    ddsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    ddsDesc.StencilEnable = FALSE;
    hr = device->CreateDepthStencilState(&ddsDesc, &mDepthStencilState);
    if (FAILED(hr))
    {
        return false;
    }

    mParticles.Initialize(instanceNum);

    NS_FW_UTIL::Random rand(static_cast<unsigned long>(time(0)));
    for (uint32_t i = 0; i < instanceNum; ++i)
    {
        Particle particle;
        particle.pos = Vector4(
            (-1.0f + rand.NextFloat() * 2.0f),
            (-1.0f + rand.NextFloat() * 2.0f),
            (-1.0f + rand.NextFloat() * 10.0f), 
            0.0f);
        particle.speed = rand.NextFloat() * 0.1f; // 回転速度をいれておく
        particle.lifespan = rand.NextFloat() * 30.0f;
        mParticles.Enqueue(particle);
    }

    mInstanceNum = instanceNum;

    return true;
}

void Particles::Destroy()
{
    mParticles.Finalize();

    SafeRelease(mDepthStencilState);

    SafeRelease(mSampler);
    SafeRelease(mShaderResView);
    SafeRelease(mTexture);

    SafeRelease(mPixelShader);
    SafeRelease(mVertexShader);

    SafeRelease(mBdState);

    SafeRelease(mConstBuffer);

    SafeRelease(mIndexBuffer);
    for (int i = 0; i < InstancingBufferType::Num; ++i)
    {
        SafeRelease(mInstancingBuffer[i]);
    }
    SafeRelease(mVertexBuffer);
    SafeRelease(mVertexLayout);
}


void Particles::Render(ID3D11DeviceContext* context)
{
    //定数バッファ
    ConstBuffer cbuff;

    // プロジェクション行列
    float aspect = NS_FW_CONST::WIDTH / NS_FW_CONST::HEIGHT;//アスペクト比
    float min_z = 0.01f;
    float max_z = 1000.0f;
    float fov = DirectX::XM_PIDIV4;//画角
    cbuff.mtxProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, min_z, max_z));

    // カメラ行列
    DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
    DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cbuff.mtxView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));
    float RotateY = 0.0f;
    cbuff.mtxWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(RotateY));
    cbuff.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1);
    // シェーダーでは行列を転置してから渡す

    // 頂点バッファ
    uint32_t vb_slot = 0;
    const uint32_t vb_size = 1 + InstancingBufferType::Num;
    ID3D11Buffer* vb[vb_size]; vb[0] = mVertexBuffer;
    uint32_t stride[vb_size]; stride[0] = mVertexDataSize;
    uint32_t offset[vb_size]; offset[0] = 0;
    for (int i = 1; i < vb_size; ++i)
    {
        int index = i - 1;
        vb[i] = mInstancingBuffer[index];
        stride[i] = instancingBufferSize[index];
        offset[i] = 0;
    }
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

    // 定数バッファ内容更新
    context->UpdateSubresource(mConstBuffer, 0, NULL, &cbuff, 0, 0);

    // 定数バッファ
    uint32_t cb_slot = 0;
    ID3D11Buffer* cb[1] = { mConstBuffer };
    context->VSSetConstantBuffers(cb_slot, ARRAYSIZE(cb), cb);
    context->PSSetConstantBuffers(cb_slot, ARRAYSIZE(cb), cb);

    // サンプラー
    if (mTexture)
    {
        uint32_t smp_slot = 0;
        ID3D11SamplerState* smp[] = { mSampler };
        context->PSSetSamplers(smp_slot, ARRAYSIZE(smp), smp);

        // シェーダーリソースビュー（テクスチャ）
        uint32_t srv_slot = 0;
        ID3D11ShaderResourceView* srv[] = { mShaderResView };
        context->PSSetShaderResources(srv_slot, ARRAYSIZE(srv), srv);
    }

    // パーティクル描画数
    int drawParticleNum = 0;

    // インスタンシング描画位置
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        HRESULT hr = context->Map(mInstancingBuffer[InstancingBufferType::Position], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return;
        InstancingPos* instancing = (InstancingPos*)(mappedResource.pData);

        for (unsigned int index = 0; index < mInstanceNum; ++index)
        {
            if (mParticles[index].lifespan <= 0) continue;

            mParticles[index].pos.w += mParticles[index].speed;
            mParticles[index].lifespan -= 0.1f;
            instancing[drawParticleNum].pos = mParticles[index].pos;
            ++drawParticleNum;
        }

        // パーティクルを復活
        NS_FW_UTIL::Random rand(static_cast<unsigned long>(time(0)));
        for (unsigned int index = 0; index < mInstanceNum; ++index)
        {
            if (rand.NextFloat() > 0.8f && mParticles[index].lifespan <= 0)
            {
                mParticles[index].lifespan = rand.NextFloat() * 30.0f;
            }
        }
        context->Unmap(mInstancingBuffer[InstancingBufferType::Position], 0);
    }
    // インスタンシング描画色
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        HRESULT hr = context->Map(mInstancingBuffer[InstancingBufferType::Color], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return;
        InstancingColor* instancing = (InstancingColor*)(mappedResource.pData);
        for (unsigned int index = 0; index < drawParticleNum; ++index)
        {
            instancing[index].color = Vector4(1, 0, 0, 1);
        }

        context->Unmap(mInstancingBuffer[InstancingBufferType::Color], 0);
    }

    // 以前のステート保存
    ID3D11DepthStencilState* beforeState;
    context->OMGetDepthStencilState(&beforeState, 0);

    // デプス無効化
    context->OMSetDepthStencilState(mDepthStencilState, 0);

    // ブレンドステート
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->OMSetBlendState(mBdState, blendFactor, 0xfffffff);

    // ポリゴン描画
    context->DrawIndexedInstanced(mVertexNum, drawParticleNum, 0, 0, 0);

    // ステートを戻す
    context->OMSetDepthStencilState(beforeState, 0);
}

NS_FW_GFX_END
