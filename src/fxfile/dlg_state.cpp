//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "dlg_state.h"

#include "winapi_ex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxfile
{
#define MAX_COMBOX_LIST 20

DlgState::DlgState(const xpr_tchar_t *aSection)
    : mDlg(XPR_NULL)
    , mPos(XPR_FALSE)
{
    if (XPR_IS_NOT_NULL(aSection))
        mSection = aSection;
}

DlgState::~DlgState(void)
{
    clear();
}

void DlgState::clear(void)
{
    mListCtrlMap.clear();
    mEditMap.clear();
    mComboBoxMap.clear();
    mComboBoxListMap.clear();
    mCheckBoxMap.clear();
    mValueMap.clear();
}

void DlgState::setDialog(CDialog *aDlg, xpr_bool_t aPos)
{
    mDlg = aDlg;
    mPos = aPos;
}

void DlgState::setListCtrl(const xpr_tchar_t *aEntry, xpr_uint_t aId)
{
    if (XPR_IS_NOT_NULL(aEntry))
        mListCtrlMap[aEntry] = aId;
}

void DlgState::setEditCtrl(const xpr_tchar_t *aEntry, xpr_uint_t aId)
{
    if (XPR_IS_NOT_NULL(aEntry))
        mEditMap[aEntry] = aId;
}

void DlgState::setComboBox(const xpr_tchar_t *aEntry, xpr_uint_t aId)
{
    if (XPR_IS_NOT_NULL(aEntry))
        mComboBoxMap[aEntry] = aId;
}

void DlgState::setComboBoxList(const xpr_tchar_t *aEntry, xpr_uint_t aId)
{
    if (XPR_IS_NOT_NULL(aEntry))
        mComboBoxListMap[aEntry] = aId;
}

void DlgState::setCheckBox(const xpr_tchar_t *aEntry, xpr_uint_t aId)
{
    if (XPR_IS_NOT_NULL(aEntry))
        mCheckBoxMap[aEntry] = aId;
}

xpr_bool_t DlgState::existState(const xpr_tchar_t *aEntry) const
{
    if (XPR_IS_NULL(aEntry))
        return XPR_FALSE;

    ValueMap::const_iterator sIterator = mValueMap.find(aEntry);
    if (sIterator == mValueMap.end())
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_sint_t DlgState::getStateB(const xpr_tchar_t *aEntry, xpr_bool_t aDefault) const
{
    if (XPR_IS_NULL(aEntry))
        return aDefault;

    ValueMap::const_iterator sIterator = mValueMap.find(aEntry);
    if (sIterator == mValueMap.end())
        return aDefault;

    const xpr::tstring &sValue = sIterator->second;

    xpr_bool_t sBoolValue = aDefault;
    _stscanf(sValue.c_str(), XPR_STRING_LITERAL("%d"), &sBoolValue);

    if (XPR_IS_TRUE(sBoolValue))
        sBoolValue = XPR_TRUE;

    return sBoolValue;
}

xpr_bool_t DlgState::getStateI(const xpr_tchar_t *aEntry, xpr_sint_t aDefault) const
{
    if (XPR_IS_NULL(aEntry))
        return aDefault;

    ValueMap::const_iterator sIterator = mValueMap.find(aEntry);
    if (sIterator == mValueMap.end())
        return aDefault;

    const xpr::tstring &sValue = sIterator->second;

    xpr_bool_t sIntValue = aDefault;
    _stscanf(sValue.c_str(), XPR_STRING_LITERAL("%d"), &sIntValue);

    return sIntValue;
}

const xpr_tchar_t *DlgState::getStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aDefault) const
{
    if (XPR_IS_NULL(aEntry))
        return aDefault;

    ValueMap::const_iterator sIterator = mValueMap.find(aEntry);
    if (sIterator == mValueMap.end())
        return aDefault;

    const xpr::tstring &sValue = sIterator->second;

    return sValue.c_str();
}

xpr_bool_t DlgState::setStateB(const xpr_tchar_t *aEntry, xpr_bool_t aValue)
{
    if (XPR_IS_NULL(aEntry))
        return XPR_FALSE;

    xpr_tchar_t sValue[0xff] = {0};
    _stprintf(sValue, XPR_STRING_LITERAL("%d"), XPR_IS_TRUE(aValue) ? XPR_TRUE : XPR_FALSE);

    mValueMap[aEntry] = sValue;

    return XPR_TRUE;
}

xpr_bool_t DlgState::setStateI(const xpr_tchar_t *aEntry, xpr_sint_t aValue)
{
    if (XPR_IS_NULL(aEntry))
        return XPR_FALSE;

    xpr_tchar_t sValue[0xff] = {0};
    _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue);

    mValueMap[aEntry] = sValue;

    return XPR_TRUE;
}

