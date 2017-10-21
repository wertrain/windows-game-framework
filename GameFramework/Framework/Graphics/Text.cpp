#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <Shlwapi.h>
#include "../Common/Includes.h"
#include "../System/Includes.h"

#include "Text.h"

namespace Framework {
namespace Graphics {

Text::Text()
    : GraphicsBase()
    , m_FontHandle(nullptr)
    , m_Texture(nullptr)
{

}

Text::~Text()
{

}

bool Text::Create(const wchar_t* font_ttf_file, const wchar_t* face_name, const s32 font_size)
{
    // フォントを使えるようにする
    DESIGNVECTOR design;
    AddFontResourceEx(
        font_ttf_file, // ttfファイルへのパス
        FR_PRIVATE,
        &design
    );

    // フォントの生成
    LOGFONT lf = {
        font_size, 0, 0, 0, 0, 0, 0, 0,
        SHIFTJIS_CHARSET,
        OUT_TT_ONLY_PRECIS,
        CLIP_DEFAULT_PRECIS,
        PROOF_QUALITY,
        FIXED_PITCH | FF_MODERN,
        TEXT("")
    };
    wcscpy_s(lf.lfFaceName, face_name);
    m_FontHandle = CreateFontIndirect(&lf);

    // リソース削除
    RemoveFontResourceEx(
        font_ttf_file, 
        FR_PRIVATE,
        &design
    );

    return m_FontHandle != NULL;
}

void Text::Destroy()
{

}

bool Text::WriteText(ID3D11Device* device, ID3D11DeviceContext* context)
{
    HDC hdc = GetDC(NULL);
    HFONT oldFont = (HFONT)SelectObject(hdc, m_FontHandle);
    UINT code = (UINT)'A';     // テクスチャに書き込む文字

    // フォントビットマップ取得
    TEXTMETRIC TM;
    GetTextMetrics(hdc, &TM);
    GLYPHMETRICS GM;
    CONST MAT2 Mat = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };
    DWORD size = GetGlyphOutline(
        hdc,
        code,
        GGO_GRAY4_BITMAP,
        &GM,
        0,
        NULL,
        &Mat);
    BYTE* ptr = new BYTE[size];
    GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

    // デバイスコンテキストとフォントハンドルの開放
    SelectObject(hdc, oldFont);
    DeleteObject(m_FontHandle);
    ReleaseDC(NULL, hdc);

    // CPUで書き込みができるテクスチャを作成
    D3D11_TEXTURE2D_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Width = GM.gmCellIncX;
    desc.Height = TM.tmHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    // RGBA(255,255,255,255)タイプ
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    // 動的（書き込みするための必須条件）
    desc.Usage = D3D11_USAGE_DYNAMIC;
    // シェーダリソースとして使う	
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    // CPUからアクセスして書き込みOK
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->CreateTexture2D(&desc, 0, &m_Texture);

    // フォント情報をテクスチャに書き込む部分
    D3D11_MAPPED_SUBRESOURCE hMappedResource;
    hr = context->Map(
        m_Texture,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &hMappedResource);

    BYTE* pBits = (BYTE*)hMappedResource.pData;
    // フォント情報の書き込み
    // iOfs_x, iOfs_y : 書き出し位置(左上)
    // iBmp_w, iBmp_h : フォントビットマップの幅高
    // Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)
    int iOfs_x = GM.gmptGlyphOrigin.x;
    int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
    int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
    int iBmp_h = GM.gmBlackBoxY;
    int Level = 17;
    int x, y;
    DWORD Alpha, Color;
    memset(pBits, 0, hMappedResource.RowPitch * TM.tmHeight);
    for (y = iOfs_y; y < iOfs_y + iBmp_h; y++)
    {
        for (x = iOfs_x; x < iOfs_x + iBmp_w; x++)
        {
            Alpha =
                (255 * ptr[x - iOfs_x + iBmp_w*(y - iOfs_y)])
                / (Level - 1);
            Color = 0x00ffffff | (Alpha << 24);

            memcpy(
                (BYTE*)pBits
                + hMappedResource.RowPitch * y + 4 * x,
                &Color,
                sizeof(DWORD));
        }
    }
    context->Unmap(m_Texture, 0);

    return SUCCEEDED(hr);
}

void Text::Render(ID3D11DeviceContext* context)
{

}

} // namespace Graphics
} // namespace Framework
