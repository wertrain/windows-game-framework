/**
 * @file GameMain.cpp
 * @brief ÉQÅ[ÉÄÉÅÉCÉìèàóù
 */
#include <nnfw/Includes.h>

#include <nnfw/gfx/Includes.h>
//#include "Tool/Archiver/Includes.h"
#include <nnfw/gfx/Model/ModelMqo.h>

fw::gfx::TextWriter s_Text;
fw::gfx::Texture s_Texure;
fw::gfx::Render2D s_Render2D;
fw::gfx::DefaultFont s_DefaultFont;
fw::gfx::ModelMqo s_Mqo;

//#define ARCHIVER_TEST
//#define TEXTWRITE_TEST
//#define DEFAULTFONT_TEST
//#define MODEL_TEST

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
    s_Text.Create(TEXT("APJapanesefontF.ttf"), TEXT("Ç†ÇÒÇ∏Ç‡Ç∂íX"), 180);
    s_Text.WriteText(L"abcdfÇ†Ç¢Ç§", device, context, &s_Texure);
}
#endif // TEXTWRITE_TEST

#ifdef DEFAULTFONT_TEST
void DefaultFontTest(ID3D11Device* device, ID3D11DeviceContext* context)
{
    s_DefaultFont.Create(device, context);
}
#endif // DEFAULTFONT_TEST

/**
 * create game object
 */
bool Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
#if defined ARCHIVER_TEST
    ArchiverTest(device);
#elif defined TEXTWRITE_TEST
    s_Render2D.Create(device);
    TextWriteTest(device, context);
#elif defined DEFAULTFONT_TEST
    DefaultFontTest(device, context);
#elif defined MODEL_TEST
    if (!s_Mqo.Create(device, context, L"manto/manto.mqo"))
        return false;
#else
    s_Render2D.Create(device);
    s_Texure.CreateFromFile(device, TEXT("usa.png"));
#endif

    return true;
}

/**
 * update frame
 */
void Update(const uint64_t /*elapsedTime*/, const uint64_t /*nowTime*/)
{

}

/**
 * draw game object
 */
void Draw(ID3D11DeviceContext* context)
{
#if defined ARCHIVER_TEST
#elif defined TEXTWRITE_TEST
    s_Render2D.Render(context, 0.0f, 80.0f, static_cast<f32>(s_Texure.GetWidth()), static_cast<f32>(s_Texure.GetHeight()), &s_Texure);
#elif defined DEFAULTFONT_TEST
    s_DefaultFont.Render(context); 
#elif defined MODEL_TEST
    s_Mqo.Render(context);
#else
    //s_Render2D.Render(context, 0.0f, 80.0f, static_cast<f32>(s_Texure.GetWidth()), static_cast<f32>(s_Texure.GetHeight()), &s_Texure);
#endif
}

/**
 * destroy game object
 */
void Destroy()
{
#if defined ARCHIVER_TEST
#elif defined TEXTWRITE_TEST
    s_Text.Destroy();
    s_Render2D.Destroy();
#elif DEFAULTFONT_TEST
    s_DefaultFont.Destroy();
#elif defined MODEL_TEST
    s_Mqo.Destroy();
#else
    s_Text.Destroy();
    s_Render2D.Destroy();
#endif
}
