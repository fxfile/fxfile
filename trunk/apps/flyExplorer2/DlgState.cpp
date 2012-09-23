//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DlgState.h"

#include "fxb/fxb_ini_file.h"
#include "CfgPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_COMBOX_LIST 20

DlgState::DlgState(const xpr_tchar_t *aSection)
    : mDlg(XPR_NULL)
    , mListCtrl(XPR_NULL)
    , mIniFile(XPR_NULL)
    , mPos(XPR_FALSE)
{
    setSection(aSection);
}

DlgState::~DlgState(void)
{
    mEditMap.clear();
    mComboBoxMap.clear();
    mComboBoxListMap.clear();
    mCheckBoxMap.clear();

    XPR_SAFE_DELETE(mIniFile);
}

void DlgState::setSection(const xpr_tchar_t *aSection)
{
    if (XPR_IS_NULL(aSection))
        return;

    if (XPR_IS_NOT_NULL(mIniFile))
        return;

    mSection = aSection;

    std::tstring sOldSection = mSection;
    std::tstring sNewSection = mSection;
    sNewSection.insert(0, XPR_STRING_LITERAL("dlg"));

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeSettings, sOldPath, XPR_MAX_PATH, sOldSection.c_str());
    CfgPath::instance().getSavePath(CfgPath::TypeSettings, sNewPath, XPR_MAX_PATH, sNewSection.c_str());

    if (fxb::IsExistFile(sNewPath) == XPR_FALSE)
        ::MoveFile(sOldPath, sNewPath);

    mIniFile = new fxb::IniFile(sNewPath);
}

void DlgState::setDialog(CDialog *aDlg, xpr_bool_t aPos)
{
    mDlg = aDlg;
    mPos = aPos;
}

void DlgState::setListCtrl(CWnd *aWnd)
{
    mListCtrl = (CListCtrl *)aWnd;
}

void DlgState::setEditCtrl(std::tstring aEntry, xpr_uint_t aId)
{
    mEditMap[aEntry] = aId;
}

void DlgState::setComboBox(std::tstring aEntry, xpr_uint_t aId)
{
    mComboBoxMap[aEntry] = aId;
}

void DlgState::setComboBoxList(std::tstring aEntry, xpr_uint_t aId)
{
    mComboBoxListMap[aEntry] = aId;
}

void DlgState::setCheckBox(std::tstring aEntry, xpr_uint_t aId)
{
    mCheckBoxMap[aEntry] = aId;
}

xpr_sint_t DlgState::getStateB(const xpr_tchar_t *aEntry, xpr_bool_t aDefault)
{
    if (XPR_IS_NULL(mIniFile))
        return aDefault;

    return mIniFile->getValueB(mSection.c_str(), aEntry, aDefault);
}

xpr_bool_t DlgState::getStateI(const xpr_tchar_t *aEntry, xpr_sint_t aDefault)
{
    if (XPR_IS_NULL(mIniFile))
        return aDefault;

    return mIniFile->getValueI(mSection.c_str(), aEntry, aDefault);
}

const xpr_tchar_t *DlgState::getStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aDefault)
{
    if (XPR_IS_NULL(mIniFile))
        return aDefault;

    return mIniFile->getValueS(mSection.c_str(), aEntry, aDefault);
}

xpr_bool_t DlgState::setStateB(const xpr_tchar_t *aEntry, xpr_bool_t aValue)
{
    if (XPR_IS_NULL(mIniFile))
        return XPR_FALSE;

    return mIniFile->setValueB(mSection.c_str(), aEntry, aValue);
}

xpr_bool_t DlgState::setStateI(const xpr_tchar_t *aEntry, xpr_sint_t aValue)
{
    if (XPR_IS_NULL(mIniFile))
        return XPR_FALSE;

    return mIniFile->setValueI(mSection.c_str(), aEntry, aValue);
}

xpr_bool_t DlgState::setStateS(const xpr_tchar_t *aEntry, const xpr_tchar_t *aValue)
{
    if (XPR_IS_NULL(mIniFile))
        return XPR_FALSE;

    return mIniFile->setValueS(mSection.c_str(), aEntry, aValue);
}

