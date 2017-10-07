#ifndef QUADRANGLE_H_
#define QUADRANGLE_H_

#include "Primitive.h"

namespace Framework {
namespace Graphics {

class Quadrangle : public Primitive
{
public:
    Quadrangle() ;
    virtual ~Quadrangle();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
    void Destroy();

    void Render(ID3D11DeviceContext* context);
private:
    ID3D11InputLayout* mVertexLayout;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;

    ID3D11Buffer* mCBuffer;
    ID3D11RasterizerState* mRsState;
    ID3D11DepthStencilState* mDsState;
    ID3D11BlendState* mBdState;

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
};

} // namespace Graphics
} // namespace Framework

#endif // QUADRANGLE_H_
