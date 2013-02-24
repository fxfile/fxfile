//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_thumbnail.h"

#include "gfl/libgfl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
struct Thumbnail::ThumbItem
{
    HWND         mHwnd;
    xpr_uint_t   mMsg;
    xpr_uint_t   mCode;
    std::tstring mPath;
    xpr_bool_t   mLoadByExt;
    CSize        mThumbSize;
};

Thumbnail::Thumbnail(void)
    : mThumbSize(CSize(100, 100))
    , mEvent(XPR_NULL)
    , mThumbImgList(XPR_NULL)
    , mIdMgr(0)
{
}

Thumbnail::~Thumbnail(void)
{
    destroy();
}

void Thumbnail::create(void)
{
    if (XPR_IS_NOT_NULL(mThumbImgList))
        return;

    mThumbImgList = new ThumbImgList;
    mThumbImgList->create(mThumbSize.cx, mThumbSize.cy, ILC_COLOR24);
}

void Thumbnail::destroy(void)
{
    Stop();

    clearAysncImage();

    CLOSE_HANDLE(mEvent);

    XPR_SAFE_DELETE(mThumbImgList);

    mIdMgr = 0;
}

xpr_bool_t Thumbnail::loadCache(void)
{
    if (XPR_IS_NOT_NULL(mThumbImgList))
        return XPR_FALSE;

    ThumbImgList *sThumbImgList = new ThumbImgList;
    if (XPR_IS_NULL(sThumbImgList))
        return XPR_FALSE;

    if (sThumbImgList->load() == XPR_FALSE)
    {
        XPR_SAFE_DELETE(sThumbImgList);
        return XPR_FALSE;
    }

    xpr_sint_t i;
    xpr_sint_t sCount;
    ThumbImgList::ThumbElement *sThumbElement;

    sCount = sThumbImgList->getImageCount();
    for (i = 0; i < sCount; ++i)
    {
        sThumbElement = sThumbImgList->getImage(i);
        if (XPR_IS_NOT_NULL(sThumbElement))
            mIdMap[sThumbElement->mThumbImageId] = sThumbElement->mImageIndex;
    }

    if (sCount > 0)
    {
        sThumbElement = sThumbImgList->getImage(sCount - 1);
        if (XPR_IS_NOT_NULL(sThumbElement))
            mIdMgr = sThumbElement->mThumbImageId + 1;
    }

    mThumbImgList = sThumbImgList;

    return XPR_TRUE;
}

void Thumbnail::saveCache(void)
{
    if (XPR_IS_NULL(mThumbImgList))
        return;

    mThumbImgList->save();
}

ThumbImgList *Thumbnail::getThumbImgList(void) const
{
    return mThumbImgList;
}

CSize Thumbnail::getThumbSize(void)
{
    return mThumbSize;
}

void Thumbnail::setThumbSize(CSize aThumbSize)
{
    if (XPR_IS_NOT_NULL(mThumbImgList) && (aThumbSize != mThumbSize))
    {
        mThumbImgList->removeAll();
        delete mThumbImgList;

        mThumbImgList = new ThumbImgList;
        mThumbImgList->create(aThumbSize.cx, aThumbSize.cy, ILC_COLOR24);
    }

    mThumbSize = aThumbSize;
}

void Thumbnail::setThumbPriority(xpr_sint_t aPriority)
{
    switch (aPriority)
    {
    case THUMBNAIL_PRIORITY_BELOW_NORMAL: aPriority = THREAD_PRIORITY_BELOW_NORMAL; break;
    case THUMBNAIL_PRIORITY_ABOVE_NORMAL: aPriority = THREAD_PRIORITY_ABOVE_NORMAL; break;

    case THUMBNAIL_PRIORITY_NORMAL:
    default:                              aPriority = THREAD_PRIORITY_NORMAL;       break;
    }

    SetPriority(aPriority);
}

void Thumbnail::verifyThumb(void)
{
    if (XPR_IS_NOT_NULL(mThumbImgList))
        mThumbImgList->verify();
}

void Thumbnail::initThumb(void)
{
    if (XPR_IS_NOT_NULL(mThumbImgList))
    {
        mThumbImgList->clear();
        mThumbImgList->create(mThumbSize.cx, mThumbSize.cy, ILC_COLOR24);
    }
}

