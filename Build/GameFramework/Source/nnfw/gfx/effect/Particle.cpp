/**
 * @file Particle.cpp
 * @brief �p�[�e�B�N����\���N���X
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

// �V�F�[�_�萔�o�b�t�@
struct ConstBuffer
{
    Matrix44 mtxProj;
    Matrix44 mtxView;
    Matrix44 mtxWorld;
    Vector4 Diffuse;
};

// �C���X�^���X�ꖇ������̃T�C�Y
static const float PLANE_SIZE = 1.0f;
static const float UV_WIDTH = 1.0f;
static const float UV_HEIGHT = 1.0f;

struct InstancingData
{
    Vector4 pos;
    Vector4 color;
};
static const uint32_t instancingDataSize = sizeof(InstancingData);
static_assert(sizeof(InstancingData) == 32, "sizeof InstancingData == 32");

static NS_FW_UTIL::Random sRand(static_cast<unsigned long>(time(0)));

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

}

Particles::~Particles()
{

}

bool Particles::Create(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t *fileName, const uint32_t instanceNum)
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
        { { 0.0f,       0.0f,       0.0f }, { 0.0f,     UV_HEIGHT } },
        { { PLANE_SIZE, 0.0f,       0.0f }, { UV_WIDTH, UV_HEIGHT } },
        { { 0.0f,       PLANE_SIZE, 0.0f }, { 0.0f,     0.0f      } },
        { { PLANE_SIZE, PLANE_SIZE, 0.0f }, { UV_WIDTH, 0.0f      } }
    };
    // z ���W�ɂ̓T�C�Y�̔����̒l�����Ă���
    vertices[0].pos[2] = vertices[1].pos[2] = vertices[2].pos[2] = vertices[3].pos[2] = PLANE_SIZE * 0.5f;

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
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // ���̓A�Z���u���ɃW�I���g�������p�̍s���ǉ��ݒ肷��
        { "IPOSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "ICOLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        //{ "ICOLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2,  0, D3D11_INPUT_PER_INSTANCE_DATA, 2 },
        //{ "IPOSITION", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        //{ "IPOSITION", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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

    // �C���X�^���V���O�`��p�o�b�t�@
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = instancingDataSize * instanceNum;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = device->CreateBuffer(&bd, nullptr, &mInstancingBuffer);
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

    // �萔�o�b�t�@
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
        std::wstring wpath = fileName;

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
    mInstanceNum = instanceNum;

    EmitAll();

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
    SafeRelease(mInstancingBuffer);
    SafeRelease(mVertexBuffer);
    SafeRelease(mVertexLayout);
}

bool Particles::Emit()
{
    static int count = 0;
    Particle particle;
    particle.pos = Vector4(
        (-1.0f + sRand.NextFloat() * 2.0f),
        (-1.0f + sRand.NextFloat() * 2.0f),
        (-1.0f + sRand.NextFloat() * 10.0f),
        0.0f);
    particle.speed = sRand.NextFloat() * 0.1f; // ��]���x
    particle.lifeSpan = particle.maxLifeSpan = sRand.NextFloat() * 100.0f;
    //particle.maxLifeSpan = sRand.NextFloat();
    particle.flag = Particle::Flags::Alive;
    return mParticles.Enqueue(particle);
}

void Particles::EmitAll()
{
    for (unsigned int index = 0; index < mInstanceNum; ++index)
    {
        Emit();
    }
}

void Particles::Render(ID3D11DeviceContext* context)
{
    //�萔�o�b�t�@
    ConstBuffer cbuff;

    // �v���W�F�N�V�����s��
    float aspect = NS_FW_CONST::WIDTH / NS_FW_CONST::HEIGHT;//�A�X�y�N�g��
    float min_z = 0.01f;
    float max_z = 1000.0f;
    float fov = DirectX::XM_PIDIV4;//��p
    cbuff.mtxProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, min_z, max_z));

    // �J�����s��
    DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
    DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cbuff.mtxView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));
    float RotateY = 0.0f;
    cbuff.mtxWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(RotateY));
    cbuff.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1);
    // �V�F�[�_�[�ł͍s���]�u���Ă���n��

    // ���_�o�b�t�@
    uint32_t vb_slot = 0;
    const uint32_t vb_size = 2;
    ID3D11Buffer* vb[vb_size] = { mVertexBuffer, mInstancingBuffer };
    uint32_t stride[vb_size] = { mVertexDataSize, instancingDataSize };
    uint32_t offset[vb_size] = { 0, 0 };
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

    // �萔�o�b�t�@���e�X�V
    context->UpdateSubresource(mConstBuffer, 0, NULL, &cbuff, 0, 0);

    // �萔�o�b�t�@
    uint32_t cb_slot = 0;
    ID3D11Buffer* cb[1] = { mConstBuffer };
    context->VSSetConstantBuffers(cb_slot, ARRAYSIZE(cb), cb);
    context->PSSetConstantBuffers(cb_slot, ARRAYSIZE(cb), cb);

    // �T���v���[
    if (mTexture)
    {
        uint32_t smp_slot = 0;
        ID3D11SamplerState* smp[] = { mSampler };
        context->PSSetSamplers(smp_slot, ARRAYSIZE(smp), smp);

        // �V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j
        uint32_t srv_slot = 0;
        ID3D11ShaderResourceView* srv[] = { mShaderResView };
        context->PSSetShaderResources(srv_slot, ARRAYSIZE(srv), srv);
    }

    // �p�[�e�B�N���`�搔
    int drawParticleNum = 0;

    // �C���X�^���V���O�`��
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = context->Map(mInstancingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr))
        {
            return;
        }

        for (unsigned int index = 0; index < mInstanceNum; ++index)
        {
            Particle& particle = mParticles[index];
            particle.pos.w += particle.speed;
            if ((particle.lifeSpan -= 0.1f) <= 0)
            {
                continue;
            }
            InstancingData* instancing = (InstancingData*)(mappedResource.pData);
            instancing[drawParticleNum].pos = particle.pos;
            instancing[drawParticleNum].color = Vector4(0, 0, 0, (particle.lifeSpan / particle.maxLifeSpan));

            ++drawParticleNum;
        }

        context->Unmap(mInstancingBuffer, 0);
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
    context->DrawIndexedInstanced(mVertexNum, drawParticleNum, 0, 0, 0);

    // �X�e�[�g��߂�
    context->OMSetDepthStencilState(beforeState, 0);
}

NS_FW_GFX_END
