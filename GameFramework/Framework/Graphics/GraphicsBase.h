#ifndef GRAPHICS_GRAPHICSBASE_H_
#define GRAPHICS_GRAPHICSBASE_H_

#include "../Core/ObjectBase.h"

NS_FW_GFX_BEGIN

class GraphicsBase : public NS_FW_SYS::ObjectBase
{
public:
    GraphicsBase() ;
    virtual ~GraphicsBase() = 0;
};

NS_FW_GFX_END

#endif // GRAPHICS_GRAPHICSBASE_H_
