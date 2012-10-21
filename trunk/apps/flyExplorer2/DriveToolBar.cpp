//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DriveToolBar.h"

#include "fxb/fxb_sys_img_list.h"
#include "fxb/fxb_drive_shcn.h"
#include "fxb/fxb_context_menu.h"
#include "fxb/fxb_file_op_thread.h"
#include "fxb/fxb_size_format.h"

#include "rgc/DragImage.h"

#include "MainFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// user defined message
enum
{
    WM_SHCN_DRIVE        = WM_USER + 20,
    WM_DRIVE_ICON_UPDATE = WM_USER + 30,
};

DriveToolBar::DriveToolBar(void)
    : mBarCreated(XPR_FALSE)
    , mInit(XPR_FALSE)
    , mThread(XPR_NULL)
    , mStopEvent(XPR_NULL)
    , mOldDrive(-1)
{
}

DriveToolBar::~DriveToolBar(void)
{
}

BEGIN_MESSAGE_MAP(DriveToolBar, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_CONTEXTMENU()
    ON_MESSAGE(WM_SHCN_DRIVE, OnDriveShcn)
    ON_MESSAGE(WM_DRIVE_ICON_UPDATE, OnDriveIconUpdate)
END_MESSAGE_MAP()

xpr_sint_t DriveToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    ModifyStyle(0, TBSTYLE_LIST | TBSTYLE_FLAT);
    sToolBarCtrl.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

    sToolBarCtrl.SetBitmapSize(CSize(16,16));
    sToolBarCtrl.SetImageList(&fxb::SysImgListMgr::instance().mSysImgList16);

    CRect sRect;
    GetClientRect(&sRect);
    mToolTip.Create(this);
    mToolTip.AddTool(this, LPSTR_TEXTCALLBACK, sRect, 200);

    return 0;
}

unsigned __stdcall DriveToolBar::DriveProc(void *aParam)
{
    DWORD sExitCode = 0;

    DriveToolBar *sDriveToolBar = (DriveToolBar *)aParam;
    if (XPR_IS_NOT_NULL(sDriveToolBar))
        sExitCode = sDriveToolBar->OnDriveProc();

    ::_endthreadex(sExitCode);
    return 0;
}

DWORD DriveToolBar::OnDriveProc(void)
{
    if (::WaitForSingleObject(mStopEvent, 0) == WAIT_OBJECT_0)
        return 0;

    SHFILEINFO sShFileInfo = {0};
    xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};

    DriveDeque::iterator it = mDriveDeque.begin();
    for (; it != mDriveDeque.end(); ++it)
    {
        if (::WaitForSingleObject(mStopEvent, 0) == WAIT_OBJECT_0)
            break;

        _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), it->mDriveChar);
        ::SHGetFileInfo(sDrive, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);

        PostMessage(WM_DRIVE_ICON_UPDATE, (WPARAM)it->mIndex, (LPARAM)sShFileInfo.iIcon);
    }

    return 0;
}

LRESULT DriveToolBar::OnDriveIconUpdate(WPARAM wParam, LPARAM lParam)
{
    xpr_sint_t sIndex = (xpr_sint_t)wParam;
    xpr_sint_t sIconIndex  = (xpr_sint_t)lParam;

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    if (!XPR_IS_RANGE(0, sIndex, sToolBarCtrl.GetButtonCount()-1))
        return 0;

    TBBUTTONINFO sTbButtonInfo = {0};
    sTbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
    sTbButtonInfo.dwMask = TBIF_BYINDEX | TBIF_IMAGE;
    sTbButtonInfo.iImage = sIconIndex;

    sToolBarCtrl.SetButtonInfo(sIndex, &sTbButtonInfo);

    return 0;
}

