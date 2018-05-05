/**
 * @file Primitive.h
 * @brief 単純なポリゴンを表す基底クラス
 */
#ifndef GRAPHICS_PRIMITIVE_PRIMITIVE_H_
#define GRAPHICS_PRIMITIVE_PRIMITIVE_H_

#include <nnfw/gfx/GraphicsBase.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

NS_FW_GFX_BEGIN

/// <summary>
/// 単純なポリゴンを表す基底クラス
/// </summary>
class Primitive : public GraphicsBase
{
protected:
    Primitive() ;
    virtual ~Primitive();

    bool Create(
        ID3D11Device* device, ID3D11DeviceContext* context,
        const float* vertices, const uint32_t vertex_data_size, const uint32_t vertex_num,
        const wchar_t* texture_filename
    );

public:
    void Destroy();
    void Render(ID3D11DeviceContext* context);

protected:
    ID3D11InputLayout* mVertexLayout;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;

    ID3D11Buffer* mCBuffer;
    ID3D11RasterizerState* mRsState;
    ID3D11DepthStencilState* mDsState;
    ID3D11BlendState* mBdState;

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    uint32_t mVertexNum;
    uint32_t mVertexDataSize;

    ID3D11Resource* mTexture;
    ID3D11ShaderResourceView* mShaderResView;
    ID3D11SamplerState* mSampler;
};

NS_FW_GFX_END

#endif // GRAPHICS_PRIMITIVE_PRIMITIVE_H_
