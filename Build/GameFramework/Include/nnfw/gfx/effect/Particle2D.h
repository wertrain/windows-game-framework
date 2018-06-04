/**
 * @file Particle2D.h
 * @brief �p�[�e�B�N����\���N���X
 */
#ifndef GRAPHICS_EFFECT_PARTICLE_H_
#define GRAPHICS_EFFECT_PARTICLE_H_

#include <nnfw/ut/CircularArray.h>
#include <nnfw/gfx/GraphicsBase.h>
#include <DirectXMath.h>

NS_FW_GFX_BEGIN

/// <summary>
/// �p�[�e�B�N����\���N���X
/// </summary>
class Particles2D
{
public:
    Particles2D();
    ~Particles2D();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context, const uint32_t instanceNum);
    void Destroy();
    void Render(ID3D11DeviceContext* context);

private:
    struct Particle
    {
        Vector4 pos;
    };

private:
    ID3D11InputLayout* mVertexLayout;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mInstancingVertexBuffer;
    ID3D11Buffer* mIndexBuffer;

    ID3D11BlendState* mBdState;

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    uint32_t mVertexNum;
    uint32_t mVertexDataSize;

    ID3D11Resource* mTexture;
    ID3D11ShaderResourceView* mShaderResView;
    ID3D11SamplerState* mSampler;
    ID3D11DepthStencilState* mDepthStencilState;

    uint32_t mInstanceNum;
    NS_FW_UTIL::CircularArray<Particle> mParticles;
};

NS_FW_GFX_END

#endif // GRAPHICS_EFFECT_PARTICLE_H_
