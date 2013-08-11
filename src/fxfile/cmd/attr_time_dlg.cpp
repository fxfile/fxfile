//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "attr_time_dlg.h"
#include "attr_time.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
#define MIN_DEPTH 0
#define MAX_DEPTH 100

// user timer
enum
{
    TM_ID_STATUS = 100,
};

// user message
enum
{
    WM_FINALIZE = WM_USER + 100,
};

AttrTimeDlg::AttrTimeDlg(void)
    : super(IDD_ATTRIBUTE, XPR_NULL)
    , mAttrTime(XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

AttrTimeDlg::~AttrTimeDlg(void)
{
    XPR_SAFE_DELETE(mAttrTime);

    mList.clear();

    DESTROY_ICON(mIcon);
}

void AttrTimeDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ATTR_PROGRESS,      mProgressCtrl);
    DDX_Control(pDX, IDC_ATTR_CREATION_DATE, mCreatedDateCtrl);
    DDX_Control(pDX, IDC_ATTR_CREATION_TIME, mCreatedTimeCtrl);
    DDX_Control(pDX, IDC_ATTR_MODIFIED_DATE, mModifiedDateCtrl);
    DDX_Control(pDX, IDC_ATTR_MODIFIED_TIME, mModifiedTimeCtrl);
    DDX_Control(pDX, IDC_ATTR_ACCESS_DATE,   mAccessDateCtrl);
    DDX_Control(pDX, IDC_ATTR_ACCESS_TIME,   mAccessTimeCtrl);
    DDX_Control(pDX, IDC_ATTR_LIST,          mListCtrl);
}

BEGIN_MESSAGE_MAP(AttrTimeDlg, super)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE,           OnFinalize)
    ON_BN_CLICKED(IDC_ATTR_CREATION,  OnCreation)
    ON_BN_CLICKED(IDC_ATTR_MODIFIED,  OnModified)
    ON_BN_CLICKED(IDC_ATTR_ACCESS,    OnAccess)
    ON_BN_CLICKED(IDC_ATTR_ATTRIBUTE, OnAttribute)
    ON_BN_CLICKED(IDC_ATTR_TIME,      OnTime)
    ON_BN_CLICKED(IDOK,               OnOK)
END_MESSAGE_MAP()

void AttrTimeDlg::setDir(const xpr::tstring &aDir)
{
    mDir = aDir;
}

void AttrTimeDlg::setItemList(const PathList &aList)
{
    mList = aList;
}