xpr_bool_t DlgState::setStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aValue)
{
    if (XPR_IS_NULL(aEntry))
        return XPR_FALSE;

    if (XPR_IS_NULL(aValue))
        aValue = XPR_STRING_LITERAL("");

    mValueMap[aEntry] = aValue;

    return XPR_TRUE;
}

xpr_bool_t DlgState::load(void)
{
    if (XPR_IS_NULL(mDlg) || XPR_IS_NULL(mDlg->m_hWnd))
        return XPR_FALSE;

    xpr_sint_t i, sSel;
    xpr_tchar_t sEntry[0xff];

    if (XPR_IS_TRUE(mPos))
    {
        const xpr_tchar_t *sWndSize = getStateS(XPR_STRING_LITERAL("Window"));
        if (XPR_IS_NOT_NULL(sWndSize))
        {
            xpr_sint_t x = -1, y = -1, w = -1, h = -1;
            _stscanf(sWndSize, XPR_STRING_LITERAL("%d,%d,%d,%d"), &x, &y, &w, &h);

            if (w > 0 && h > 0)
            {
                CRect sWindowRect(x,y,x+w,y+h);

                CRect sWorkAreaRect;
                GetDesktopRect(sWindowRect, sWorkAreaRect, XPR_TRUE);

                if (sWorkAreaRect.PtInRect(CPoint(sWindowRect.left,  sWindowRect.top))    == XPR_FALSE &&
                    sWorkAreaRect.PtInRect(CPoint(sWindowRect.right, sWindowRect.top))    == XPR_FALSE &&
                    sWorkAreaRect.PtInRect(CPoint(sWindowRect.right, sWindowRect.bottom)) == XPR_FALSE &&
                    sWorkAreaRect.PtInRect(CPoint(sWindowRect.left,  sWindowRect.bottom)) == XPR_FALSE)
                {
                    xpr_sint_t cx = sWindowRect.Width();
                    xpr_sint_t cy = sWindowRect.Height();

                    if (cx > sWorkAreaRect.Width())  cx = sWorkAreaRect.Width();
                    if (cy > sWorkAreaRect.Height()) cy = sWorkAreaRect.Height();

                    sWindowRect.left   = (sWorkAreaRect.Width()  - cx) / 2;
                    sWindowRect.top    = (sWorkAreaRect.Height() - cy) / 2;
                    sWindowRect.right  = sWindowRect.left + cx;
                    sWindowRect.bottom = sWindowRect.top  + cy;
                }

                // if the task bar is placed to left side or up side
                CRect sDesktopRect;
                GetDesktopRect(sWindowRect, sDesktopRect, XPR_FALSE);

                if (sWorkAreaRect.left != sDesktopRect.left ||
                    sWorkAreaRect.top  != sDesktopRect.top)
                {
                    xpr_sint_t xOffset = sWorkAreaRect.left - sDesktopRect.left;
                    xpr_sint_t yOffset = sWorkAreaRect.top  - sDesktopRect.top;

                    sWindowRect.left   += xOffset;
                    sWindowRect.right  += xOffset;
                    sWindowRect.top    += yOffset;
                    sWindowRect.bottom += yOffset;
                }

                mDlg->MoveWindow(sWindowRect);
            }
        }
    }

    if (mListCtrlMap.empty() == false)
    {
        CtrlMap::iterator sIterator;
        xpr_sint_t i, cx;
        xpr_sint_t sColumnCount;
        CListCtrl *sListCtrl;

        sIterator = mListCtrlMap.begin();
        for (; sIterator != mListCtrlMap.end(); ++sIterator)
        {
            sListCtrl = (CListCtrl *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sListCtrl))
                continue;

            sColumnCount = sListCtrl->GetHeaderCtrl()->GetItemCount();
            for (i = 0; i < sColumnCount; ++i)
            {
                _stprintf(sEntry, XPR_STRING_LITERAL("%s_Column%02d"), sIterator->first.c_str(), i + 1);

                cx = getStateI(sEntry, -1);
                if (cx == -1)
                    break;

                sListCtrl->SetColumnWidth(i, cx);
            }
        }
    }

    if (mEditMap.empty() == false)
    {
        CtrlMap::iterator sIterator;
        const xpr_tchar_t *sValue;
        CEdit *sEdit;

        sIterator = mEditMap.begin();
        for (; sIterator != mEditMap.end(); ++sIterator)
        {
            sEdit = (CEdit *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sEdit) || XPR_IS_NULL(sEdit->m_hWnd))
                continue;

            sValue = getStateS(sIterator->first.c_str());
            if (XPR_IS_NOT_NULL(sValue))
            {
                sEdit->SetWindowText(sValue);
            }
        }
    }

    if (mComboBoxMap.empty() == false)
    {
        CtrlMap::iterator sIterator;
        CComboBox *sComboBox;

        sIterator = mComboBoxMap.begin();
        for (; sIterator != mComboBoxMap.end(); ++sIterator)
        {
            sComboBox = (CComboBox *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sComboBox) || XPR_IS_NULL(sComboBox->m_hWnd))
                continue;

            sSel = getStateI(sIterator->first.c_str(), -1);
            if (sSel < 0)
                continue;

            sComboBox->SetCurSel(sSel);
        }
    }

    if (mComboBoxListMap.empty() == false)
    {
        xpr_sint_t sIndex;
        xpr_sint_t sCurSel;
        const xpr_tchar_t *sSelWndText;
        const xpr_tchar_t *sItemText;
        CtrlMap::iterator sIterator;
        CComboBox *sComboBox;

        sIterator = mComboBoxListMap.begin();
        for (; sIterator != mComboBoxListMap.end(); ++sIterator)
        {
            sComboBox = (CComboBox *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sComboBox) || XPR_IS_NULL(sComboBox->m_hWnd))
                continue;

            sCurSel = -1;
            sSelWndText = getStateS(sIterator->first.c_str());

            for (i = 0; i < MAX_COMBOX_LIST; ++i)
            {
                _stprintf(sEntry, XPR_STRING_LITERAL("%s_List%02d"), sIterator->first.c_str(), i + 1);

                sItemText = getStateS(sEntry);
                if (XPR_IS_NULL(sItemText))
                    continue;

                sIndex = sComboBox->AddString(sItemText);
                if (sCurSel != -1 && XPR_IS_NOT_NULL(sSelWndText) && _tcscmp(sSelWndText, sItemText) == 0)
                    sCurSel = sIndex;
            }

            sComboBox->SetCurSel(sCurSel);
            if (sCurSel == -1)
                sComboBox->SetWindowText(XPR_IS_NULL(sSelWndText) ? XPR_STRING_LITERAL("") : sSelWndText);
        }
    }

    if (mCheckBoxMap.empty() == false)
    {
        CtrlMap::iterator sIterator;
        CButton *sCheckBox;

        sIterator = mCheckBoxMap.begin();
        for (; sIterator != mCheckBoxMap.end(); ++sIterator)
        {
            sCheckBox = (CButton *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sCheckBox) || XPR_IS_NULL(sCheckBox->m_hWnd))
                continue;

            sSel = getStateI(sIterator->first.c_str(), -1);
            if (sSel < 0)
                continue;

            sCheckBox->SetCheck(sSel);
        }
    }

    return XPR_TRUE;
}

