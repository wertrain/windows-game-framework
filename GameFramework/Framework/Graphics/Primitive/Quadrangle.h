#ifndef QUADRANGLE_H_
#define QUADRANGLE_H_

#include "Primitive.h"

NS_FW_GFX_BEGIN

class Quadrangle : public Primitive
{
public:
    Quadrangle() ;
    virtual ~Quadrangle();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
};

NS_FW_GFX_END

#endif // QUADRANGLE_H_
