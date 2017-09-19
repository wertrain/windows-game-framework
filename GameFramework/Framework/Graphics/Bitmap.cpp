#include "Bitmap.h"
#pragma comment (lib, "msimg32.lib")

namespace Framework {
namespace Graphics {

Bitmap::Bitmap()
    : mHBitmap(NULL)
    , mHMemDC(NULL)
    , mTransparentColor(0)
{

}

Bitmap::~Bitmap()
{

}

bool Bitmap::CreateFromFile(LPCTSTR filename)
{
    // 画像読み込み
    mHBitmap = static_cast<HBITMAP>(LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

    if(mHBitmap == NULL)
    {
        return false;
    }
    else
    {
        // ビットマップの情報を取得
        GetObject(mHBitmap, sizeof(BITMAP), &mBitmap);
        // メモリオブジェクトにコピー
        mHMemDC = CreateCompatibleDC(NULL);
        SelectObject(mHMemDC ,mHBitmap);
    }
    // デフォルトでは緑を透過
    mTransparentColor = RGB(0,255,0);

    return true;
}

void Bitmap::Draw(const HDC hdc, const int x, const int y)
{
    // 画像のcolorを透過してコピーする
    TransparentBlt(hdc, x, y, mBitmap.bmWidth, mBitmap.bmHeight,
        mHMemDC, 0, 0, mBitmap.bmWidth, mBitmap.bmHeight,
        mTransparentColor);
}

void Bitmap::Destroy()
{
    if(mHMemDC)
    {
        DeleteDC(mHMemDC);
        mHMemDC = NULL;
    }
    if(mHBitmap)
    {
        DeleteObject(mHBitmap);
        mHBitmap = NULL;
    }
}

void Bitmap::SetTransparentColor(const int color)
{
    mTransparentColor = color;
}

} // namespace Graphics
} // namespace Framework