void DriveToolBar::createDriveBar(void)
{
    if (XPR_IS_NOT_NULL(mThread))
    {
        ::SetEvent(mStopEvent);
        ::WaitForSingleObject(mThread, INFINITE);

        CLOSE_HANDLE(mThread);
        CLOSE_HANDLE(mStopEvent);
    }

    mDriveDeque.clear();
    DriveInfo sDriveInfo;

    xpr_tchar_t *sDrive = XPR_NULL;
    xpr_tchar_t sDriveStrings[XPR_MAX_PATH + 1] = {0};
    fxb::GetDriveStrings(sDriveStrings);

    xpr_sint_t i;
    xpr_sint_t sPos = 0;
    for (i = 0; ; ++i)
    {
        if (sDriveStrings[sPos] == 0)
            break;

        sDrive = sDriveStrings + sPos;
        addDriveButton(sDrive, i);

        sDriveInfo.mIndex = i;
        sDriveInfo.mDriveChar = sDriveStrings[sPos];
        mDriveDeque.push_back(sDriveInfo);

        sPos += (xpr_sint_t)_tcslen(sDriveStrings)+1;
    }

    if (fxb::DriveShcn::instance().isRegisteredWatch(this) == XPR_FALSE)
    {
        xpr_slong_t sEventMask = 0;
        sEventMask|= SHCNE_MEDIAINSERTED;
        sEventMask|= SHCNE_MEDIAREMOVED;
        sEventMask|= SHCNE_DRIVEADD;
        sEventMask|= SHCNE_DRIVEREMOVED;
        sEventMask|= SHCNE_RENAMEFOLDER;

        fxb::DriveShcn::instance().registerWatch(this, WM_SHCN_DRIVE, sEventMask);
    }

    mStopEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread    = (HANDLE)::_beginthreadex(XPR_NULL, 0, DriveProc, this, 0, &mThreadId);
    ::SetThreadPriority(mThread, THREAD_PRIORITY_BELOW_NORMAL);

    UpdateToolbarSize();

    mBarCreated = XPR_TRUE;
    mInit = XPR_TRUE;
}

void DriveToolBar::destroyDriveBar(void)
{
    if (XPR_IS_NOT_NULL(mThread))
    {
        ::SetEvent(mStopEvent);
        ::WaitForSingleObject(mThread, INFINITE);

        CLOSE_HANDLE(mThread);
        CLOSE_HANDLE(mStopEvent);
    }

    mDriveDeque.clear();

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = sToolBarCtrl.GetButtonCount();
    for (i = sCount - 1; i >= 0; --i)
        sToolBarCtrl.DeleteButton(i);

    mBarCreated = XPR_FALSE;
}

void DriveToolBar::refresh(void)
{
    destroyDriveBar();
    createDriveBar();
}

void DriveToolBar::OnDestroy(void)
{
    fxb::DriveShcn::instance().unregisterWatch(this);
    destroyDriveBar();

    super::OnDestroy();
}

//----------------------------------------------------------------------
// ID of Drive
//----------------------------------------------------------------------
// A : ID_DRIVE + 0
// B : ID_DRIVE + 1
// C : ID_DRIVE + 2
// ...
// Z : ID_DRIVE + 25
//----------------------------------------------------------------------
xpr_uint_t DriveToolBar::getDriveId(xpr_tchar_t aDriveChar)
{
    return ID_DRIVE_FIRST + (aDriveChar - 'A');
}

xpr_uint_t DriveToolBar::getDriveId(const xpr_tchar_t *aPath)
{
    return getDriveId(aPath[0]);
}

void DriveToolBar::addDriveButton(const xpr_tchar_t *aDrive, xpr_sint_t aInsert)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_sint_t sId = getDriveId(aDrive);
    if (aInsert == -1)
        aInsert = sToolBarCtrl.GetButtonCount();

    xpr_tchar_t sDriveChar = *aDrive;

    TBBUTTON sTbButton = {0};
    sTbButton.iBitmap   = -1;
    sTbButton.fsState   = TBSTATE_ENABLED;
    sTbButton.fsStyle   = BTNS_BUTTON | BTNS_SHOWTEXT | TBSTYLE_AUTOSIZE;
    sTbButton.idCommand = sId;
    sTbButton.iString   = 0;
    sTbButton.dwData    = sDriveChar;
    sToolBarCtrl.InsertButton(aInsert, &sTbButton);

    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    if (gFrame->isDriveShortText() == XPR_TRUE)
    {
        _stprintf(sText, XPR_STRING_LITERAL("%c"), sDriveChar);
        _tcsupr(sText);
    }
    else
    {
        LPITEMIDLIST sFullPidl = fxb::IL_CreateFromPath(aDrive);
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);
        COM_FREE(sFullPidl);
    }

    SetButtonText(CommandToIndex(sId), sText);
}

