#include <d3d11.h>

#include "DefaultFont.h"

namespace Framework {
namespace Graphics {

DefaultFont::DefaultFont()
{

}

DefaultFont::~DefaultFont()
{

}

bool Create(ID3D11Device* device)
{
    return true;
}

void Destroy()
{

}

} // namespace Graphics
} // namespace Framework
