/**
 * @file DefaultFont.h
 * @brief �V�X�e���p�̃t�H���g�`��N���X
 */
#ifndef GRAPHICS_FONT_DEFAULTFONT_H_
#define GRAPHICS_FONT_DEFAULTFONT_H_

#include <nnfw/gfx/GraphicsBase.h>

NS_FW_GFX_BEGIN

/// <summary>
/// �V�X�e���t�H���g�`��N���X
/// </summary>
class DefaultFont : public GraphicsBase
{
public:
    DefaultFont();
    virtual ~DefaultFont();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
    void Destroy();
    void ClearText();
    void SetText(const int x, const int y, const wchar_t* text);
    void Render(ID3D11DeviceContext* context);

protected:
    ID3D11InputLayout* mVertexLayout;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mInstancingVertexBuffer;
    ID3D11Buffer* mIndexBuffer;

    ID3D11BlendState* mBdState;

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    u32 mVertexNum;
    u32 mVertexDataSize;

    ID3D11Resource* mTexture;
    ID3D11ShaderResourceView* mShaderResView;
    ID3D11SamplerState* mSampler;
    wchar_t* mPrintText;
};

NS_FW_GFX_END

#endif // GRAPHICS_FONT_DEFAULTFONT_H_