xpr_bool_t DlgState::load(void)
{
    if (XPR_IS_NULL(mIniFile))
        return XPR_FALSE;

    if (mIniFile->readFileW() == XPR_FALSE)
        return XPR_FALSE;

    xpr_sint_t i, sSel;
    CString sText;
    CString sEntry;

    CEdit *sEdit;
    CComboBox *sComboBox;
    CButton *sCheckBox;

    // Dialog
    if (XPR_IS_TRUE(mPos) && XPR_IS_NOT_NULL(mDlg) && XPR_IS_NOT_NULL(mDlg->m_hWnd))
    {
        CString sWndSize;
        sWndSize = mIniFile->getValueS(mSection.c_str(), XPR_STRING_LITERAL("Window"), XPR_STRING_LITERAL(""));
        if (sWndSize.GetLength() > 0)
        {
            xpr_sint_t x = -1, y = -1, w = -1, h = -1;
            _stscanf(sWndSize, XPR_STRING_LITERAL("%d,%d,%d,%d"), &x, &y, &w, &h);

            if (w > 0 && h > 0)
            {
                CRect sWindowRect(x,y,x+w,y+h);

                CRect sWorkAreaRect;
                fxb::GetDesktopRect(sWindowRect, sWorkAreaRect, XPR_TRUE);

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
                fxb::GetDesktopRect(sWindowRect, sDesktopRect, XPR_FALSE);

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

    // ListCtrl
    if (XPR_IS_NOT_NULL(mListCtrl) && XPR_IS_NOT_NULL(mListCtrl->m_hWnd))
    {
        xpr_sint_t cx;
        xpr_sint_t sCount = mListCtrl->GetHeaderCtrl()->GetItemCount();
        for (i = 0; i < sCount; ++i)
        {
            sEntry.Format(XPR_STRING_LITERAL("Column%d"), i+1);
            cx = mIniFile->getValueI(mSection.c_str(), sEntry, -1);
            if (cx == -1)
                break;

            mListCtrl->SetColumnWidth(i, cx);
        }
    }

    // Edit
    {
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

        sIterator = mEditMap.begin();
        for (; sIterator != mEditMap.end(); ++sIterator)
        {
            sEdit = (CEdit *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sEdit) || XPR_IS_NULL(sEdit->m_hWnd))
                continue;

            if (mIniFile->findEntry(mSection.c_str(), sIterator->first.c_str()) == fxb::IniFile::InvalidIndex)
                continue;

            sText = mIniFile->getValueS(mSection.c_str(), sIterator->first.c_str(), XPR_STRING_LITERAL(""));
            sEdit->SetWindowText(sText);
        }
    }

    // ComboBox
    {
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

        sIterator = mComboBoxMap.begin();
        for (; sIterator != mComboBoxMap.end(); ++sIterator)
        {
            sComboBox = (CComboBox *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sComboBox) || XPR_IS_NULL(sComboBox->m_hWnd))
                continue;

            if (mIniFile->findEntry(mSection.c_str(), sIterator->first.c_str()) == fxb::IniFile::InvalidIndex)
                continue;

            sSel = mIniFile->getValueI(mSection.c_str(), sIterator->first.c_str(), -1);
            if (sSel < 0)
                continue;

            sComboBox->SetCurSel(sSel);
        }
    }

    // ComboBoxList
    {
        xpr_sint_t sIndex;
        xpr_sint_t sCurSel;
        CString sWndText;
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

        sIterator = mComboBoxListMap.begin();
        for (; sIterator != mComboBoxListMap.end(); ++sIterator)
        {
            sComboBox = (CComboBox *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sComboBox) || XPR_IS_NULL(sComboBox->m_hWnd))
                continue;

            sCurSel = -1;
            sWndText = mIniFile->getValueS(mSection.c_str(), sIterator->first.c_str(), XPR_STRING_LITERAL(""));

            for (i = 0; i < MAX_COMBOX_LIST; ++i)
            {
                sEntry.Format(XPR_STRING_LITERAL("%s_List%02d"), sIterator->first.c_str(), i+1);
                sText = mIniFile->getValueS(mSection.c_str(), sEntry, XPR_STRING_LITERAL(""));
                if (sText.IsEmpty())
                    continue;

                sIndex = sComboBox->AddString(sText);
                if (sCurSel != -1 && sWndText == sText)
                    sCurSel = sIndex;
            }

            sComboBox->SetCurSel(sCurSel);
            if (sCurSel == -1)
                sComboBox->SetWindowText(sWndText);
        }
    }

    // CheckBox
    {
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

        sIterator = mCheckBoxMap.begin();
        for (; sIterator != mCheckBoxMap.end(); ++sIterator)
        {
            sCheckBox = (CButton *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sCheckBox) || XPR_IS_NULL(sCheckBox->m_hWnd))
                continue;

            if (mIniFile->findEntry(mSection.c_str(), sIterator->first.c_str()) == fxb::IniFile::InvalidIndex)
                continue;

            sSel = mIniFile->getValueI(mSection.c_str(), sIterator->first.c_str(), -1);
            if (sSel < 0)
                continue;

            sCheckBox->SetCheck(sSel);
        }
    }

    return XPR_TRUE;
}

