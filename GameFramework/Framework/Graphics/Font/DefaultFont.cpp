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

static const s32 FONT_X_NUM = 34;
static const s32 FONT_Y_NUM = 20;
static const s32 FONT_WIDTH = Constants::WIDTH / FONT_X_NUM;
static const s32 FONT_HEIGHT = Constants::HEIGHT / FONT_Y_NUM;
static const s32 INSTANCE_NUM = FONT_X_NUM * FONT_Y_NUM;

// �V�F�[�_�萔�o�b�t�@
struct ConstBuffer
{
    Matrix44 mtxProj;
    Matrix44 mtxView;
    Vector2 worldPos[INSTANCE_NUM];
    Vector4 Diffuse;
};

DefaultFont::DefaultFont()
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

DefaultFont::~DefaultFont()
{

}

bool DefaultFont::Create(
    ID3D11Device* device, ID3D11DeviceContext* context,
    const f32* vertices, const u32 vertex_data_size, const u32 vertex_num,
    const wchar_t* texture_filename
)
{
    mVertexDataSize = vertex_data_size;
    mVertexNum = vertex_num;

    Framework::System::File::Binary vsFile;
    Framework::System::File::Binary psFile;

    // �R���p�C���ς݃o�[�e�b�N�X�V�F�[�_�[�t�@�C���̓ǂݍ���
    if (!vsFile.Read(L"vs_2d_instancing.cso"))
    {
        return false;
    }
    // �R���p�C���ς݃s�N�Z���V�F�[�_�[�t�@�C���̓ǂݍ���
    if (!psFile.Read(L"ps_2d_instancing.cso"))
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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // ���̓A�Z���u���ɃW�I���g�������p�̍s���ǉ��ݒ肷��
        { "VECTOR2",  0, DXGI_FORMAT_R32G32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "VECTOR2",  1, DXGI_FORMAT_R32G32_FLOAT, 1,  8, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "VECTOR2",  2, DXGI_FORMAT_R32G32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "VECTOR2",  3, DXGI_FORMAT_R32G32_FLOAT, 1, 24, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    const UINT elem_num = ARRAYSIZE(layout);

    // ���̓��C�A�E�g�쐬
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return false;
    }

    // �o�[�e�b�N�X�o�b�t�@
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

    // �C���f�b�N�X�o�b�t�@
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

    /*if (use_texture)
    {
        // �e�N�X�`���쐬
        hr = DirectX::CreateWICTextureFromFile(device, texture_filename, &mTexture, &mShaderResView);
        if (FAILED(hr)) {
            return hr;
        }

        // �T���v���[�쐬
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
    }*/

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

    // ���X�^���C�U�X�e�[�g
    CD3D11_DEFAULT default_state;
    CD3D11_RASTERIZER_DESC rsdesc(default_state);
    rsdesc.CullMode = D3D11_CULL_NONE;
    hr = device->CreateRasterizerState(&rsdesc, &mRsState);
    if (FAILED(hr))
    {
        return hr;
    }

    // �f�v�X�X�e���V���X�e�[�g
    CD3D11_DEPTH_STENCIL_DESC dsdesc(default_state);
    hr = device->CreateDepthStencilState(&dsdesc, &mDsState);
    if (FAILED(hr)) {
        return hr;
    }

    // �u�����h�X�e�[�g
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

void DefaultFont::Render(ID3D11DeviceContext* context)
{
    // ���_�o�b�t�@
    u32 vb_slot = 0;
    ID3D11Buffer* vb[1] = { mVertexBuffer };
    u32 stride[1] = { mVertexDataSize };
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


    // �T���v���[
    if (mTexture)
    {
        u32 smp_slot = 0;
        ID3D11SamplerState* smp[1] = { mSampler };
        context->PSSetSamplers(smp_slot, 1, smp);

        // �V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j
        u32 srv_slot = 0;
        ID3D11ShaderResourceView* srv[1] = { mShaderResView };
        context->PSSetShaderResources(srv_slot, 1, srv);
    }

    //�萔�o�b�t�@
    ConstBuffer cbuff;

    // �v���W�F�N�V�����s��
    f32 aspect = Framework::Constants::WIDTH / Framework::Constants::HEIGHT;//�A�X�y�N�g��
    f32 min_z = 0.01f;
    f32 max_z = 1000.0f;
    f32 fov = DirectX::XM_PIDIV4;//��p
    cbuff.mtxProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, min_z, max_z));

    // �J�����s��
    DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
    DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cbuff.mtxView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));
    f32 RotateY = 0.0f;
    for (s32 y = 0; y < FONT_Y_NUM; ++y)
    {
        for (s32 x = 0; x < FONT_X_NUM; ++x)
        {
            s32 index = y * FONT_X_NUM + x;
            cbuff.worldPos[index].x = x * FONT_WIDTH;
            cbuff.worldPos[index].y = y * FONT_HEIGHT;
        }
    }
    cbuff.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1);
    // �V�F�[�_�[�ł͍s���]�u���Ă���n��

    // �萔�o�b�t�@���e�X�V
    context->UpdateSubresource(mCBuffer, 0, NULL, &cbuff, 0, 0);

    // �萔�o�b�t�@
    u32 cb_slot = 0;
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
    //context->DrawIndexed(mVertexNum, 0, 0);

    context->DrawIndexedInstanced(mVertexNum, INSTANCE_NUM, 0, 0, 0);
}

} // namespace Graphics
} // namespace Framework
