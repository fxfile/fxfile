//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FolderPane.h"

#include "fxb/fxb_sys_img_list.h"
#include "fxb/fxb_file_scrap.h"
#include "fxb/fxb_clip_format.h"

#include "FolderPaneObserver.h"
#include "MainFrame.h"
#include "FolderCtrl.h"
#include "ExplorerView.h"
#include "AddressBar.h"
#include "OptionMgr.h"
#include "CtrlId.h"

#include "cmd/cmd_parameters.h"
#include "cmd/cmd_parameter_define.h"
#include "cmd/cmd_clipboard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("FolderPane");

FolderPane::FolderPane(void)
    : mObserver(XPR_NULL)
    , mViewIndex(-1)
    , mFolderCtrl(XPR_NULL)
{
}

FolderPane::~FolderPane(void)
{
}

xpr_bool_t FolderPane::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;

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

BEGIN_MESSAGE_MAP(FolderPane, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_NOTIFY(NM_SETFOCUS, AFX_IDW_FOLDER_CTRL, OnFolderSetFocus)
END_MESSAGE_MAP()

xpr_sint_t FolderPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    mFolderCtrl = new FolderCtrl;
    if (XPR_IS_NULL(mFolderCtrl))
        return -1;

    mFolderCtrl->setObserver(dynamic_cast<FolderCtrlObserver *>(this));
    mFolderCtrl->setViewIndex(mViewIndex);

    if (mFolderCtrl->Create(this, AFX_IDW_FOLDER_CTRL, CRect(0, 0, 0, 0)) == XPR_FALSE)
    {
        XPR_SAFE_DELETE(mFolderCtrl);
        return -1;
    }

    mFolderCtrl->setImageList(&fxb::SysImgListMgr::instance().mSysImgList16);

    CWnd *sToolTipWnd = FromHandle((HWND)mFolderCtrl->SendMessage(TVM_GETTOOLTIPS));
    if (XPR_IS_NOT_NULL(sToolTipWnd))
        sToolTipWnd->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    mFolderCtrl->init(XPR_NULL, XPR_FALSE);

    OptionMgr::instance().applyFolderCtrl(mFolderCtrl, XPR_TRUE);

    return 0;
}

void FolderPane::OnDestroy(void)
{
    DESTROY_DELETE(mFolderCtrl);

    super::OnDestroy();
}

void FolderPane::setObserver(FolderPaneObserver *aObserver)
{
    mObserver = aObserver;
}

void FolderPane::setViewIndex(xpr_sint_t aViewIndex)
{
    mViewIndex = aViewIndex;
}

xpr_sint_t FolderPane::getViewIndex(void) const
{
    return mViewIndex;
}

void FolderPane::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void FolderPane::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (XPR_IS_NOT_NULL(mFolderCtrl))
    {
        if (XPR_IS_NOT_NULL(mObserver))
            mObserver->onSetFocus(*this);

        mFolderCtrl->SetFocus();
    }
}

FolderCtrl *FolderPane::getFolderCtrl(void) const
{
    return mFolderCtrl;
}

void FolderPane::recalcLayout(void)
{
    if (XPR_IS_NOT_NULL(mFolderCtrl))
    {
        CRect sRect;
        GetClientRect(&sRect);
        mFolderCtrl->MoveWindow(sRect.left, sRect.top, sRect.Width(), sRect.Height());
    }
}

xpr_bool_t FolderPane::onExplore(FolderCtrl &aFolderCtrl, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        return mObserver->onExplore(*this, aFullPidl, XPR_FALSE);
    }

    return XPR_FALSE;
}

void FolderPane::onUpdateStatus(FolderCtrl &aFolderCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onUpdateStatus(*this);
    }
}

void FolderPane::onRename(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onRename(*this, aTreeItem);
    }
}

void FolderPane::onContextMenuDelete(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onContextMenuDelete(*this, aTreeItem);
    }
}

void FolderPane::onContextMenuRename(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onContextMenuRename(*this, aTreeItem);
    }
}

void FolderPane::onContextMenuPaste(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onContextMenuPaste(*this, aTreeItem);
    }
}

xpr_sint_t FolderPane::onDrop(FolderCtrl &aFolderCtrl, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        return mObserver->onDrop(*this, aOleDataObject, aTargetDir);
    }

    return 0;
}

void FolderPane::onMoveFocus(FolderCtrl &aFolderCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onMoveFocus(*this, 2);
    }
}

void FolderPane::OnFolderSetFocus(LPNMHDR aNmHdr, LRESULT *aResult)
{
    *aResult = 0;

    static xpr_bool_t sAlreadyFocused = XPR_FALSE;
    if (XPR_IS_TRUE(sAlreadyFocused)) // prevent for infinite recursive
    {
        sAlreadyFocused = XPR_FALSE;
        return;
    }

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onSetFocus(*this);

        if (XPR_IS_NOT_NULL(mFolderCtrl))
        {
            sAlreadyFocused = XPR_TRUE;

            mFolderCtrl->SetFocus();
        }
    }
}
