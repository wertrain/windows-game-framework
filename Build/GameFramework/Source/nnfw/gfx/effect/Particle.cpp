/**
 * @file Particle.cpp
 * @brief �p�[�e�B�N����\���N���X
 */
#include "Precompiled.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/ut/Memory.h>
#include <nnfw/ut/Random.h>
#include <nnfw/external/DirectXTex/WICTextureLoader/WICTextureLoader.h>
#include <nnfw/gfx/effect/Particle.h>

NS_FW_GFX_BEGIN

// �C���X�^���X�ꖇ������̃T�C�Y
static const float PLANE_WIDTH = 0.1f;
static const float PLANE_HEIGHT = 0.1f;
static const float UV_WIDTH = 1.0f;
static const float UV_HEIGHT = 1.0f;

struct InstancingPos
{
    Vector4 pos;
};
static_assert(sizeof(InstancingPos) == 16, "sizeof InstancingPos == 16");

Particles::Particles()
    : mVertexLayout(nullptr)
    , mVertexBuffer(nullptr)
    , mInstancingVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mBdState(nullptr)
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

}

Particles::~Particles()
{

}

bool Particles::Create(ID3D11Device* device, ID3D11DeviceContext* context, const uint32_t instanceNum)
{
    NNFW_UNUSED(context);

    // ���_�f�[�^�\����
    struct VertexData
    {
        float pos[3];
        float uv[2];
    };
    static_assert(sizeof(VertexData) == (4 * 3 + 4 * 2), "sizeof VertexData == (4 * 3 + 4 * 2)");

    VertexData vertices[] =
    {
        // UV �͏㉺���E���]
        { { 0.0f,        0.0f,         0.0f }, { 0.0f,     UV_HEIGHT } },
        { { PLANE_WIDTH, 0.0f,         0.0f }, { UV_WIDTH, UV_HEIGHT } },
        { { 0.0f,        PLANE_HEIGHT, 0.0f }, { 0.0f,     0.0f      } },
        { { PLANE_WIDTH, PLANE_HEIGHT, 0.0f }, { UV_WIDTH, 0.0f      } }
    };

    mVertexDataSize = sizeof(VertexData);
    mVertexNum = 4;

#if BUILTIN_PARTICLE_SHADER
    // ���_�V�F�[�_�[����
    if (FAILED(device->CreateVertexShader(vs_particle_simple_cso, vs_particle_simple_len, NULL, &mVertexShader)))
    {
        return false;
    }

    // �s�N�Z���V�F�[�_�[����
    if (FAILED(device->CreatePixelShader(ps_particle_simple_cso, ps_particle_simple_cso_len, NULL, &mPixelShader)))
    {
        return false;
    }
#else
    NS_FW_SYS::Binary vsFile;
    NS_FW_SYS::Binary psFile;

    // �R���p�C���ς݃o�[�e�b�N�X�V�F�[�_�[�t�@�C���̓ǂݍ���
    if (!vsFile.Read(L"vs_particle_simple.cso"))
    {
        return false;
    }
    // �R���p�C���ς݃s�N�Z���V�F�[�_�[�t�@�C���̓ǂݍ���
    if (!psFile.Read(L"ps_particle_simple.cso"))
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
#endif

    // ���̓��C�A�E�g��`
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,    DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD_ADD", 0,    DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // ���̓A�Z���u���ɃW�I���g�������p�̍s���ǉ��ݒ肷��
        { "IPOSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "IPOSITION", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "IPOSITION", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "IPOSITION", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    const UINT elem_num = ARRAYSIZE(layout);

    // ���̓��C�A�E�g�쐬
#if BUILTIN_PARTICLE_SHADER
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vs_particle_simple_cso, vs_particle_simple_cso_len, &mVertexLayout);
#else
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
#endif
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
        if (FAILED(hr))
        {
            return false;
        }
    }

    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(InstancingPos) * instanceNum;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = device->CreateBuffer(&bd, nullptr, &mInstancingVertexBuffer);
        if (FAILED(hr))
        {
            return false;
        }
    }

    // �C���f�b�N�X�o�b�t�@
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

    {
        std::wstring wpath = L"particle4u.png";

        // �e�N�X�`���쐬
        hr = DirectX::CreateWICTextureFromFile(device, context, wpath.c_str(), &mTexture, &mShaderResView);
        if (FAILED(hr))
        {
            return false;
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
        if (FAILED(hr))
        {
            return false;
        }
    }

    // �u�����h�X�e�[�g
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

    // �f�v�X����
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

    NS_FW_UTIL::Random rand(time(0));
    for (uint32_t i = 0; i < instanceNum; ++i)
    {
        Particle particle;
        particle.pos = Vector4(
            (-1.0f + rand.NextFloat() * 2.0f) - PLANE_WIDTH * 0.5f, 
            (-1.0f + rand.NextFloat() * 2.0f) - PLANE_HEIGHT * 0.5f,
            (-1.0f + rand.NextFloat() * 2.0f), 1.0f);
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

    SafeRelease(mIndexBuffer);
    SafeRelease(mVertexBuffer);
    SafeRelease(mVertexLayout);
}


void Particles::Render(ID3D11DeviceContext* context)
{
    // ���_�o�b�t�@
    uint32_t vb_slot = 0;
    ID3D11Buffer* vb[2] = { mVertexBuffer, mInstancingVertexBuffer };
    uint32_t stride[2] = { mVertexDataSize, sizeof(InstancingPos) };
    uint32_t offset[2] = { 0, 0 };
    context->IASetVertexBuffers(vb_slot, ARRAYSIZE(vb), vb, stride, offset);

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
        uint32_t smp_slot = 0;
        ID3D11SamplerState* smp[] = { mSampler, mSampler };
        context->PSSetSamplers(smp_slot, ARRAYSIZE(smp), smp);

        // �V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j
        uint32_t srv_slot = 0;
        ID3D11ShaderResourceView* srv[] = { mShaderResView, mShaderResView };
        context->PSSetShaderResources(srv_slot, ARRAYSIZE(srv), srv);
    }

    // �C���X�^���V���O�`��ʒu
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        HRESULT hr = context->Map(mInstancingVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return;
        InstancingPos* instancing = (InstancingPos*)(mappedResource.pData);
        
        for (unsigned int index = 0; index < mInstanceNum; ++index)
        {
            instancing[index].pos = mParticles[index].pos;
        }
        context->Unmap(mInstancingVertexBuffer, 0);
    }
    
    // �ȑO�̃X�e�[�g�ۑ�
    ID3D11DepthStencilState* beforeState;
    context->OMGetDepthStencilState(&beforeState, 0);

    // �f�v�X������
    context->OMSetDepthStencilState(mDepthStencilState, 0);

    // �u�����h�X�e�[�g
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->OMSetBlendState(mBdState, blendFactor, 0xfffffff);

    // �|���S���`��
    context->DrawIndexedInstanced(mVertexNum, mInstanceNum, 0, 0, 0);

    // �X�e�[�g��߂�
    context->OMSetDepthStencilState(beforeState, 0);
}

NS_FW_GFX_END