xpr_bool_t DriveToolBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if (((LPNMHDR)lParam)->code == TTN_GETDISPINFO)
    {
        LPNMTTDISPINFO lpnmttdi = (LPNMTTDISPINFO)lParam;

        CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

        CPoint sPoint;
        GetCursorPos(&sPoint);
        ScreenToClient(&sPoint);
        xpr_sint_t sIndex = sToolBarCtrl.HitTest(&sPoint);
        if (sIndex < 0)
            return XPR_FALSE;

        CString sText;
        GetButtonText(sIndex, sText);

        xpr_tchar_t sDriveChar;
        xpr_tchar_t sDrive[50];
        sDriveChar = (gFrame->isDriveShortText()) ? sText[0] : sText[sText.GetLength()-3];
        _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), sDriveChar);

        LPITEMIDLIST sFullPidl = fxb::IL_CreateFromPath(sDrive);
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);
        COM_FREE(sFullPidl);

        // [2009/11/15] bug patched
        // Crash Occurrence in Windows NT 4.0 below
        xpr_uint64_t sTemp1 = {0};
        xpr_uint64_t sTemp2 = {0};
        xpr_uint64_t sFreeSize  = {0};
        if (sDriveChar != 'A' && sDriveChar != 'B' && ::GetDiskFreeSpaceEx(sDrive, (PULARGE_INTEGER)&sTemp1, (PULARGE_INTEGER)&sTemp2, (PULARGE_INTEGER)&sFreeSize))
        {
            xpr_tchar_t sFormatedSize[0xff] = {0};
            fxb::SizeFormat::getSizeUnitFormat(sFreeSize, SIZE_UNIT_AUTO, sFormatedSize, 0xfe);
            _stprintf(mToolTipText, XPR_STRING_LITERAL("%s\n(%s: %s)"), sText, theApp.loadString(XPR_STRING_LITERAL("drive.tooltip.free_size")), sFormatedSize);
        }
        else
        {
            _stprintf(mToolTipText, XPR_STRING_LITERAL("%s"), sText);
        }

        lpnmttdi->lpszText = mToolTipText;
    }

    return super::OnNotify(wParam, lParam, pResult);
}

void DriveToolBar::setShortText(xpr_bool_t aShortText)
{
    if (XPR_IS_TRUE(mBarCreated))
        refresh();
}

LRESULT DriveToolBar::OnDriveShcn(WPARAM wParam, LPARAM lParam)
{
    fxb::Shcn *sShcn = (fxb::Shcn *)wParam;
    xpr_slong_t sEventId = (xpr_slong_t)lParam;

    switch (sEventId)
    {
    case SHCNE_RENAMEFOLDER:
    case SHCNE_MEDIAINSERTED:
    case SHCNE_MEDIAREMOVED:
        OnShcnUpdateDrive(sShcn);
        break;

    case SHCNE_DRIVEADD:
        OnShcnDriveAdd(sShcn);
        break;

    case SHCNE_DRIVEREMOVED:
        OnShcnDriveRemove(sShcn);
        break;
    }

    return 0;
}

void DriveToolBar::OnShcnUpdateDrive(fxb::Shcn *aShcn)
{
    refresh();
}

void DriveToolBar::OnShcnDriveAdd(fxb::Shcn *aShcn)
{
    refresh();
}

void DriveToolBar::OnShcnDriveRemove(fxb::Shcn *aShcn)
{
    refresh();
}

void DriveToolBar::OnShcnAllDriveUpdate(fxb::Shcn *aShcn)
{
    refresh();
}

xpr_tchar_t DriveToolBar::getButtonDrive(xpr_uint_t aIndex)
{
    CString sText;
    GetButtonText(aIndex, sText);
    return (gFrame->isDriveShortText()) ? sText[0] : sText[sText.GetLength()-3];
}

void DriveToolBar::GetButtonTextByCommand(xpr_uint_t aId, CString &aText)
{
    GetButtonText(CommandToIndex(aId), aText);
}

