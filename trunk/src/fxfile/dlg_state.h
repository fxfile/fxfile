//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DLG_STATE_H__
#define __FXFILE_DLG_STATE_H__ 1
#pragma once

namespace fxfile
{
class DlgStateManager;

class DlgState
{
    friend class DlgStateManager;

protected:
    DlgState(const xpr_tchar_t *aSection);
    virtual ~DlgState(void);

public:
    void setDialog(CDialog *aDlg, xpr_bool_t aPos = XPR_FALSE);
    void setListCtrl(const xpr_tchar_t *aEntry, xpr_uint_t aId);
    void setEditCtrl(const xpr_tchar_t *aEntry, xpr_uint_t aId);
    void setComboBox(const xpr_tchar_t *aEntry, xpr_uint_t aId);
    void setComboBoxList(const xpr_tchar_t *aEntry, xpr_uint_t aId);
    void setCheckBox(const xpr_tchar_t *aEntry, xpr_uint_t aId);

    xpr_bool_t         existState(const xpr_tchar_t *aEntry) const;
    xpr_bool_t         getStateB(const xpr_tchar_t *aEntry, xpr_bool_t aDefault = XPR_FALSE) const;
    xpr_sint_t         getStateI(const xpr_tchar_t *aEntry, xpr_sint_t aDefault = 0) const;
    const xpr_tchar_t *getStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aDefault = XPR_NULL) const;
    xpr_bool_t         setStateB(const xpr_tchar_t *aEntry, xpr_bool_t aValue);
    xpr_bool_t         setStateI(const xpr_tchar_t *aEntry, xpr_sint_t aValue);
    xpr_bool_t         setStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aValue);

    xpr_bool_t load(void);
    xpr_bool_t save(void);

    void reset(void);

    void clear(void);

public:
    static void insertComboEditString(CComboBox *aComboBox, xpr_bool_t aCase = XPR_TRUE);

protected:
    typedef std::tr1::unordered_map<xpr::string, xpr::string> ValueMap;

    const xpr::string &getSection(void) const;
    const ValueMap &getValueMap(void) const;
    void setValueMap(const ValueMap &aValueMap);

protected:
    typedef std::tr1::unordered_map<xpr::string, xpr_uint_t> CtrlMap;

    CDialog   *mDlg;
    CtrlMap    mListCtrlMap;
    CtrlMap    mEditMap;
    CtrlMap    mComboBoxMap;
    CtrlMap    mComboBoxListMap;
    CtrlMap    mCheckBoxMap;
    xpr_bool_t mPos;

protected:
    xpr::string mSection;
    ValueMap    mValueMap;
};
} // namespace fxfile

#endif // __FXFILE_DLG_STATE_H__
