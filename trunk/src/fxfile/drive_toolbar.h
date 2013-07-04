//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DRIVE_TOOL_BAR_H__
#define __FXFILE_DRIVE_TOOL_BAR_H__ 1
#pragma once

#include "shell_change_notify.h"

#include "gui/DragDropToolBar.h"
#include "gui/BCMenu.h"

namespace fxfile
{
class DriveToolBarObserver;

class DriveToolBar : public DragDropToolBar
{
    typedef DragDropToolBar super;

public:
    DriveToolBar(void);
    virtual ~DriveToolBar(void);

public:
    void setObserver(DriveToolBarObserver *aObserver);

public:
    void createDriveBar(void);
    void destroyDriveBar(void);
    void refresh(void);

    void setShortText(xpr_bool_t aShortText);

    xpr_tchar_t getButtonDrive(xpr_uint_t aIndex);
    virtual void GetButtonTextByCommand(xpr_uint_t aId, CString &aText);

    xpr_uint_t getDriveId(xpr_tchar_t aDriveChar);
    xpr_uint_t getDriveId(const xpr_tchar_t *aPath);

protected:
    DriveToolBarObserver *mObserver;

    CToolTipCtrl mToolTip;
    xpr_tchar_t mToolTipText[XPR_MAX_PATH * 2];

    HANDLE mThread;
    HANDLE mStopEvent;
    unsigned mThreadId;
    static unsigned __stdcall DriveProc(void *aParam);
    DWORD OnDriveProc(void);

    typedef struct DriveInfo
    {
        xpr_sint_t  mIndex;
        xpr_tchar_t mDriveChar;
    } DriveInfo;

    typedef std::deque<DriveInfo> DriveDeque;
    DriveDeque mDriveDeque;

    xpr_bool_t mInit;
    xpr_bool_t mBarCreated;
    xpr_bool_t mShortText;

    void addDriveButton(const xpr_tchar_t *aDrive, xpr_sint_t aInsert);

protected:
    xpr_sint_t mOldDrive;

    virtual DROPEFFECT OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragLeave(void);
    virtual DROPEFFECT OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragScroll(DWORD aKeyState, CPoint aPoint);
    virtual void       OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint);

    void OnDropFileOperation(STGMEDIUM *aStgMedium, DROPEFFECT aDropEffect, xpr_sint_t aDrive);
    void OnDropShortcut(STGMEDIUM *aStgMedium, xpr_sint_t aDrive);

    void OnShcnUpdateDrive(Shcn *aShcn);
    void OnShcnDriveAdd(Shcn *aShcn);
    void OnShcnDriveRemove(Shcn *aShcn);
    void OnShcnAllDriveUpdate(Shcn *aShcn);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDestroy(void);
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
    virtual xpr_bool_t OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    afx_msg LRESULT OnDriveShcn(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDriveIconUpdate(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile

#endif // __FXFILE_DRIVE_TOOL_BAR_H__
