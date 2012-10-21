//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_bitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP aBitmap)
{
    BITMAP sBitmapHeader = {0};
    if (GetObject(aBitmap, sizeof(BITMAP), (xpr_char_t *)&sBitmapHeader) == XPR_FALSE)
        return XPR_NULL;

    WORD sColorBits = (WORD)(sBitmapHeader.bmPlanes * sBitmapHeader.bmBitsPixel);
    if (sColorBits == 1)       sColorBits = 1;
    else if (sColorBits <= 4)  sColorBits = 4;
    else if (sColorBits <= 8)  sColorBits = 8;
    else if (sColorBits <= 16) sColorBits = 16;
    else if (sColorBits <= 24) sColorBits = 24;
    else                       sColorBits = 32;

    PBITMAPINFO sBitmapInfo = XPR_NULL;

    if (sColorBits != 24)
        sBitmapInfo = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << sColorBits));
    else
        sBitmapInfo = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

    if (XPR_IS_NULL(sBitmapInfo))
        return XPR_NULL;

    sBitmapInfo->bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    sBitmapInfo->bmiHeader.biWidth        = sBitmapHeader.bmWidth;
    sBitmapInfo->bmiHeader.biHeight       = sBitmapHeader.bmHeight;
    sBitmapInfo->bmiHeader.biPlanes       = sBitmapHeader.bmPlanes;
    sBitmapInfo->bmiHeader.biBitCount     = sBitmapHeader.bmBitsPixel;
    sBitmapInfo->bmiHeader.biCompression  = BI_RGB;
    sBitmapInfo->bmiHeader.biSizeImage    = ((sBitmapInfo->bmiHeader.biWidth * sColorBits +31) & ~31) / 8 * sBitmapInfo->bmiHeader.biHeight;
    sBitmapInfo->bmiHeader.biClrImportant = 0;

    if (sColorBits < 24)
        sBitmapInfo->bmiHeader.biClrUsed = (1 << sColorBits);

    return sBitmapInfo;
}

xpr_bool_t WriteBitmapFile(xpr_tchar_t *aFile, PBITMAPINFO aBitmapInfo)
{
    if (XPR_IS_NULL(aFile) || XPR_IS_NULL(aBitmapInfo))
        return XPR_FALSE;

    PBITMAPINFOHEADER sBitmapInfoHeader = (PBITMAPINFOHEADER)aBitmapInfo;
    xpr_byte_t *sBits = (xpr_byte_t *)aBitmapInfo + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * sBitmapInfoHeader->biClrUsed);

    xpr_bool_t sResult = XPR_FALSE;

    {
        // Create the .BMP file.
        HANDLE sFile = ::CreateFile(
            aFile,
            GENERIC_READ | GENERIC_WRITE,
            (DWORD)0,
            XPR_NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            (HANDLE)XPR_NULL);

        sResult = (sFile != INVALID_HANDLE_VALUE);

        if (XPR_IS_TRUE(sResult))
        {
            DWORD sWrittenBytes;

            BITMAPFILEHEADER sBitmapFileHeader;
            sBitmapFileHeader.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"
            sBitmapFileHeader.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + sBitmapInfoHeader->biSize + sBitmapInfoHeader->biClrUsed * sizeof(RGBQUAD) + sBitmapInfoHeader->biSizeImage);
            sBitmapFileHeader.bfReserved1 = 0;
            sBitmapFileHeader.bfReserved2 = 0;
            sBitmapFileHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + sBitmapInfoHeader->biSize + sBitmapInfoHeader->biClrUsed * sizeof (RGBQUAD);

            // BITMAPFILEHEADER
            ::WriteFile(sFile, (LPVOID)&sBitmapFileHeader, sizeof(BITMAPFILEHEADER), (LPDWORD)&sWrittenBytes,  XPR_NULL);

            // BITMAPINFOHEADER
            ::WriteFile(sFile, (LPVOID) sBitmapInfoHeader, sizeof(BITMAPINFOHEADER) + sBitmapInfoHeader->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&sWrittenBytes, XPR_NULL);

            // bitmap image
            ::WriteFile(sFile, (xpr_char_t *)sBits, (xpr_sint_t)sBitmapInfoHeader->biSizeImage, (LPDWORD)&sWrittenBytes,XPR_NULL);

            ::CloseHandle(sFile);
        }
    }

    return sResult;
}

