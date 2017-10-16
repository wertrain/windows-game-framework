#ifndef OBJECTBASE_H_
#define OBJECTBASE_H_

// ヘッダーでインクルードするのは極力避けたいが
#include "../Common/Defines.h"
#include "../Common/Types.h"

namespace Framework {
namespace System {

class ObjectBase
{
public:
    ObjectBase() ;
    virtual ~ObjectBase();
};

} // namespace System 
} // namespace Framework

#endif // OBJECTBASE_H_
