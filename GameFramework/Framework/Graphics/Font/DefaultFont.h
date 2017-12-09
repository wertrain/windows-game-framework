#ifndef DEFAULTFONT_H_
#define DEFAULTFONT_H_

#include "../GraphicsBase.h"

namespace Framework {
namespace Graphics {

class DefaultFont 
{
public:
    DefaultFont();
    virtual ~DefaultFont();

public:
    bool Create(ID3D11Device* device);
    void Destroy();
};

} // namespace Graphics
} // namespace Framework

#endif // DEFAULTFONT_H_