void DriveToolBar::OnContextMenu(CWnd* pWnd, CPoint pt)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    CPoint sClientPoint(pt);
    ScreenToClient(&sClientPoint);

    xpr_bool_t sSucceeded = XPR_FALSE;
    xpr_sint_t sIndex = sToolBarCtrl.HitTest(&sClientPoint);
    if (GetKeyState(VK_CONTROL) < 0 && sIndex >= 0)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        _stprintf(sPath, XPR_STRING_LITERAL("%c:\\"), getButtonDrive(sIndex));

        LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sPath);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPITEMIDLIST sPidl = XPR_NULL;
            HRESULT sHResult = fxb::SH_BindToParent(sFullPidl, IID_IShellFolder, (LPVOID *)&sShellFolder, (LPCITEMIDLIST *)&sPidl);
            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
            {
                fxb::ContextMenu sContextMenu(GetSafeHwnd());
                if (sContextMenu.init(sShellFolder, &sPidl, 1))
                {
                    TBBUTTON sTbButton = {0};
                    sToolBarCtrl.GetButton(sIndex, &sTbButton);
                    sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_MARKED | TBSTATE_ENABLED);

                    xpr_uint_t sId = sContextMenu.trackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, &pt, CMF_EXPLORE);
                    if (sId != -1)
                    {
                        sId -= sContextMenu.getIdFirst();

                        xpr_tchar_t sVerb[0xff] = {0};
                        sContextMenu.getCommandVerb(sId, sVerb, 0xfe);

                        xpr_bool_t sSelfInvoke = XPR_FALSE;

                        if (_tcsicmp(sVerb, CMID_VERB_OPEN) == 0)
                        {
                            gFrame->gotoDrive(sPath[0]);
                            sSelfInvoke = XPR_TRUE;
                        }

                        if (XPR_IS_FALSE(sSelfInvoke))
                            sContextMenu.invokeCommand(sId);
                    }

                    sToolBarCtrl.GetButton(sIndex, &sTbButton);
                    sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
                }

                sContextMenu.destroySubclass();
                sContextMenu.release();

                sSucceeded = XPR_TRUE;
            }

            COM_RELEASE(sShellFolder);
            COM_FREE(sFullPidl);
        }
    }

    if (XPR_IS_FALSE(sSucceeded))
    {
        BCMenu sMenu;
        if (sMenu.LoadMenu(IDR_COOLBAR_DRIVEBAR) == XPR_TRUE)
        {
            BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
            if (XPR_IS_NOT_NULL(sPopupMenu))
                sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt, AfxGetMainWnd());
        }
    }
}

DROPEFFECT DriveToolBar::OnDragEnter(COleDataObject* pDataObject, DWORD aKeyState, CPoint aPoint)
{
    DROPEFFECT sDropEffect = DROPEFFECT_LINK;

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            sMyDragImage->DragEnter(GetDesktopWindow(), aPoint);
            sMyDragImage->DragShowNolock(XPR_TRUE);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
        {
            CPoint aDragPoint(aPoint);
            IDataObject *sDataObject = pDataObject->GetIDataObject(XPR_FALSE);
            mDropTarget.getDropHelper()->DragEnter(GetSafeHwnd(), sDataObject, &aDragPoint, sDropEffect);
        }
    }

    mOldDrive = -1;

    return sDropEffect;
}

void DriveToolBar::OnDragLeave()
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        sDragImage.showMyDragNoLock(XPR_FALSE);
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            mDropTarget.getDropHelper()->DragLeave();
    }

    if (mOldDrive >= 0)
    {
        TBBUTTON sTbButton = {0};
        sToolBarCtrl.GetButton(mOldDrive, &sTbButton);
        sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
    }

    gFrame->setStatusPaneDriveText(0);

    mOldDrive = -1;
}

