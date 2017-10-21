#ifndef TEXTURE_H_
#define TEXTURE_H_

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

namespace Framework {
namespace Graphics {

class Texture
{
public:
    Texture() ;
    virtual ~Texture();
    bool CreateFromFile(ID3D11Device* device, const wchar_t* filename);
    const ID3D11Resource* GetTexture();
    const ID3D11SamplerState* GetSamplerState();

protected:
    ID3D11Resource* mTexture;
    ID3D11ShaderResourceView* mShaderResView;
    ID3D11SamplerState* mSampler;
};

} // namespace Graphics 
} // namespace Framework

#endif // TEXTURE_H_