xpr_bool_t WriteBitmapFile(xpr_tchar_t *aFile, PBITMAPINFO aBitmapInfo, HBITMAP aBitmap, HDC aDC)
{
    if (XPR_IS_NULL(aFile) || XPR_IS_NULL(aBitmapInfo) || XPR_IS_NULL(aBitmap) || XPR_IS_NULL(aDC))
        return XPR_FALSE;

    PBITMAPINFOHEADER sBitmapInfoHeader = (PBITMAPINFOHEADER)aBitmapInfo;
    xpr_byte_t *sBits = (xpr_byte_t *)GlobalAlloc(GMEM_FIXED, sBitmapInfoHeader->biSizeImage);
    if (XPR_IS_NULL(sBits))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sCopiedLines = GetDIBits(aDC, aBitmap, 0, (WORD)sBitmapInfoHeader->biHeight, sBits, aBitmapInfo, DIB_RGB_COLORS);
    if (sCopiedLines != 0 && XPR_IS_NOT_NULL(sBits))
    {
        // Create the .BMP file.
        HANDLE sFile = ::CreateFile(
            aFile,
            GENERIC_READ | GENERIC_WRITE,
            (DWORD)0,
            XPR_NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            (HANDLE)XPR_NULL);

        sResult = (sFile != INVALID_HANDLE_VALUE) ? XPR_TRUE : XPR_FALSE;

        if (XPR_IS_TRUE(sResult))
        {
            DWORD sWrittenBytes;

            BITMAPFILEHEADER sBitmapFileHeader;
            sBitmapFileHeader.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"
            sBitmapFileHeader.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + sBitmapInfoHeader->biSize + sBitmapInfoHeader->biClrUsed * sizeof(RGBQUAD) + sBitmapInfoHeader->biSizeImage);
            sBitmapFileHeader.bfReserved1 = 0;
            sBitmapFileHeader.bfReserved2 = 0;
            sBitmapFileHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + sBitmapInfoHeader->biSize + sBitmapInfoHeader->biClrUsed * sizeof (RGBQUAD);

            // BITMAPFILEHEADER
            ::WriteFile(sFile, (LPVOID)&sBitmapFileHeader, sizeof(BITMAPFILEHEADER), (LPDWORD)&sWrittenBytes,  XPR_NULL);

            // BITMAPINFOHEADER
            ::WriteFile(sFile, (LPVOID)sBitmapInfoHeader, sizeof(BITMAPINFOHEADER) + sBitmapInfoHeader->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&sWrittenBytes, XPR_NULL);

            // bitmap image
            ::WriteFile(sFile, (xpr_char_t *)sBits, (xpr_sint_t)sBitmapInfoHeader->biSizeImage, (LPDWORD)&sWrittenBytes, XPR_NULL);

            ::CloseHandle(sFile);
        }
    }

    ::GlobalFree((HGLOBAL)sBits);

    return sResult;
}

xpr_bool_t WriteBitmapFile(xpr_tchar_t *aFile, HBITMAP aBitmap, HDC aDC)
{
    if (XPR_IS_NULL(aFile) || XPR_IS_NULL(aBitmap) || XPR_IS_NULL(aDC))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    PBITMAPINFO aBitmapInfo = CreateBitmapInfoStruct(aBitmap);
    if (XPR_IS_NOT_NULL(aBitmapInfo))
    {
        sResult = WriteBitmapFile(aFile, aBitmapInfo, aBitmap, aDC);

        ::LocalFree(aBitmapInfo);
    }

    return XPR_TRUE;
}

xpr_bool_t WriteBitmapFile(xpr_tchar_t *aFile, HDC aDC, LPCRECT aRect)
{
    if (XPR_IS_NULL(aFile) || XPR_IS_NULL(aDC) || XPR_IS_NULL(aRect))
        return XPR_FALSE;

    CDC *sDC = CDC::FromHandle(aDC);
    if (XPR_IS_NULL(sDC))
        return XPR_FALSE;

    CRect sRect(aRect);

    CDC sMemDC;
    sMemDC.CreateCompatibleDC(sDC);

    CBitmap sBitmap;
    sBitmap.CreateCompatibleBitmap(sDC, sRect.Width(), sRect.Height());

    CBitmap *sOldBitmap = sMemDC.SelectObject(&sBitmap);

    sMemDC.BitBlt(0, 0, sRect.Width(), sRect.Height(), sDC, 0, 0, SRCCOPY);

    WriteBitmapFile(aFile, sBitmap, aDC);

    sMemDC.SelectObject(sOldBitmap);

    return XPR_TRUE;
}
} // namespace fxb
