//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "context_menu.h"

#include "option.h"

#include "file_scrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4244)
#pragma warning(disable:4312)

namespace fxfile
{
static const xpr_tchar_t PROP_CONTEXT_MENU[] = XPR_STRING_LITERAL("fxfile::SubclassData");

xpr_bool_t ContextMenu::mShowFileScrapMenu = XPR_FALSE;
xpr_bool_t ContextMenu::mAnimationMenu     = XPR_FALSE;

ContextMenu::ContextMenu(HWND aHwnd)
    : mHwnd(aHwnd)
    , mShellFolder(XPR_NULL), mPidls(XPR_NULL), mCount(0)
    , mMenu(XPR_NULL)
    , mContextMenu(XPR_NULL), mContextMenu2(XPR_NULL)
    , mFirstId(CMID_DEF_ID_FIRST)
    , mSubclassData(XPR_NULL)
{
}

ContextMenu::~ContextMenu(void)
{
    destroySubclass();
    release();
}

void ContextMenu::setFileScrapMenu(xpr_bool_t aShowFileScrapMenu)
{
    mShowFileScrapMenu = aShowFileScrapMenu;
}

void ContextMenu::setAnimationMenu(xpr_bool_t aAnimationMenu)
{
    mAnimationMenu = aAnimationMenu;
}

xpr_bool_t ContextMenu::init(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_uint_t aCount)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidls) || aCount == 0)
        return XPR_FALSE;

    mShellFolder = aShellFolder;
    mPidls = aPidls;
    mCount = aCount;

    HRESULT sHResult;
    sHResult = mShellFolder->GetUIObjectOf(mHwnd, mCount, (LPCITEMIDLIST *)mPidls, IID_IContextMenu, XPR_NULL, (LPVOID *)&mContextMenu);
    if (FAILED(sHResult))
        return XPR_FALSE;

    mContextMenu->QueryInterface(IID_IContextMenu2, (LPVOID *)&mContextMenu2);

    return XPR_TRUE;
}

xpr_bool_t ContextMenu::init(LPSHELLFOLDER aShellFolder)
{
    if (XPR_IS_NULL(aShellFolder))
        return XPR_FALSE;

    mShellFolder = aShellFolder;

    HRESULT sHResult;
    sHResult = mShellFolder->CreateViewObject(mHwnd, IID_IContextMenu, (LPVOID*)&mContextMenu);
    if (FAILED(sHResult))
        return XPR_FALSE;

    mContextMenu->QueryInterface(IID_IContextMenu2, (LPVOID *)&mContextMenu2);

    return XPR_TRUE;
}

LRESULT CALLBACK ContextMenu::subclassWndProc(HWND aWnd, xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam)
{
    LPSUBCLASSDATA sSubclassData = (LPSUBCLASSDATA)::GetProp(aWnd, PROP_CONTEXT_MENU);
    if (XPR_IS_NOT_NULL(sSubclassData))
    {
        LPCONTEXTMENU2 sContextMenu2 = sSubclassData->mContextMenu2;
        switch (aMsg)
        {
        case WM_INITMENUPOPUP:
        case WM_DRAWITEM:
        case WM_MENUCHAR:
        case WM_MEASUREITEM:
            if (XPR_IS_NOT_NULL(sContextMenu2))
            {
                if (SUCCEEDED(sContextMenu2->HandleMenuMsg(aMsg, wParam, lParam)))
                    return XPR_FALSE;
            }

            return XPR_TRUE;
        }

        return CallWindowProc(sSubclassData->mOldWndProc, aWnd, aMsg, wParam, lParam);
    }

    return DefWindowProc(aWnd, aMsg, wParam, lParam);
}

void ContextMenu::getInterface(LPCONTEXTMENU *aContextMenu, LPCONTEXTMENU2 *aContextMenu2)
{
    *aContextMenu  = mContextMenu;
    *aContextMenu2 = mContextMenu2;
}

