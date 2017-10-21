#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Primitive.h"

namespace Framework {
namespace Graphics {

class Triangle : public Primitive
{
public:
    Triangle() ;
    virtual ~Triangle();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
};

} // namespace Graphics
} // namespace Framework

#endif // TRIANGLE_H_
