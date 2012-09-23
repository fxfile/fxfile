//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CREATE_ITEM_DLG_H__
#define __FX_CREATE_ITEM_DLG_H__
#pragma once

#include "TitleDlg.h"

class CreateItemDlg : public TitleDlg
{
    typedef TitleDlg super;

public:
    enum
    {
        MSG_ID_NONE = 0,
        MSG_ID_EXIST,
        MSG_ID_EMPTY,
    };

public:
    CreateItemDlg(void);

public:
    const xpr_tchar_t *getNewItem(void);
    void setNewItem(const xpr_tchar_t *aPath);
    void setDir(const xpr_tchar_t *aDir);
    void setExt(const xpr_tchar_t *aExt);
    void setExistCheck(xpr_bool_t aExistCheck = XPR_TRUE);
    void setMsg(xpr_uint_t aId, const xpr_tchar_t *aMsg);

protected:
    const xpr_tchar_t *getMsg(xpr_uint_t aId);

protected:
    std::tstring mPath;
    std::tstring mDir;
    std::tstring mExt;
    xpr_bool_t   mExistCheck;

    typedef std::map<xpr_uint_t, std::tstring> MsgMap;
    MsgMap mMsgMap;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
};

#endif // __FX_CREATE_ITEM_DLG_H__
