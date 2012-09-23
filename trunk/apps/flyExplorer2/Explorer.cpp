//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "Explorer.h"

#include "fxb/fxb_file_op_undo.h"     // for Undo
#include "fxb/fxb_drive_shcn.h"
#include "fxb/fxb_cmd_line_parser.h"
#include "fxb/fxb_filter.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_language_table.h"
#include "fxb/fxb_language_pack.h"
#include "fxb/fxb_string_table.h"
#include "fxb/fxb_format_string_table.h"

#include "TraceWin.h"           // for Trace External Output
#include "MainFrame.h"
#include "ExplorerView.h"
#include "OptionMgr.h"
#include "CfgPath.h"
#include "RecentFileListEx.h"
#include "AppVer.h"
#include "gfl/libgfl.h"

#include "cfg/CfgMgrDlg.h"

#include "cmd/TipDlg.h"

#ifdef _MEM_LEAK
#include "StackWalker.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kFileSection[]    = XPR_STRING_LITERAL("Recent File List");
static const xpr_tchar_t kFileEntry[]      = XPR_STRING_LITERAL("File%d");
static const xpr_tchar_t kPreviewSection[] = XPR_STRING_LITERAL("Settings");
static const xpr_tchar_t kPreviewEntry[]   = XPR_STRING_LITERAL("PreviewPages");

static const xpr_tchar_t kSingleInstanceMutexName[] = XPR_STRING_LITERAL("flyExplorer - Single Instance");

ExplorerApp theApp;
Option *gOpt;

ExplorerApp::ExplorerApp(void)
    : mSingleInstanceMutex(XPR_NULL)
    , mLanguageTable(XPR_NULL), mStringTable(XPR_NULL), mFormatStringTable(XPR_NULL)
{
#if defined(XPR_CFG_BUILD_RELEASE)

    xpr_tchar_t sAppVer[100] = {0};
    getAppVer(sAppVer);

#ifdef XPR_CFG_UNICODE
    _tcscat(sAppVer, XPR_STRING_LITERAL(" Unicode"));
#else
    _tcscat(sAppVer, XPR_STRING_LITERAL(""));
#endif

    fxCrashHandler();
    fxSetAppName(XPR_STRING_LITERAL("flyExplorer"));
    fxSetAppVer(sAppVer);
    fxSetDevelopInfo(XPR_STRING_LITERAL("flychk@flychk.com"), XPR_STRING_LITERAL("http://www.flychk.com"));

#endif
}

ExplorerApp::~ExplorerApp(void)
{
}

BEGIN_MESSAGE_MAP(ExplorerApp, super)
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, super::OnFileOpen)
END_MESSAGE_MAP()

xpr_bool_t ExplorerApp::InitInstance(void)
{
#ifdef _MEM_LEAK
    InitAllocCheck(ACOutput_Advanced);
#endif

    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return XPR_FALSE;
    }

    AfxEnableControlContainer();

    //
    // initialize XPR
    //
    xpr::initialize();

    // Html Help File Name
    xpr_tchar_t *sDot = (xpr_tchar_t *)_tcsrchr(m_pszHelpFilePath, '.');
    if (sDot != XPR_NULL)
        _tcscpy(sDot, XPR_STRING_LITERAL(".chm"));

    // Program file name verification routine
#if defined(XPR_CFG_BUILD_RELEASE)
    // flyExplorer File Name Check Routine
    if (_tcsicmp(m_pszExeName, XPR_STRING_LITERAL("flyExplorer")) != 0)
    {
        CString sMsg;
        sMsg  = XPR_STRING_LITERAL("This program is \'flyExplorer\' file manager.\n");
        sMsg += XPR_STRING_LITERAL("It is contained that a routine verfiy for this program file name.\n");
        sMsg += XPR_STRING_LITERAL("The file name must be \'flyExplorer.exe\'.");
        sMsg += XPR_STRING_LITERAL("Please, rename to \'flyExplorer.exe\', ater then execute this program again.\n");
        sMsg += XPR_STRING_LITERAL("\n");
        sMsg += XPR_STRING_LITERAL("Please, contact to homepage: http://flychk.com or e-mail: flychk@flychk.com for any question.");
        sMsg += XPR_STRING_LITERAL("\n");
        MessageBox(XPR_NULL, sMsg, XPR_STRING_LITERAL("flyExplorer"), MB_OK | MB_ICONSTOP);
        return XPR_FALSE;

        //CString sMsg;
        //sMsg  = XPR_STRING_LITERAL("본 프로그램은 flyExplorer 파일 관리 프로그램입니다.\n");
        //sMsg += XPR_STRING_LITERAL("이 프로그램에는 실행 파일명을 검사하는 루틴이 포함되어 있습니다.\n");
        //sMsg += XPR_STRING_LITERAL("실행 파일명을 \'flyExplorer.exe\'로 변경한 후, 다시 실행하여 주시기 바랍니다.\n");
        //sMsg += XPR_STRING_LITERAL("\n");
        //sMsg += XPR_STRING_LITERAL("문의 사항은 홈페이지 http://flychk.com이나 전자 우편 flychk@flychk.com으로 해 주시기 바랍니다.");
        //sMsg += XPR_STRING_LITERAL("\n");
        //MessageBox(XPR_NULL, sMsg, XPR_STRING_LITERAL("flyExplorer"), MB_OK | MB_ICONSTOP);
        //return XPR_FALSE;
    }
