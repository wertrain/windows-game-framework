#include "Framework/Includes.h"

#include "Framework/Graphics/Includes.h"
#include "Tool/Archiver/Includes.h"

Framework::Graphics::Text s_Text;
Framework::Graphics::Texture s_Texure;
Framework::Graphics::Render2D s_Render2D;

ID3D11BlendState* s_BlendState;

//#define ARCHIVER_TEST
#define TEXTWRITE_TEST

#ifdef ARCHIVER_TEST
void ArchiverTest(ID3D11Device* device)
{
    Framework::System::File::Binary file;
    file.Read(L"bintest.bin");

    Framework::Tool::Archiver archiver;
    archiver.ReadBinary(file.Get(), file.Size());

    const s32 index = 0;
    const size_t size = archiver.GetDataSize(index);
    std::unique_ptr<char> data(new char[size]);
    archiver.GetData(index, data.get());

    s_Texure.CreateFromMemory(device, data.get(), size);
}
#endif // ARCHIVER_TEST

#ifdef TEXTWRITE_TEST
void TextWriteTest(ID3D11Device* device, ID3D11DeviceContext* context)
{
    s_Text.Create(TEXT("APJapanesefontF.ttf"), TEXT("‚ ‚ñ‚¸‚à‚¶’X"), 200);
    s_Text.WriteText(device, context, &s_Texure);
}
#endif // TEXTWRITE_TEST

/**
 * create game object
 */
bool Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    //ƒuƒŒƒ“ƒh•`‰æ‚ÌÝ’è
    D3D11_BLEND_DESC BlendDesc;
    ZeroMemory(&BlendDesc, sizeof(D3D11_BLEND_DESC));
    BlendDesc.AlphaToCoverageEnable = FALSE;
    BlendDesc.IndependentBlendEnable = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable = TRUE;
    BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device->CreateBlendState(&BlendDesc, &s_BlendState);

    s_Render2D.Create(device);

#if defined ARCHIVER_TEST
    ArchiverTest(device);
#elif defined TEXTWRITE_TEST
    TextWriteTest(device, context);
#else
    s_Texure.CreateFromFile(device, TEXT("usa.png"));
#endif

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
    context->OMSetBlendState(s_BlendState, 0, 0xffffffff);

    s_Render2D.Render(context, 200.0f, 100.0f, s_Texure.GetWidth(), s_Texure.GetHeight(), &s_Texure);
}

/**
 * destroy game object
 */
void Destroy()
{
    s_Render2D.Destroy();
    s_Text.Destroy();
}