xpr_bool_t DlgState::save(void)
{
    if (XPR_IS_NULL(mDlg) || XPR_IS_NULL(mDlg->m_hWnd))
        return XPR_FALSE;

    xpr_sint_t i, sSel;
    xpr_sint_t sCount;
    xpr_tchar_t sEntry[0xff];

    if (XPR_IS_TRUE(mPos))
    {
        CRect sWindowRect;
        mDlg->GetWindowRect(sWindowRect);

        xpr_tchar_t sValue[0xff] = {0};
        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d,%d,%d"), sWindowRect.left, sWindowRect.top, sWindowRect.Width(), sWindowRect.Height());
        setStateS(XPR_STRING_LITERAL("Window"), sValue);
    }

    if (mListCtrlMap.empty() == false)
    {
        CtrlMap::iterator sIterator;
        xpr_sint_t i, cx;
        xpr_sint_t sColumnCount;
        CListCtrl *sListCtrl;

        sIterator = mListCtrlMap.begin();
        for (; sIterator != mListCtrlMap.end(); ++sIterator)
        {
            sListCtrl = (CListCtrl *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sListCtrl))
                continue;

            sColumnCount = sListCtrl->GetHeaderCtrl()->GetItemCount();
            for (i = 0; i < sColumnCount; ++i)
            {
                cx = sListCtrl->GetColumnWidth(i);

                _stprintf(sEntry, XPR_STRING_LITERAL("%s_Column%02d"), sIterator->first.c_str(), i + 1);
                setStateI(sEntry, cx);
            }
        }
    }

    if (mEditMap.empty() == false)
    {
        xpr_sint_t sLen;
        xpr_tchar_t *sValue;
        CtrlMap::iterator sIterator;
        CEdit *sEdit;

        sIterator = mEditMap.begin();
        for (; sIterator != mEditMap.end(); ++sIterator)
        {
            sEdit = (CEdit *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sEdit) || XPR_IS_NULL(sEdit->m_hWnd))
                continue;

            sLen = sEdit->GetWindowTextLength();
            sValue = new xpr_tchar_t[sLen + 2];
            sEdit->GetWindowText(sValue, sLen + 1);

            setStateS(sIterator->first.c_str(), sValue);

            XPR_SAFE_DELETE_ARRAY(sValue);
        }
    }

    if (mComboBoxMap.empty() == false)
    {
        CtrlMap::iterator sIterator;
        CComboBox *sComboBox;

        sIterator = mComboBoxMap.begin();
        for (; sIterator != mComboBoxMap.end(); ++sIterator)
        {
            sComboBox = (CComboBox *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sComboBox) || XPR_IS_NULL(sComboBox->m_hWnd))
                continue;

            sSel = sComboBox->GetCurSel();
            setStateI(sIterator->first.c_str(), sSel);
        }
    }

    if (mComboBoxListMap.empty() == false)
    {
        xpr_sint_t sLen;
        xpr_tchar_t *sValue;
        CtrlMap::iterator sIterator;
        CComboBox *sComboBox;

        sIterator = mComboBoxListMap.begin();
        for (; sIterator != mComboBoxListMap.end(); ++sIterator)
        {
            sComboBox = (CComboBox *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sComboBox) || XPR_IS_NULL(sComboBox->m_hWnd))
                continue;

            sCount = sComboBox->GetCount();
            sCount = min(sCount, MAX_COMBOX_LIST);
            for (i = 0; i < sCount; ++i)
            {
                sLen = sComboBox->GetLBTextLen(i);
                sValue = new xpr_tchar_t[sLen + 1];
                sComboBox->GetLBText(i, sValue);

                _stprintf(sEntry, XPR_STRING_LITERAL("%s_List%02d"), sIterator->first.c_str(), i + 1);
                setStateS(sEntry, sValue);

                XPR_SAFE_DELETE_ARRAY(sValue);
            }

            sLen = sComboBox->GetWindowTextLength();
            sValue = new xpr_tchar_t[sLen + 2];
            sComboBox->GetWindowText(sValue, sLen + 1);

            setStateS(sIterator->first.c_str(), sValue);

            XPR_SAFE_DELETE_ARRAY(sValue);
        }
    }

    if (mCheckBoxMap.empty() == false)
    {
        CtrlMap::iterator sIterator;
        CButton *sCheckBox;

        sIterator = mCheckBoxMap.begin();
        for (; sIterator != mCheckBoxMap.end(); ++sIterator)
        {
            sCheckBox = (CButton *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sCheckBox) || XPR_IS_NULL(sCheckBox->m_hWnd))
                continue;

            sSel = sCheckBox->GetCheck();
            setStateI(sIterator->first.c_str(), sSel);
        }
    }

    return XPR_TRUE;
}

