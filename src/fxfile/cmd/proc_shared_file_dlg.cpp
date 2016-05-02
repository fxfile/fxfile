//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "proc_shared_file_dlg.h"

#include "context_menu.h"
#include "SystemInfo.h"
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
ProcSharedFileDlg::ProcSharedFileDlg(const xpr_tchar_t *aPath)
    : super(IDD_PROC_SHARED_FILE, XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    if (aPath != XPR_NULL)
        _tcscpy(mPath, aPath);
}

ProcSharedFileDlg::~ProcSharedFileDlg(void)
{
    DESTROY_ICON(mIcon);
}

void ProcSharedFileDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SHARED_PROC_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(ProcSharedFileDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_PROPERTIES, &ProcSharedFileDlg::OnProperties)
END_MESSAGE_MAP()

xpr_sint_t ProcSharedFileDlg::setPath(const xpr_tchar_t *aPath)
{
    CString sName;
    CString sProcessName;
    CString sDeviceFileName;
    SystemProcessInformation::SYSTEM_PROCESS_INFORMATION *sSystemProcessInformationPtr;
    SystemProcessInformation sSystemProcessInformation;
    SystemHandleInformation sSystemHandleInformation;
    xpr_bool_t sShow = XPR_FALSE;

    if (SystemInfoUtils::GetDeviceFileName(aPath, sDeviceFileName) == XPR_FALSE)
    {
        //_tprintf( XPR_STRING_LITERAL("GetDeviceFileName() failed.\n") );
        return 0;
    }

    sSystemHandleInformation.SetFilter(XPR_STRING_LITERAL("File"), XPR_TRUE);

    if (sSystemHandleInformation.m_HandleInfos.GetHeadPosition() == XPR_NULL)
    {
        //_tprintf( XPR_STRING_LITERAL("No handle information\n") );
        return 0;
    }

    sSystemProcessInformation.Refresh();

    xpr_sint_t sCount = 0;
    for (POSITION sPos = sSystemHandleInformation.m_HandleInfos.GetHeadPosition(); sPos != XPR_NULL; )
    {
        SystemHandleInformation::SYSTEM_HANDLE &sSystemHandle = sSystemHandleInformation.m_HandleInfos.GetNext(sPos);

        sProcessName = "";
        if (sSystemProcessInformation.m_ProcessInfos.Lookup(sSystemHandle.ProcessID, sSystemProcessInformationPtr))
        {
            SystemInfoUtils::Unicode2CString(&sSystemProcessInformationPtr->usName, sProcessName);
        }

        // NT4 Stupid thing if it is the services.exe and I call GetName :((
        if (INtDll::dwNTMajorVersion == 4 && _tcsicmp(sProcessName, XPR_STRING_LITERAL("services.exe")) == 0)
            continue;

        sSystemHandleInformation.GetName((HANDLE)sSystemHandle.HandleNumber, sName, sSystemHandle.ProcessID);

        sShow = _tcsicmp(sName, sDeviceFileName) == 0;
        if (sShow == XPR_TRUE)
        {
            LVITEM sLvItem = {0};
            xpr_sint_t sIndex = sCount;
            xpr_tchar_t sText[XPR_MAX_PATH + 1];

            _stprintf(sText, XPR_STRING_LITERAL("%d"), sIndex + 1);
            sLvItem.mask     = LVIF_TEXT;
            sLvItem.iItem    = sIndex;
            sLvItem.iSubItem = 0;
            sLvItem.pszText  = sText;
            mListCtrl.InsertItem(&sLvItem);

            _stprintf(sText, XPR_STRING_LITERAL("%d"), sSystemHandle.ProcessID);
            sLvItem.iSubItem = 1;
            mListCtrl.SetItem(&sLvItem);

            _stprintf(sText, XPR_STRING_LITERAL("%s"), (const xpr_tchar_t *)sProcessName);
            sLvItem.iSubItem = 2;
            mListCtrl.SetItem(&sLvItem);

            sCount++;

            //_stprintf( XPR_STRING_LITERAL("0x%04X  %-20s  %s\n"), sSystemHandle.ProcessID, processName, lpFileName);
        }
    }

    return sCount;
}