xpr_uint_t Thumbnail::addImage(ThumbImage *aThumbImage)
{
    if (XPR_IS_NULL(aThumbImage))
        return -1;

    IdMap::iterator sIterator;
    xpr_uint_t sThumbImageId = InvalidThumbImageId;

    while (true)
    {
        sIterator = mIdMap.find(mIdMgr);
        if (sIterator != mIdMap.end())
        {
            mIdMgr++;
            if (mIdMgr == InvalidThumbImageId)
                mIdMgr++;
        }
        else
        {
            sThumbImageId = mIdMgr;
            break;
        }
    }

    xpr_sint_t sImageIndex = mThumbImgList->add(aThumbImage, sThumbImageId);
    if (sImageIndex < 0)
        return InvalidThumbImageId;

    mIdMap[mIdMgr] = sImageIndex;

    mIdMgr++;
    if (mIdMgr == InvalidThumbImageId)
        mIdMgr++;

    return sThumbImageId;
}

xpr_bool_t Thumbnail::getImageId(const xpr_tchar_t *aPath, xpr_uint_t &aThumbImageId)
{
    aThumbImageId = InvalidThumbImageId;

    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    ThumbImgList::ThumbElement *sThumbElement = mThumbImgList->getImage(aPath);
    if (XPR_IS_NULL(sThumbElement))
        return XPR_FALSE;

    aThumbImageId = sThumbElement->mThumbImageId;

    xpr_bool_t sMatched = XPR_TRUE;

    FILETIME sModifiedFileTime = {0};
    if (GetFileLastWriteTime(aPath, &sModifiedFileTime) == XPR_FALSE)
        sMatched = XPR_FALSE;
    else
    {
        xpr_sint_t sCompare = mThumbImgList->compareTime(sThumbElement->mImageIndex, &sModifiedFileTime);
        if (sCompare == -1 || sCompare == 1)
            sMatched = XPR_FALSE;
    }

    if (sMatched == XPR_FALSE)
    {
        removeImageId(aThumbImageId);
        aThumbImageId = InvalidThumbImageId;
    }

    return XPR_TRUE;
}

xpr_bool_t Thumbnail::getAsyncImage(HWND aHwnd, xpr_uint_t aMsg, xpr_uint_t aCode, const xpr_tchar_t *aPath, xpr_bool_t aLoadByExt)
{
    if (XPR_IS_NULL(aPath))
        return -1;

    ThumbItem *sThumbItem = new ThumbItem;
    sThumbItem->mHwnd      = aHwnd;
    sThumbItem->mMsg       = aMsg;
    sThumbItem->mCode      = aCode;
    sThumbItem->mPath      = aPath;
    sThumbItem->mLoadByExt = aLoadByExt;
    sThumbItem->mThumbSize = mThumbSize;

    {
        CsLocker sLocker(mCs);

        mThumbDeque.push_front(sThumbItem);
        ::SetEvent(mEvent);
    }

    return XPR_TRUE;
}

void Thumbnail::drawImageId(CDC *aDC, xpr_uint_t aThumbImageId, POINT aPoint, xpr_uint_t aStyle)
{
    if (XPR_IS_NULL(mThumbImgList))
        return;

    xpr_sint_t sImageIndex = IdToImage(aThumbImageId);
    if (sImageIndex < 0)
        return;

    mThumbImgList->draw(aDC, sImageIndex, aPoint, aStyle);
}

void Thumbnail::removeImageId(xpr_uint_t aThumbImageId)
{
    if (XPR_IS_NULL(mThumbImgList))
        return;

    IdMap::iterator sIterator = mIdMap.find(aThumbImageId);
    if (sIterator == mIdMap.end())
        return;

    xpr_sint_t sImageIndex = sIterator->second;

    mIdMap.erase(sIterator);

    mThumbImgList->remove(sImageIndex);
}

xpr_sint_t Thumbnail::IdToImage(xpr_uint_t aThumbImageId)
{
    IdMap::iterator sIterator = mIdMap.find(aThumbImageId);
    if (sIterator == mIdMap.end())
        return -1;

    return sIterator->second;
}

xpr_bool_t Thumbnail::OnPreEntry()
{
    CLOSE_HANDLE(mEvent);

    mEvent = ::CreateEvent(XPR_NULL, XPR_TRUE ,XPR_FALSE, XPR_NULL);

    return XPR_TRUE;
}