#endif // XPR_CFG_BUILD_RELEASE

    // shell change notification
    fxb::ShellChangeNotify &sShellChangeNotify = fxb::ShellChangeNotify::instance();
    sShellChangeNotify.create();
    sShellChangeNotify.start();

    fxb::DriveShcn &sDriveShcn = fxb::DriveShcn::instance();
    sDriveShcn.create();
    sDriveShcn.start();

    // initailze gfl Library
    gflLibraryInit();
    gflEnableLZW(GFL_TRUE);

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need.
    INITCOMMONCONTROLSEX sInitCommonContrlEx;
    sInitCommonContrlEx.dwSize = sizeof(sInitCommonContrlEx);
    sInitCommonContrlEx.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&sInitCommonContrlEx);

    // Change the registry key under which our settings are stored.
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization.
    // Company Name\\AFX_IDS_APP_TITLE
#ifdef XPR_CFG_BUILD_DEBUG
    SetRegistryKey(XPR_STRING_LITERAL("flyExplorer_dbg"));
#else
    SetRegistryKey(XPR_STRING_LITERAL("flyExplorer"));
#endif

    // load configuration path
    CfgPath::instance().load();

    // load configuration
    OptionMgr &sOptionMgr = OptionMgr::instance();

    xpr_bool_t sInitCfg = XPR_FALSE;
    sOptionMgr.load(sInitCfg);

    gOpt = sOptionMgr.getOption();

    // load language table & string table
    if (loadLanguageTable() == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = XPR_STRING_LITERAL("Language file(s) don't exist!!!");
        MessageBox(XPR_NULL, sMsg, XPR_STRING_LITERAL("flyExplorer"), MB_OK | MB_ICONSTOP);
        return XPR_FALSE;
    }

    loadLanguage(gOpt->mLanguage);

    // load link
    fxb::BookmarkMgr::instance().load();

    // single instance by option
    if (XPR_IS_TRUE(gOpt->mSingleInstance))
    {
        mSingleInstanceMutex = ::OpenMutex(MUTEX_ALL_ACCESS, XPR_FALSE, kSingleInstanceMutexName);
        if (mSingleInstanceMutex != XPR_NULL)
        {
            extern xpr_uint_t WM_SINGLEINST;
            ::PostMessage(HWND_BROADCAST, WM_SINGLEINST, 0, 0);

            return XPR_FALSE;
        }

        setSingleInstance(XPR_TRUE);
    }

    LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

    MainFrame *sMainFrame = new MainFrame;
    if (XPR_IS_NULL(sMainFrame))
        return XPR_FALSE;

    m_pMainWnd = sMainFrame;
    sMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, XPR_NULL, XPR_NULL);

    fxb::CmdLineParser::instance().parse();

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo sCommandLineInfo;
    sCommandLineInfo.m_nShellCommand = CCommandLineInfo::FileNew;

    // Dispatch commands specified on the command line
    if (ProcessShellCommand(sCommandLineInfo) == XPR_FALSE)
        return XPR_FALSE;

    fxb::CmdLineParser::instance().clear();

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->SetForegroundWindow();
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    if (sInitCfg == XPR_TRUE)
    {
        CfgMgrDlg sDlg;
        sDlg.DoModal();
    }

    if (gOpt->mTipOfTheToday == XPR_TRUE)
    {
        TipDlg sDlg;
        sDlg.DoModal();
    }

    sMainFrame->ShowWindow(SW_SHOW);

    return XPR_TRUE;
}

xpr_sint_t ExplorerApp::ExitInstance(void) 
{
    // delete undo directory
    fxb::FileOpUndo::deleteUndoDir();

    // Exit gfl Library
    gflLibraryExit(); 

    // Save Main Option
    if (OptionMgr::isInstance() == XPR_TRUE)
        OptionMgr::instance().saveMainOption();

    if (CfgPath::isInstance() == XPR_TRUE)
        CfgPath::instance().save();

    fxb::ShellChangeNotify::instance().stop();
    fxb::ShellChangeNotify::instance().destroy();

    fxb::DriveShcn::instance().stop();
    fxb::DriveShcn::instance().destroy();

    XPR_SAFE_DELETE(mLanguageTable);

    CLOSE_HANDLE(mSingleInstanceMutex);

#ifdef _MEM_LEAK
    DeInitAllocCheck();
#endif

    //
    // finalize XPR
    //
    xpr::finalize();

    return super::ExitInstance();
}

