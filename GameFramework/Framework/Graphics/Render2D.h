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
    void Render(ID3D11DeviceContext* context, const f32 x1, const f32 y1, const f32 x2, const f32 y2);
    void Render(ID3D11DeviceContext* context, const f32 x1, const f32 y1, const f32 x2, const f32 y2, Texture* texture);

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
