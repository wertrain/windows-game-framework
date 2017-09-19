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

    const int GetX(){ return mX; }
    const int GetY(){ return mY; }
    const int GetWidth(){ return mWidth; }
    const int GetHeight(){ return mHeight; }

protected:
    int mX;
    int mY;
    int mWidth;
    int mHeight;
};

} // namespace Graphics
} // namespace Framework

#endif // PRIMITIVE_H_
