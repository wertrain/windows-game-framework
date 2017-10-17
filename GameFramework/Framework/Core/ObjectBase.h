#ifndef OBJECTBASE_H_
#define OBJECTBASE_H_

// �w�b�_�[�ŃC���N���[�h����̂͋ɗ͔���������
#include "../Common/Defines.h"
#include "../Common/Types.h"

namespace Framework {
namespace System {

class ObjectBase
{
public:
    ObjectBase();
    virtual ~ObjectBase() = 0;
};

} // namespace System 
} // namespace Framework

#endif // OBJECTBASE_H_