void GetDIBFromBitmap(GFL_BITMAP *bitmap, BITMAPINFOHEADER *bitmap_info, xpr_uchar_t **data)
{
    register xpr_sint_t i, j, bytes_per_line; 
    register xpr_uchar_t *ptr_src, *ptr_dst; 

    *data = XPR_NULL; 

    memset(bitmap_info, 0, sizeof(BITMAPINFOHEADER)); 

    bitmap_info->biSize   = sizeof(BITMAPINFOHEADER); 
    bitmap_info->biWidth  = bitmap->Width; 
    bitmap_info->biHeight = bitmap->Height; 
    bitmap_info->biPlanes = 1; 

    bytes_per_line = (bitmap->Width * 3 + 3) & ~3;
    bitmap_info->biClrUsed      = 0;
    bitmap_info->biBitCount     = 24; 
    bitmap_info->biCompression  = BI_RGB; 
    bitmap_info->biSizeImage    = bytes_per_line * bitmap->Height; 
    bitmap_info->biClrImportant = 0; 

    if (bitmap->Type != GFL_BINARY &&
        bitmap->Type != GFL_GREY   &&
        bitmap->Type != GFL_COLORS  )
        return; 

    *data = (xpr_uchar_t *)malloc(bitmap_info->biSizeImage);

    if ((bitmap->Type == GFL_COLORS || bitmap->Type == GFL_GREY) && (bitmap->TransparentIndex >= 0))
    {
        COLORREF clrWindow = GetSysColor(COLOR_WINDOW);
        bitmap->ColorMap->Blue[bitmap->TransparentIndex]  = GetBValue(clrWindow);
        bitmap->ColorMap->Green[bitmap->TransparentIndex] = GetGValue(clrWindow);
        bitmap->ColorMap->Red[bitmap->TransparentIndex]   = GetRValue(clrWindow);
    }

    for (i = 0; i < bitmap->Height; ++i)
    {
        ptr_src = bitmap->Data + i * bitmap->BytesPerLine; 
        ptr_dst = *data + i * bytes_per_line; 

        if (bitmap->Type == GFL_BINARY || bitmap->Type == GFL_GREY)
        {
            for (j = 0; j < bitmap->Width; ++j)
            {
                *ptr_dst++ = *ptr_src; 
                *ptr_dst++ = *ptr_src; 
                *ptr_dst++ = *ptr_src++; 
            }
        }
        else
        {
            if (bitmap->Type == GFL_COLORS)
            {
                for (j = 0; j < bitmap->Width; ++j)
                {
                    *ptr_dst++ = bitmap->ColorMap->Blue[*ptr_src]; 
                    *ptr_dst++ = bitmap->ColorMap->Green[*ptr_src]; 
                    *ptr_dst++ = bitmap->ColorMap->Red[*ptr_src++]; 
                }
            }
            else
            {
                for (j = 0; j < bitmap->Width; ++j)
                {
                    *ptr_dst++ = ptr_src[2]; 
                    *ptr_dst++ = ptr_src[1]; 
                    *ptr_dst++ = ptr_src[0]; 
                    ptr_src += bitmap->BytesPerPixel; 
                }
            }
        }
    }
}

