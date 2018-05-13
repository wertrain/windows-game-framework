/**
 * @file TextWriter.cpp
 * @brief �e�N�X�`���ɔC�ӂ̃t�H���g�̕������`�悷��N���X
 */
#include "Precompiled.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <Shlwapi.h>
#include <string>
#include <vector>
#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/Constants.h>
#include <nnfw/gfx/Texture.h>

#include <nnfw/gfx/TextWriter.h>


NS_FW_GFX_BEGIN

// �w��̈�̃r�b�g�}�b�v�ۑ�
BOOL funcSaveRect(LPCTSTR lpFname, HDC hDC, LONG cx, LONG cy, LONG sx, LONG sy)
{
    HANDLE hFile = CreateFile(lpFname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        LONG    lHeadSize = (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO));
        LONG    lWidthSize = (sx * sizeof(DWORD));
        LONG    lImageSize = (lWidthSize * sy);
        DWORD   dwSize;

        // BITMAPFILEHEADER�̏�����
        BITMAPFILEHEADER bmpHead = { 0 };
        bmpHead.bfType = 0x4D42;       // ���ʎq(BM)
        bmpHead.bfSize = lHeadSize + lImageSize;
        bmpHead.bfReserved1 = 0;
        bmpHead.bfReserved2 = 0;
        bmpHead.bfOffBits = lHeadSize;

        // BITMAPINFO�̏�����
        BITMAPINFO bmpInfo = { 0 };
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = sx;
        bmpInfo.bmiHeader.biHeight = sy;
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = 32;
        bmpInfo.bmiHeader.biCompression = BI_RGB;
        bmpInfo.bmiHeader.biSizeImage = 0;
        bmpInfo.bmiHeader.biXPelsPerMeter = 0;
        bmpInfo.bmiHeader.biYPelsPerMeter = 0;
        bmpInfo.bmiHeader.biClrUsed = 0;
        bmpInfo.bmiHeader.biClrImportant = 0;

        // DIB�Z�N�V�����̍쐬
        LPDWORD     lpPixel;    // �s�N�Z���z��
        HBITMAP     hBitmap;    // �r�b�g�}�b�v
        HDC         hSaveDC;    // �ۑ��X�N���[��
        hBitmap = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, (LPVOID*)&lpPixel, NULL, 0);
        hSaveDC = CreateCompatibleDC(hDC);
        SelectObject(hSaveDC, hBitmap);

        // �ۑ��̈�̃R�s�[
        BitBlt(hSaveDC, 0, 0, sx, sy, hDC, cx, cy, SRCCOPY);

        // �t�@�C���ɏ�������
        WriteFile(hFile, &bmpHead, sizeof(BITMAPFILEHEADER), &dwSize, NULL);
        WriteFile(hFile, &bmpInfo, sizeof(BITMAPINFO), &dwSize, NULL);
        WriteFile(hFile, lpPixel, lImageSize, &dwSize, NULL);

        // DIB�Z�N�V�����̔j��
        DeleteDC(hSaveDC);
        DeleteObject(hBitmap);
        CloseHandle(hFile);
        return TRUE;
    }
    return FALSE;
}

VOID funcSaveFile(HWND hWnd, LPCTSTR lpFname)
{
    HDC hDC;

    hDC = GetDC(hWnd);
    funcSaveRect(lpFname, hDC, 0, 0, 256, 256);
    ReleaseDC(hWnd, hDC);
}

VOID funcSaveFile(HDC hDC, LPCTSTR lpFname)
{
    funcSaveRect(lpFname, hDC, 0, 0, 256, 256);
}

TextWriter::TextWriter()
    : GraphicsBase()
    , mFontFileName(std::wstring())
    , mFontHandle(nullptr)
{

}

TextWriter::~TextWriter()
{

}

