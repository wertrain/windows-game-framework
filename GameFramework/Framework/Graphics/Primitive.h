#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include "GraphicsBase.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

namespace Framework {
namespace Graphics {

class Primitive : public GraphicsBase
{
public:
    // 頂点データ構造体
    struct VertexData
    {
        float x;
        float y;
        float z;
    };

protected:
    Primitive() ;
    virtual ~Primitive();

    bool Create(
        ID3D11Device* device, ID3D11DeviceContext* context,
        VertexData* vertices, const unsigned int vertex_num
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
    unsigned int mVertexNum;
};

} // namespace Graphics
} // namespace Framework

#endif // PRIMITIVE_H_
