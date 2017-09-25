#include "..\System\Types.h"
#include "Primitive.h"

namespace Framework {
namespace Graphics {

Primitive::Primitive()
    : mPos()
{

}

Primitive::~Primitive()
{

}

Vector3 Primitive::GetPos() const
{
    return mPos;
}

} // namespace Graphics
} // namespace Framework
