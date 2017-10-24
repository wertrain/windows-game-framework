#include "Framework/Includes.h"

#include "Framework/Graphics/Text.h"
#include "Framework/Graphics/Texture.h"
#include "Framework/Graphics/Render2D.h"

Framework::Graphics::Text s_Text;
Framework::Graphics::Texture s_Texure;
Framework::Graphics::Render2D s_Render2D;

/**
 * create game object
 */
bool Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    s_Text.Create(TEXT("APJapanesefontF.ttf"), TEXT("‚ ‚ñ‚¸‚à‚¶’X"), 24);
    s_Render2D.Create(device);
    s_Texure.CreateFromFile(device, TEXT("image.png"));
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
    s_Render2D.Render(context, 0, 0, 1.0f, 1.0f, &s_Texure);
}

/**
 * destroy game object
 */
void Destroy()
{
    s_Render2D.Destroy();
    s_Text.Destroy();
}
