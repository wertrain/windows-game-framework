#ifndef OBJECTBASE_H_
#define OBJECTBASE_H_

#include "../Common/Defines.h"
#include "../Common/Types.h"

NS_FW_SYS_BEGIN

class ObjectBase
{
public:
    ObjectBase();
    virtual ~ObjectBase() = 0;
};

NS_FW_SYS_END

#endif // OBJECTBASE_H_
