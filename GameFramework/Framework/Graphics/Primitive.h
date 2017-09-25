#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include "GraphicsBase.h"

namespace Framework {
namespace Graphics {

class Primitive : public GraphicsBase
{
public:
    Primitive() ;
    virtual ~Primitive();

    Vector3 GetPos() const;

protected:
    Vector3 mPos;
};

} // namespace Graphics
} // namespace Framework

#endif // PRIMITIVE_H_
