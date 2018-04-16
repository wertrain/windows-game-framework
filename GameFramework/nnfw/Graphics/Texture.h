/**
 * @file Texture.h
 * @brief テクスチャクラス
 */
#ifndef GRAPHICS_TEXTURE_H_
#define GRAPHICS_TEXTURE_H_

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct D3D11_TEXTURE2D_DESC;

NS_FW_GFX_BEGIN

/// <summary>
/// テクスチャクラス
/// </summary>
class Texture
{
public:
    Texture() ;
    virtual ~Texture();
    bool CreateFromFile(ID3D11Device* device, const wchar_t* filename);
    bool CreateFromMemory(ID3D11Device* device, const void* buffer, const size_t buffersize);
    bool Create(ID3D11Device* device, ID3D11Resource* texture);
    const ID3D11Resource* GetTexture();
    const ID3D11SamplerState* GetSamplerState();
    const ID3D11ShaderResourceView* GetShaderResourceView();
    const u32 GetWidth();
    const u32 GetHeight();

protected:
    ID3D11Resource* mTexture;
    ID3D11ShaderResourceView* mShaderResView;
    ID3D11SamplerState* mSampler;
    D3D11_TEXTURE2D_DESC mTexDesc;
};

NS_FW_GFX_END

#endif // GRAPHICS_TEXTURE_H_
