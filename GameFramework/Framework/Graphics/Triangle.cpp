#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../Common/Includes.h"
#include "../System/Includes.h"
#include "../Constants.h"

#include "Triangle.h"

namespace Framework {
namespace Graphics {

//���_�f�[�^�\����
struct VertexData
{
    FLOAT x, y, z;
};

//�V�F�[�_�萔�o�b�t�@
struct ConstBuffer
{
    Matrix44 mtxProj;
    Matrix44 mtxView;
    Matrix44 mtxWorld;
    Vector4 Diffuse;
};

Triangle::Triangle()
    : mVertexLayout(nullptr)
    , mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mCBuffer(nullptr)
    , mRsState(nullptr)
    , mDsState(nullptr)
    , mBdState(nullptr)
    , mVertexShader(nullptr)
    , mPixelShader(nullptr)
{

}

Triangle::~Triangle()
{

}

bool Triangle::Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // �R���p�C���ς݃o�[�e�b�N�X�V�F�[�_�[�t�@�C���̓ǂݍ���
    Framework::System::File::Binary vsFile;
    if (!vsFile.Read(L"vs_primitive.cso"))
    {
        return false;
    }
    // �R���p�C���ς݃s�N�Z���V�F�[�_�[�t�@�C���̓ǂݍ���
    Framework::System::File::Binary psFile;
    if (!psFile.Read(L"ps_primitive.cso"))
    {
        return false;
    }

    // ���_�V�F�[�_�[����

    if (FAILED(device->CreateVertexShader(vsFile.Get(), vsFile.Size(), NULL, &mVertexShader)))
    {
        return false;
    }

    // �s�N�Z���V�F�[�_�[����
    if (FAILED(device->CreatePixelShader(psFile.Get(), psFile.Size(), NULL, &mPixelShader)))
    {
        return false;
    }

    // ���̓��C�A�E�g��`
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT elem_num = ARRAYSIZE(layout);

    // ���̓��C�A�E�g�쐬
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return false;
    }

    // ���_�o�b�t�@�쐬
    VertexData vertices[] = {
        { 0.0f,  0.5f, 0.0f  },
        { 0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
    };
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(VertexData) * 3;
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

    //�C���f�b�N�X�o�b�t�@
    UINT indices[] = { 0,1,2 };
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(int) * 3;
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

    // �萔�o�b�t�@
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

    //���X�^���C�U�X�e�[�g
    CD3D11_DEFAULT default_state;
    CD3D11_RASTERIZER_DESC rsdesc(default_state);
    rsdesc.CullMode = D3D11_CULL_NONE;
    hr = device->CreateRasterizerState(&rsdesc, &mRsState);
    if (FAILED(hr))
    {
        return hr;
    }

    //�f�v�X�X�e���V���X�e�[�g
    CD3D11_DEPTH_STENCIL_DESC dsdesc(default_state);
    hr = device->CreateDepthStencilState(&dsdesc, &mDsState);
    if (FAILED(hr)) {
        return hr;
    }


    //�u�����h�X�e�[�g
    CD3D11_BLEND_DESC bddesc(default_state);
    hr = device->CreateBlendState(&bddesc, &mBdState);
    if (FAILED(hr)) {
        return hr;
    }

    return true;
}

void Triangle::Destroy()
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

void Triangle::Render(ID3D11DeviceContext* context)
{
    // ���_�o�b�t�@
    UINT vb_slot = 0;
    ID3D11Buffer* vb[1] = { mVertexBuffer };
    UINT stride[1] = { sizeof(VertexData) };
    UINT offset[1] = { 0 };
    context->IASetVertexBuffers(vb_slot, 1, vb, stride, offset);

    // ���̓��C�A�E�g
    context->IASetInputLayout(mVertexLayout);

    // �C���f�b�N�X�o�b�t�@
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // �v���~�e�B�u�`��i�O�p�`���X�g�j
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // �V�F�[�_
    context->VSSetShader(mVertexShader, nullptr, 0);
    context->PSSetShader(mPixelShader, nullptr, 0);

    // �T���v���[
    //UINT smp_slot = 0;
    //ID3D11SamplerState* smp[1] = { pSampler.Get() };
    //pImContext->PSSetSamplers(smp_slot, 1, smp);

    // �V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j
    //UINT srv_slot = 0;
    //ID3D11ShaderResourceView* srv[1] = { pShaderResView.Get() };
    //pImContext->PSSetShaderResources(srv_slot, 1, srv);

    //�萔�o�b�t�@
    ConstBuffer cbuff;

    // �v���W�F�N�V�����s��
    FLOAT aspect = Framework::Constants::WIDTH / Framework::Constants::HEIGHT;//�A�X�y�N�g��
    FLOAT min_z = 0.01f;
    FLOAT max_z = 1000.0f;
    FLOAT fov = DirectX::XM_PIDIV4;//��p
    cbuff.mtxProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, min_z, max_z));

    // �J�����s��
    DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 1.0f, -1.5f, 0.0f);
    DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cbuff.mtxView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));
    FLOAT RotateY = 0.0f;
    cbuff.mtxWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(RotateY));
    cbuff.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1);
    // �V�F�[�_�[�ł͍s���]�u���Ă���n��

    // �萔�o�b�t�@���e�X�V
    context->UpdateSubresource(mCBuffer, 0, NULL, &cbuff, 0, 0);

    // �萔�o�b�t�@
    UINT cb_slot = 0;
    ID3D11Buffer* cb[1] = { mCBuffer };
    context->VSSetConstantBuffers(cb_slot, 1, cb);
    context->PSSetConstantBuffers(cb_slot, 1, cb);

    // ���X�^���C�U�X�e�[�g
    context->RSSetState(mRsState);

    // �f�v�X�X�e���V���X�e�[�g
    context->OMSetDepthStencilState(mDsState, 0);

    // �u�����h�X�e�[�g
    context->OMSetBlendState(mBdState, NULL, 0xfffffff);

    // �|���S���`��
    context->DrawIndexed(3, 0, 0);
}

} // namespace Graphics
} // namespace Framework
