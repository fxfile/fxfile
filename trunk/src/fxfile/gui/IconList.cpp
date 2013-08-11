//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "IconList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GAP      4
#define GAP_HALF (GAP/2)

IconList::IconList(void)
    : mLargeImgList(XPR_NULL), mSmallImgList(XPR_NULL)
    , mOffset(0)
{
}

IconList::~IconList(void)
{
    XPR_SAFE_DELETE(mLargeImgList);
    XPR_SAFE_DELETE(mSmallImgList);
}

BEGIN_MESSAGE_MAP(IconList, CListBox)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t IconList::PreCreateWindow(CREATESTRUCT &aCreateStruct)
{
    return CListBox::PreCreateWindow(aCreateStruct);
}

void IconList::OnDestroy(void)
{
    CListBox::OnDestroy();

    if (XPR_IS_NOT_NULL(mLargeImgList)) mLargeImgList->DeleteImageList();
    if (XPR_IS_NOT_NULL(mSmallImgList)) mSmallImgList->DeleteImageList();
}

void IconList::PreSubclassWindow(void) 
{
    DWORD sStyle = LBS_NOTIFY | LBS_MULTICOLUMN | LBS_NOINTEGRALHEIGHT;
    ModifyStyle(0, sStyle);

    CListBox::PreSubclassWindow();
}

void IconList::insertIcons(void)
{
    xpr_sint_t sWidth  = GAP * 2;
    xpr_sint_t sHeight = GAP * 2;

    if (XPR_IS_NOT_NULL(mSmallImgList))
    {
        sWidth  += 16;
        sHeight += 16;
    }
    if (XPR_IS_NOT_NULL(mLargeImgList))
    {
        sWidth  += 32;
        sHeight += 32;
    }
    if (XPR_IS_NOT_NULL(mLargeImgList) && XPR_IS_NOT_NULL(mSmallImgList))
    {
        sWidth  += GAP_HALF;
        sHeight -= 16;
    }

    SetColumnWidth(sWidth);
    SetItemHeight(-1, sHeight);

    xpr_sint_t i;
    xpr_sint_t sCount = 0;

    if (XPR_IS_NOT_NULL(mLargeImgList) && XPR_IS_NOT_NULL(mLargeImgList->m_hImageList))
    {
        sCount = mLargeImgList->GetImageCount();
    }
    else if (XPR_IS_NOT_NULL(mSmallImgList) && XPR_IS_NOT_NULL(mSmallImgList->m_hImageList))
    {
        sCount = mSmallImgList->GetImageCount();
    }

    sCount -= mOffset;
    for (i = 0; i < sCount; ++i)
        AddString(XPR_STRING_LITERAL(""));
}

void IconList::setIconOffset(xpr_sint_t aOffset)
{
    mOffset = aOffset;
}

void IconList::setImageList(CImageList *aLargeImgList, CImageList *aSmallImgList)
{
    if (XPR_IS_NULL(aLargeImgList) && XPR_IS_NULL(aSmallImgList))
        return;

    if (XPR_IS_NOT_NULL(aLargeImgList))
    {
        mLargeImgList = new CImageList;
        HIMAGELIST sImageList = ImageList_Duplicate(aLargeImgList->m_hImageList);
        mLargeImgList->Attach(sImageList);
    }

    if (XPR_IS_NOT_NULL(aSmallImgList))
    {
        mSmallImgList = new CImageList;
        HIMAGELIST sImageList = ImageList_Duplicate(aSmallImgList->m_hImageList);
        mSmallImgList->Attach(sImageList);
    }

    insertIcons();
}

void IconList::setImageList(xpr_uint_t aLargeId, xpr_uint_t aSmallId)
{
    if (aLargeId <= 0 && aSmallId <= 0)
        return;

    if (aLargeId > 0)
    {
        CBitmap sBitmap;
        if (sBitmap.LoadBitmap(aLargeId) == XPR_TRUE)
        {
            mLargeImgList = new CImageList;
            if (XPR_IS_NULL(mLargeImgList))
                return;

            mLargeImgList->Create(32, 32, ILC_COLORDDB | ILC_MASK, -1, -1);
            mLargeImgList->Add(&sBitmap, RGB(255,0,255));
            sBitmap.DeleteObject();
        }
    }

    if (aSmallId > 0)
    {
        CBitmap sBitmap;
        if (sBitmap.LoadBitmap(aSmallId) == XPR_TRUE)
        {
            mSmallImgList = new CImageList;
            if (XPR_IS_NULL(mSmallImgList))
                return;

            mSmallImgList->Create(16, 16, ILC_COLORDDB | ILC_MASK, -1, -1);
            mSmallImgList->Add(&sBitmap, RGB(255,0,255));
            sBitmap.DeleteObject();
        }
    }

    insertIcons();
}

void IconList::DrawItem(LPDRAWITEMSTRUCT aDrawItemStruct)
{
    CDC *sDC = CDC::FromHandle(aDrawItemStruct->hDC);
    xpr_sint_t sItem  = aDrawItemStruct->itemID;
    xpr_sint_t sImage = sItem + mOffset;
    CRect sItemRect(aDrawItemStruct->rcItem);

    sDC->FillSolidRect(sItemRect, ::GetSysColor(COLOR_WINDOW));

    if ((aDrawItemStruct->itemState & ODS_SELECTED))// && (aDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
        sDC->FillSolidRect(sItemRect, ::GetSysColor(COLOR_HIGHLIGHT));

    CPoint sPoint(sItemRect.TopLeft());
    sPoint.y += GAP;

    HICON sIcon;
    if (XPR_IS_NOT_NULL(mLargeImgList) && XPR_IS_NOT_NULL(mLargeImgList->m_hImageList))
    {
        sPoint.x += GAP;

        if (sImage < mLargeImgList->GetImageCount())
        {
            sIcon = mLargeImgList->ExtractIcon(sImage);
            if (XPR_IS_NOT_NULL(sIcon))
            {
                ::DrawIconEx(sDC->m_hDC, sPoint.x, sPoint.y, sIcon, 32, 32, 0, XPR_NULL, DI_NORMAL);
                ::DestroyIcon(sIcon);
            }
        }

        sPoint.x += 32;
    }

    if (XPR_IS_NOT_NULL(mSmallImgList) && XPR_IS_NOT_NULL(mSmallImgList->m_hImageList))
    {
        sPoint.x += XPR_IS_NOT_NULL(mLargeImgList) ? GAP_HALF : GAP;
        sPoint.y += XPR_IS_NOT_NULL(mLargeImgList) ? 16 : 0;

        if (sImage < mSmallImgList->GetImageCount())
        {
            sIcon = mSmallImgList->ExtractIcon(sImage);
            if (XPR_IS_NOT_NULL(sIcon))
            {
                ::DrawIconEx(sDC->m_hDC, sPoint.x, sPoint.y, sIcon, 16, 16, 0, XPR_NULL, DI_NORMAL);
                ::DestroyIcon(sIcon);
            }
        }
    }
}