void ExplorerApp::setSingleInstance(xpr_bool_t aSingleInstance)
{
    if (aSingleInstance == XPR_TRUE)
    {
        if (mSingleInstanceMutex == XPR_NULL)
            mSingleInstanceMutex = ::CreateMutex(XPR_NULL, XPR_FALSE, kSingleInstanceMutexName);
    }
    else
    {
        CLOSE_HANDLE(mSingleInstanceMutex);
    }
}

void ExplorerApp::LoadStdProfileSettings(xpr_uint_t nMaxMRU)
{
    ASSERT_VALID(this);
    ASSERT(m_pRecentFileList == XPR_NULL);

    if (nMaxMRU != 0)
    {
        // create file MRU since nMaxMRU not zero
        m_pRecentFileList = new RecentFileListEx(0, kFileSection, kFileEntry, nMaxMRU);
        m_pRecentFileList->ReadList();
    }

    // 0 by default means not set
    m_nNumPreviewPages = GetProfileInt(kPreviewSection, kPreviewEntry, 0);
}

xpr_bool_t ExplorerApp::updateRecentMenu(BCMenu *aMenu, xpr_sint_t aInsert)
{
    RecentFileListEx *sRecentFileListEx = (RecentFileListEx *)m_pRecentFileList;
    if (sRecentFileListEx == XPR_NULL)
        return XPR_FALSE;

    sRecentFileListEx->updateMenu(aMenu, aInsert);

    return XPR_TRUE;
}

xpr_bool_t ExplorerApp::getRecentFile(xpr_uint_t aId, xpr_tchar_t *aPath)
{
    if (!XPR_IS_RANGE(ID_FILE_RECENT_FIRST, aId, ID_FILE_RECENT_LAST))
        return XPR_FALSE;

    RecentFileListEx *sRecentFileListEx = (RecentFileListEx *)m_pRecentFileList;
    if (sRecentFileListEx == XPR_NULL)
        return XPR_FALSE;

    xpr_sint_t sIndex = aId - ID_FILE_RECENT_FIRST;
    sRecentFileListEx->getRecentFile(sIndex, aPath);

    return XPR_TRUE;
}

void ExplorerApp::removeRecentFileList(void)
{
    xpr_sint_t i;
    xpr_sint_t sSize = m_pRecentFileList->GetSize();
    for (i = sSize - 1; i >= 0; --i)
        m_pRecentFileList->Remove(i);
}

void ExplorerApp::OnFileNew(void)
{
    super::OnFileNew();
}

xpr_bool_t ExplorerApp::loadLanguageTable(void)
{
    mLanguageTable = new fxb::LanguageTable;
    if (XPR_IS_NULL(mLanguageTable))
        return XPR_FALSE;

    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    fxb::GetModuleDir(sDir, XPR_MAX_PATH);

    if (sDir[_tcslen(sDir) - 1] != XPR_STRING_LITERAL('\\'))
        _tcscat(sDir, XPR_STRING_LITERAL("\\"));

    _tcscat(sDir, XPR_STRING_LITERAL("Languages"));

    mLanguageTable->setDir(sDir);

    if (mLanguageTable->scan() == XPR_FALSE)
    {
        // TODO load embedded english language pack
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerApp::loadLanguage(const xpr_tchar_t *aLanguage)
{
    if (XPR_IS_NULL(aLanguage))
        return XPR_FALSE;

    mStringTable = XPR_NULL;
    mFormatStringTable = XPR_NULL;

    if (mLanguageTable->loadLanguage(aLanguage) == XPR_FALSE)
        return XPR_FALSE;

    mStringTable = mLanguageTable->getStringTable();
    mFormatStringTable = mLanguageTable->getFormatStringTable();

    fxb::Filter::setString(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.folder")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.general_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.executable_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.compressed_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.document_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.image_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.sound_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.movie_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.web_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.programming_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.default_filter_name.temporary_file")));

    fxb::SizeFormat::setText(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.size.unit.none")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.size.unit.automatic")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.size.unit.default")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.size.unit.custom")),
        theApp.loadString(XPR_STRING_LITERAL("common.size.byte")));

    return XPR_TRUE;
}

const xpr_tchar_t *ExplorerApp::loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable)
{
    if (XPR_IS_NULL(mStringTable))
        return aId;

    return mStringTable->loadString(aId, aNullAvailable);
}

const xpr_tchar_t *ExplorerApp::loadFormatString(const xpr_tchar_t *aId, const xpr_tchar_t *aReplaceFormatSpecifier)
{
    if (XPR_IS_NULL(mFormatStringTable))
        return aId;

    return mFormatStringTable->loadString(aId, aReplaceFormatSpecifier);
}
