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

#include "FileScrapCtrlObserver.h"

class FileScrapPaneObserver;
class FileScrapToolBar;

class FileScrapPane : public CWnd, public FileScrapCtrlObserver, public fxb::FileScrapObserver
{
    typedef CWnd super;

public:
    FileScrapPane(void);
    virtual ~FileScrapPane(void);

public:
    void setObserver(FileScrapPaneObserver *aObserver);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void setViewIndex(xpr_sint_t aViewIndex);
    xpr_sint_t getViewIndex(void) const;

    FileScrapCtrl *getFileScrapCtrl(void) const;

    fxb::FileScrap::Group *getCurGroup(void) const;

    xpr_sint_t getItemCount(void) const;
    xpr_sint_t getSelectedItemCount(void) const;
    void getSelFileScrapItemIdSet(fxb::FileScrap::IdSet &sIdSet);

    void refresh(void);

protected:
    xpr_bool_t registerWindowClass(void);
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
    virtual void onSetFocus(FileScrapCtrl &aFileScrapCtrl);
    virtual xpr_bool_t onExplore(FileScrapCtrl &aFileScrapCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onExplore(FileScrapCtrl &aFileScrapCtrl, LPITEMIDLIST aFullPid);
    virtual void onUpdateStatus(FileScrapCtrl &aFileScrapCtrl);

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
    FileScrapPaneObserver *mObserver;

    xpr_sint_t mViewIndex;

    FileScrapCtrl     *mFileScrapCtrl;
    CRect              mGroupLabelRect;
    const xpr_tchar_t *mGroupLabelText;
    CComboBoxEx       *mGroupComboBox;
    FileScrapToolBar  *mToolBar;
    StatusBar         *mStatusBar;
    CImageList         mGroupImgList;
    CFont              mFont;
    BCMenu             mMenu;

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