DROPEFFECT DriveToolBar::OnDragOver(COleDataObject* pDataObject, DWORD aKeyState, CPoint aPoint)
{
    DROPEFFECT sDropEffect = DROPEFFECT_LINK;

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            CPoint aDragPoint(aPoint);
            ClientToScreen(&aDragPoint);
            sMyDragImage->DragMove(aDragPoint);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
        {
            CPoint aDragPoint(aPoint);
            mDropTarget.getDropHelper()->DragOver(&aDragPoint, sDropEffect);
        }
    }

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_sint_t sDriveIndex = sToolBarCtrl.HitTest(&aPoint);
    if (sDriveIndex >= sToolBarCtrl.GetButtonCount())
        sDriveIndex = -1;

    if (sDriveIndex < 0)
        sDriveIndex = -1;

    xpr_tchar_t sDriveChar = 0;
    if (sDriveIndex != -1)
    {
        sDriveChar = getButtonDrive(sDriveIndex);

        if (mOldDrive != sDriveIndex)
        {
            TBBUTTON sTbButton = {0};

            sToolBarCtrl.GetButton(mOldDrive, &sTbButton);
            sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);

            sToolBarCtrl.GetButton(sDriveIndex, &sTbButton);
            sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_MARKED | TBSTATE_PRESSED | TBSTATE_ENABLED);
        }

        xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
        _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), sDriveChar);

        sDropEffect = DROPEFFECT_NONE;

        LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sDrive);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            //sDropEffect = DROPEFFECT_MOVE;
            //if ((aKeyState & MK_CONTROL) == MK_CONTROL)
            //    sDropEffect = DROPEFFECT_COPY;

            sDropEffect = DROPEFFECT_MOVE;
            if (aKeyState & MK_ALT)     sDropEffect = DROPEFFECT_LINK;
            else if (aKeyState & MK_CONTROL) sDropEffect = DROPEFFECT_COPY;
            else if (aKeyState & MK_SHIFT)   sDropEffect = DROPEFFECT_MOVE;
            else
            {
                switch (gOpt->mDragDefaultFileOp)
                {
                case DRAG_FILE_OP_DEFAULT: sDropEffect = DROPEFFECT_MOVE; break;
                case DRAG_FILE_OP_COPY:    sDropEffect = DROPEFFECT_COPY; break;
                case DRAG_FILE_OP_LINK:    sDropEffect = DROPEFFECT_LINK; break;
                default:                   sDropEffect = DROPEFFECT_MOVE; break;
                }
            }
        }
        COM_FREE(sFullPidl);
    }
    else
    {
        if (mOldDrive != -1)
        {
            TBBUTTON sTbButton = {0};

            sToolBarCtrl.GetButton(mOldDrive, &sTbButton);
            sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
        }

        sDropEffect = DROPEFFECT_NONE;
    }

    mOldDrive = sDriveIndex;

    gFrame->setStatusPaneDriveText(sDriveChar, sDropEffect);

    return sDropEffect;
}

void DriveToolBar::OnDragScroll(DWORD aKeyState, CPoint aPoint)
{
}

void DriveToolBar::OnDrop(COleDataObject* pDataObject, DROPEFFECT aDropEffect, CPoint aPoint)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            sMyDragImage->EndDrag();
            sMyDragImage->DragShowNolock(XPR_FALSE);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            mDropTarget.getDropHelper()->DragLeave();
    }

    if (mOldDrive >= 0)
    {
        TBBUTTON sTbButton = {0};
        sToolBarCtrl.GetButton(mOldDrive, &sTbButton);
        sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
    }

    gFrame->setStatusPaneDriveText(0);

    COleDataObject *sOleDataObject = pDataObject;
    if (sOleDataObject->IsDataAvailable(mShellIDListClipFormat) == XPR_FALSE)
        return;

    if (mOldDrive < 0)
        return;

    // 3 Operation
    //--------------------------------------------------
    // 1. Insert Link Item
    // 2. Run Program & Link File
    // 3. File Operation

    STGMEDIUM sStgMedium = {0};
    FORMATETC sFormatEtc = {mShellIDListClipFormat, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    if (sOleDataObject->GetData(mShellIDListClipFormat, &sStgMedium, &sFormatEtc) == XPR_FALSE)
        return;

    if (aDropEffect == DROPEFFECT_LINK)
        OnDropShortcut(&sStgMedium, mOldDrive);
    else
        OnDropFileOperation(&sStgMedium, aDropEffect, mOldDrive);

    ::ReleaseStgMedium(&sStgMedium);
}

