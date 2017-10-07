#include "Framework/Includes.h"

#include "Framework/Graphics/Quadrangle.h"

Framework::Graphics::Quadrangle s_Quadrangle;

/**
 * create game object
 */
bool Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    s_Quadrangle.Create(device, context);
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
    s_Quadrangle.Render(context);
}

/**
 * destroy game object
 */
void Destroy()
{

}
