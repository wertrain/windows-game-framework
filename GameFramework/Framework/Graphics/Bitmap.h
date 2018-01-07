#ifndef BITMAP_H_
#define BITMAP_H_

#include "Primitive/Primitive.h"
#include <Windows.h>

NS_FW_GFX_BEGIN

class Bitmap : public Primitive
{
public:
    Bitmap() ;
    virtual ~Bitmap();

    bool CreateFromFile(wchar_t* filename);
    void Draw(const HDC hdc, const int x, const int y);
    void Destroy();
    void SetTransparentColor(const int color);

protected:
    HBITMAP mHBitmap;
    BITMAP mBitmap;
    HDC mHMemDC;
    int mTransparentColor;
};

NS_FW_GFX_END

#endif // BITMAP_H_
