#ifndef GRAPHICS_TEXTWRITER_H_
#define GRAPHICS_TEXTWRITER_H_

#include "GraphicsBase.h"

NS_FW_GFX_BEGIN

class Texture;

class TextWriter : public GraphicsBase
{
public:
    TextWriter() ;
    virtual ~TextWriter();

    bool Create(const wchar_t* font_ttf_file, const wchar_t* face_name, const s32 font_size);
    void Destroy();
    bool WriteChar(const wchar_t c, ID3D11Device* device, ID3D11DeviceContext* context, Texture* texture);
    bool WriteText(const wchar_t* text, ID3D11Device* device, ID3D11DeviceContext* context, Texture* texture); 
    bool WriteText(ID3D11Device* device, ID3D11DeviceContext* context);
    void Render(ID3D11DeviceContext* context);

private:
    std::wstring mFontFileName;
    HFONT mFontHandle;
    DESIGNVECTOR mDesign;
};

NS_FW_GFX_END

#endif // GRAPHICS_TEXTWRITER_H_
