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
    s_Render2D.Create(device);
    //s_Texure.CreateFromFile(device, TEXT("usa.png"));

    s_Text.Create(TEXT("APJapanesefontF.ttf"), TEXT("‚ ‚ñ‚¸‚à‚¶’X"), 24);

    s_Text.WriteText(device, context, &s_Texure);

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
    s_Render2D.Render(context, 200.0f, 100.0f, 400.0f, 300.0f, &s_Texure);
}

/**
 * destroy game object
 */
void Destroy()
{
    s_Render2D.Destroy();
    s_Text.Destroy();
}
