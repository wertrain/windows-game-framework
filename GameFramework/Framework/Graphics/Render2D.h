#ifndef RENDER2D_H_
#define RENDER2D_H_

namespace Framework {
namespace Graphics {

class Texture;

class Render2D
{
public:
    Render2D() ;
    virtual ~Render2D();
    bool Create(ID3D11Device* device);
    void Destroy();
    void Render(ID3D11DeviceContext* context, const f32 x, const f32 y, const f32 width, const f32 height);
    void Render(ID3D11DeviceContext* context, const f32 x, const f32 y, const f32 width, const f32 height, Texture* texture);

private:
    HRESULT CreateShader(ID3D11Device* device);
    HRESULT CreateBuffer(ID3D11Device* device);

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
    ID3D11Buffer* mConstantBuffer;
    ID3D11InputLayout* mVertexLayout;
};

} // namespace Graphics 
} // namespace Framework

#endif // RENDER2D_H_