void DriveToolBar::OnDropFileOperation(STGMEDIUM *pstg, DROPEFFECT aDropEffect, xpr_sint_t aDriveIndex)
{
    if (aDriveIndex < 0)
        return;

    LPIDA sIda = (LPIDA)::GlobalLock(pstg->hGlobal);
    if (XPR_IS_NULL(sIda))
        return;

    xpr_sint_t sCount = sIda->cidl;
    LPITEMIDLIST sFullPidl2 = XPR_NULL; // fully-qualified PIDL
    LPITEMIDLIST sPidl1 = XPR_NULL; // folder PIDL
    LPITEMIDLIST sPidl2 = XPR_NULL; // item PIDL

    // get folder PIDL
    sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

    // File Operation
    xpr_sint_t i;
    xpr_sint_t sSourceCount = 0;
    xpr_tchar_t *sSource = XPR_NULL;
    xpr_tchar_t *sTarget = XPR_NULL;
    xpr_tchar_t *sSourceEnum = XPR_NULL;

    {
        xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
        _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getButtonDrive(aDriveIndex));

        LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sDrive);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            sTarget = new xpr_tchar_t[XPR_MAX_PATH + 1];
            fxb::GetName(sFullPidl, SHGDN_FORPARSING, sTarget);
            sTarget[_tcslen(sTarget)+1] = '\0';
        }

        COM_FREE(sFullPidl);
    }

    sSource = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sCount + 1]; // double null end
    sSourceEnum = sSource;

    for (i = 0; i < sCount; ++i)
    {
        // get item PIDL and get full-qualified PIDL
        sPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
        sFullPidl2 = fxb::ConcatPidls(sPidl1, sPidl2);

        if (fxb::IsFileSystem(sFullPidl2) == XPR_TRUE)
        {
            fxb::GetName(sFullPidl2, SHGDN_FORPARSING, sSourceEnum);
            sSourceEnum += _tcslen(sSourceEnum) + 1;
            sSourceCount++;
        }

        COM_FREE(sFullPidl2);
    }

    if (XPR_IS_NOT_NULL(sTarget) && XPR_IS_NOT_NULL(sSource) && sSourceCount > 0)
    {
        // Last Element - 2 Null
        *sSourceEnum = '\0';

        xpr_uint_t sFunc = 0;
        WORD sFlags = 0;
        // Drop Effect
        // 1-Copy, 2-Move, 3-Link, 5-Copy+Link
        if (aDropEffect == DROPEFFECT_MOVE)
            sFunc = FO_MOVE;
        else
        {
            sFunc = FO_COPY;

            CString sCompare;
            AfxExtractSubString(sCompare, sSource, 0, '\0');
            sCompare = sCompare.Left(sCompare.ReverseFind('\\'));
            if (_tcscmp(sCompare, sTarget) == 0)
                sFlags |= FOF_RENAMEONCOLLISION;
        }

        // windows vista bug
        //if (sSourceCount > 1)
        //    sFlags |= FOF_MULTIDESTFILES;

        SHFILEOPSTRUCT *sShFileOpStruct = new SHFILEOPSTRUCT;
        memset(sShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
        sShFileOpStruct->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
        sShFileOpStruct->wFunc  = sFunc;
        sShFileOpStruct->fFlags = sFlags;
        sShFileOpStruct->pFrom  = sSource;
        sShFileOpStruct->pTo    = sTarget;

        fxb::FileOpThread *sFileOpThread = new fxb::FileOpThread;
        sFileOpThread->start(sShFileOpStruct);
    }

    ::GlobalUnlock(pstg->hGlobal);
}

void DriveToolBar::OnDropShortcut(STGMEDIUM *pstg, xpr_sint_t aDriveIndex)
{
    if (aDriveIndex < 0)
        return;

    LPIDA sIda = (LPIDA)::GlobalLock(pstg->hGlobal);
    if (XPR_IS_NULL(sIda))
        return;

    xpr_sint_t sCount = sIda->cidl;
    LPITEMIDLIST sFullPidl2 = XPR_NULL; // fully-qualified PIDL
    LPITEMIDLIST sPidl1 = XPR_NULL;     // folder PIDL
    LPITEMIDLIST sPidl2 = XPR_NULL;     // item PIDL

    // get folder PIDL
    sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

    // File Operation
    xpr_sint_t i;
    xpr_size_t sColon;
    std::tstring sDir;
    std::tstring sName;
    std::tstring sLinkPath;
    const xpr_tchar_t *sLinkSuffix = theApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

    {
        xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
        _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getButtonDrive(aDriveIndex));

        LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sDrive);
        if (XPR_IS_NOT_NULL(sFullPidl))
            fxb::GetName(sFullPidl, SHGDN_FORPARSING, sDir);
        COM_FREE(sFullPidl);
    }

    for (i = 0; i < sCount; ++i)
    {
        // get item PIDL and get full-qualified PIDL
        sPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
        sFullPidl2 = fxb::ConcatPidls(sPidl1, sPidl2);

        fxb::GetName(sFullPidl2, SHGDN_INFOLDER, sName);

        sColon = sName.find(XPR_STRING_LITERAL(':'));
        if (sColon != std::tstring::npos)
            sName.erase(sColon, 1);

        sName += sLinkSuffix;

        if (fxb::SetNewPath(sLinkPath, sDir, sName, XPR_STRING_LITERAL(".lnk")))
        {
            fxb::CreateShortcut(sLinkPath.c_str(), sFullPidl2);
        }

        COM_FREE(sFullPidl2);
    }

    ::GlobalUnlock(pstg->hGlobal);
}
