//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
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

#include "MainFrame.h"
#include "ExplorerView.h"
#include "Option.h"
#include "OptionMgr.h"
#include "CfgPath.h"
#include "AppVer.h"
#include "command_string_table.h"
#include "ShellRegistry.h"
#include "LauncherMgr.h"

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
    if (AfxOleInit() == XPR_FALSE)
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
        sMsg += XPR_STRING_LITERAL("Please, rename to \'flyExplorer.exe\' and then execute this program again.\n");
        sMsg += XPR_STRING_LITERAL("\n");
        sMsg += XPR_STRING_LITERAL("Please, contact to homepage: http://flychk.com or e-mail: flychk@flychk.com for any question.");
        sMsg += XPR_STRING_LITERAL("\n");
        MessageBox(XPR_NULL, sMsg, XPR_STRING_LITERAL("flyExplorer"), MB_OK | MB_ICONSTOP);
        return XPR_FALSE;
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

    // set registry key of this program
#ifdef XPR_CFG_BUILD_DEBUG
    SetRegistryKey(XPR_STRING_LITERAL("flyExplorer_dbg"));
#else
    SetRegistryKey(XPR_STRING_LITERAL("flyExplorer"));
#endif

    // load configuration path
    CfgPath &sCfgPath = CfgPath::instance();
    sCfgPath.load();

    // load options from file or load default option if configuration file does not exist
    OptionMgr &sOptionMgr = OptionMgr::instance();

    xpr_bool_t sInitCfg = XPR_FALSE;
    sOptionMgr.load(sInitCfg);

    gOpt = sOptionMgr.getOption();

    gOpt->setObserver(dynamic_cast<OptionObserver *>(this));

    // load language table & string table
    if (loadLanguageTable() == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = XPR_STRING_LITERAL("Language file(s) don't exist!!!");
        MessageBox(XPR_NULL, sMsg, XPR_STRING_LITERAL("flyExplorer"), MB_OK | MB_ICONSTOP);
        return XPR_FALSE;
    }

    // load language
    loadLanguage(gOpt->mConfig.mLanguage);

    // load command string table
    CommandStringTable::instance().load();

    // load bookmark
    fxb::BookmarkMgr::instance().load();

    // check it if single instance by option
    if (XPR_IS_TRUE(gOpt->mConfig.mSingleInstance))
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

    // load recent executed file list
    LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

    // load main frame
    MainFrame *sMainFrame = new MainFrame;
    if (XPR_IS_NULL(sMainFrame))
        return XPR_FALSE;

    m_pMainWnd = sMainFrame;
    if (sMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, XPR_NULL, XPR_NULL) == XPR_FALSE)
        return XPR_FALSE;

    // parse command line arguments
    fxb::CmdLineParser &sCmdLineParser = fxb::CmdLineParser::instance();
    sCmdLineParser.parse();
    sCmdLineParser.clear();

    // The one and only window has been initialized, so show and update it.
    sMainFrame->SetForegroundWindow();
    sMainFrame->ShowWindow(m_nCmdShow);
    sMainFrame->UpdateWindow();

    // popup preference dialog if configuration file does not exist on loading time.
    if (XPR_IS_TRUE(sInitCfg))
    {
        CfgMgrDlg sDlg;
        sDlg.DoModal();
    }

    // popup tip of today dialog by option
    if (XPR_IS_TRUE(gOpt->mMain.mTipOfTheToday))
    {
        TipDlg sDlg;
        sDlg.DoModal();
    }

    // show main frame
    sMainFrame->ShowWindow(SW_SHOW);

    return XPR_TRUE;
}

xpr_sint_t ExplorerApp::ExitInstance(void) 
{
    // delete undo directory
    fxb::FileOpUndo::deleteUndoDir();

    // Exit gfl Library
    gflLibraryExit(); 

    // save main option
    if (OptionMgr::isInstance() == XPR_TRUE)
        OptionMgr::instance().saveMainOption();

    // destroy shell change notify
    fxb::ShellChangeNotify &sShellChangeNotify = fxb::ShellChangeNotify::instance();
    sShellChangeNotify.stop();
    sShellChangeNotify.destroy();

    // destroy drive shell change notify
    fxb::DriveShcn &sDriveShcn = fxb::DriveShcn::instance();
    sDriveShcn.stop();
    sDriveShcn.destroy();

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

void ExplorerApp::LoadStdProfileSettings(xpr_uint_t aMaxMRU)
{
    ASSERT_VALID(this);

    // 0 by default means not set
    m_nNumPreviewPages = GetProfileInt(kPreviewSection, kPreviewEntry, 0);
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

    xpr_time_t sTime1 = xpr::timer_ms();

    if (mLanguageTable->loadLanguage(aLanguage) == XPR_FALSE)
        return XPR_FALSE;

    xpr_time_t sTime2 = xpr::timer_ms();

    XPR_TRACE(XPR_STRING_LITERAL("Loading time of language file = %I64dms\n"), sTime2 - sTime1);

    mStringTable = mLanguageTable->getStringTable();
    mFormatStringTable = mLanguageTable->getFormatStringTable();

    fxb::Filter::setString(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.folder")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.general_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.executable_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.compressed_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.document_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.image_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.sound_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.movie_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.web_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.programming_file")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.temporary_file")));

    fxb::SizeFormat::setText(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.none")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.automatic")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.default")),
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.custom")),
        theApp.loadString(XPR_STRING_LITERAL("common.size.byte")));

    return XPR_TRUE;
}

const fxb::LanguageTable *ExplorerApp::getLanguageTable(void) const
{
    return mLanguageTable;
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

void ExplorerApp::onChangedConfig(Option &aOption)
{
    // unregister shell registry for old language
    ShellRegistry::unregisterShell();

    // popup warning message about language change
    const fxb::LanguagePack::Desc *sLoadedLanguagePackDesc = mLanguageTable->getLanguageDesc();
    if (_tcsicmp(sLoadedLanguagePackDesc->mLanguage, aOption.mConfig.mLanguage) != 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.msg.apply_language_on_next_loading_time"));
        AfxMessageBox(sMsg, MB_OK | MB_ICONWARNING);
    }

    // load language on runtime
    //loadLanguage(aOption.mConfig.mLanguage);

    // set single instance
    setSingleInstance(aOption.mConfig.mSingleInstance);

    // shell registry
    if (XPR_IS_TRUE(aOption.mConfig.mRegShellContextMenu))
    {
        ShellRegistry::registerShell();
    }
    else
    {
        ShellRegistry::unregisterShell();
    }

    // launcher
    if (XPR_IS_TRUE(aOption.mConfig.mLauncher))
    {
        LauncherMgr::startLauncher(aOption.mConfig.mLauncherGlobalHotKey, aOption.mConfig.mLauncherTray);
    }
    else
    {
        LauncherMgr::stopLauncher(aOption.mConfig.mLauncherGlobalHotKey, aOption.mConfig.mLauncherTray);
    }

    if (XPR_IS_TRUE(aOption.mConfig.mLauncherWinStartup))
    {
        LauncherMgr::registerWinStartup();
    }
    else
    {
        LauncherMgr::unregisterWinStartup();
    }

    MainFrame *sMainFrame = (MainFrame *)GetMainWnd();

    sMainFrame->setChangedOption(aOption);
}

void ExplorerApp::saveAllOptions(void)
{
    // save main frame options
    MainFrame *sMainFrame = (MainFrame *)GetMainWnd();
    sMainFrame->saveAllOptions();
}
