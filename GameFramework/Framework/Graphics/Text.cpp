#include <d3d11.h>
#include <algorithm>

#include "Text.h"

NS_FW_GFX_BEGIN

bool DefaultFontManager::CreateLayer(ID3D11Device* device, ID3D11DeviceContext* context, const s32 id, const s32 priority)
{
    for each(auto param in mLayerArray)
    {
        if (param->id == id)
            return false;
    }

    LayerParam* param = new LayerParam();
    if (!param->font.Create(device, context))
    {
        delete param;
        return false;
    }
    param->id = id;
    param->priority = priority;
    param->empty = true;
    mLayerArray.push_back(param);

    std::sort(mLayerArray.begin(), mLayerArray.end(),
        [](const LayerParam* x, const LayerParam* y) -> int {
            return (x->priority > y->priority);
    });
    return true;
}

void DefaultFontManager::Destroy()
{
    for each(auto param in mLayerArray)
    {
        param->font.Destroy();
        delete param;
    }
    mLayerArray.clear();
}

void DefaultFontManager::Render(ID3D11DeviceContext* context)
{
    for each(auto param in mLayerArray)
    {
        if (!param->empty)
        {
            param->font.Render(context);
            param->font.ClearText();
            param->empty = true;
        }
    }
}

bool DefaultFontManager::SetText(const int id, const int x, const int y, const wchar_t* str)
{
    for each(auto param in mLayerArray)
    {
        if (param->id == id)
        {
            param->font.SetText(x, y, str);
            param->empty = false;
            return true;
        }
    }
    return false;
}

bool DefaultFontManager::SetTextFormat(const int id, const int x, const int y, const wchar_t* format, ...)
{
    wchar_t buffer[1024];

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vswprintf(buffer, 1024, format, arg_ptr);
    va_end(arg_ptr);

    return SetText(id, x, y, buffer);
}

void Puts(const int x, const int y, const wchar_t* str)
{
    DefaultFontManager::GetInstance().SetText(NS_FW_GFX::DefaultFontManager::eID_User, x, y, str);
}

void Printf(const int x, const int y, const wchar_t* format, ...)
{
    wchar_t buffer[1024];

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vswprintf(buffer, 1024, format, arg_ptr);
    va_end(arg_ptr);

    Puts(x, y, buffer);
}

NS_FW_GFX_END
