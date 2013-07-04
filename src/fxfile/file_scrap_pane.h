//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SCRAP_PANE_H__
#define __FXFILE_FILE_SCRAP_PANE_H__ 1
#pragma once

#include "file_scrap.h"
#include "file_scrap_observer.h"

#include "gui/StatusBar.h"
#include "gui/BCMenu.h"

#include "tab_pane.h"
#include "file_scrap_ctrl_observer.h"

namespace fxfile
{
class FileScrapToolBar;

class FileScrapPane
    : public TabPane
    , public FileScrapCtrlObserver
    , public FileScrapObserver
{
    typedef TabPane super;

public:
    FileScrapPane(void);
    virtual ~FileScrapPane(void);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    // from TabPane
    virtual CWnd *     newSubPane(xpr_uint_t aId);
    virtual CWnd *     getSubPane(xpr_uint_t aId = InvalidId) const;
    virtual xpr_size_t getSubPaneCount(void) const;
    virtual xpr_uint_t getCurSubPaneId(void) const;
    virtual xpr_uint_t setCurSubPane(xpr_uint_t aId);
    virtual void       destroySubPane(xpr_uint_t aId);
    virtual void       destroySubPane(void);

    virtual StatusBar *getStatusBar(void) const;
    virtual const xpr_tchar_t *getStatusPaneText(xpr_sint_t aIndex) const;

    virtual void setChangedOption(Option &aOption);

public:
    FileScrapCtrl *getFileScrapCtrl(void) const;

    FileScrap::Group *getCurGroup(void) const;

    xpr_sint_t getItemCount(void) const;
    xpr_sint_t getSelectedItemCount(void) const;
    void getSelFileScrapItemIdSet(FileScrap::IdSet &sIdSet);

    void refresh(void);

protected:
    void recalcLayout(void);
    void createFont(void);
    void recreateFont(void);
    void destroyFont(void);

    void updateGroup(void);
    xpr_sint_t addGroup(FileScrap::Group *aGroup, xpr_sint_t aIndex = -1);
    xpr_bool_t isCurGroup(FileScrap::Group *aGroup);
    void updateDefGroup(void);
    void updateStatus(void);

protected:
    // from FileScrapCtrlObserver
    virtual xpr_bool_t onOpenFolder(FileScrapCtrl &aFileScrapCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onOpenFolder(FileScrapCtrl &aFileScrapCtrl, LPCITEMIDLIST aFullPid);
    virtual void onUpdateStatus(FileScrapCtrl &aFileScrapCtrl);
    virtual void onSetFocus(FileScrapCtrl &aFileScrapCtrl);
    virtual void onMoveFocus(FileScrapCtrl &aFileScrapCtrl);

    // from FileScrapObserver
    virtual void onAddGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup);
    virtual void onSetCurGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup);
    virtual void onRenameGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup);
    virtual void onRemoveGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup);
    virtual void onRemoveAllGroups(FileScrap &aFileScrap);
    virtual void onUpdateList(FileScrap &aFileScrap, FileScrap::Group *aGroup);
    virtual void onRemovedItems(FileScrap &aFileScrap, FileScrap::Group *aGroup, const FileScrap::IdSet &aIdSet);
    virtual void onRemovedAllItems(FileScrap &aFileScrap, FileScrap::Group *aGroup);

protected:
    virtual xpr_bool_t OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo);

protected:
    xpr_uint_t         mSubPaneId;
    FileScrapCtrl     *mFileScrapCtrl;
    CRect              mGroupLabelRect;
    const xpr_tchar_t *mGroupLabelText;
    CComboBoxEx       *mGroupComboBox;
    FileScrapToolBar  *mToolBar;
    StatusBar         *mStatusBar;
    CImageList         mGroupImgList;
    CFont              mFont;
    BCMenu             mMenu;

    xpr_tchar_t mStatusText0[0xff];
    xpr_tchar_t mStatusText1[0xff];

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnPaint(void);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnCbnSelchangeGroup(void);
    afx_msg void OnToolbarDropDown(NMHDR *aNmHdr, LRESULT *aLResult);
    afx_msg void OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection);
    afx_msg void OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu);
    afx_msg LRESULT OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu);
    afx_msg void OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct);
};
} // namespace fxfile

#endif // __FXFILE_FILE_SCRAP_PANE_H__
