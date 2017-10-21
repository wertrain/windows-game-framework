#ifndef TEXT_H_
#define TEXT_H_

#include "GraphicsBase.h"

namespace Framework {
namespace Graphics {

class Text : public GraphicsBase
{
public:
    Text() ;
    virtual ~Text();

    bool Create(const wchar_t* font_ttf_file, const wchar_t* face_name, const s32 font_size);
    void Destroy();
    bool WriteText(ID3D11Device* device, ID3D11DeviceContext* context);
    void Render(ID3D11DeviceContext* context);

private:
    HFONT m_FontHandle;
    ID3D11Texture2D* m_Texture;  // 2Dテクスチャ
};

} // namespace Graphics
} // namespace Framework

#endif // TEXT_H_
