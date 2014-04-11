//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_language_dlg.h"

#include "language_table.h"
#include "language_pack.h"

#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceLanguageDlg::CfgAppearanceLanguageDlg(void)
    : super(IDD_CFG_APPEARANCE_LANGUAGE, XPR_NULL)
{
}

void CfgAppearanceLanguageDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceLanguageDlg, super)
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_CFG_LANGUAGE_PACK, OnSelChangeLanguagePack)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceLanguageDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sLanguagePackComboBox = (CComboBox *)GetDlgItem(IDC_CFG_LANGUAGE_PACK);

    xpr_size_t i, sLanguageCount;
    const fxfile::base::LanguagePack::Desc *sLanguagePackDesc;
    const fxfile::base::LanguageTable *sLanguageTable = gApp.getLanguageTable();

    sLanguageCount = sLanguageTable->getLanguageCount();
    for (i = 0; i < sLanguageCount; ++i)
    {
        sLanguagePackDesc = sLanguageTable->getLanguageDesc(i);
        XPR_ASSERT(sLanguagePackDesc != XPR_NULL);

        sLanguagePackComboBox->AddString(sLanguagePackDesc->mLanguage.c_str());
    }

    SetDlgItemText(IDC_CFG_LANGUAGE_LABEL_CHOOSE,         gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.label.choose_language_pack")));
    SetDlgItemText(IDC_CFG_LANGUAGE_GROUP_DESC,           gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.group.description")));
    SetDlgItemText(IDC_CFG_LANGUAGE_LABEL_DESC_LANGUAGE,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.label.description.language")));
    SetDlgItemText(IDC_CFG_LANGUAGE_LABEL_DESC_FILE_PATH, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.label.description.file_path")));
    SetDlgItemText(IDC_CFG_LANGUAGE_LABEL_DESC_AUTHOR,    gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.label.description.author")));
    SetDlgItemText(IDC_CFG_LANGUAGE_LABEL_DESC_HOMEPAGE,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.label.description.homepage")));
    SetDlgItemText(IDC_CFG_LANGUAGE_LABEL_DESC_EMAIL,     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.label.description.e-mail")));
    SetDlgItemText(IDC_CFG_LANGUAGE_LABEL_DESC_DESC,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.language.label.description.description")));

    return XPR_TRUE;
}

void CfgAppearanceLanguageDlg::OnDestroy(void)
{

    super::OnDestroy();
}

void CfgAppearanceLanguageDlg::onInit(const Option::Config &aConfig)
{
    chooseLanguagePack(aConfig.mLanguage);
}

void CfgAppearanceLanguageDlg::onApply(Option::Config &aConfig)
{
    CComboBox *sLanguagePackComboBox = (CComboBox *)GetDlgItem(IDC_CFG_LANGUAGE_PACK);

    xpr_tchar_t sLanguage[fxfile::base::LanguagePack::kMaxLanguageLength + 1] = {0};

    xpr_sint_t sCurSel = sLanguagePackComboBox->GetCurSel();
    if (sCurSel != CB_ERR)
        sLanguagePackComboBox->GetLBText(sCurSel, sLanguage);

    _tcscpy(aConfig.mLanguage, sLanguage);
}

void CfgAppearanceLanguageDlg::OnSelChangeLanguagePack(void)
{
    CComboBox *sLanguagePackComboBox = (CComboBox *)GetDlgItem(IDC_CFG_LANGUAGE_PACK);

    xpr_sint_t sCurSel = sLanguagePackComboBox->GetCurSel();
    if (sCurSel == CB_ERR)
        return;

    chooseLanguagePack(sCurSel);
}

void CfgAppearanceLanguageDlg::chooseLanguagePack(xpr_sint_t aIndex)
{
    CComboBox *sLanguagePackComboBox = (CComboBox *)GetDlgItem(IDC_CFG_LANGUAGE_PACK);

    xpr_tchar_t sLanguage[fxfile::base::LanguagePack::kMaxLanguageLength + 1] = {0};
    sLanguagePackComboBox->GetLBText(aIndex, sLanguage);

    chooseLanguagePack(sLanguage);
}

void CfgAppearanceLanguageDlg::chooseLanguagePack(const xpr_tchar_t *aLanguage)
{
    CComboBox *sLanguagePackComboBox = (CComboBox *)GetDlgItem(IDC_CFG_LANGUAGE_PACK);

    xpr_sint_t i, sCount, sCurSel = -1;
    xpr_tchar_t sLanguage[fxfile::base::LanguagePack::kMaxLanguageLength + 1];

    sCount = sLanguagePackComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sLanguagePackComboBox->GetLBText(i, sLanguage);

        if (_tcsicmp(sLanguage, aLanguage) == 0)
        {
            sCurSel = i;
            break;
        }
    }

    const fxfile::base::LanguagePack::Desc *sLanguagePackDesc = XPR_NULL;
    if (sCurSel != -1)
    {
        sLanguagePackComboBox->SetCurSel(i);

        const fxfile::base::LanguageTable *sLanguageTable = gApp.getLanguageTable();

        sLanguagePackDesc = sLanguageTable->getLanguageDesc(aLanguage);
    }

    if (XPR_IS_NOT_NULL(sLanguagePackDesc))
    {
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_LANGUAGE,  sLanguagePackDesc->mLanguage.c_str());
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_FILE_PATH, sLanguagePackDesc->mFilePath.c_str());
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_AUTHOR,    sLanguagePackDesc->mAuthor.c_str());
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_HOMEPAGE,  sLanguagePackDesc->mHomepage.c_str());
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_EMAIL,     sLanguagePackDesc->mEmail.c_str());
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_DESC,      sLanguagePackDesc->mDescription.c_str());
    }
    else
    {
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_LANGUAGE,  XPR_STRING_LITERAL(""));
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_FILE_PATH, XPR_STRING_LITERAL(""));
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_AUTHOR,    XPR_STRING_LITERAL(""));
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_HOMEPAGE,  XPR_STRING_LITERAL(""));
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_EMAIL,     XPR_STRING_LITERAL(""));
        SetDlgItemText(IDC_CFG_LANGUAGE_DESC_DESC,      XPR_STRING_LITERAL(""));
    }
}
} // namespace cfg
} // namespace fxfile
