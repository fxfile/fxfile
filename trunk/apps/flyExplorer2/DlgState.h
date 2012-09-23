//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DLG_STATE_H__
#define __FX_DLG_STATE_H__
#pragma once

namespace fxb
{
class IniFile;
} // namespace fxb

class DlgState
{
public:
    DlgState(const xpr_tchar_t *aSection = XPR_NULL);
    virtual ~DlgState(void);

public:
    void setSection(const xpr_tchar_t *aSection);

    void setDialog(CDialog *aDlg, xpr_bool_t aPos = XPR_FALSE);
    void setListCtrl(CWnd *aWnd);
    void setEditCtrl(std::tstring aEntry, xpr_uint_t aId);
    void setComboBox(std::tstring aEntry, xpr_uint_t aId);
    void setComboBoxList(std::tstring aEntry, xpr_uint_t aId);
    void setCheckBox(std::tstring aEntry, xpr_uint_t aId);

    xpr_sint_t         getStateB(const xpr_tchar_t *aEntry, xpr_bool_t aDefault = XPR_FALSE);
    xpr_bool_t         getStateI(const xpr_tchar_t *aEntry, xpr_sint_t aDefault = 0);
    const xpr_tchar_t *getStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aDefault = XPR_NULL);
    xpr_bool_t         setStateB(const xpr_tchar_t *aEntry, xpr_bool_t aValue);
    xpr_bool_t         setStateI(const xpr_tchar_t *aEntry, xpr_sint_t aValue);
    xpr_bool_t         setStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aValue);

    xpr_bool_t load(void);
    xpr_bool_t save(void);

    void reset(void);

public:
    static void insertComboEditString(CComboBox *aComboBox, xpr_bool_t aCase = XPR_TRUE);

protected:
    CDialog   *mDlg;
    CListCtrl *mListCtrl;

    std::map<std::tstring, xpr_uint_t> mEditMap;
    std::map<std::tstring, xpr_uint_t> mComboBoxMap;
    std::map<std::tstring, xpr_uint_t> mComboBoxListMap;
    std::map<std::tstring, xpr_uint_t> mCheckBoxMap;

    fxb::IniFile *mIniFile;
    std::tstring  mSection;
    xpr_bool_t    mPos;
};

#endif // __FX_DLG_STATE_H__
