#include <d3d11.h>

#include "../External/DirectXTex/WICTextureLoader/WICTextureLoader.h"
#include "../System/Includes.h"
#include "Texture.h"

namespace Framework {
namespace Graphics {

Texture::Texture()
    : mTexture(nullptr)
    , mShaderResView(nullptr)
    , mSampler(nullptr)
{

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
    Framework::System::File::Binary textureFile;
    if (!textureFile.Read(filename))
        return false;

    HRESULT hr = DirectX::CreateWICTextureFromFile(device, filename, &mTexture, &mShaderResView);
    if (FAILED(hr))
    {
        return hr;
    }

    // ƒTƒ“ƒvƒ‰[ì¬
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
        return hr;
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

} // namespace Graphics
} // namespace Framework
