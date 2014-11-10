//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "folder_pane.h"

#include "sys_img_list.h"
#include "file_scrap.h"
#include "clip_format.h"

#include "folder_pane_observer.h"
#include "main_frame.h"
#include "folder_ctrl.h"
#include "option.h"
#include "filter.h"

#include "cmd/router/cmd_parameters.h"
#include "cmd/router/cmd_parameter_define.h"
#include "cmd/cmd_clipboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("FolderPane");

//
// control id
//
enum
{
    CTRL_ID_FOLDER_CTRL = 50,
};
} // namespace anonymous

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
    ON_NOTIFY(NM_SETFOCUS, CTRL_ID_FOLDER_CTRL, OnFolderSetFocus)
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

    if (mFolderCtrl->Create(this, CTRL_ID_FOLDER_CTRL, CRect(0, 0, 0, 0)) == XPR_FALSE)
    {
        XPR_SAFE_DELETE(mFolderCtrl);
        return -1;
    }

    mFolderCtrl->setImageList(&SysImgListMgr::instance().mSysImgList16);

    CWnd *sToolTipWnd = FromHandle((HWND)mFolderCtrl->SendMessage(TVM_GETTOOLTIPS));
    if (XPR_IS_NOT_NULL(sToolTipWnd))
        sToolTipWnd->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    mFolderCtrl->init(XPR_NULL, XPR_FALSE);

    setFolderOption(mFolderCtrl, *gOpt);

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

void FolderPane::setFolderOption(FolderCtrl *aFolderCtrl, Option &aOption)
{
    XPR_ASSERT(aFolderCtrl != XPR_NULL);

    FilterMgr &sFilterMgr = FilterMgr::instance();

    FolderCtrl::Option sOption;

    sOption.mMouseClick               = aOption.mConfig.mMouseClick;
    sOption.mShowHiddenAttribute      = aOption.mConfig.mShowHiddenAttribute;
    sOption.mShowSystemAttribute      = aOption.mConfig.mShowSystemAttribute;
    sOption.mRenameByMouse            = aOption.mConfig.mRenameByMouse;

    if (aOption.mConfig.mFolderTreeBkgndColorType[mViewIndex] == COLOR_TYPE_FILE_LIST)
    {
        sOption.mBkgndColorType       = aOption.mConfig.mFileListBkgndColorType[mViewIndex];
        sOption.mBkgndColor           = aOption.mConfig.mFileListBkgndColor[mViewIndex];
    }
    else
    {
        sOption.mBkgndColorType       = aOption.mConfig.mFolderTreeBkgndColorType[mViewIndex];
        sOption.mBkgndColor           = aOption.mConfig.mFolderTreeBkgndColor[mViewIndex];
    }

    if (sOption.mBkgndColorType == COLOR_TYPE_FILTERING)
    {
        sOption.mBkgndColorType       = COLOR_TYPE_CUSTOM;
        sOption.mBkgndColor           = sFilterMgr.getColor(XPR_NULL, XPR_TRUE);
    }

    if (aOption.mConfig.mFolderTreeTextColorType[mViewIndex] == COLOR_TYPE_FILE_LIST)
    {
        sOption.mTextColorType        = aOption.mConfig.mFileListTextColorType[mViewIndex];
        sOption.mTextColor            = aOption.mConfig.mFileListTextColor[mViewIndex];
    }
    else
    {
        sOption.mTextColorType        = aOption.mConfig.mFolderTreeTextColorType[mViewIndex];
        sOption.mTextColor            = aOption.mConfig.mFolderTreeTextColor[mViewIndex];
    }

    if (sOption.mTextColorType == COLOR_TYPE_FILTERING)
    {
        sOption.mTextColorType        = COLOR_TYPE_CUSTOM;
        sOption.mTextColor            = sFilterMgr.getColor(XPR_NULL, XPR_TRUE);
    }

    if (XPR_IS_TRUE(aOption.mConfig.mFolderTreeCustomFont))
    {
        sOption.mCustomFont            = aOption.mConfig.mFolderTreeCustomFont;
        _tcscpy(sOption.mCustomFontText, aOption.mConfig.mFolderTreeCustomFontText);
    }
    else
    {
        sOption.mCustomFont            = aOption.mConfig.mCustomFont;
        _tcscpy(sOption.mCustomFontText, aOption.mConfig.mCustomFontText);
    }

    sOption.mIsItemHeight             = aOption.mConfig.mFolderTreeIsItemHeight;
    sOption.mItemHeight               = aOption.mConfig.mFolderTreeItemHeight;
    sOption.mHighlight                = aOption.mConfig.mFolderTreeHighlight[mViewIndex];
    sOption.mHighlightColor           = aOption.mConfig.mFolderTreeHighlightColor[mViewIndex];
    sOption.mInitNoExpand             = aOption.mConfig.mFolderTreeInitNoExpand;
    sOption.mSelDelay                 = aOption.mConfig.mFolderTreeSelDelay;
    sOption.mSelDelayTime             = aOption.mConfig.mFolderTreeSelDelayTime;

    sOption.mExternalCopyFileOp       = aOption.mConfig.mExternalCopyFileOp;
    sOption.mExternalMoveFileOp       = aOption.mConfig.mExternalMoveFileOp;

    sOption.mDragType                 = aOption.mConfig.mDragType;
    sOption.mDragDist                 = aOption.mConfig.mDragDist;
    sOption.mDropType                 = aOption.mConfig.mDropType;
    sOption.mDragFolderTreeExpandTime = aOption.mConfig.mDragFolderTreeExpandTime;
    sOption.mDragScrollTime           = aOption.mConfig.mDragScrollTime;
    sOption.mDragDefaultFileOp        = aOption.mConfig.mDragDefaultFileOp;
    sOption.mDragNoContents           = aOption.mConfig.mDragNoContents;

    aFolderCtrl->setOption(sOption);
}

void FolderPane::setChangedOption(Option &aOption)
{
    XPR_ASSERT(mFolderCtrl != XPR_NULL);

    setFolderOption(mFolderCtrl, aOption);
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

void FolderPane::onClick(FolderCtrl &aFolderCtrl)
{
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        CPoint sPoint(0,0);
        GetCursorPos(&sPoint);
        aFolderCtrl.ScreenToClient(&sPoint);

        xpr_uint_t sFlags = 0;
        HTREEITEM sTreeItem = aFolderCtrl.HitTest(sPoint, &sFlags);

        HTREEITEM sSelTreeItem = aFolderCtrl.GetSelectedItem();
        if (sTreeItem == sSelTreeItem && XPR_ANY_BITS(sFlags, TVHT_ONITEM))
        {
            mFolderCtrl->explore(sTreeItem);
        }
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
} // namespace fxfile