void DlgState::reset(void)
{
    mValueMap.clear();
}

void DlgState::insertComboEditString(CComboBox *aComboBox, xpr_bool_t aCase)
{
    if (XPR_IS_NULL(aComboBox))
        return;

    CString sText;
    CString sText2;

    xpr_sint_t sCurSel = aComboBox->GetCurSel();
    if (sCurSel >= 0)
    {
        aComboBox->GetLBText(sCurSel, sText);
        aComboBox->GetWindowText(sText2);
        if (_tcscmp(sText, sText2) == 0)
            aComboBox->DeleteString(sCurSel);
        else
            sCurSel = -1;
    }

    if (sCurSel < 0)
    {
        aComboBox->GetWindowText(sText);

        xpr_sint_t i;
        xpr_sint_t sCount;
        CString sText2;

        xpr_bool_t sEqualed = XPR_FALSE;

        sCount = aComboBox->GetCount();
        for (i = 0; i < sCount; ++i)
        {
            aComboBox->GetLBText(i, sText2);

            if (XPR_IS_TRUE(aCase)) { if (_tcscmp(sText, sText2) == 0)  sEqualed = XPR_TRUE; }
            else                    { if (_tcsicmp(sText, sText2) == 0) sEqualed = XPR_TRUE; }

            if (XPR_IS_TRUE(sEqualed))
                break;
        }

        if (XPR_IS_TRUE(sEqualed))
            aComboBox->DeleteString(i);
    }

    aComboBox->InsertString(0, sText);
    aComboBox->SetCurSel(0);
}

const xpr::tstring &DlgState::getSection(void) const
{
    return mSection;
}

const DlgState::ValueMap &DlgState::getValueMap(void) const
{
    return mValueMap;
}

void DlgState::setValueMap(const ValueMap &aValueMap)
{
    mValueMap = aValueMap;
}
} // namespace fxfile
