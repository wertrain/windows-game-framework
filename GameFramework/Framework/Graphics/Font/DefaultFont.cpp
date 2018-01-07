#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../../Common/Includes.h"
#include "../../System/Includes.h"
#include "../../Constants.h"
#include "../../External/DirectXTex/WICTextureLoader/WICTextureLoader.h"
#include "FontImage.h"

#include "DefaultFont.h"

NS_FW_GFX_BEGIN

// �e�N�X�`���̕�������`
static const s32 TEXTURE_CHAR_X_NUM = 36;
static const s32 TEXTURE_CHAR_Y_NUM = 3;
static const s32 TEXTURE_FONT_WIDTH = 16;
static const s32 TEXTURE_FONT_HEIGHT = 16;
// �X�N���[�����̈�s�E��񂠂��蕶������`
static const s32 FONT_X_NUM = NS_FW_CONST::WIDTH / TEXTURE_FONT_WIDTH;
static const s32 FONT_Y_NUM = NS_FW_CONST::HEIGHT / TEXTURE_FONT_HEIGHT;
static const s32 INSTANCE_NUM = FONT_X_NUM * FONT_Y_NUM;
// �C���X�^���X�ꖇ������̃T�C�Y
static const f32 PLANE_WIDTH = 2.0f / FONT_X_NUM;
static const f32 PLANE_HEIGHT = 2.0f / FONT_Y_NUM;
static const f32 UV_WIDTH = 1.0f / TEXTURE_CHAR_X_NUM;
static const f32 UV_HEIGHT = 1.0f / TEXTURE_CHAR_Y_NUM;

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
        { {        0.0f,         0.0f, 0.0f}, {     0.0f, UV_HEIGHT } },
        { { PLANE_WIDTH,         0.0f, 0.0f}, { UV_WIDTH, UV_HEIGHT } },
        { {        0.0f, PLANE_HEIGHT, 0.0f}, {     0.0f,      0.0f } },
        { { PLANE_WIDTH, PLANE_HEIGHT, 0.0f}, { UV_WIDTH,      0.0f } }
    };

    mVertexDataSize = sizeof(VertexData);
    mVertexNum = 4;

    NS_FW_SYS::Binary vsFile;
    NS_FW_SYS::Binary psFile;

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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,    DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // ���̓A�Z���u���ɃW�I���g�������p�̍s���ǉ��ݒ肷��
        { "IPOSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "IPOSITION", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "IPOSITION", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "IPOSITION", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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

    // �C���f�b�N�X�o�b�t�@
    UINT indices[] = { 0, 1, 2, 3 };
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
        // �e�N�X�`���쐬
        hr = DirectX::CreateWICTextureFromMemory(device, gfont_DDS, gfont_DDS_len, &mTexture, &mShaderResView);
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

    // �u�����h�X�e�[�g
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
    // ���_�o�b�t�@
    u32 vb_slot = 0;
    ID3D11Buffer* vb[2] = { mVertexBuffer, mInstancingVertexBuffer };
    u32 stride[2] = { mVertexDataSize, sizeof(InstancingPos) };
    u32 offset[2] = { 0, 0 };
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
        u32 smp_slot = 0;
        ID3D11SamplerState* smp[1] = { mSampler };
        context->PSSetSamplers(smp_slot, ARRAYSIZE(smp), smp);

        // �V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j
        u32 srv_slot = 0;
        ID3D11ShaderResourceView* srv[1] = { mShaderResView };
        context->PSSetShaderResources(srv_slot, ARRAYSIZE(srv), srv);
    }

    // �C���X�^���V���O�`��ʒu
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        HRESULT hr = context->Map(mInstancingVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return;
        InstancingPos* instancing = (InstancingPos*)(mappedResource.pData);
        
        for (s32 y = 0; y < FONT_Y_NUM; ++y)
        {
            for (s32 x = 0; x < FONT_X_NUM; ++x)
            {
                s32 index = y * FONT_X_NUM + x;
                instancing[index].pos.x = -1.0f + static_cast<f32>(x * PLANE_WIDTH);
                instancing[index].pos.y = -1.0f + static_cast<f32>(y * PLANE_HEIGHT);
                instancing[index].pos.z = UV_WIDTH * x;
                instancing[index].pos.w = UV_HEIGHT * y;
            }
        }
        context->Unmap(mInstancingVertexBuffer, 0);
    }

    // ���X�^���C�U�X�e�[�g
    context->RSSetState(mRsState);

    // �u�����h�X�e�[�g
    context->OMSetBlendState(mBdState, NULL, 0xfffffff);

    // �|���S���`��
    context->DrawIndexedInstanced(mVertexNum, INSTANCE_NUM, 0, 0, 0);
}

NS_FW_GFX_END
