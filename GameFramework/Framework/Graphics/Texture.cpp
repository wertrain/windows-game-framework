#include <d3d11.h>

#include "../External/DirectXTex/WICTextureLoader/WICTextureLoader.h"
#include "../Common/Includes.h"
#include "../Utility/Memory.h"
#include "Texture.h"

NS_FW_GFX_BEGIN

Texture::Texture()
    : mTexture(nullptr)
    , mShaderResView(nullptr)
    , mSampler(nullptr)
{
    NS_FW_UTIL::memset_zero(&mTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
}

Texture::~Texture()
{
    if (mSampler != nullptr)
    {
        mSampler->Release();
        mSampler = nullptr;
    }

    if (mShaderResView != nullptr)
    {
        mShaderResView->Release();
        mShaderResView = nullptr;
    }

    if (mTexture != nullptr)
    {
        mTexture->Release();
        mTexture = nullptr;
    }
}

bool Texture::CreateFromFile(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr = DirectX::CreateWICTextureFromFile(device, filename, &mTexture, &mShaderResView);
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

    return true;
}

bool Texture::CreateFromMemory(ID3D11Device* device, const void* buffer, const size_t buffersize)
{
    HRESULT hr = DirectX::CreateWICTextureFromMemory(device, (const uint8_t*)buffer, buffersize, &mTexture, &mShaderResView, 0);
    if (FAILED(hr))
    {
        return false;
    }

    // テクスチャ情報を取得する
    static_cast<ID3D11Texture2D*>(mTexture)->GetDesc(&mTexDesc);

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

    return true;
}

bool Texture::Create(ID3D11Device* device, ID3D11Resource* texture)
{
    mTexture = texture;

    // テクスチャ情報を取得する
    static_cast<ID3D11Texture2D*>(mTexture)->GetDesc(&mTexDesc);

    // ShaderResourceViewの情報を作成する
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = mTexDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = mTexDesc.MipLevels;

    // ShaderResourceViewを作成する
    HRESULT hr = device->CreateShaderResourceView(mTexture, &srvDesc, &mShaderResView);
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

    return true;
}

const ID3D11Resource* Texture::GetTexture()
{
    return mTexture;
}

const ID3D11SamplerState* Texture::GetSamplerState()
{
    return mSampler;
}

const ID3D11ShaderResourceView* Texture::GetShaderResourceView()
{
    return mShaderResView;
}

const u32 Texture::GetWidth()
{
    return mTexDesc.Width;
}

const u32 Texture::GetHeight()
{
    return mTexDesc.Height;
}

NS_FW_GFX_END