bool TextWriter::Create(const wchar_t* font_ttf_file, const wchar_t* face_name, const int32_t font_size)
{
    // �t�H���g���g����悤�ɂ���
    if (0 == AddFontResourceEx(
        font_ttf_file, // ttf�t�@�C���ւ̃p�X
        FR_PRIVATE,
        &mDesign
    ))
    {
        return false;
    }
    mFontFileName = font_ttf_file;

    // �t�H���g�̐���
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
    mFontHandle = CreateFontIndirect(&lf);

    assert(mFontHandle);

    return mFontHandle != NULL;
}

void TextWriter::Destroy()
{
    // ���\�[�X�폜
    RemoveFontResourceEx(
        mFontFileName.c_str(),
        FR_PRIVATE,
        &mDesign
    );

    if (mFontHandle != nullptr)
    {
        DeleteObject(mFontHandle);
        mFontHandle = nullptr;
    }
}

bool TextWriter::WriteChar(const wchar_t c, ID3D11Device* device, ID3D11DeviceContext* context, Texture* texture)
{
    HDC hdc = GetDC(NULL);
    HFONT oldFont = (HFONT)SelectObject(hdc, mFontHandle);
    UINT code = static_cast<UINT>(c); // �e�N�X�`���ɏ������ޕ���

    // �t�H���g�r�b�g�}�b�v�擾
    TEXTMETRIC TM;
    GetTextMetrics(hdc, &TM);
    GLYPHMETRICS GM;
    CONST MAT2 Mat = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };
    DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
    fw::unique_ptr<BYTE> ptr(new BYTE[size]);
    if (GDI_ERROR == GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr.get(), &Mat))
    {
        return false;
    }

    // �f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���̊J��
    SelectObject(hdc, oldFont);
    DeleteObject(mFontHandle);
    ReleaseDC(NULL, hdc);

    // CPU�ŏ������݂��ł���e�N�X�`�����쐬
    D3D11_TEXTURE2D_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Width = GM.gmCellIncX;
    desc.Height = TM.tmHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    // RGBA(255,255,255,255)�^�C�v
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    // ���I�i�������݂��邽�߂̕K�{�����j
    desc.Usage = D3D11_USAGE_DYNAMIC;
    // �V�F�[�_���\�[�X�Ƃ��Ďg��	
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    // CPU����A�N�Z�X���ď�������OK
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ID3D11Texture2D* texture2d;
    HRESULT hr = device->CreateTexture2D(&desc, 0, &texture2d);
    assert(SUCCEEDED(hr));

    // �t�H���g�����e�N�X�`���ɏ������ޕ���
    D3D11_MAPPED_SUBRESOURCE hMappedResource;
    hr = context->Map(
        texture2d,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &hMappedResource);
    assert(SUCCEEDED(hr));

    BYTE* pBits = (BYTE*)hMappedResource.pData;
    // �t�H���g���̏�������
    // iOfs_x, iOfs_y : �����o���ʒu(����)
    // iBmp_w, iBmp_h : �t�H���g�r�b�g�}�b�v�̕���
    // Level : ���l�̒i�K (GGO_GRAY4_BITMAP�Ȃ̂�17�i�K)
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
            Alpha = (255 * ptr.get()[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
            Color = 0x00ffffff | (Alpha << 24);

            memcpy((BYTE*)pBits + hMappedResource.RowPitch * y + 4 * x, &Color, sizeof(DWORD));
        }
    }

    context->Unmap(texture2d, 0);

    if (!texture->Create(device, static_cast<ID3D11Resource*>(texture2d)))
    {
        return false;
    }

    return SUCCEEDED(hr);
}