unsigned Thumbnail::OnEntryProc(void)
{
    HWND sHwnd;
    xpr_uint_t sMsg;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    xpr_char_t sPathA[XPR_MAX_PATH + 1];
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;
    CSize sThumbSize;
    ThumbItem *sThumbItem = XPR_NULL;

    GFL_BITMAP *sGflBitmap = XPR_NULL;
    GFL_LOAD_PARAMS sGflLoadParams;
    xpr_uchar_t *sDibData = XPR_NULL;
    BITMAPINFOHEADER sBitmapInfoHeader;
    CBitmap *sBitmap, *sOldBitmap;
    ThumbImage *sThumbImage;

    CSize sPicSize;
    xpr_sint_t x, y, cx, cy;
    xpr_double_t sRatio;
    CRect sDestRect;

    DWORD sWait;
    HANDLE sEvents[2] = { mStopThread, mEvent };

    while (true)
    {
        sWait = ::WaitForMultipleObjects(2, sEvents, XPR_FALSE, INFINITE);
        if (sWait == WAIT_OBJECT_0)
            break;

        if (sWait != WAIT_OBJECT_0+1)
            continue;

        {
            CsLocker sLocker(mCs);

            if (mThumbDeque.empty() == true)
            {
                ::ResetEvent(mEvent);
                continue;
            }

            sThumbItem = mThumbDeque.front();
            mThumbDeque.pop_front();

            if (XPR_IS_NULL(sThumbItem))
                continue;
        }

        sThumbImage = XPR_NULL;

        sHwnd = sThumbItem->mHwnd;
        sMsg = sThumbItem->mMsg;
        sThumbSize = sThumbItem->mThumbSize;

        _tcscpy(sPath, sThumbItem->mPath.c_str());
        sInputBytes = _tcslen(sPath) * sizeof(xpr_tchar_t);
        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
        XPR_TCS_TO_MBS(sPath, &sInputBytes, sPathA, &sOutputBytes);
        sPathA[sOutputBytes / sizeof(xpr_char_t)] = 0;

        XPR_TRACE(XPR_STRING_LITERAL("Thumbnail Thread, path=%s\n"), sPath);

        {
            gflGetDefaultPreviewParams(&sGflLoadParams); 
            sGflLoadParams.Flags      |= GFL_LOAD_SKIP_ALPHA | GFL_LOAD_PREVIEW_NO_CANVAS_RESIZE;
            sGflLoadParams.Origin      = GFL_BOTTOM_LEFT;
            sGflLoadParams.ColorModel  = GFL_BGR; 
            sGflLoadParams.LinePadding = 4;

            if (sThumbItem->mLoadByExt == XPR_TRUE)
                sGflLoadParams.Flags |= GFL_LOAD_BY_EXTENSION_ONLY;

            if (gflLoadPreview(sPathA, sThumbSize.cx, sThumbSize.cy, &sGflBitmap, &sGflLoadParams, XPR_NULL) == XPR_FALSE)
            {
                // image bilinear scale
                sPicSize.cx = sGflBitmap->Width;
                sPicSize.cy = sGflBitmap->Height;
                x = y = 0;
                cx = sThumbSize.cx;
                cy = sThumbSize.cy;
                sRatio = (xpr_double_t)sPicSize.cx / (xpr_double_t)sPicSize.cy;

                if (sPicSize.cx >= sThumbSize.cx || sPicSize.cy >= sThumbSize.cy)
                {
                    x = (sThumbSize.cx - sPicSize.cx) / 2;
                    y = (sThumbSize.cy - sPicSize.cy) / 2;
                    sDestRect = CRect(x, y, x+cx, y+cy);
                }
                else
                {
                    x = (sThumbSize.cx - sPicSize.cx) / 2;
                    y = (sThumbSize.cy - sPicSize.cy) / 2;
                    sDestRect = CRect(x, y, x+sPicSize.cx, y+sPicSize.cy);
                }

                CClientDC dc(CWnd::GetDesktopWindow());

                sBitmap = new CBitmap;
                sBitmap->CreateCompatibleBitmap(&dc, sThumbSize.cx, sThumbSize.cy);

                CDC sMemDC;
                sMemDC.CreateCompatibleDC(&dc);
                sOldBitmap = sMemDC.SelectObject(sBitmap);

                sDibData = XPR_NULL;
                GetDIBFromBitmap(sGflBitmap, &sBitmapInfoHeader, &sDibData);

                sMemDC.FillSolidRect(0, 0, sThumbSize.cx, sThumbSize.cy, RGB(255,255,255));
                ::StretchDIBits(
                    sMemDC.m_hDC,
                    x,
                    y,
                    sGflBitmap->Width,
                    sGflBitmap->Height,
                    0,
                    0,
                    sGflBitmap->Width,
                    sGflBitmap->Height,
                    (sDibData != XPR_NULL) ? sDibData : sGflBitmap->Data,
                    (BITMAPINFO *)&sBitmapInfoHeader,
                    DIB_RGB_COLORS,
                    SRCCOPY);

                sMemDC.SelectObject(sOldBitmap);

                sThumbImage = new ThumbImage;
                sThumbImage->mPath   = sThumbItem->mPath;
                sThumbImage->mImage  = sBitmap;
                sThumbImage->mWidth  = sGflBitmap->Width;
                sThumbImage->mHeight = sGflBitmap->Height;
                sThumbImage->mDepth  = sGflBitmap->BytesPerPixel * 8;
                GetFileLastWriteTime(sThumbItem->mPath.c_str(), &sThumbImage->mModifiedFileTime);

                if (XPR_IS_NOT_NULL(sDibData)) { free(sDibData); sDibData = XPR_NULL; }
                if (XPR_IS_NOT_NULL(sGflBitmap)) { gflFreeBitmap(sGflBitmap); sGflBitmap = XPR_NULL; }
            }
        }

        if (XPR_IS_NOT_NULL(sThumbImage))
        {
            if (::PostMessage(sHwnd, sMsg, (WPARAM)sThumbItem->mCode, (LPARAM)sThumbImage) == XPR_FALSE)
            {
                if (XPR_IS_NOT_NULL(sThumbImage) && XPR_IS_NOT_NULL(sThumbImage->mImage))
                {
                    sThumbImage->mImage->DeleteObject();
                    XPR_SAFE_DELETE(sThumbImage->mImage);
                }

                XPR_SAFE_DELETE(sThumbImage);
            }
        }

        XPR_SAFE_DELETE(sThumbItem);
    }

    return 0;
}

void Thumbnail::clearAysncImage(void)
{
    if (XPR_IS_NOT_NULL(mThread))
    {
        CsLocker sLocker(mCs);

        ThumbItem *sThumbItem;
        ThumbDeque::iterator sIterator;

        sIterator = mThumbDeque.begin();
        for (; sIterator != mThumbDeque.end(); ++sIterator)
        {
            sThumbItem = *sIterator;
            XPR_SAFE_DELETE(sThumbItem);
        }

        mThumbDeque.clear();
    }
}
} // namespace fxb