xpr_bool_t ProcSharedFileDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_SHARED_PROC_PATH,       sizeResize, sizeNone);
    AddControl(IDC_SHARED_PROC_LIST,       sizeResize, sizeResize);
    AddControl(IDC_SHARED_PROC_STATUS,     sizeResize, sizeRepos, XPR_FALSE);
    AddControl(IDC_SHARED_PROC_PROPERTIES, sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDOK,                       sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    CWaitCursor sWaitCursor;

    SetDlgItemText(IDC_SHARED_PROC_PATH, mPath);

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.list.column.no")),      LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.list.column.pid")),     LVCFMT_RIGHT,  50, -1);
    mListCtrl.InsertColumn(2, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.list.column.process")), LVCFMT_LEFT,  200, -1);

    xpr_tchar_t sStatusText[0xff] = {0};
    if (xpr::getOsVer() >= xpr::kOsVerWinNT)
    {
        xpr_sint_t nCount = setPath(mPath);
        if (nCount > 0)
        {
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.shared_process.status.count"), XPR_STRING_LITERAL("%d")), nCount);
        }
        else
        {
            _stprintf(sStatusText, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.status.none")));
        }
    }
    else
    {
        _stprintf(sStatusText, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.status.not_support_os_platform")));
    }

    setStatus(sStatusText);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.title")));
    SetDlgItemText(IDC_SHARED_PROC_LABEL_PATH, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.label.path")));
    SetDlgItemText(IDC_SHARED_PROC_LABEL_LIST, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.label.list")));
    SetDlgItemText(IDC_SHARED_PROC_PROPERTIES, gApp.loadString(XPR_STRING_LITERAL("popup.shared_process.button.properties")));
    SetDlgItemText(IDOK,                       gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("ProcSharedFile"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

void ProcSharedFileDlg::setStatus(const xpr_tchar_t *aStatusText)
{
    if (aStatusText != XPR_NULL)
        SetDlgItemText(IDC_SHARED_PROC_STATUS, aStatusText);
}

void ProcSharedFileDlg::OnDestroy(void) 
{
    super::OnDestroy();

    DESTROY_ICON(mIcon);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

xpr_bool_t ProcSharedFileDlg::DestroyWindow(void) 
{
    return super::DestroyWindow();
}

static DWORD GetModuleFileNameEx(HANDLE       aProcess,   // handle to process
                                 HMODULE      aModule,    // handle to module
                                 xpr_tchar_t *aFilename,  // path buffer
                                 DWORD        aSize)      // maximum characters to retrieve
{
    DWORD sResult = 0;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("PSAPI.DLL"));
    if (sDll == XPR_NULL)
        return sResult;

    typedef DWORD (WINAPI *PFN_GetModuleFileNameEx)(
        HANDLE       hProcess,    // handle to process
        HMODULE      hModule,     // handle to module
        xpr_tchar_t *lpFilename,  // path buffer
        DWORD        nSize        // maximum characters to retrieve
        );

    PFN_GetModuleFileNameEx pfnGetModuleFileNameEx;
    pfnGetModuleFileNameEx = (PFN_GetModuleFileNameEx)GetProcAddress(sDll, 
#ifdef XPR_CFG_UNICODE
        "GetModuleFileNameExW");
#else
        "GetModuleFileNameExA");
#endif

    if (pfnGetModuleFileNameEx != XPR_NULL)
        sResult = (pfnGetModuleFileNameEx)(aProcess, aModule, aFilename, aSize);

    ::FreeLibrary(sDll);
    sDll = XPR_NULL;

    return sResult;
}

void ProcSharedFileDlg::OnProperties(void)
{
    if (mListCtrl.GetSelectedCount() != 1)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    xpr_tchar_t sText[0xff] = {0};
    mListCtrl.GetItemText(sIndex, 1, sText, 0xfe);

    DWORD sProcessId;
    _stscanf(sText, XPR_STRING_LITERAL("%ld"), &sProcessId);

    HANDLE sProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, XPR_FALSE, sProcessId);
    if (sProcess == XPR_NULL)
        return;

    DWORD sResult;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    sResult = GetModuleFileNameEx((HMODULE)sProcess, XPR_NULL, sPath, XPR_MAX_PATH);
    if (sResult != 0)
    {
        HRESULT sHResult;
        LPITEMIDLIST sFullPidl = XPR_NULL;

        sHResult = fxfile::base::Pidl::create(sPath, sFullPidl);
        if (SUCCEEDED(sHResult) && sFullPidl != XPR_NULL)
        {
            xpr_bool_t    sResult;
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPCITEMIDLIST sPidl        = XPR_NULL;

            sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
            if (XPR_IS_TRUE(sResult) && sShellFolder != XPR_NULL && sPidl != XPR_NULL)
            {
                ContextMenu::invokeCommand(sShellFolder, &sPidl, 1, CMID_VERB_PROPERTIES, GetSafeHwnd());
            }

            COM_RELEASE(sShellFolder);
        }

        COM_FREE(sFullPidl);
    }

    CLOSE_HANDLE(sProcess);
}
} // namespace cmd
} // namespace fxfile
