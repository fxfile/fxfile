//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SCRAP_PANE_H__
#define __FX_FILE_SCRAP_PANE_H__
#pragma once

#include "fxb/fxb_file_scrap.h"
#include "fxb/fxb_file_scrap_observer.h"

#include "rgc/StatusBar.h"
#include "rgc/BCMenu.h"

#include "TabPane.h"
#include "FileScrapCtrlObserver.h"

class FileScrapToolBar;

class FileScrapPane
    : public TabPane
    , public FileScrapCtrlObserver
    , public fxb::FileScrapObserver
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

public:
    FileScrapCtrl *getFileScrapCtrl(void) const;

    fxb::FileScrap::Group *getCurGroup(void) const;

    xpr_sint_t getItemCount(void) const;
    xpr_sint_t getSelectedItemCount(void) const;
    void getSelFileScrapItemIdSet(fxb::FileScrap::IdSet &sIdSet);

    void refresh(void);

protected:
    void recalcLayout(void);
    void createFont(void);
    void recreateFont(void);
    void destroyFont(void);

    void updateGroup(void);
    xpr_sint_t addGroup(fxb::FileScrap::Group *aGroup, xpr_sint_t aIndex = -1);
    xpr_bool_t isCurGroup(fxb::FileScrap::Group *aGroup);
    void updateDefGroup(void);
    void updateStatus(void);

protected:
    // from FileScrapCtrlObserver
    virtual xpr_bool_t onOpenFolder(FileScrapCtrl &aFileScrapCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onOpenFolder(FileScrapCtrl &aFileScrapCtrl, LPITEMIDLIST aFullPid);
    virtual void onUpdateStatus(FileScrapCtrl &aFileScrapCtrl);
    virtual void onSetFocus(FileScrapCtrl &aFileScrapCtrl);
    virtual void onMoveFocus(FileScrapCtrl &aFileScrapCtrl);

    // from FileScrapObserver
    virtual void onAddGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup);
    virtual void onSetCurGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup);
    virtual void onRenameGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup);
    virtual void onRemoveGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup);
    virtual void onRemoveAllGroups(fxb::FileScrap &aFileScrap);
    virtual void onUpdateList(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup);
    virtual void onRemovedItems(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup, const fxb::FileScrap::IdSet &aIdSet);
    virtual void onRemovedAllItems(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup);

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

#endif // __FX_FILE_SCRAP_PANE_H__
