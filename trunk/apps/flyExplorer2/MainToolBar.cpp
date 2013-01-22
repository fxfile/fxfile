//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "MainCoolBar.h"

#include "fxb/fxb_file_ass.h"

#include "rgc/DragImage.h"

#include "MainFrame.h"
#include "ExplorerView.h"
#include "command_string_table.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const COLORREF IL_COLOR_MASK = RGB(255,0,255);

static const xpr_byte_t TBSTYLE_1 = TBSTYLE_BUTTON;
static const xpr_byte_t TBSTYLE_2 = TBSTYLE_SEP;
static const xpr_byte_t TBSTYLE_3 = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;
static const xpr_byte_t TBSTYLE_4 = TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN;

static TBBUTTONEX kTbButtons[] = {
    { {  0, ID_GO_BACK,               TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 1 },
    { {  1, ID_GO_FORWARD,            TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 1 },
    { {  2, ID_GO_UP,                 TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 1 },
    { {  3, ID_GO_HISTORY,            TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 0 },
    { {  0, 0,                        TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { {  7, ID_VIEW_FOLDER_TREE_SHOW, TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {  8, ID_VIEW_BAR_SEARCH,       TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {  0, 0,                        TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { { 10, ID_GO_BOOKMARK,           TBSTATE_ENABLED, TBSTYLE_4, 0, 0, 0 }, 1 },
    { { 11, ID_GO_DRIVE,              TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 1 },
    { { 43, ID_GO_WORKING_FOLDER,     TBSTATE_ENABLED, TBSTYLE_4, 0, 0, 0 }, 1 },
    { { 44, ID_GO_WORKING_FOLDER_0,   TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 45, ID_GO_WORKING_FOLDER_1,   TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 46, ID_GO_WORKING_FOLDER_2,   TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 47, ID_GO_WORKING_FOLDER_3,   TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 48, ID_GO_WORKING_FOLDER_4,   TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { {  0, 0,                        TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { { 12, ID_EDIT_CUT,              TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 13, ID_EDIT_COPY,             TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 14, ID_EDIT_PASTE,            TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 34, ID_EDIT_FOLDER_MOVE,      TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 35, ID_EDIT_FOLDER_COPY,      TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { {  0, 0,                        TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { { 18, ID_FILE_RENAME,           TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 19, ID_FILE_DELETE,           TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 29, ID_EDIT_UNDO,             TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 33, ID_FILE_ATTRTIME,         TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 20, ID_FILE_PROPERTY,         TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {  0, 0,                        TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { { 26, ID_VIEW_STYLE,            TBSTATE_ENABLED, TBSTYLE_4, 0, 0, 0 }, 1 },
    { { 21, ID_WINDOW_SPLIT,          TBSTATE_ENABLED, TBSTYLE_4, 0, 0, 0 }, 1 },
    { { 22, ID_WINDOW_SPLIT_SINGLE,   TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 23, ID_WINDOW_SPLIT_1x2,      TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 24, ID_WINDOW_SPLIT_2x1,      TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 39, ID_WINDOW_SPLIT_EQUAL,    TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 40, ID_WINDOW_SPLIT_SWAP,     TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { {  0, 0,                        TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { { 37, ID_FILE_NEW_FOLDER,       TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 30, ID_EDIT_FILE_SCRAP_VIEW,  TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 31, ID_TOOL_CMD,              TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 27, ID_TOOL_EMPTY_RECYCLEBIN, TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { { 32, ID_FILE_PRINT,            TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 36, ID_VIEW_PICVIEWER,        TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 41, ID_FILE_VIEW,             TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 0 },
    { { 42, ID_FILE_EDIT,             TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 0 },
    { {  0, 0,                        TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { { 38, ID_TOOL_OPTION,           TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 0 },
    { { 28, ID_APP_ABOUT,             TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
};

MainToolBar::MainToolBar(void)
    : mOldDragId(0)
{
    mDragDropIds.push_back(ID_GO_BOOKMARK);
    mDragDropIds.push_back(ID_FILE_VIEW);
    mDragDropIds.push_back(ID_FILE_EDIT);

    xpr_size_t sId;
    for (sId = ID_GO_WORKING_FOLDER_FIRST; sId <= ID_GO_WORKING_FOLDER_LAST; ++sId)
        mDragDropIds.push_back(sId);
}

MainToolBar::~MainToolBar(void)
{
    mDragDropIds.clear();
}

BEGIN_MESSAGE_MAP(MainToolBar, super)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void MainToolBar::Init(void)
{
    SetIconSize(CSize(22, 22), CSize(16, 16));

    SetImageList(
        IDB_TB_MAIN_COLD_SMALL, IDB_TB_MAIN_HOT_SMALL, IDB_TB_MAIN_HOT_SMALL,
        IDB_TB_MAIN_COLD_LARGE, IDB_TB_MAIN_HOT_LARGE, IDB_TB_MAIN_HOT_LARGE,
        TBI_SMALL, IL_COLOR_MASK);

    xpr_sint_t sCount = sizeof(kTbButtons) / sizeof(kTbButtons[0]);
    SetButtons(sCount, kTbButtons);
}

xpr_bool_t MainToolBar::HasButtonText(xpr_uint_t aId)
{
    if (GetTextType() == TBT_RIGHT)
    {
        xpr_bool_t sResult = XPR_FALSE;
        switch (aId)
        {
        case ID_GO_BACK:
        case ID_VIEW_FOLDER_TREE_SHOW:
        case ID_VIEW_BAR_SEARCH:
        case ID_FILE_RENAME:
        case ID_FILE_NEW_FOLDER:
            sResult = XPR_TRUE;
            break;
        }

        return sResult;
    }

    return XPR_TRUE;
}

xpr_bool_t MainToolBar::HasButtonTip(xpr_uint_t aId)
{
    xpr_bool_t sResult = XPR_TRUE;

    if (GetTextType() == TBT_RIGHT)
    {
        switch (aId)
        {
        case ID_GO_BACK:
        case ID_VIEW_FOLDER_TREE_SHOW:
        case ID_VIEW_BAR_SEARCH:
        case ID_FILE_RENAME:
        case ID_FILE_NEW_FOLDER:
            sResult = XPR_FALSE;
            break;
        }
    }
    else if (GetTextType() == TBT_TEXT)
        sResult = XPR_FALSE;

    return sResult;
}

void MainToolBar::GetButtonTextByCommand(xpr_uint_t aId, CString &aText)
{
    CommandStringTable &sCommandStringTable = CommandStringTable::instance();

    const xpr_tchar_t *sStringId = sCommandStringTable.loadButtonString(aId);
    ASSERT(sStringId != NULL);

    if (XPR_IS_NULL(sStringId))
        return;

    aText = theApp.loadString(sStringId);
}

void MainToolBar::OnContextMenu(CWnd* pWnd, CPoint pt)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_COOLBAR_TOOLBAR) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt, AfxGetMainWnd());
        }
    }
}

xpr_bool_t MainToolBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    NMHDR *pnmhdr = (NMHDR *)lParam;

    if (pnmhdr->code == TTN_GETDISPINFO)
    {
        LPNMTTDISPINFO lpnmttdi = (LPNMTTDISPINFO)pnmhdr;

        if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_FIRST, pnmhdr->idFrom, ID_GO_WORKING_FOLDER_LAST))
        {
            xpr_sint_t sIndex = (xpr_sint_t)pnmhdr->idFrom - ID_GO_WORKING_FOLDER_FIRST;
        }
    }

    return super::OnNotify(wParam, lParam, pResult);
}

TBBUTTONEX *MainToolBar::getButton(xpr_sint_t *aCount)
{
    if (XPR_IS_NOT_NULL(aCount))
        *aCount = sizeof(kTbButtons) / sizeof(kTbButtons[0]);

    return kTbButtons;
}

COLORREF MainToolBar::getColorMask(void)
{
    return IL_COLOR_MASK;
}

DROPEFFECT MainToolBar::OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
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
            IDataObject* pIDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);
            mDropTarget.getDropHelper()->DragEnter(GetSafeHwnd(), pIDataObject, &aDragPoint, sDropEffect);
        }
    }

    mOldDragId = 0;

    return sDropEffect;
}

void MainToolBar::OnDragLeave(void)
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

    if (mOldDragId != 0)
    {
        sToolBarCtrl.SetState(mOldDragId, TBSTATE_ENABLED);
        mOldDragId = 0;
    }
}

DROPEFFECT MainToolBar::OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    DROPEFFECT sDropEffect = DROPEFFECT_NONE;

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

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

    xpr_uint_t sId = 0;
    xpr_uint_t sNewDragId = 0;
    DragDropDeque::iterator sIterator;
    xpr_sint_t sIndex;

    sIterator = mDragDropIds.begin();
    for (; sIterator != mDragDropIds.end(); ++sIterator)
    {
        sId = *sIterator;

        sIndex = CommandToIndex(sId);

        CRect sRect;
        GetItemRect(sIndex, sRect);
        if (sRect.PtInRect(aPoint))
        {
            sNewDragId = sId;
            break;
        }
    }

    if (mOldDragId != sNewDragId)
    {
        if (mOldDragId != 0)
            sToolBarCtrl.SetState(mOldDragId, TBSTATE_ENABLED);

        if (sNewDragId != 0)
            sToolBarCtrl.SetState(sNewDragId, TBSTATE_MARKED | TBSTATE_PRESSED | TBSTATE_ENABLED);

        mOldDragId = sNewDragId;
    }

    if (sNewDragId != 0)
        sDropEffect = DROPEFFECT_LINK;

    return sDropEffect;
}

void MainToolBar::OnDragScroll(DWORD aKeyState, CPoint aPoint)
{
}

void MainToolBar::OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint)
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

    sToolBarCtrl.SetState(mOldDragId, TBSTATE_ENABLED);

    xpr_uint_t sDragId = mOldDragId;
    mOldDragId = 0;

    COleDataObject *sOleDataObject = aOleDataObject;
    if (sOleDataObject->IsDataAvailable(mShellIDListClipFormat) == XPR_FALSE)
        return;

    STGMEDIUM sStgMedium = {0};
    FORMATETC sFormatEtc = {mShellIDListClipFormat, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    if (sOleDataObject->GetData(mShellIDListClipFormat, &sStgMedium, &sFormatEtc) == XPR_FALSE)
        return;

    LPIDA sIda = (LPIDA)::GlobalLock(sStgMedium.hGlobal);
    if (XPR_IS_NOT_NULL(sIda))
    {
        xpr_sint_t sCount = sIda->cidl;
        LPITEMIDLIST sFullPidl = XPR_NULL; // fully-qualified PIDL
        LPITEMIDLIST sPidl1 = XPR_NULL; // folder PIDL
        LPITEMIDLIST sPidl2 = XPR_NULL; // item PIDL

        // get folder PIDL
        sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            // get item PIDL and get full-qualified PIDL
            sPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
            sFullPidl = fxb::ConcatPidls(sPidl1, sPidl2);

            switch (sDragId)
            {
            case ID_GO_BOOKMARK:
                {
                    OnDropBookmark(sFullPidl);
                    break;
                }

            case ID_FILE_VIEW:
            case ID_FILE_EDIT:
                {
                    xpr_uint_t sType = (sDragId == ID_FILE_VIEW) ? fxb::FileAssTypeViewer : fxb::FileAssTypeEditor;
                    OnDropFileAss(sFullPidl, sType);
                    break;
                }

            default:
                {
                    if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_FIRST, sDragId, ID_GO_WORKING_FOLDER_LAST))
                    {
                        xpr_sint_t sIndex = sDragId - ID_GO_WORKING_FOLDER_FIRST;
                        OnDropWorkingFolder(sIndex, sFullPidl);
                        break;
                    }
                }
            }

            COM_FREE(sFullPidl);
        }

        ::GlobalUnlock(sStgMedium.hGlobal);
    }

    ::ReleaseStgMedium(&sStgMedium);
}

void MainToolBar::OnDropBookmark(LPITEMIDLIST aFullPidl)
{
    xpr_sint_t sResult = gFrame->addBookmark(aFullPidl);
    if (sResult != -1)
    {
        if (sResult != 0)
            gFrame->updateBookmark();
    }
}

void MainToolBar::OnDropFileAss(LPITEMIDLIST aFullPidl, xpr_uint_t aType)
{
    xpr_ulong_t sShellAttributes = SFGAO_FILESYSTEM | SFGAO_FOLDER;
    fxb::GetItemAttributes(aFullPidl, sShellAttributes);

    if (!XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM) ||
         XPR_TEST_BITS(sShellAttributes, SFGAO_FOLDER))
        return;

    std::tstring sPath;
    fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);

    fxb::FileAssItem *sFileAssItem;
    sFileAssItem = fxb::FileAssMgr::instance().getItemFromPath(sPath.c_str(), aType);
    if (XPR_IS_NULL(sFileAssItem))
        return;

    std::tstring sFile;
    std::tstring sParameters;

    fxb::GetEnvRealPath(sFileAssItem->mPath, sFile);
    sParameters = sPath;

    ::ShellExecute(m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
}

void MainToolBar::OnDropWorkingFolder(xpr_sint_t aIndex, LPITEMIDLIST aFullPidl)
{
    gFrame->setWorkingFolder(aIndex, aFullPidl);
}

xpr_bool_t MainToolBar::loadFromFile(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_uint_t sId;
    ButtonMap::iterator sIterator;
    TBBUTTONEX *sTbButtons;

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadSize;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(aPath, xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    // Delete all buttons
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();
    while (sToolBarCtrl.DeleteButton(0) == XPR_TRUE) ;

    xpr_sint_t sTextType = TBT_RIGHT;
    xpr_sint_t sIconType = TBI_SMALL;

    sRcode = sFileIo.read(&sTextType, sizeof(sTextType), &sReadSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sRcode = sFileIo.read(&sIconType, sizeof(sIconType), &sReadSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    SetIconType(sIconType, XPR_FALSE);
    SetTextType(sTextType, XPR_FALSE);

    do
    {
        sRcode = sFileIo.read(&sId, sizeof(sId), &sReadSize);
        if (XPR_RCODE_IS_ERROR(sRcode) || sReadSize <= 0)
            break;

        if (sId == 0xffffffff)
            sId = 0;

        sIterator = m_mpButtons.find(sId);
        if (sIterator != m_mpButtons.end())
        {
            sTbButtons = sIterator->second;
            sToolBarCtrl.AddButtons(1, &sTbButtons->tbinfo);
        }
    } while (XPR_RCODE_IS_SUCCESS(sRcode) && sReadSize > 0);

    sFileIo.close();

    UpdateToolbarSize();

    return XPR_TRUE;
}

void MainToolBar::saveToFile(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_uint_t sId;
    TBBUTTON sTbButton;
    xpr_sint_t i, sCount;

    xpr_rcode_t sRcode;
    xpr_ssize_t sWrittenSize;
    xpr_sint_t  sOpenMode;
    xpr::FileIo sFileIo;

    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    sRcode = sFileIo.open(aPath, sOpenMode);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return;

    sRcode = sFileIo.write(&m_nTextType, sizeof(m_nTextType), &sWrittenSize);
    sRcode = sFileIo.write(&m_nIconType, sizeof(m_nIconType), &sWrittenSize);

    sCount = sToolBarCtrl.GetButtonCount();
    for (i = 0; i < sCount; ++i)
    {
        sToolBarCtrl.GetButton(i, &sTbButton);

        sId = (xpr_uint_t)sTbButton.idCommand;
        if (XPR_TEST_BITS(sTbButton.fsStyle, TBSTYLE_SEP))
            sId = 0xffffffff;

        sRcode = sFileIo.write(&sId, sizeof(sId), &sWrittenSize);
    }

    sFileIo.close();
}