xpr_bool_t DlgState::save(void)
{
    if (XPR_IS_NULL(mIniFile))
        return XPR_FALSE;

    xpr_tchar_t sComment[200] = {0};
    _stprintf(sComment, XPR_STRING_LITERAL("flyExplorer %s File"), mSection.c_str());
    mIniFile->setComment(sComment);

    xpr_sint_t i, sSel;
    xpr_sint_t sCount;
    CString sText;
    CString sEntry;

    CEdit *sEdit;
    CComboBox *sComboBox;
    CButton *sCheckBox;

    // Dialog
    if (XPR_IS_TRUE(mPos) && XPR_IS_NOT_NULL(mDlg) && XPR_IS_NOT_NULL(mDlg->m_hWnd))
    {
        CRect sWindowRect;
        mDlg->GetWindowRect(sWindowRect);

        CString sWndSize;
        sWndSize.Format(XPR_STRING_LITERAL("%d,%d,%d,%d"), sWindowRect.left, sWindowRect.top, sWindowRect.Width(), sWindowRect.Height());
        mIniFile->setValueS(mSection.c_str(), XPR_STRING_LITERAL("Window"), sWndSize);
    }

    // ListCtrl
    if (XPR_IS_NOT_NULL(mListCtrl) && XPR_IS_NOT_NULL(mListCtrl->m_hWnd))
    {
        xpr_sint_t cx;
        xpr_sint_t sCount = mListCtrl->GetHeaderCtrl()->GetItemCount();
        for (i = 0; i < sCount; ++i)
        {
            cx = mListCtrl->GetColumnWidth(i);
            sEntry.Format(XPR_STRING_LITERAL("Column%d"), i+1);
            mIniFile->setValueI(mSection.c_str(), sEntry, cx);
        }
    }

    // Edit
    {
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

        sIterator = mEditMap.begin();
        for (; sIterator != mEditMap.end(); ++sIterator)
        {
            sEdit = (CEdit *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sEdit) || XPR_IS_NULL(sEdit->m_hWnd))
                continue;

            xpr_sint_t nLen = ::GetWindowTextLength(sEdit->m_hWnd);
            ::GetWindowText(sEdit->m_hWnd, sText.GetBufferSetLength(nLen), nLen+1);
            sText.ReleaseBuffer();

            //sEdit->GetWindowText(sText);
            mIniFile->setValueS(mSection.c_str(), sIterator->first.c_str(), sText);
        }
    }

    // ComboBox
    {
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

        sIterator = mComboBoxMap.begin();
        for (; sIterator != mComboBoxMap.end(); ++sIterator)
        {
            sComboBox = (CComboBox *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sComboBox) || XPR_IS_NULL(sComboBox->m_hWnd))
                continue;

            sSel = sComboBox->GetCurSel();
            mIniFile->setValueI(mSection.c_str(), sIterator->first.c_str(), sSel);
        }
    }

    // ComboBoxList
    {
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

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
                sComboBox->GetLBText(i, sText);
                sEntry.Format(XPR_STRING_LITERAL("%s_List%02d"), sIterator->first.c_str(), i+1);
                mIniFile->setValueS(mSection.c_str(), sEntry, sText);
            }

            sComboBox->GetWindowText(sText);
            mIniFile->setValueS(mSection.c_str(), sIterator->first.c_str(), sText);
        }
    }

    // CheckBox
    {
        std::map<std::tstring, xpr_uint_t>::iterator sIterator;

        sIterator = mCheckBoxMap.begin();
        for (; sIterator != mCheckBoxMap.end(); ++sIterator)
        {
            sCheckBox = (CButton *)mDlg->GetDlgItem(sIterator->second);
            if (XPR_IS_NULL(sCheckBox) || XPR_IS_NULL(sCheckBox->m_hWnd))
                continue;

            sSel = sCheckBox->GetCheck();
            mIniFile->setValueI(mSection.c_str(), sIterator->first.c_str(), sSel);
        }
    }

    mIniFile->writeFileW();

    return XPR_TRUE;
}

void DlgState::reset(void)
{
    XPR_SAFE_DELETE(mIniFile);

    std::tstring sSection;
    sSection = mSection;

    setSection(sSection.c_str());
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