xpr_bool_t ContextMenu::getMenu(CMenu *aMenu, xpr_uint_t aFirstId, xpr_uint_t aQueryFlags)
{
    if (XPR_IS_NULL(aMenu) || IsWindow(mHwnd) == XPR_FALSE)
        return XPR_FALSE;

    HRESULT sHResult = E_FAIL;
    if (XPR_IS_NOT_NULL(mContextMenu2))
        sHResult = mContextMenu2->QueryContextMenu(aMenu->m_hMenu, 0, aFirstId, aFirstId + 0x7FFF, aQueryFlags);

    if (FAILED(sHResult))
    {
        sHResult = mContextMenu->QueryContextMenu(aMenu->m_hMenu, 0, aFirstId, aFirstId + 0x7FFF, aQueryFlags);
        COM_RELEASE(mContextMenu2);

        if (FAILED(sHResult))
            return XPR_FALSE;
    }

    if (XPR_IS_NOT_NULL(mContextMenu2))
    {
        mSubclassData = new SUBCLASSDATA;
        mSubclassData->mContextMenu2 = mContextMenu2;
        mSubclassData->mOldWndProc   = (WNDPROC)::SetWindowLongPtr(mHwnd, GWL_WNDPROC, (LONG_PTR)subclassWndProc);
        ::SetProp(mHwnd, PROP_CONTEXT_MENU, (HANDLE)mSubclassData);
    }

    if (XPR_IS_NOT_NULL(mPidls) && mCount > 0 && XPR_IS_TRUE(mShowFileScrapMenu))
    {
        xpr_sint_t nInsert = aMenu->GetMenuItemCount();
        nInsert -= 2;

        ::InsertMenu(aMenu->m_hMenu, nInsert,   MF_BYPOSITION | MF_SEPARATOR, 0, XPR_NULL);
        ::InsertMenu(aMenu->m_hMenu, nInsert+1, MF_BYPOSITION, aFirstId + CMID_FILE_SCRAP, theApp.loadString(XPR_STRING_LITERAL("cmd.shell.add_to_scrap")));
    }

    return XPR_TRUE;
}

xpr_uint_t ContextMenu::getFirstId(void)
{
    return mFirstId;
}

xpr_uint_t ContextMenu::trackPopupMenu(xpr_uint_t aFlags, LPPOINT aPoint, xpr_uint_t aQueryFlags)
{
    if (XPR_IS_FALSE(mAnimationMenu))
        aFlags |= 0x4000L;

    CMenu sMenu;
    if (sMenu.CreatePopupMenu() == XPR_FALSE)
        return -1;

    xpr_uint_t sId = -1;
    if (getMenu(&sMenu, CMID_DEF_ID_FIRST, aQueryFlags) == XPR_TRUE)
    {
        sId = ::TrackPopupMenuEx(sMenu.m_hMenu, aFlags, aPoint->x, aPoint->y, mHwnd, XPR_NULL);
        mMenu = sMenu.Detach();
    }

    return sId;
}

xpr_bool_t ContextMenu::getCommandVerb(xpr_uint_t aId, xpr_tchar_t *aVerb, xpr_sint_t aMaxVerbLength)
{
    if (XPR_IS_NULL(mContextMenu))
        return XPR_FALSE;

    if (aId == 0xFFFFFFFF)
        return XPR_FALSE;

    HRESULT sHResult;
    sHResult = mContextMenu->GetCommandString(aId, GCS_VERB, XPR_NULL, (xpr_char_t *)aVerb, aMaxVerbLength);

    return SUCCEEDED(sHResult);
}

xpr_bool_t ContextMenu::invokeCommand(xpr_uint_t aId)
{
    if (XPR_IS_TRUE(mShowFileScrapMenu) && aId == CMID_FILE_SCRAP)
    {
        FileScrap &sFileScrap = FileScrap::instance();
        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

        sFileScrap.addItem(sGroupId, mShellFolder, mPidls, mCount);
        return XPR_TRUE;
    }

    xpr_tchar_t sVerb[0xff] = {0};
    getCommandVerb(aId, sVerb, 0xfe);

    xpr_bool_t sHResult = XPR_FALSE;

    if (XPR_IS_RANGE(0, aId, 0x7FFF - 1))
    {
        CMINVOKECOMMANDINFO sCmInvokeCommandInfo = {0};
        sCmInvokeCommandInfo.cbSize = sizeof(CMINVOKECOMMANDINFO);
        sCmInvokeCommandInfo.hwnd   = mHwnd;
        sCmInvokeCommandInfo.lpVerb = (const xpr_char_t *)MAKEINTRESOURCE(aId);
        sCmInvokeCommandInfo.nShow  = SW_SHOW;

        HRESULT sHResult;
        if (mContextMenu2 != XPR_NULL) sHResult = mContextMenu2->InvokeCommand(&sCmInvokeCommandInfo);
        else                           sHResult = mContextMenu->InvokeCommand(&sCmInvokeCommandInfo);

        if (SUCCEEDED(sHResult))
            sHResult = XPR_TRUE;
    }

    return sHResult;
}

