#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../Common/Includes.h"
#include "../System/Includes.h"
#include "../Constants.h"

#include "Primitive.h"

namespace Framework {
namespace Graphics {

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
{

}

Primitive::~Primitive()
{

}

bool Primitive::Create(
    ID3D11Device* device, ID3D11DeviceContext* context,
    VertexData* vertices, const unsigned int vertex_num,
    const wchar_t* texture_filename
)
{
    mVertexNum = vertex_num;

    Framework::System::File::Binary vsFile;
    Framework::System::File::Binary psFile;

    if (texture_filename == nullptr)
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
    else
    {

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
    UINT elem_num = (texture_filename == nullptr) ? 1 : ARRAYSIZE(layout);

    // 入力レイアウト作成
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return false;
    }

    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(VertexData) * mVertexNum;
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
    if (mCBuffer)
    {
        mCBuffer->Release();
        mCBuffer = nullptr;
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

void Primitive::Render(ID3D11DeviceContext* context)
{
    // 頂点バッファ
    UINT vb_slot = 0;
    ID3D11Buffer* vb[1] = { mVertexBuffer };
    UINT stride[1] = { sizeof(VertexData) };
    UINT offset[1] = { 0 };
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
    //UINT smp_slot = 0;
    //ID3D11SamplerState* smp[1] = { pSampler.Get() };
    //pImContext->PSSetSamplers(smp_slot, 1, smp);

    // シェーダーリソースビュー（テクスチャ）
    //UINT srv_slot = 0;
    //ID3D11ShaderResourceView* srv[1] = { pShaderResView.Get() };
    //pImContext->PSSetShaderResources(srv_slot, 1, srv);

    //定数バッファ
    ConstBuffer cbuff;

    // プロジェクション行列
    FLOAT aspect = Framework::Constants::WIDTH / Framework::Constants::HEIGHT;//アスペクト比
    FLOAT min_z = 0.01f;
    FLOAT max_z = 1000.0f;
    FLOAT fov = DirectX::XM_PIDIV4;//画角
    cbuff.mtxProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, min_z, max_z));

    // カメラ行列
    DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
    DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cbuff.mtxView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));
    FLOAT RotateY = 0.0f;
    cbuff.mtxWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(RotateY));
    cbuff.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1);
    // シェーダーでは行列を転置してから渡す

    // 定数バッファ内容更新
    context->UpdateSubresource(mCBuffer, 0, NULL, &cbuff, 0, 0);

    // 定数バッファ
    UINT cb_slot = 0;
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

} // namespace Graphics
} // namespace Framework
