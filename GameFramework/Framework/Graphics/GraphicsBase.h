#ifndef GRAPHICSBASE_H_
#define GRAPHICSBASE_H_

#include "../Core/ObjectBase.h"

NS_FW_GFX_BEGIN

class GraphicsBase : public NS_FW_SYS::ObjectBase
{
public:
    GraphicsBase() ;
    virtual ~GraphicsBase() = 0;
};

NS_FW_GFX_END

#endif // GRAPHICSBASE_H_
