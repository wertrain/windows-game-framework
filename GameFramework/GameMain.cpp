#include "Framework/Includes.h"

#include "Framework/Graphics/Text.h"

Framework::Graphics::Text s_Text;

/**
 * create game object
 */
bool Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    s_Text.Create(TEXT("APJapanesefontF.ttf"), TEXT("‚ ‚ñ‚¸‚à‚¶’X"), 24);
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

}

/**
 * destroy game object
 */
void Destroy()
{
    s_Text.Destroy();
}
