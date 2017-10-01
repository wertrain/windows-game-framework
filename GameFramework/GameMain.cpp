#include "Framework/Includes.h"

#include "Framework/Graphics/Triangle.h"

static Framework::Graphics::Triangle s_Triangle;

/**
 * create game object
 */
bool Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    s_Triangle.Create(device, context);

    return true;
}

/**
 * update frame
 */
void Update(const DWORD /*nowTime*/)
{

}

/**
 * draw game object
 */
void Draw(ID3D11DeviceContext* context)
{
    s_Triangle.Render(context);
}

/**
 * destroy game object
 */
void Destroy()
{
    s_Triangle.Destroy();
}
