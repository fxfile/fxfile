//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_context_menu.h"

#include "../Option.h"

#include "fxb_file_scrap_mgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable:4244)
#pragma warning(disable:4312)

namespace fxb
{
static const xpr_tchar_t PROP_CONTEXT_MENU[] = XPR_STRING_LITERAL("flyExplorer::SubclassData");

ContextMenu::ContextMenu(HWND aHwnd)
    : mHwnd(aHwnd)
    , mShellFolder(XPR_NULL), mPidls(XPR_NULL), mCount(0)
    , mMenu(XPR_NULL)
    , mContextMenu(XPR_NULL), mContextMenu2(XPR_NULL)
    , mIdFirst(CMID_DEF_ID_FIRST)
    , mSubclassData(XPR_NULL)
{
}

ContextMenu::~ContextMenu(void)
{
    destroySubclass();
    release();
}

xpr_bool_t ContextMenu::init(LPSHELLFOLDER aShellFolder, LPITEMIDLIST *aPidls, xpr_uint_t aCount)
{
    mShellFolder = aShellFolder;
    mPidls = aPidls;
    mCount = aCount;

    HRESULT sResult;
    sResult = mShellFolder->GetUIObjectOf(mHwnd, mCount, (LPCITEMIDLIST *)mPidls, IID_IContextMenu, XPR_NULL, (LPVOID *)&mContextMenu);
    if (FAILED(sResult))
        return XPR_FALSE;

    mContextMenu->QueryInterface(IID_IContextMenu2, (LPVOID *)&mContextMenu2);

    return XPR_TRUE;
}

xpr_bool_t ContextMenu::init(LPSHELLFOLDER aShellFolder)
{
    mShellFolder = aShellFolder;

    HRESULT hr;
    hr = mShellFolder->CreateViewObject(mHwnd, IID_IContextMenu, (LPVOID*)&mContextMenu);
    if (FAILED(hr))
        return XPR_FALSE;

    mContextMenu->QueryInterface(IID_IContextMenu2, (LPVOID *)&mContextMenu2);

    return XPR_TRUE;
}

LRESULT CALLBACK ContextMenu::subclassWndProc(HWND aWnd, xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam)
{
    LPSUBCLASSDATA sSubclassData = (LPSUBCLASSDATA)::GetProp(aWnd, PROP_CONTEXT_MENU);
    if (sSubclassData != XPR_NULL)
    {
        LPCONTEXTMENU2 sContextMenu2 = sSubclassData->mContextMenu2;
        switch (aMsg)
        {
        case WM_INITMENUPOPUP:
        case WM_DRAWITEM:
        case WM_MENUCHAR:
        case WM_MEASUREITEM:
            if (sContextMenu2 != XPR_NULL)
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

xpr_bool_t ContextMenu::getMenu(CMenu *aMenu, xpr_uint_t aIdFirst, xpr_uint_t aQueryFlags)
{
    if (IsWindow(mHwnd) == XPR_FALSE)
        return XPR_FALSE;

    HRESULT sResult = E_FAIL;
    if (mContextMenu2 != XPR_NULL)
        sResult = mContextMenu2->QueryContextMenu(aMenu->m_hMenu, 0, aIdFirst, aIdFirst+0x7FFF, aQueryFlags);

    if (FAILED(sResult))
    {
        sResult = mContextMenu->QueryContextMenu(aMenu->m_hMenu, 0, aIdFirst, aIdFirst+0x7FFF, aQueryFlags);
        COM_RELEASE(mContextMenu2);

        if (FAILED(sResult))
            return XPR_FALSE;
    }

    if (mContextMenu2 != XPR_NULL)
    {
        mSubclassData = new SUBCLASSDATA;
        mSubclassData->mContextMenu2 = mContextMenu2;
        mSubclassData->mOldWndProc   = (WNDPROC)::SetWindowLongPtr(mHwnd, GWL_WNDPROC, (LONG_PTR)subclassWndProc);
        ::SetProp(mHwnd, PROP_CONTEXT_MENU, (HANDLE)mSubclassData);
    }

    if (mPidls != XPR_NULL && mCount > 0 && gOpt->mFileScrapContextMenu == XPR_TRUE)
    {
        xpr_sint_t nInsert = aMenu->GetMenuItemCount();
        nInsert -= 2;

        ::InsertMenu(aMenu->m_hMenu, nInsert,   MF_BYPOSITION | MF_SEPARATOR, 0, XPR_NULL);
        ::InsertMenu(aMenu->m_hMenu, nInsert+1, MF_BYPOSITION, aIdFirst + CMID_FILE_SCRAP, theApp.loadString(XPR_STRING_LITERAL("cmd.shell.add_to_scrap")));
    }

    return XPR_TRUE;
}

xpr_uint_t ContextMenu::getIdFirst(void)
{
    return mIdFirst;
}

xpr_uint_t ContextMenu::trackPopupMenu(xpr_uint_t aFlags, LPPOINT aPoint, xpr_uint_t aQueryFlags)
{
    if (gOpt->mAnimationMenu == XPR_FALSE)
        aFlags |= 0x4000L;

    CMenu sMenu;
    sMenu.CreatePopupMenu();
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
    if (mContextMenu == XPR_NULL)
        return XPR_FALSE;

    if (aId == 0xFFFFFFFF)
        return XPR_FALSE;

    HRESULT sResult;
    sResult = mContextMenu->GetCommandString(aId, GCS_VERB, XPR_NULL, (xpr_char_t *)aVerb, aMaxVerbLength);

    return SUCCEEDED(sResult);
}

xpr_bool_t ContextMenu::invokeCommand(xpr_uint_t aId, LPPOINT aPoint)
{
    if (gOpt->mFileScrapContextMenu == XPR_TRUE && aId == CMID_FILE_SCRAP)
    {
        FileScrapMgr::instance().add(mShellFolder, mPidls, mCount);
        return XPR_TRUE;
    }

    xpr_tchar_t sVerb[0xff] = {0};
    getCommandVerb(aId, sVerb, 0xfe);

    xpr_bool_t sResult = XPR_FALSE;

    if (0 <= aId && aId < 0x7FFF)
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
            sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t ContextMenu::invokeCommand(const xpr_tchar_t *aVerb, LPPOINT aPoint)
{
    if (aVerb == XPR_NULL)
        return XPR_FALSE;

    xpr_char_t sVerb[0xff] = {0};
#ifdef XPR_CFG_UNICODE
    xpr_size_t sOutputBytes = 0xfe;
    XPR_UTF16_TO_MBS(aVerb, _tcslen(aVerb) * sizeof(xpr_tchar_t), sVerb, &sOutputBytes);
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
    if (mSubclassData != XPR_NULL)
    {
        ::SetWindowLongPtr(mHwnd, GWL_WNDPROC, (LONG_PTR)mSubclassData->mOldWndProc);
        ::RemoveProp(mHwnd, PROP_CONTEXT_MENU);
        XPR_SAFE_DELETE(mSubclassData);
    }
}

void ContextMenu::release(void)
{
    if (mMenu != XPR_NULL)
        ::DestroyMenu(mMenu);
    COM_RELEASE(mContextMenu);
    COM_RELEASE(mContextMenu2);
}

xpr_bool_t ContextMenu::trackItemMenu(LPSHELLFOLDER  aShellFolder,
                                      LPITEMIDLIST  *aPidls,
                                      xpr_uint_t     aCount,
                                      LPPOINT        aPoint,
                                      xpr_uint_t     aFlags,
                                      HWND           aWnd,
                                      xpr_uint_t     aQueryFlags)
{
    xpr_bool_t sResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder, aPidls, aCount))
    {
        xpr_uint_t sId = sContextMenu.trackPopupMenu(aFlags, aPoint, aQueryFlags);
        if (sId != -1)
            sResult = sContextMenu.invokeCommand(sId - sContextMenu.getIdFirst(), aPoint);
    }
    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sResult;
}

xpr_bool_t ContextMenu::trackBackMenu(LPSHELLFOLDER aShellFolder, LPPOINT aPoint, xpr_uint_t aFlags, HWND aWnd)
{
    xpr_bool_t sResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder))
    {
        xpr_uint_t sId = sContextMenu.trackPopupMenu(aFlags, aPoint);
        if (sId != -1)
            sResult = sContextMenu.invokeCommand(sId - sContextMenu.getIdFirst(), aPoint);
    }
    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sResult;
}

xpr_bool_t ContextMenu::invokeCommand(LPSHELLFOLDER aShellFolder, LPITEMIDLIST *aPidls, xpr_uint_t aCount, xpr_uint_t aId, HWND aWnd)
{
    xpr_bool_t sResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder, aPidls, aCount))
    {
        CMenu sMenu;
        sMenu.CreatePopupMenu();
        if (sContextMenu.getMenu(&sMenu, CMID_DEF_ID_FIRST, CMF_DEFAULTONLY))
            sResult = sContextMenu.invokeCommand(aId);
    }
    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sResult;
}

xpr_bool_t ContextMenu::invokeCommand(LPSHELLFOLDER aShellFolder, LPITEMIDLIST *aPidls, xpr_uint_t aCount, const xpr_tchar_t *aVerb, HWND aWnd)
{
    if (aVerb == XPR_NULL)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    ContextMenu sContextMenu(aWnd);
    if (sContextMenu.init(aShellFolder, aPidls, aCount))
    {
        CMenu sMenu;
        sMenu.CreatePopupMenu();
        if (sContextMenu.getMenu(&sMenu, CMID_DEF_ID_FIRST, CMF_DEFAULTONLY))
            sResult = sContextMenu.invokeCommand(aVerb);
    }
    sContextMenu.destroySubclass();
    sContextMenu.release();

    return sResult;
}
} // namespace fxb
