//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ContentsWnd.h"

#include "fxb/fxb_bookmark.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_sys_img_list.h"

#include "rgc/MemDC.h"

#include "Option.h"
#include "BookmarkSetWnd.h"
#include "CtrlId.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("ContentsWnd");

//
// control id
//
enum
{
    CTRL_ID_BOOKMARK_SET_WND = 50,
};

IMPLEMENT_DYNAMIC(ContentsWnd, CWnd)

ContentsWnd::ContentsWnd(void)
    : mMode(ModeNone)
    , mFolderIcon(XPR_NULL)
    , mTitleFont(XPR_NULL), mTextFont(XPR_NULL), mFileNameFont(XPR_NULL), mFullPathFont(XPR_NULL)
    , mBookmarkSetWnd(XPR_NULL)
    , mEnableBookmark(XPR_FALSE)
    , mVisibleBookmark(XPR_FALSE)
    , mBookmarkTop(0)
{
}

ContentsWnd::~ContentsWnd(void)
{
    DESTROY_ICON(mFolderIcon);
}

xpr_bool_t ContentsWnd::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;

    WNDCLASS sWndClass = {0};
    HINSTANCE sInstance = AfxGetInstanceHandle();

    //Check weather the class is registerd already
    if (::GetClassInfo(sInstance, kClassName, &sWndClass) == XPR_FALSE)
    {
        //If not then we have to register the new class
        sWndClass.style         = CS_DBLCLKS;
        sWndClass.lpfnWndProc   = ::DefWindowProc;
        sWndClass.cbClsExtra    = 0;
        sWndClass.cbWndExtra    = 0;
        sWndClass.hInstance     = sInstance;
        sWndClass.hIcon         = XPR_NULL;
        sWndClass.hCursor       = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        sWndClass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
        sWndClass.lpszMenuName  = XPR_NULL;
        sWndClass.lpszClassName = kClassName;

        if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

    return CWnd::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(ContentsWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SETTINGCHANGE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

xpr_sint_t ContentsWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    updateChildWnd();

    return 0;
}

void ContentsWnd::OnDestroy(void) 
{
    CWnd::OnDestroy();

    DESTROY_DELETE(mBookmarkSetWnd);

    XPR_SAFE_DELETE(mTitleFont);
    XPR_SAFE_DELETE(mTextFont);
    XPR_SAFE_DELETE(mFileNameFont);
    XPR_SAFE_DELETE(mFullPathFont);

    clear();
}

void ContentsWnd::OnSettingChange(xpr_uint_t uFlags, const xpr_tchar_t *lpszSection) 
{
    CWnd::OnSettingChange(uFlags, lpszSection);

    updateChildWnd();
}

void ContentsWnd::OnPaint(void) 
{
    CPaintDC sPaintDc(this); // device context for painting

    CRect sClientRect;
    GetClientRect(sClientRect);

    CMemDC sMemDc(&sPaintDc);

    if (gOpt->mContentsStyle == CONTENTS_NONE)
        sMemDc.FillSolidRect(&sClientRect, ::GetSysColor(COLOR_WINDOW));
    else if (gOpt->mContentsStyle == CONTENTS_EXPLORER)
        sMemDc.FillSolidRect(&sClientRect, RGB(255,255,255));
    else if (gOpt->mContentsStyle == CONTENTS_BASIC)
    {
        COLORREF sBackColor = fxb::CalculateColor(RGB(255,255,255), GetSysColor(COLOR_3DFACE), 180);
        sMemDc.FillSolidRect(&sClientRect, sBackColor);
    }

    // fill foreground
    sMemDc.SetBkMode(TRANSPARENT);

    // contents picture
    if (gOpt->mContentsStyle == CONTENTS_EXPLORER)
    {
        COLORREF sBackColor = fxb::CalculateColor(RGB(255,255,255), GetSysColor(COLOR_3DFACE), 200);
        COLORREF sBarColor  = GetSysColor(COLOR_APPWORKSPACE);
        CRect sContentsRect(0, sClientRect.top, 200, sClientRect.top+60);

        // background
        sMemDc.FillSolidRect(sContentsRect, sBackColor);

        // bar
        CPen sPen(PS_SOLID, 2, sBarColor);
        CPen *sOldPen = sMemDc.SelectObject(&sPen);

        sMemDc.MoveTo(0, sClientRect.top+60);
        sMemDc.LineTo(200, sClientRect.top+60);

        sMemDc.SelectObject(sOldPen);
        sPen.DeleteObject();

        // icon box
        sPen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
        sOldPen = sMemDc.SelectObject(&sPen);
        CBrush sBrush;
        sBrush.CreateSolidBrush(RGB(255,255,255));
        CBrush *sOldBrush = sMemDc.SelectObject(&sBrush);

        CRect sBoxRect(sContentsRect);
        sBoxRect.left += 9;
        sBoxRect.top  += 8;
        sBoxRect.right  = sBoxRect.left + 45;
        sBoxRect.bottom = sBoxRect.top  + 43;
        sMemDc.Rectangle(sBoxRect);

        sMemDc.SelectObject(sOldBrush);
        sMemDc.SelectObject(sOldPen);
    }

    // folder icon
    if (gOpt->mContentsStyle != 0)
    {
        if (mFolderIcon != XPR_NULL)
        {
            if (gOpt->mContentsStyle == CONTENTS_BASIC) sMemDc.DrawIcon(5,  sClientRect.top+2,  mFolderIcon);
            else                                        sMemDc.DrawIcon(16, sClientRect.top+14, mFolderIcon);
        }
    }

    if (gOpt->mContentsStyle != 0)
    {
        // current folder
        xpr_sint_t sDesc = 0;
        CFont *sOldFont = sMemDc.SelectObject(mTitleFont);
        if (gOpt->mContentsStyle == CONTENTS_EXPLORER)
        {
            sMemDc.DrawText(mFolderName.c_str(), CRect(62, sClientRect.top+7, 198, sClientRect.top+60), DT_LEFT | DT_END_ELLIPSIS | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
            sDesc = sClientRect.top+72;
        }
        else
        {
            sMemDc.DrawText(mFolderName.c_str(), CRect(42, sClientRect.top-3, 500, sClientRect.top+38), DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
        sMemDc.SelectObject(sOldFont);

        if (gOpt->mContentsStyle == CONTENTS_EXPLORER)
        {
            // description
            xpr_sint_t sBookmarkTextTop = drawItem(&sMemDc, sDesc);

            // bookmark
            CFont *sOldFont = sMemDc.SelectObject(mTextFont);
            if (fxb::BookmarkMgr::instance().getCount() > 0 && mEnableBookmark == XPR_TRUE)
            {
                xpr_tchar_t sBookmark[0xff] = {0};
                _stprintf(sBookmark, XPR_STRING_LITERAL("%s:"), theApp.loadString(XPR_STRING_LITERAL("contents.bookmark")));

                CSize sSize = sMemDc.GetTextExtent(mFolderName.c_str());
                CRect sBookmarkRect(10, sBookmarkTextTop, 200, sBookmarkTextTop+sSize.cy);

                sMemDc.DrawText(sBookmark, sBookmarkRect, DT_LEFT);
                if (mBookmarkTop != (sBookmarkTextTop+sSize.cy))
                {
                    mBookmarkTop = sBookmarkTextTop+sSize.cy;
                    updateBookmark(mBookmarkTop);
                }
            }
            sMemDc.SelectObject(sOldFont);
        }
    }
}

xpr_bool_t ContentsWnd::OnEraseBkgnd(CDC* pDC) 
{
    return XPR_TRUE;
    //return CWnd::OnEraseBkgnd(pDC);
}

void ContentsWnd::clear(void)
{
    Item *sItem;
    DispDeque::iterator sIterator;

    sIterator = mDispDeque.begin();
    for (; sIterator != mDispDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (sItem == XPR_NULL)
            continue;

        switch (sItem->mType)
        {
        case TypeHelp:       { HelpItem       *sDerivedItem = (HelpItem       *)sItem; XPR_SAFE_DELETE(sDerivedItem); break; }
        case TypeInfotip:    { InfotipItem    *sDerivedItem = (InfotipItem    *)sItem; XPR_SAFE_DELETE(sDerivedItem); break; }
        case TypeSingleSel:  { SingleSelItem  *sDerivedItem = (SingleSelItem  *)sItem; XPR_SAFE_DELETE(sDerivedItem); break; }
        case TypeMultiSel:   { MultiSelItem   *sDerivedItem = (MultiSelItem   *)sItem; XPR_SAFE_DELETE(sDerivedItem); break; }
        case TypeDriveSpace: { DriveSpaceItem *sDerivedItem = (DriveSpaceItem *)sItem; XPR_SAFE_DELETE(sDerivedItem); break; }

        default:
            {
                XPR_SAFE_DELETE(sItem);
                break;
            }
        }
    }

    mDispDeque.clear();
}

ContentsWnd::Item::Item(xpr_size_t aType)
    : mType(aType)
{
}

ContentsWnd::HelpItem::HelpItem(const xpr_tchar_t *aHelp)
    : Item(TypeHelp)
{
    if (aHelp != XPR_NULL)
        mHelp = aHelp;
}

ContentsWnd::InfotipItem::InfotipItem(const xpr_tchar_t *aInfotip)
    : Item(TypeInfotip)
{
    if (aInfotip != XPR_NULL)
        mInfotip = aInfotip;
}

ContentsWnd::SingleSelItem::SingleSelItem(void)
    : Item(TypeSingleSel)
{
}

ContentsWnd::MultiSelItem::MultiSelItem(void)
    : Item(TypeMultiSel)
{
}

ContentsWnd::DriveSpaceItem::DriveSpaceItem(void)
    : Item(TypeDriveSpace)
{
}

xpr_sint_t ContentsWnd::HelpItem::draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont)
{
    CRect sRect(10, aTop, aWidth, aTop+aHeight*2);

    aDc->DrawText(mHelp.c_str(), sRect, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
    aDc->DrawText(mHelp.c_str(), sRect, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

    return sRect.bottom + aHeight;
}

xpr_sint_t ContentsWnd::InfotipItem::draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont)
{
    CRect sTipRect(10, aTop, aWidth-2, aTop+aHeight);
    aDc->DrawText(mInfotip.c_str(), sTipRect, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
    sTipRect.right = aWidth-2;

    sTipRect.InflateRect(1, 3, 1, 1);
    aDc->FillSolidRect(sTipRect, RGB(222,222,222));
    sTipRect.DeflateRect(1, 1, 1, 1);
    aDc->FillSolidRect(sTipRect, ::GetSysColor(COLOR_INFOBK));

    sTipRect.DeflateRect(2, 1, 0, 0);
    aDc->SetTextColor(::GetSysColor(COLOR_INFOTEXT));
    aDc->DrawText(mInfotip.c_str(), sTipRect, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

    return sTipRect.bottom + aHeight;
}

xpr_sint_t ContentsWnd::SingleSelItem::draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont)
{
    CFont *sOldFont = aDc->SelectObject(aBoldTextFont);

    CRect sRect;
    sRect.left = 10;
    sRect.right = aWidth;
    sRect.top = aTop;
    sRect.bottom = sRect.top + aHeight;
    aDc->DrawText(mName.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
    aDc->DrawText(mName.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

    aDc->SelectObject(sOldFont);

    sOldFont = aDc->SelectObject(aTextFont);

    sRect.right = aWidth;
    sRect.top = sRect.bottom;
    sRect.bottom = sRect.top + aHeight;
    aDc->DrawText(mType.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
    aDc->DrawText(mType.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

    if (mDate.empty() == false)
    {
        sRect.right = aWidth;
        sRect.top = sRect.bottom + aHeight;
        sRect.bottom = sRect.top + aHeight;
        aDc->DrawText(mDate.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
        aDc->DrawText(mDate.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);
    }

    if (mSize.empty() == false)
    {
        sRect.right = aWidth;
        sRect.top = sRect.bottom + aHeight;
        sRect.bottom = sRect.top + aHeight;
        aDc->DrawText(mSize.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
        aDc->DrawText(mSize.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);
    }

    if (mAttr.empty() == false)
    {
        sRect.right = aWidth;
        sRect.top = sRect.bottom + aHeight;
        sRect.bottom = sRect.top + aHeight;
        aDc->DrawText(mAttr.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
        aDc->DrawText(mAttr.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);
    }

    return sRect.bottom + aHeight;
}

xpr_sint_t ContentsWnd::MultiSelItem::draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont)
{
    CRect sRect;
    sRect.left   = 10;
    sRect.right  = aWidth;
    sRect.top    = aTop;
    sRect.bottom = sRect.top + aHeight;
    aDc->DrawText(mCount.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
    aDc->DrawText(mCount.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

    sRect.right  = aWidth;
    sRect.top    = sRect.bottom + aHeight;
    sRect.bottom = sRect.top    + aHeight;
    aDc->DrawText(mSize.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
    aDc->DrawText(mSize.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

    sRect.right  = aWidth;
    sRect.top    = sRect.bottom + aHeight;
    sRect.bottom = sRect.top    + aHeight;
    aDc->DrawText(mNames.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
    aDc->DrawText(mNames.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

    return sRect.bottom;
}

xpr_sint_t ContentsWnd::DriveSpaceItem::draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont)
{
    CRect sRect;
    sRect.left   = 10;
    sRect.right  = aWidth;
    sRect.top    = aTop;
    sRect.bottom = sRect.top + aHeight;
    aDc->DrawText(mTotalSize.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
    aDc->DrawText(mTotalSize.c_str(), sRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

    xpr_sint_t sGap = aHeight * 3 / 2;

    sRect.left    = 30;
    sRect.right   = aWidth;
    sRect.top    += sGap;
    sRect.bottom += sGap;
    CRect sRect2(sRect.left, sRect.top, sRect.left+12, sRect.top+12);
    aDc->FillSolidRect(sRect2, RGB(0,0,0));
    sRect2.DeflateRect(1, 1, 1, 1);
    aDc->FillSolidRect(sRect2, RGB(222,222,222));
    sRect.left = sRect2.right + 5;
    aDc->DrawText(mUsedSize.c_str(), sRect, DT_SINGLELINE | DT_LEFT | DT_NOPREFIX);

    sRect.left    = 30;
    sRect.top    += sGap;
    sRect.bottom += sGap;
    sRect2 = sRect;
    sRect2 = CRect(sRect.left, sRect.top, sRect.left+12, sRect.top+12);
    aDc->FillSolidRect(sRect2, RGB(0,0,0));
    sRect2.DeflateRect(1, 1, 1, 1);
    aDc->FillSolidRect(sRect2, RGB(255,255,255));
    sRect.left = sRect2.right + 5;
    aDc->DrawText(mFreeSize.c_str(), sRect, DT_SINGLELINE | DT_LEFT | DT_NOPREFIX);

    // progress
    sRect.left    = 10;
    sRect.top    += aHeight;
    sRect.bottom += aHeight;
    CRect sProgressRect(sRect.left, sRect.bottom+5, aWidth-2, 0);
    sProgressRect.right  = aWidth-10;
    sProgressRect.bottom = sProgressRect.top + aHeight;
    sProgressRect.InflateRect(1, 1, 1, 1);
    aDc->FillSolidRect(sProgressRect, RGB(0,0,0));
    sProgressRect.DeflateRect(1, 1, 1, 1);
    sProgressRect.right = sProgressRect.left + (xpr_sint_t)(sProgressRect.Width()*(mUsedPercentage/100.0));
    aDc->FillSolidRect(sProgressRect, RGB(222,222,222));
    sProgressRect.left  = sProgressRect.right;
    sProgressRect.right = aWidth-10;
    aDc->FillSolidRect(sProgressRect, RGB(255,255,255));

    sProgressRect.left = 10;
    xpr_tchar_t sUsedPercent[0x20] = {0};
    _stprintf(sUsedPercent, XPR_STRING_LITERAL("%d%%"), mUsedPercentage);
    aDc->SetTextColor(RGB(0,0,0));
    aDc->SetBkMode(TRANSPARENT);
    aDc->DrawText(sUsedPercent, sProgressRect, DT_SINGLELINE | DT_CENTER | DT_NOPREFIX | DT_VCENTER);

    return sProgressRect.bottom + aHeight;
}

xpr_sint_t ContentsWnd::drawItem(CDC *aDc, xpr_sint_t aTop)
{
    CRect sClientRect;
    GetClientRect(&sClientRect);

    xpr_sint_t sWidth = sClientRect.Width();
    xpr_sint_t sHeight = sClientRect.Height();

    CSize sSize = aDc->GetTextExtent(mFolderName.c_str());
    sSize.cy -= 2;

    CFont *sOldFont = aDc->SelectObject(mTextFont);
    if (sOldFont == XPR_NULL)
        return aTop;

    Item *sItem;
    DispDeque::iterator sIterator;

    sIterator = mDispDeque.begin();
    for (; sIterator != mDispDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (sItem == XPR_NULL)
            continue;

        aTop = sItem->draw(aDc, aTop, sWidth, sSize.cy, mTextFont, mFileNameFont);
    }

    aDc->SelectObject(sOldFont);

    return aTop;
}

void ContentsWnd::setContentsFolder(LPTVITEMDATA aTvItemData)
{
    if (aTvItemData == XPR_NULL)
        return;

    DESTROY_ICON(mFolderIcon);

    xpr_sint_t sIconIndex = fxb::GetItemIconIndex(aTvItemData->mFullPidl, XPR_FALSE);
    mFolderIcon = fxb::SysImgListMgr::instance().mSysImgList32.ExtractIcon(sIconIndex);

    mFolderName.clear();
    fxb::GetName(aTvItemData->mShellFolder, aTvItemData->mPidl, SHGDN_INFOLDER, mFolderName);

    Invalidate(XPR_FALSE);
}

void ContentsWnd::setContentsNormal(LPTVITEMDATA aTvItemData, xpr_bool_t aUpdate)
{
    if (aTvItemData == XPR_NULL)
        return;

    if (XPR_IS_TRUE(gOpt->mContentsNoDispInfo))
    {
        clear();
        return;
    }

    if (gOpt->mContentsStyle != CONTENTS_EXPLORER)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetFolderPath(aTvItemData->mShellFolder, aTvItemData->mPidl, sPath, aTvItemData->mShellAttributes);
    if (sPath[0] != XPR_STRING_LITERAL('\0') && _tcslen(sPath) <= 3) // length 1~3
    {
        setContentsDrive(aTvItemData, sPath[0], XPR_TRUE, aUpdate);
        return;
    }

    if (sPath[0] == XPR_STRING_LITERAL('\\') && sPath[1] == XPR_STRING_LITERAL('\\') && _tcschr(sPath+2, XPR_STRING_LITERAL('\\')))
    {
        xpr_tchar_t *sSplit = _tcschr(sPath+2, XPR_STRING_LITERAL('\\'));
        if (sSplit != XPR_NULL && (sSplit[1] != XPR_STRING_LITERAL(':') || sSplit[2] != XPR_STRING_LITERAL(':')))
        {
            sSplit++;
            sSplit = _tcschr(sSplit, XPR_STRING_LITERAL('\\'));
            if (sSplit == XPR_NULL)
            {
                setContentsDrive(aTvItemData, sPath[0], XPR_TRUE, aUpdate);
                return;
            }
        }
    }

    if (aUpdate == XPR_FALSE && mMode == ModeNormal)
        return;

    clear();

    mDispDeque.push_back(new HelpItem(theApp.loadString(XPR_STRING_LITERAL("contents.no_sel.desc"))));

    xpr_tchar_t sInfotip[INFOTIPSIZE + 1] = {0};
    if (fxb::GetInfotip(m_hWnd, aTvItemData->mShellFolder, aTvItemData->mPidl, sInfotip))
        mDispDeque.push_back(new InfotipItem(sInfotip));

    mMode = ModeNormal;

    xpr_bool_t sEnableBookmark = gOpt->mContentsStyle == CONTENTS_EXPLORER || gOpt->mContentsStyle == CONTENTS_BASIC && gOpt->mContentsBookmark;
    if (sEnableBookmark == XPR_TRUE)
        enableBookmark(XPR_TRUE);

    Invalidate(XPR_FALSE);
    UpdateWindow();

    if (sEnableBookmark == XPR_TRUE)
        updateBookmark(mBookmarkTop);
}

void ContentsWnd::setContentsSingleItem(LPLVITEMDATA       aLvItemData,
                                        const xpr_tchar_t *aName,
                                        const xpr_tchar_t *aType,
                                        const xpr_tchar_t *aDate,
                                        const xpr_tchar_t *aSize,
                                        const xpr_tchar_t *aAttr)
{
    if (aLvItemData == XPR_NULL)
        return;

    if (XPR_IS_TRUE(gOpt->mContentsNoDispInfo))
    {
        clear();
        return;
    }

    if (gOpt->mContentsStyle != CONTENTS_EXPLORER)
        return;

    if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sPath);

        // drive
        if ((_tcslen(sPath) <= 3) &&
            (XPR_IS_RANGE(XPR_STRING_LITERAL('A'), sPath[0], XPR_STRING_LITERAL('Z')) || XPR_IS_RANGE(XPR_STRING_LITERAL('a'), sPath[0], XPR_STRING_LITERAL('z'))))
        {
            setContentsDrive(aLvItemData, sPath[0], XPR_FALSE, XPR_TRUE);
            return;
        }
    }

    clear();

    // general
    xpr_tchar_t sText[0xff] = {0};

    SingleSelItem *sSingleSelItem = new SingleSelItem;
    sSingleSelItem->mName = aName;
    sSingleSelItem->mType = aType;

    if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
    {
        if (aDate != XPR_NULL && *aDate != XPR_STRING_LITERAL('\0'))
        {
            _stprintf(sText, XPR_STRING_LITERAL("%s: %s"), theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.modified_date")), aDate);
            sSingleSelItem->mDate = sText;
        }

        if (aSize != XPR_NULL && *aSize != XPR_STRING_LITERAL('\0'))
        {
            _stprintf(sText, XPR_STRING_LITERAL("%s: %s"), theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.size")), aSize);
            sSingleSelItem->mSize = sText;
        }

        if (gOpt->mContentsARHSAttribute == XPR_TRUE)
        {
            _stprintf(sText, XPR_STRING_LITERAL("%s: %s"), theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.ARHS_attributes")), aAttr);
            sSingleSelItem->mAttr = sText;
        }
        else
        {
            xpr_bool_t sNoAttrs = XPR_TRUE;
            sText[0] = XPR_STRING_LITERAL('\0');

            if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_READONLY))
            {
                _stprintf(sText+_tcslen(sText), theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.attribute.read_only")));
                sNoAttrs = XPR_FALSE;
            }

            if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_GHOSTED))
            {
                if (_tcslen(sText) > 0)
                    _tcscat(sText, XPR_STRING_LITERAL(", "));

                _stprintf(sText+_tcslen(sText), theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.attribute.hidden")));
                sNoAttrs = XPR_FALSE;
            }

            if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_SHARE))
            {
                if (_tcslen(sText) > 0)
                    _tcscat(sText, XPR_STRING_LITERAL(", "));

                _stprintf(sText+_tcslen(sText), theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.attribute.share")));
                sNoAttrs = XPR_FALSE;
            }

            if (sNoAttrs == XPR_TRUE)
                _tcscat(sText, theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.attribute.general")));

            sSingleSelItem->mAttr = sText;
            sSingleSelItem->mAttr.insert(0, XPR_STRING_LITERAL(": "));
            sSingleSelItem->mAttr.insert(0, theApp.loadString(XPR_STRING_LITERAL("contents.single_sel.attributes")));
        }
    }

    mDispDeque.push_back(sSingleSelItem);

    if (fxb::IsVirtualItem(aLvItemData->mShellFolder, aLvItemData->mPidl))
    {
        xpr_tchar_t sInfotip[INFOTIPSIZE + 1] = {0};
        if (fxb::GetInfotip(m_hWnd, aLvItemData->mShellFolder, aLvItemData->mPidl, sInfotip))
            mDispDeque.push_back(new InfotipItem(sInfotip));
    }

    mMode = ModeSingleItem;
    enableBookmark(XPR_FALSE);
    Invalidate(XPR_FALSE);
    UpdateWindow();
}

void ContentsWnd::setContentsMultiItem(xpr_size_t aCount, const xpr_tchar_t *aSize, const xpr_tchar_t *aNames)
{
    if (XPR_IS_TRUE(gOpt->mContentsNoDispInfo))
    {
        clear();
        return;
    }

    if (gOpt->mContentsStyle != CONTENTS_EXPLORER)
        return;

    clear();

    xpr_tchar_t sText[0xff] = {0};
    MultiSelItem *sMultiSelItem = new MultiSelItem;

    xpr_tchar_t sCountText[0xff] = {0};
    fxb::GetFormatedNumber(aCount, sCountText, XPR_COUNT_OF(sCountText) - 1);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("contents.multi_sel.desc"), XPR_STRING_LITERAL("%s")), sCountText);
    sMultiSelItem->mCount = sText;

    if (aCount <= 100 && XPR_IS_NOT_NULL(aSize))
    {
        _stprintf(sText, XPR_STRING_LITERAL("%s: %s"), theApp.loadString(XPR_STRING_LITERAL("contents.multi_sel.size")), aSize);
        sMultiSelItem->mSize = sText;
    }

    sMultiSelItem->mNames = aNames;

    mDispDeque.push_back(sMultiSelItem);

    mMode = ModeMultiItem;
    enableBookmark(XPR_FALSE);
    Invalidate(XPR_FALSE);
    UpdateWindow();
}

void ContentsWnd::setContentsDrive(LPLVITEMDATA aLvItemData, xpr_tchar_t aDriveChar, xpr_bool_t aShowBookmark, xpr_bool_t aUpdate)
{
    if (gOpt->mContentsStyle != CONTENTS_EXPLORER)
        return;

    setContentsDrive(aLvItemData->mShellFolder, aLvItemData->mPidl, aLvItemData->mShellAttributes, aDriveChar, aShowBookmark, aUpdate);
}

void ContentsWnd::setContentsDrive(LPTVITEMDATA aTvItemData, xpr_tchar_t aDriveChar, xpr_bool_t aShowBookmark, xpr_bool_t aUpdate)
{
    if (gOpt->mContentsStyle != CONTENTS_EXPLORER)
        return;

    setContentsDrive(aTvItemData->mShellFolder, aTvItemData->mPidl, aTvItemData->mShellAttributes, aDriveChar, aShowBookmark, aUpdate);
}

void ContentsWnd::setContentsDrive(LPSHELLFOLDER aShellFolder,
                                   LPITEMIDLIST  aPidl,
                                   xpr_ulong_t   aAttributes,
                                   xpr_tchar_t   aDriveChar,
                                   xpr_bool_t    aShowBookmark,
                                   xpr_bool_t    aUpdate)
{
    if (XPR_IS_TRUE(gOpt->mContentsNoDispInfo))
    {
        clear();
        return;
    }

    if (!aUpdate && mMode == ModeDrive)
        return;

    if (gOpt->mContentsStyle != CONTENTS_EXPLORER)
        return;

    clear();

    mDispDeque.push_back(new HelpItem(theApp.loadString(XPR_STRING_LITERAL("contents.no_sel.desc"))));

    xpr_tchar_t sText[0xff] = {0};
    fxb::GetName(aShellFolder, aPidl, SHGDN_INFOLDER, sText);
    if (_tcschr(sText, XPR_STRING_LITERAL(':')) == XPR_NULL)
    {
        xpr_tchar_t sInfotip[INFOTIPSIZE + 1] = {0};
        if (fxb::GetInfotip(m_hWnd, aShellFolder, aPidl, sInfotip))
            mDispDeque.push_back(new InfotipItem(sInfotip));
    }

    DriveSpaceItem *sDriveSpaceItem = new DriveSpaceItem;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetFolderPath(aShellFolder, aPidl, sPath, aAttributes);

    xpr_uint64_t sTotalSize = 0;
    xpr_uint64_t sUsedSize  = 0;
    xpr_uint64_t sFreeSize  = 0;
    fxb::GetDiskSize(sPath, sTotalSize, sUsedSize, sFreeSize);

    xpr_tchar_t sFormatedSize[0xff] = {0};
    fxb::SizeFormat::getSizeUnitFormat(sTotalSize, SIZE_UNIT_AUTO, sFormatedSize, 0xfe);
    _stprintf(sText, XPR_STRING_LITERAL("%s: %s"), theApp.loadString(XPR_STRING_LITERAL("contents.no_sel.drive.total_size")), sFormatedSize);
    sDriveSpaceItem->mTotalSize = sText;

    fxb::SizeFormat::getSizeUnitFormat(sUsedSize, SIZE_UNIT_AUTO, sFormatedSize, 0xfe);
    _stprintf(sText, XPR_STRING_LITERAL("%s: %s"), theApp.loadString(XPR_STRING_LITERAL("contents.no_sel.drive.used_size")), sFormatedSize);
    sDriveSpaceItem->mUsedSize = sText;

    fxb::SizeFormat::getSizeUnitFormat(sFreeSize, SIZE_UNIT_AUTO, sFormatedSize, 0xfe);
    _stprintf(sText, XPR_STRING_LITERAL("%s: %s"), theApp.loadString(XPR_STRING_LITERAL("contents.no_sel.drive.free_size")), sFormatedSize);
    sDriveSpaceItem->mFreeSize = sText;

    sDriveSpaceItem->mUsedPercentage = sTotalSize > 0ui64 ? (xpr_sint_t)(((xpr_double_t)sUsedSize / (xpr_double_t)sTotalSize)*100) : 0;

    mDispDeque.push_back(sDriveSpaceItem);

    mMode = ModeDrive;
    enableBookmark(aShowBookmark);
    Invalidate(XPR_FALSE);
    UpdateWindow();

    if (XPR_IS_TRUE(aShowBookmark))
        updateBookmark(mBookmarkTop);
}

void ContentsWnd::updateBookmark(void)
{
    if (mBookmarkSetWnd == XPR_NULL)
    {
        mBookmarkSetWnd = new BookmarkSetWnd;
        mBookmarkSetWnd->Create(this, CTRL_ID_BOOKMARK_SET_WND, CRect(0,0,0,0));
    }

    mBookmarkSetWnd->updateBookmark();
}

void ContentsWnd::updateBookmark(xpr_sint_t aTop)
{
    if (mBookmarkSetWnd == XPR_NULL)
        return;

    CRect sRect;
    GetClientRect(&sRect);

    sRect.top = aTop;

    mBookmarkSetWnd->SetWindowPos(XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
    mBookmarkSetWnd->Invalidate();
    //mBookmarkSetWnd->UpdateWindow();
}

void ContentsWnd::updateBookmarkPosition(void)
{
    updateBookmark(mBookmarkTop);
}

void ContentsWnd::enableBookmark(xpr_bool_t bEnable)
{
    if (mMode == ModeSingleItem || mMode == ModeMultiItem || !gOpt->mContentsBookmark)
        bEnable = XPR_FALSE;

    if (mBookmarkSetWnd != XPR_NULL)
        mBookmarkSetWnd->enableBookmark(bEnable);

    mEnableBookmark = bEnable;
}

void ContentsWnd::setBookmarkPopup(xpr_bool_t bPopup)
{
    if (mBookmarkSetWnd != XPR_NULL)
        mBookmarkSetWnd->setBookmarkPopup(bPopup);
}

void ContentsWnd::setBookmarkColor(COLORREF clrBookmark)
{
    if (mBookmarkSetWnd != XPR_NULL)
        mBookmarkSetWnd->setBookmarkColor(clrBookmark);
}

void ContentsWnd::updateChildWnd(void)
{
    if (mTitleFont    != XPR_NULL) { XPR_SAFE_DELETE(mTitleFont);    }
    if (mTextFont     != XPR_NULL) { XPR_SAFE_DELETE(mTextFont);     }
    if (mFileNameFont != XPR_NULL) { XPR_SAFE_DELETE(mFileNameFont); }
    if (mFullPathFont != XPR_NULL) { XPR_SAFE_DELETE(mFullPathFont); }

    mTitleFont    = new CFont;
    mTextFont     = new CFont;
    mFileNameFont = new CFont;
    mFullPathFont = new CFont;

    // font Creation
    NONCLIENTMETRICS sNonClientMetrics;
    sNonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &sNonClientMetrics, 0);
    LOGFONT sLogFont = sNonClientMetrics.lfMenuFont;
    if (mTextFont->CreateFontIndirect(&sLogFont) == XPR_FALSE)
        return;

    sLogFont.lfWeight = FW_BOLD;
    if (mFileNameFont->CreateFontIndirect(&sLogFont) == XPR_FALSE)
        return;

    if (mFullPathFont->CreateFontIndirect(&sLogFont) == XPR_FALSE)
        return;

    sLogFont.lfHeight = -MulDiv(13, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
    if (mTitleFont->CreateFontIndirect(&sLogFont) == XPR_FALSE)
        return;
}

void ContentsWnd::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (mBookmarkSetWnd != XPR_NULL)
    {
        CRect sRect;
        GetClientRect(&sRect);

        sRect.top = mBookmarkTop;

        mBookmarkSetWnd->SetWindowPos(XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
        //mBookmarkSetWnd->Invalidate();
    }
}