xpr_bool_t AttrTimeDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_ATTR_PATH,     sizeResize, sizeNone);

    AddControl(IDC_ATTR_PROGRESS, sizeResize, sizeRepos);
    AddControl(IDC_ATTR_STATUS,   sizeResize, sizeRepos);
    AddControl(IDC_ATTR_LIST,     sizeResize, sizeResize);

    AddControl(IDOK,              sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,          sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_ATTR_TYPE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.combo.type_file")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.combo.type_folder")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.combo.type_file_and_folder")));

    SetDlgItemText(IDC_ATTR_PATH,
                   (mDir.empty() == XPR_TRUE && mDir[0] == '\0') ?
                   theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.multiple_folders")) : mDir.c_str());

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.list.column.no")),         LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.list.column.file")),       LVCFMT_LEFT,  340, -1);
    mListCtrl.InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.list.column.attributes")), LVCFMT_LEFT,   50, -1);

    LVITEM sLvItem = {0};
    DWORD sTotalAttrs = 0;
    DWORD sAttributes;
    xpr_tchar_t sText[0xff] = {0};

    xpr_sint_t i;
    xpr::tstring sPath;
    PathList::iterator sIterator;

    sIterator = mList.begin();
    for (i = 0; sIterator != mList.end(); ++sIterator, ++i)
    {
        sPath = *sIterator;

        _stprintf(sText, XPR_STRING_LITERAL("%d"), i + 1);

        sLvItem.mask     = LVIF_TEXT;
        sLvItem.iItem    = i;
        sLvItem.iSubItem = 0;
        sLvItem.pszText  = sText;
        mListCtrl.InsertItem(&sLvItem);

        sText[0] = '\0';
        sAttributes = GetFileAttributes(sPath.c_str());
        sTotalAttrs |= sAttributes;
        _tcscpy(sText, (sAttributes & FILE_ATTRIBUTE_ARCHIVE ) ? XPR_STRING_LITERAL("A") : XPR_STRING_LITERAL("_"));
        _tcscat(sText, (sAttributes & FILE_ATTRIBUTE_READONLY) ? XPR_STRING_LITERAL("R") : XPR_STRING_LITERAL("_"));
        _tcscat(sText, (sAttributes & FILE_ATTRIBUTE_HIDDEN  ) ? XPR_STRING_LITERAL("H") : XPR_STRING_LITERAL("_"));
        _tcscat(sText, (sAttributes & FILE_ATTRIBUTE_SYSTEM  ) ? XPR_STRING_LITERAL("S") : XPR_STRING_LITERAL("_"));

        sPath = sPath.substr(sPath.rfind(XPR_STRING_LITERAL('\\'))+1);

        sLvItem.mask     = LVIF_TEXT;
        sLvItem.iItem    = i;
        sLvItem.iSubItem = 1;
        sLvItem.pszText  = (xpr_tchar_t *)sPath.c_str();
        mListCtrl.SetItem(&sLvItem);

        sLvItem.mask     = LVIF_TEXT;
        sLvItem.iItem    = i;
        sLvItem.iSubItem = 2;
        sLvItem.pszText  = sText;
        mListCtrl.SetItem(&sLvItem);
    }

    ((CButton *)GetDlgItem(IDC_ATTR_ARCHIVE ))->SetCheck((sTotalAttrs & FILE_ATTRIBUTE_ARCHIVE ) ? 2 : 0);
    ((CButton *)GetDlgItem(IDC_ATTR_HIDDEN  ))->SetCheck((sTotalAttrs & FILE_ATTRIBUTE_HIDDEN  ) ? 2 : 0);
    ((CButton *)GetDlgItem(IDC_ATTR_READONLY))->SetCheck((sTotalAttrs & FILE_ATTRIBUTE_READONLY) ? 2 : 0);
    ((CButton *)GetDlgItem(IDC_ATTR_SYSTEM  ))->SetCheck((sTotalAttrs & FILE_ATTRIBUTE_SYSTEM  ) ? 2 : 0);

    ((CButton *)GetDlgItem(IDC_ATTR_ATTRIBUTE))->SetCheck(1);
    ((CButton *)GetDlgItem(IDC_ATTR_MODIFIED))->SetCheck(1);

    mProgressCtrl.SetRange32(0, mListCtrl.GetItemCount());

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_ATTR_DEPTH_SPIN);
    sSpinCtrl->SetRange32(MIN_DEPTH, MAX_DEPTH);

    ((CComboBox *)GetDlgItem(IDC_ATTR_TYPE))->SetCurSel(0);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.attr_time.status.count"), XPR_STRING_LITERAL("%d")), mList.size());
    setStatus(sText);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.title")));
    SetDlgItemText(IDC_ATTR_LABEL_PATH,  theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.label.path")));
    SetDlgItemText(IDC_ATTR_ATTRIBUTE,   theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.attribute")));
    SetDlgItemText(IDC_ATTR_ARCHIVE,     theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.archive")));
    SetDlgItemText(IDC_ATTR_HIDDEN,      theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.hidden")));
    SetDlgItemText(IDC_ATTR_READONLY,    theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.read_only")));
    SetDlgItemText(IDC_ATTR_SYSTEM,      theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.system")));
    SetDlgItemText(IDC_ATTR_TIME,        theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.time")));
    SetDlgItemText(IDC_ATTR_CREATION,    theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.created_time")));
    SetDlgItemText(IDC_ATTR_MODIFIED,    theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.modified_time")));
    SetDlgItemText(IDC_ATTR_ACCESS,      theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.access_time")));
    SetDlgItemText(IDC_ATTR_SUBFOLDER,   theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.check.sub_folder")));
    SetDlgItemText(IDC_ATTR_LABEL_DEPTH, theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.label.depth")));
    SetDlgItemText(IDC_ATTR_LABEL_TYPE,  theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.label.type")));
    SetDlgItemText(IDOK,                 theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.button.apply")));
    SetDlgItemText(IDCANCEL,             theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.button.close")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("AttrTime"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"),       mListCtrl.GetDlgCtrlID());
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Attribute"),  IDC_ATTR_ATTRIBUTE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Time"),       IDC_ATTR_TIME);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Sub Folder"), IDC_ATTR_SUBFOLDER);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Type"),       IDC_ATTR_TYPE);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Level"),      IDC_ATTR_DEPTH);
        mDlgState->load();
    }

    OnAttribute();
    OnTime();

    return XPR_TRUE;
}

void AttrTimeDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (mAttrTime != XPR_NULL)
    {
        mAttrTime->Stop();
    }

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void AttrTimeDlg::setStatus(const xpr_tchar_t *aStatus)
{
    if (aStatus != XPR_NULL)
        SetDlgItemText(IDC_ATTR_STATUS, aStatus);
}

void AttrTimeDlg::OnAttribute(void) 
{
    xpr_bool_t sAttribute = ((CButton *)GetDlgItem(IDC_ATTR_ATTRIBUTE))->GetCheck();
    GetDlgItem(IDC_ATTR_ARCHIVE)->EnableWindow(sAttribute);
    GetDlgItem(IDC_ATTR_READONLY)->EnableWindow(sAttribute);
    GetDlgItem(IDC_ATTR_HIDDEN)->EnableWindow(sAttribute);
    GetDlgItem(IDC_ATTR_SYSTEM)->EnableWindow(sAttribute);
}

void AttrTimeDlg::OnTime(void) 
{
    xpr_bool_t sTime = ((CButton *)GetDlgItem(IDC_ATTR_TIME))->GetCheck();
    GetDlgItem(IDC_ATTR_CREATION)->EnableWindow(sTime);
    GetDlgItem(IDC_ATTR_MODIFIED)->EnableWindow(sTime);
    GetDlgItem(IDC_ATTR_ACCESS)->EnableWindow(sTime);

    OnCreation();
    OnModified();
    OnAccess();
}

void AttrTimeDlg::OnCreation(void) 
{
    CButton *sButton = (CButton *)GetDlgItem(IDC_ATTR_CREATION);
    xpr_bool_t sEnable = sButton->GetCheck() && !(sButton->GetStyle() & WS_DISABLED);

    GetDlgItem(IDC_ATTR_CREATION_DATE)->EnableWindow(sEnable);
    GetDlgItem(IDC_ATTR_CREATION_TIME)->EnableWindow(sEnable);
}

void AttrTimeDlg::OnModified(void) 
{
    CButton *sButton = (CButton *)GetDlgItem(IDC_ATTR_MODIFIED);
    xpr_bool_t sEnable = sButton->GetCheck() && !(sButton->GetStyle() & WS_DISABLED);

    GetDlgItem(IDC_ATTR_MODIFIED_DATE)->EnableWindow(sEnable);
    GetDlgItem(IDC_ATTR_MODIFIED_TIME)->EnableWindow(sEnable);
}

void AttrTimeDlg::OnAccess(void) 
{
    CButton *sButton = (CButton *)GetDlgItem(IDC_ATTR_ACCESS);
    xpr_bool_t sEnable = sButton->GetCheck() && !(sButton->GetStyle() & WS_DISABLED);

    GetDlgItem(IDC_ATTR_ACCESS_DATE)->EnableWindow(sEnable);
    GetDlgItem(IDC_ATTR_ACCESS_TIME)->EnableWindow(sEnable);
}

void AttrTimeDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK,
                   (aEnable == XPR_TRUE) ?
                   theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.button.apply")) :
                   theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.button.stop")));

    GetDlgItem(IDC_ATTR_ATTRIBUTE)->EnableWindow(aEnable);
    OnAttribute();

    GetDlgItem(IDC_ATTR_SUBFOLDER)->EnableWindow(aEnable);
    GetDlgItem(IDC_ATTR_DEPTH)->EnableWindow(aEnable);
    GetDlgItem(IDC_ATTR_DEPTH_SPIN)->EnableWindow(aEnable);
    GetDlgItem(IDC_ATTR_TYPE)->EnableWindow(aEnable);

    GetDlgItem(IDC_ATTR_TIME)->EnableWindow(aEnable);
    OnTime();

    GetDlgItem(IDC_ATTR_LIST)->EnableWindow(aEnable);

    // System Button
    CMenu *sMenu = GetSystemMenu(XPR_FALSE);
    if (sMenu)
    {
        xpr_uint_t sFlags = MF_BYCOMMAND;

        if (aEnable == XPR_FALSE)
            sFlags |= MF_GRAYED;

        sMenu->EnableMenuItem(SC_CLOSE, sFlags);
    }

    if (aEnable == XPR_FALSE)
        GetDlgItem(IDOK)->SetFocus();
}

void AttrTimeDlg::OnOK(void) 
{
    if (mAttrTime != XPR_NULL)
    {
        if (mAttrTime->getStatus() == AttrTime::StatusChanging)
        {
            mAttrTime->Stop();
            return;
        }
    }

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.attr_time.msg.question_apply"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    xpr_sint_t sDepth = -1;
    xpr_bool_t sSubFolder = ((CButton *)GetDlgItem(IDC_ATTR_SUBFOLDER))->GetCheck();
    if (sSubFolder == XPR_TRUE)
        sDepth = GetDlgItemInt(IDC_ATTR_DEPTH);

    sDepth = max(sDepth, MIN_DEPTH);
    sDepth = min(sDepth, MAX_DEPTH);

    xpr_uint_t sType = 0;
    // sCurSel = 0 : only file
    // sCurSel = 1 : only folder
    // sCurSel = 2 : both file and folder
    xpr_sint_t sCurSel = ((CComboBox *)GetDlgItem(IDC_ATTR_TYPE))->GetCurSel();
    switch (sCurSel)
    {
    case 1:  sType = AttrTime::TypeFolder; break;
    case 2:  sType = AttrTime::TypeFile | AttrTime::TypeFolder; break;
    default: sType = AttrTime::TypeFile; break;
    }

    xpr_bool_t sAttribute = ((CButton *)GetDlgItem(IDC_ATTR_ATTRIBUTE))->GetCheck();
    xpr_sint_t sArchive   = ((CButton *)GetDlgItem(IDC_ATTR_ARCHIVE))->GetCheck();
    xpr_sint_t sReadOnly  = ((CButton *)GetDlgItem(IDC_ATTR_READONLY))->GetCheck();
    xpr_sint_t sHidden    = ((CButton *)GetDlgItem(IDC_ATTR_HIDDEN))->GetCheck();
    xpr_sint_t sSystem    = ((CButton *)GetDlgItem(IDC_ATTR_SYSTEM))->GetCheck();

    xpr_bool_t sTime      = ((CButton *)GetDlgItem(IDC_ATTR_TIME))->GetCheck();
    xpr_bool_t sCreation  = ((CButton *)GetDlgItem(IDC_ATTR_CREATION))->GetCheck();
    xpr_bool_t sModified  = ((CButton *)GetDlgItem(IDC_ATTR_MODIFIED))->GetCheck();
    xpr_bool_t sAccess    = ((CButton *)GetDlgItem(IDC_ATTR_ACCESS))->GetCheck();

    SYSTEMTIME sCreatedDate  = {0};
    SYSTEMTIME sModifiedDate = {0};
    SYSTEMTIME sAccessDate   = {0};
    SYSTEMTIME sCreatedTime  = {0};
    SYSTEMTIME sModifiedTime = {0};
    SYSTEMTIME sAccessTime   = {0};

    mCreatedDateCtrl.GetTime(&sCreatedDate);
    mCreatedTimeCtrl.GetTime(&sCreatedTime);
    mModifiedDateCtrl.GetTime(&sModifiedDate);
    mModifiedTimeCtrl.GetTime(&sModifiedTime);
    mAccessDateCtrl.GetTime(&sAccessDate);
    mAccessTimeCtrl.GetTime(&sAccessTime);

    sCreatedTime.wYear       = sCreatedDate.wYear;
    sCreatedTime.wMonth      = sCreatedDate.wMonth;
    sCreatedTime.wDayOfWeek  = sCreatedDate.wDayOfWeek;
    sCreatedTime.wDay        = sCreatedDate.wDay;

    sModifiedTime.wYear      = sModifiedDate.wYear;
    sModifiedTime.wMonth     = sModifiedDate.wMonth;
    sModifiedTime.wDayOfWeek = sModifiedDate.wDayOfWeek;
    sModifiedTime.wDay       = sModifiedDate.wDay;

    sAccessTime.wYear        = sAccessDate.wYear;
    sAccessTime.wMonth       = sAccessDate.wMonth;
    sAccessTime.wDayOfWeek   = sAccessDate.wDayOfWeek;
    sAccessTime.wDay         = sAccessDate.wDay;

    XPR_SAFE_DELETE(mAttrTime);

    mAttrTime = new AttrTime;
    mAttrTime->setOwner(m_hWnd, WM_FINALIZE);

    mAttrTime->setSubFolder(sSubFolder, sDepth);
    mAttrTime->setType(sType);
    mAttrTime->setAttributes(sAttribute, sArchive, sReadOnly, sHidden, sSystem);
    mAttrTime->setTime(
        (sTime == XPR_TRUE && sCreation == XPR_TRUE) ? &sCreatedTime  : XPR_NULL,
        (sTime == XPR_TRUE && sModified == XPR_TRUE) ? &sModifiedTime : XPR_NULL,
        (sTime == XPR_TRUE && sAccess   == XPR_TRUE) ? &sAccessTime   : XPR_NULL);

    PathList::iterator sIterator = mList.begin();
    for (; sIterator != mList.end(); ++sIterator)
        mAttrTime->addPath(*sIterator);

    if (mAttrTime->Start())
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);
    }
}

LRESULT AttrTimeDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mAttrTime->Stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    AttrTime::Status sStatus;
    xpr_size_t sProcessedCount = 0;
    xpr_size_t sSucceededCount = 0;
    sStatus = mAttrTime->getStatus(&sProcessedCount, &sSucceededCount);

    mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);

    xpr_tchar_t sStatusText[0xff] = {0};

    switch (sStatus)
    {
    case AttrTime::StatusChangeCompleted:
        {
            xpr_size_t sCount = mAttrTime->getCount();

            _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.attr_time.status.completed"), XPR_STRING_LITERAL("%d,%d,%d")), sCount, sSucceededCount, sCount - sSucceededCount);
            setStatus(sStatusText);

            super::OnOK();
            break;
        }

    case AttrTime::StatusStopped:
        {
            _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.attr_time.status.stoped"), XPR_STRING_LITERAL("%d")), sProcessedCount);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}

void AttrTimeDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sCount;
        xpr_size_t sProcessed;

        sCount = mAttrTime->getCount();
        mAttrTime->getStatus(&sProcessed);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.attr_time.status.in_progress"), XPR_STRING_LITERAL("%d,%d")), sProcessed, sCount);

        setStatus(sStatusText);
        mProgressCtrl.SetPos((xpr_sint_t)sProcessed);
    }

    super::OnTimer(nIDEvent);
}
} // namespace cmd
} // namespace fxfile