xpr_bool_t ContextMenu::invokeCommand(const xpr_tchar_t *aVerb)
{
    if (XPR_IS_NULL(aVerb))
        return XPR_FALSE;

    xpr_char_t sVerb[0xff] = {0};
#ifdef XPR_CFG_UNICODE
    xpr_size_t sInputBytes = _tcslen(aVerb) * sizeof(xpr_tchar_t);
    xpr_size_t sOutputBytes = 0xfe;
    XPR_UTF16_TO_MBS(aVerb, &sInputBytes, sVerb, &sOutputBytes);
    sVerb[sOutputBytes / sizeof(xpr_char_t)] = 0;
#else
    strcpy(sVerb, aVerb);
#endif

    CMINVOKECOMMANDINFO sCmInvokeCommandInfo = {0};
    sCmInvokeCommandInfo.cbSize = sizeof(CMINVOKECOMMANDINFO);
    sCmInvokeCommandInfo.hwnd   = mHwnd;
    sCmInvokeCommandInfo.lpVerb = sVerb;
    sCmInvokeCommandInfo.nShow  = SW_SHOW;

    HRESULT sHResult;
    if (mContextMenu2 != XPR_NULL) sHResult = mContextMenu2->InvokeCommand(&sCmInvokeCommandInfo);
    else                           sHResult = mContextMenu->InvokeCommand(&sCmInvokeCommandInfo);

    return SUCCEEDED(sHResult) ? XPR_TRUE : XPR_FALSE;
}

void ContextMenu::destroySubclass(void)
{
    if (XPR_IS_NOT_NULL(mSubclassData))
    {
        ::SetWindowLongPtr(mHwnd, GWL_WNDPROC, (LONG_PTR)mSubclassData->mOldWndProc);
        ::RemoveProp(mHwnd, PROP_CONTEXT_MENU);
        XPR_SAFE_DELETE(mSubclassData);
    }
}

void ContextMenu::release(void)
{
    if (XPR_IS_NOT_NULL(mMenu))
    {
        ::DestroyMenu(mMenu);
        mMenu = XPR_NULL;
    }

    COM_RELEASE(mContextMenu);
    COM_RELEASE(mContextMenu2);
}

xpr_bool_t ContextMenu::trackItemMenu(LPSHELLFOLDER  aShellFolder,
                                      LPCITEMIDLIST *aPidls,
                                      xpr_uint_t     aCount,
                                      LPPOINT        aPoint,
                                      xpr_uint_t     aFlags,
                                      HWND           aWnd,
                                      xpr_uint_t     aQueryFlags)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidls) || aCount == 0 || XPR_IS_NULL(aPoint))
        return XPR_FALSE;

    xpr_bool_t sHResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder, aPidls, aCount) == XPR_TRUE)
    {
        xpr_uint_t sId = sContextMenu.trackPopupMenu(aFlags, aPoint, aQueryFlags);
        if (sId != -1)
        {
            xpr_uint_t sCmdId = sId - sContextMenu.getFirstId();

            sHResult = sContextMenu.invokeCommand(sCmdId);
        }
    }

    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sHResult;
}

xpr_bool_t ContextMenu::trackBackMenu(LPSHELLFOLDER aShellFolder, LPPOINT aPoint, xpr_uint_t aFlags, HWND aWnd)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPoint))
        return XPR_FALSE;

    xpr_bool_t sHResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder) == XPR_TRUE)
    {
        xpr_uint_t sId = sContextMenu.trackPopupMenu(aFlags, aPoint);
        if (sId != -1)
        {
            xpr_uint_t sCmdId = sId - sContextMenu.getFirstId();

            sHResult = sContextMenu.invokeCommand(sCmdId);
        }
    }

    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sHResult;
}

xpr_bool_t ContextMenu::invokeCommand(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_uint_t aCount, xpr_uint_t aId, HWND aWnd)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidls) || aCount == 0)
        return XPR_FALSE;

    xpr_bool_t sHResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder, aPidls, aCount) == XPR_TRUE)
    {
        CMenu sMenu;
        if (sMenu.CreatePopupMenu() == XPR_TRUE)
        {
            if (sContextMenu.getMenu(&sMenu, CMID_DEF_ID_FIRST, CMF_EXPLORE) == XPR_TRUE)
                sHResult = sContextMenu.invokeCommand(aId);
        }
    }
    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sHResult;
}

xpr_bool_t ContextMenu::invokeCommand(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_uint_t aCount, const xpr_tchar_t *aVerb, HWND aWnd)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidls) || aCount == 0 || XPR_IS_NULL(aVerb))
        return XPR_FALSE;

    xpr_bool_t sHResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder, aPidls, aCount) == XPR_TRUE)
    {
        CMenu sMenu;
        if (sMenu.CreatePopupMenu() == XPR_TRUE)
        {
            if (sContextMenu.getMenu(&sMenu, CMID_DEF_ID_FIRST, CMF_EXPLORE) == XPR_TRUE)
                sHResult = sContextMenu.invokeCommand(aVerb);
        }
    }
    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sHResult;
}
} // namespace fxfile
