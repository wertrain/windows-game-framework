#ifndef DEFAULTFONT_H_
#define DEFAULTFONT_H_

#include "../GraphicsBase.h"

namespace Framework {
namespace Graphics {

class DefaultFont : public GraphicsBase
{
public:
    DefaultFont();
    virtual ~DefaultFont();

    bool Create(
        ID3D11Device* device, ID3D11DeviceContext* context,
        const f32* vertices, const u32 vertex_data_size, const u32 vertex_num,
        const wchar_t* texture_filename
    );

public:
    void Destroy();
    void Render(ID3D11DeviceContext* context);

protected:
    ID3D11InputLayout* mVertexLayout;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mInstancingVertexBuffer;
    ID3D11Buffer* mIndexBuffer;

    ID3D11Buffer* mCBuffer;
    ID3D11RasterizerState* mRsState;
    ID3D11DepthStencilState* mDsState;
    ID3D11BlendState* mBdState;

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    u32 mVertexNum;
    u32 mVertexDataSize;

    ID3D11Resource* mTexture;
    ID3D11ShaderResourceView* mShaderResView;
    ID3D11SamplerState* mSampler;
};

} // namespace Graphics
} // namespace Framework

#endif // DEFAULTFONT_H_