bool TextWriter::WriteText(const wchar_t* text, ID3D11Device* device, ID3D11DeviceContext* context, Texture* texture)
{
    HDC hdc = GetDC(NULL);
    HFONT oldFont = (HFONT)SelectObject(hdc, mFontHandle);

    int textureWidth = 0;
    int textureHeight = 0;

    const size_t textLength = wcslen(text);
    fw::unique_ptr<TEXTMETRIC> tmArray(new TEXTMETRIC[textLength]);
    fw::unique_ptr<GLYPHMETRICS> gmArray(new GLYPHMETRICS[textLength]);
    fw::vector<std::unique_ptr<BYTE>> ptrVector;
    for (int32_t index = 0; index < textLength; ++index)
    {
        UINT code = (UINT)text[index];
        
        TEXTMETRIC* TM = &tmArray.get()[index];
        GLYPHMETRICS* GM = &gmArray.get()[index];

        GetTextMetrics(hdc, TM);
        CONST MAT2 Mat = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };
        DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, GM, 0, NULL, &Mat);
        std::unique_ptr<BYTE> ptr(new BYTE[size]);
        GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, GM, size, ptr.get(), &Mat);

        textureWidth += GM->gmCellIncX;
        textureHeight = max(textureHeight, TM->tmHeight);

        ptrVector.push_back(std::move(ptr));
    }
    
    // �f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���̊J��
    SelectObject(hdc, oldFont);
    //DeleteObject(mFontHandle);
    ReleaseDC(NULL, hdc);

    // CPU�ŏ������݂��ł���e�N�X�`�����쐬
    D3D11_TEXTURE2D_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Width = textureWidth;
    desc.Height = textureHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    // RGBA(255,255,255,255)�^�C�v
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    // ���I�i�������݂��邽�߂̕K�{�����j
    desc.Usage = D3D11_USAGE_DYNAMIC;
    // �V�F�[�_���\�[�X�Ƃ��Ďg��	
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    // CPU����A�N�Z�X���ď�������OK
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ID3D11Texture2D* texture2d;
    HRESULT hr = device->CreateTexture2D(&desc, 0, &texture2d);
    assert(SUCCEEDED(hr));

    // �t�H���g�����e�N�X�`���ɏ������ޕ���
    D3D11_MAPPED_SUBRESOURCE hMappedResource;
    hr = context->Map(
        texture2d,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &hMappedResource);
    assert(SUCCEEDED(hr));

    BYTE* pBits = (BYTE*)hMappedResource.pData;

    int x_Offset = 0;
    for (int32_t index = 0; index < textLength; ++index)
    {
        TEXTMETRIC* TM = &tmArray.get()[index];
        GLYPHMETRICS* GM = &gmArray.get()[index];


        // �t�H���g���̏�������
        // iOfs_x, iOfs_y : �����o���ʒu(����)
        // iBmp_w, iBmp_h : �t�H���g�r�b�g�}�b�v�̕���
        // Level : ���l�̒i�K (GGO_GRAY4_BITMAP�Ȃ̂�17�i�K)
        int iOfs_x = GM->gmptGlyphOrigin.x + x_Offset;
        int iOfs_y = TM->tmAscent - GM->gmptGlyphOrigin.y;
        int iBmp_w = GM->gmBlackBoxX + (4 - (GM->gmBlackBoxX % 4)) % 4;
        int iBmp_h = GM->gmBlackBoxY;
        int Level = 17;
        int x, y;
        DWORD Alpha, Color;
        //memset(pBits, 0, hMappedResource.RowPitch * TM->tmHeight);
        for (y = iOfs_y; y < iOfs_y + iBmp_h; y++)
        {
            for (x = iOfs_x; x < iOfs_x + iBmp_w; x++)
            {
                Alpha = (255 * ptrVector[index].get()[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
                Color = 0x00ffffff | (Alpha << 24);

                memcpy((BYTE*)pBits + hMappedResource.RowPitch * y + 4 * x, &Color, sizeof(DWORD));
            }
        }
        x_Offset += iBmp_w;
    }

    context->Unmap(texture2d, 0);

    if (!texture->Create(device, static_cast<ID3D11Resource*>(texture2d)))
    {
        return false;
    }

    return SUCCEEDED(hr);
}

bool TextWriter::WriteText(ID3D11Device* device, ID3D11DeviceContext* context)
{
    return false;
}

void TextWriter::Render(ID3D11DeviceContext* context)
{

}

NS_FW_GFX_END
