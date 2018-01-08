#ifndef TEXT_H_
#define TEXT_H_

#include <vector>
#include "../Common/Defines.h"
#include "../System/Utility/Singleton.h"
#include "Font/DefaultFont.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

NS_FW_GFX_BEGIN

class DefaultFontManager : public NS_FW_UTIL::Singleton<DefaultFontManager>
{
public:
    enum
    {
        eID_System,
        eID_User,
        eID_Num
    };

public:
    bool CreateLayer(ID3D11Device* device, ID3D11DeviceContext* context, const s32 id, const s32 priority);
    void Destroy();
    void Render(ID3D11DeviceContext* context);
    bool SetText(const int id, const int x, const int y, const wchar_t* str);
    bool SetTextFormat(const int id, const int x, const int y, const wchar_t* format, ...);

private:
    struct LayerParam
    {
        s32 id;
        s32 priority;
        DefaultFont font;
        bool empty;
    };
    typedef std::vector<LayerParam*> LayerArray;

private:
    LayerArray mLayerArray;
};

void Puts(const int x, const int y, const wchar_t* str);
void Printf(const int x, const int y, const wchar_t* format, ...);

NS_FW_GFX_END

#endif // TEXT_H_
