/**
 * @file Particle.h
 * @brief パーティクルを表すクラス
 */
#ifndef GRAPHICS_EFFECT_PARTICLE_H_
#define GRAPHICS_EFFECT_PARTICLE_H_

#include <nnfw/ut/CircularArray.h>
#include <nnfw/gfx/GraphicsBase.h>
#include <DirectXMath.h>

NS_FW_GFX_BEGIN

/// <summary>
/// パーティクルを表すクラス
/// </summary>
class Particles
{
public:
    Particles();
    ~Particles();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t *fileName, const uint32_t instanceNum);
    void Destroy();
    bool Emit();
    void EmitAll();
    void Update(const float elapsedTime);
    void Render(ID3D11DeviceContext* context);

private:
    struct Particle
    {
        enum Flags
        {
            Alive = (1 << 0)
        };

        uint32_t flag;
        Vector4 pos;
        Vector4 velocity;
        float speed;
        float lifeSpan;
        float maxLifeSpan;
    };

private:
    ID3D11InputLayout* mVertexLayout;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mInstancingBuffer;
    ID3D11Buffer* mIndexBuffer;

    ID3D11BlendState* mBdState;

    ID3D11Buffer* mConstBuffer;
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
