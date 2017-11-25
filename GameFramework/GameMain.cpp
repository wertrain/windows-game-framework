#include "Framework/Includes.h"

#include "Framework/Graphics/Text.h"
#include "Framework/Graphics/Texture.h"
#include "Framework/Graphics/Render2D.h"
#include "Tool/Archiver/Includes.h"

Framework::Graphics::Text s_Text;
Framework::Graphics::Texture s_Texure;
Framework::Graphics::Render2D s_Render2D;

#define ARCHIVER_TEST
//#define TEXTWRITE_TEST

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
    s_Text.Create(TEXT("uzura.ttf"), TEXT("‚¤‚¸‚çƒtƒHƒ“ƒg"), 24);
    s_Text.WriteText(device, context, &s_Texure);
}
#endif // TEXTWRITE_TEST

/**
 * create game object
 */
bool Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    s_Render2D.Create(device);

#if defined ARCHIVER_TEST
    ArchiverTest(device);
#elif defined TEXTWRITE_TEST
    TextWriteTest(device);
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
