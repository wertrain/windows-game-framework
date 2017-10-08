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
};

} // namespace Graphics
} // namespace Framework

#endif // QUADRANGLE_H_
