#ifndef GRAPHICSBASE_H_
#define GRAPHICSBASE_H_

#include "../System/ObjectBase.h"

namespace Framework {
namespace Graphics {

class GraphicsBase : public Framework::System::ObjectBase
{
public:
    GraphicsBase() ;
    virtual ~GraphicsBase();
};

} // namespace Graphics
} // namespace Framework

#endif // GRAPHICSBASE_H_
