#include <d3d11.h>
#include <atlbase.h>
#include <DirectXMath.h>
#include <memory>

#include "../Common/Includes.h"
#include "../System/Includes.h"
#include "Texture.h"
#include "Render2D.h"

namespace Framework {
namespace Graphics {

// �V�F�[�_�萔�o�b�t�@
struct ConstBuffer
{
    Matrix44 mtxWorld;
};

const u32 sVertexNum = 4;

// ���_�f�[�^�\����
struct VertexData
{
    Vector3 pos;
    Vector3 color;
    Vector2 uv;
}
sVertices[sVertexNum] = 
{
    { Vector3( 0.5f,  0.5f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f) },
    { Vector3(-0.5f,  0.5f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f) },
    { Vector3( 0.5f, -0.5f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f) },
    { Vector3(-0.5f, -0.5f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f) }
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
    // �R���p�C���ς݃o�[�e�b�N�X�V�F�[�_�[�t�@�C���̓ǂݍ���
    Framework::System::File::Binary vsFile;
    if (!vsFile.Read(L"vs_2d.cso"))
    {
        return E_FAIL;
    }
    // �R���p�C���ς݃s�N�Z���V�F�[�_�[�t�@�C���̓ǂݍ���
    Framework::System::File::Binary psFile;
    if (!psFile.Read(L"ps_2d.cso"))
    {
        return E_FAIL;
    }

    // ���_�V�F�[�_�[����
    {
        HRESULT hr = device->CreateVertexShader(vsFile.Get(), vsFile.Size(), NULL, &mVertexShader);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // �s�N�Z���V�F�[�_�[����
    {
        HRESULT hr = device->CreatePixelShader(psFile.Get(), psFile.Size(), NULL, &mPixelShader);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // ���̓��C�A�E�g��`
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    u32 elem_num = ARRAYSIZE(layout);
    // ���̓��C�A�E�g�쐬
    // Create InputLayout VS�݂̂ɕK�v�@�ǂ̂悤�ȗv�f������
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return hr;
    }
    
    return S_OK;
}

HRESULT Render2D::CreateBuffer(ID3D11Device* device)
{
    // ���_�o�b�t�@
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

    //�C���f�b�N�X�o�b�t�@
    {
        std::unique_ptr<UINT> indices(new UINT[sVertexNum]);
        for (uint32_t i = 0; i < sVertexNum; ++i)
            indices.get()[i] = i;

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(int) * sVertexNum;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = indices.get();
        HRESULT hr = device->CreateBuffer(&bd, &InitData, &mIndexBuffer);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // �萔�o�b�t�@
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

void Render2D::Render(ID3D11DeviceContext* context, const f32 x1, const f32 y1, const f32 x2, const f32 y2)
{
    Render(context, x1, y1, x2, y2, nullptr);
}

void Render2D::Render(ID3D11DeviceContext* context, const f32 x1, const f32 y1, const f32 x2, const f32 y2, Texture* texture)
{
    sVertices[0].pos = Vector3(x1, y1, 0.0f);
    sVertices[1].pos = Vector3(x2, y1, 0.0f);
    sVertices[2].pos = Vector3(x2, y2, 0.0f);
    sVertices[3].pos = Vector3(x1, y2, 0.0f);

    // ���_�o�b�t�@���e�X�V
    context->UpdateSubresource(mVertexBuffer, 0, NULL, &sVertices, 0, 0);

    // ���_�o�b�t�@
    u32 vb_slot = 0;
    ID3D11Buffer* vb[1] = { mVertexBuffer };
    u32 stride[1] = { sizeof(VertexData) };
    u32 offset[1] = { 0 };
    context->IASetVertexBuffers(vb_slot, 1, vb, stride, offset);

    // ���̓��C�A�E�g
    context->IASetInputLayout(mVertexLayout);

    // �C���f�b�N�X�o�b�t�@
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // �v���~�e�B�u�`��
    context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // �V�F�[�_
    context->VSSetShader(mVertexShader, nullptr, 0);
    context->PSSetShader(mPixelShader, nullptr, 0);

    // �萔�o�b�t�@
    u32 cb_slot = 0;
    ID3D11Buffer* cb[1] = { mConstantBuffer };
    context->VSSetConstantBuffers(cb_slot, 1, cb);
    context->PSSetConstantBuffers(cb_slot, 1, cb);

    u32 smp_slot = 0;
    ID3D11SamplerState* smp[1] = { const_cast<ID3D11SamplerState*>(texture->GetSamplerState()) };
    context->PSSetSamplers(smp_slot, 1, smp);

    // �V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j
    u32 srv_slot = 0;
    ID3D11ShaderResourceView* srv[1] = { const_cast<ID3D11ShaderResourceView*>(texture->GetShaderResourceView()) };
    context->PSSetShaderResources(srv_slot, 1, srv);

    // �|���S���`��
    context->DrawIndexed(sVertexNum, 0, 0);
}

} // namespace Graphics
} // namespace Framework
