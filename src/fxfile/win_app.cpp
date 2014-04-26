//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "win_app.h"

#include "language_table.h"
#include "language_pack.h"
#include "string_table.h"
#include "format_string_table.h"

#include "file_op_undo.h"     // for Undo
#include "drive_shcn.h"
#include "program_opts.h"
#include "filter.h"
#include "size_format.h"
#include "winapi_ex.h"

#include "main_frame.h"
#include "explorer_view.h"
#include "option.h"
#include "option_manager.h"
#include "conf_dir.h"
#include "app_ver.h"
#include "command_string_table.h"
#include "shell_registry.h"
#include "launcher_manager.h"
#include "updater_manager.h"
#include "singleton_manager.h"
#include "single_process.h"

#include "gfl/libgfl.h"

#include "cfg/cfg_main_dlg.h"

#include "cmd/tip_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
// reference: https://svn.boost.org/trac/boost/ticket/5582
void freeTypeinfoMemory(void)
{
   __type_info_node *sNode     = __type_info_root_node.next;
   __type_info_node *sTempNode = &__type_info_root_node;

   for (; sNode != XPR_NULL; sNode = sTempNode)
   {
      sTempNode = sNode->next;
      delete sNode->memPtr;
      delete sNode;
   }
}

const xpr_tchar_t kPreviewSection[] = XPR_STRING_LITERAL("Settings");
const xpr_tchar_t kPreviewEntry[]   = XPR_STRING_LITERAL("PreviewPages");
} // namespace anonymous

WinApp gApp;
Option *gOpt;

WinApp::WinApp(void)
    : mLanguageTable(XPR_NULL), mStringTable(XPR_NULL), mFormatStringTable(XPR_NULL)
{
#if defined(XPR_CFG_BUILD_RELEASE)

    xpr_tchar_t sAppVer[0xff] = {0};
    getAppVer(sAppVer);

#ifdef XPR_CFG_UNICODE
    _tcscat(sAppVer, XPR_STRING_LITERAL(" Unicode"));
#else
    _tcscat(sAppVer, XPR_STRING_LITERAL(""));
#endif

    fxfile_crash_init();
    fxfile_crash_setAppName(FXFILE_PROGRAM_NAME);
    fxfile_crash_setAppVer(sAppVer);
    fxfile_crash_setDevelopInfo(XPR_STRING_LITERAL("flychk@flychk.com"), XPR_STRING_LITERAL("http://www.flychk.com"));

#endif
}

WinApp::~WinApp(void)
{
    // clean RTTI memory leak
    freeTypeinfoMemory();
}

BEGIN_MESSAGE_MAP(WinApp, super)
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, super::OnFileOpen)
END_MESSAGE_MAP()

xpr_bool_t WinApp::InitInstance(void)
{
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
    xpr_bool_t sResult = xpr::initialize();
    XPR_ASSERT(sResult == XPR_TRUE);

    // Html Help File Name
    xpr_tchar_t *sDot = (xpr_tchar_t *)_tcsrchr(m_pszHelpFilePath, '.');
    if (sDot != XPR_NULL)
        _tcscpy(sDot, XPR_STRING_LITERAL(".chm"));

    // Program file name verification routine
#if defined(XPR_CFG_BUILD_RELEASE)
    // fxfile File Name Check Routine
    if (_tcsicmp(m_pszExeName, FXFILE_PROGRAM_NAME) != 0)
    {
        xpr::string sMsg;
        sMsg  = XPR_STRING_LITERAL("This program is \'fxfile\' file manager.\n");
        sMsg += XPR_STRING_LITERAL("It is contained that a routine verfiy for this program file name.\n");
        sMsg += XPR_STRING_LITERAL("The file name must be \'fxfile.exe\'.");
        sMsg += XPR_STRING_LITERAL("Please, rename to \'fxfile.exe\' and then execute this program again.\n");
        sMsg += XPR_STRING_LITERAL("\n");
        sMsg += XPR_STRING_LITERAL("Please, contact to homepage: http://flychk.com or e-mail: flychk@flychk.com for any question.");
        sMsg += XPR_STRING_LITERAL("\n");
        MessageBox(XPR_NULL, sMsg.c_str(), FXFILE_PROGRAM_NAME, MB_OK | MB_ICONSTOP);
        return XPR_FALSE;
    }
#endif // XPR_CFG_BUILD_RELEASE

    // shell change notification
    ShellChangeNotify &sShellChangeNotify = ShellChangeNotify::instance();
    sShellChangeNotify.create();
    sShellChangeNotify.start();

    DriveShcn &sDriveShcn = DriveShcn::instance();
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
    SetRegistryKey(XPR_STRING_LITERAL("fxfile_dbg"));
#else
    SetRegistryKey(XPR_STRING_LITERAL("fxfile"));
#endif

    // parse command line arguments
    ProgramOpts &sProgramOpts = SingletonManager::get<ProgramOpts>();
    sProgramOpts.parse();

    if (sProgramOpts.isShowUsage() == XPR_TRUE)
    {
        ProgramOpts::showUsage();
        return XPR_FALSE;
    }

    if (sProgramOpts.isShowVersion() == XPR_TRUE)
    {
        ProgramOpts::showVersion();
    }

    xpr_bool_t  sResetConf     = sProgramOpts.isResetConf();
    xpr::string sConfDirToLoad = sProgramOpts.getConfDir();

    // load configuration directory
    ConfDir &sConfDir = ConfDir::instance();

    if (sConfDirToLoad.empty() == false)
    {
        sConfDir.setConfDir(sConfDirToLoad.c_str(), XPR_TRUE);
    }
    else
    {
        sConfDir.load();
    }

    // load options from file or load default option if configuration file does not exist
    OptionManager &sOptionManager = OptionManager::instance();

    xpr_bool_t sInitCfg = XPR_FALSE;

    if (XPR_IS_TRUE(sResetConf))
    {
        sOptionManager.initDefault();
        sInitCfg = XPR_TRUE;
    }
    else
    {
        sOptionManager.load(sInitCfg);
    }

    gOpt = sOptionManager.getOption();

    gOpt->setObserver(dynamic_cast<OptionObserver *>(this));

    // load language table & string table
    if (loadLanguageTable() == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = XPR_STRING_LITERAL("Language file(s) don't exist!!!");
        MessageBox(XPR_NULL, sMsg, FXFILE_PROGRAM_NAME, MB_OK | MB_ICONSTOP);
        return XPR_FALSE;
    }

    // load language
    loadLanguage(gOpt->mConfig.mLanguage);

    // load command string table
    CommandStringTable::instance().load();

    // load bookmark
    BookmarkMgr::instance().load();

    // check it if single process by option
    if (XPR_IS_TRUE(gOpt->mConfig.mSingleProcess))
    {
        if (SingleProcess::check() == XPR_FALSE)
        {
            SingleProcess::postMsg();

            return XPR_FALSE;
        }

        SingleProcess::lock();
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

    // The one and only window has been initialized, so show and update it.
    sMainFrame->SetForegroundWindow();
    sMainFrame->ShowWindow(m_nCmdShow);
    sMainFrame->UpdateWindow();

    // popup preference dialog if configuration file does not exist on loading time.
    if (XPR_IS_TRUE(sInitCfg))
    {
        cfg::CfgMainDlg sDlg;
        sDlg.DoModal();
    }

    // popup tip of today dialog by option
    if (XPR_IS_TRUE(gOpt->mMain.mTipOfTheToday))
    {
        cmd::TipDlg sDlg;
        sDlg.DoModal();
    }

    // show main frame
    sMainFrame->ShowWindow(SW_SHOW);

    return XPR_TRUE;
}

xpr_sint_t WinApp::ExitInstance(void) 
{
    // delete undo directory
    FileOpUndo::deleteUndoDir();

    // Exit gfl Library
    gflLibraryExit(); 

    // save main option
    if (OptionManager::isInstance() == XPR_TRUE)
        OptionManager::instance().saveMainOption();

    // destroy drive shell change notify
    DriveShcn &sDriveShcn = DriveShcn::instance();
    sDriveShcn.stop();
    sDriveShcn.destroy();

    // destroy shell change notify
    ShellChangeNotify &sShellChangeNotify = ShellChangeNotify::instance();
    sShellChangeNotify.stop();
    sShellChangeNotify.destroy();

    XPR_SAFE_DELETE(mLanguageTable);

    // unlock if single process is locked
    SingleProcess::unlock();

    // clean singleton manager
    SingletonManager::clean();

    //
    // finalize XPR
    //
    xpr::finalize();

    return super::ExitInstance();
}

void WinApp::LoadStdProfileSettings(xpr_uint_t aMaxMRU)
{
    ASSERT_VALID(this);

    // 0 by default means not set
    m_nNumPreviewPages = GetProfileInt(kPreviewSection, kPreviewEntry, 0);
}

void WinApp::OnFileNew(void)
{
    super::OnFileNew();
}

xpr_bool_t WinApp::loadLanguageTable(void)
{
    mLanguageTable = new fxfile::base::LanguageTable;
    if (XPR_IS_NULL(mLanguageTable))
        return XPR_FALSE;

    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    GetModuleDir(sDir, XPR_MAX_PATH);

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

xpr_bool_t WinApp::loadLanguage(const xpr_tchar_t *aLanguage)
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

    Filter::setString(
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.folder")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.general_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.executable_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.compressed_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.document_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.image_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.sound_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.movie_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.web_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.programming_file")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.filter.default_filter_name.temporary_file")));

    SizeFormat::setText(
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.none")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.automatic")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.default")),
        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.unit.custom")),
        gApp.loadString(XPR_STRING_LITERAL("common.size.byte")));

    return XPR_TRUE;
}

const fxfile::base::LanguageTable *WinApp::getLanguageTable(void) const
{
    return mLanguageTable;
}

const xpr_tchar_t *WinApp::loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable)
{
    if (XPR_IS_NULL(mStringTable))
        return aId;

    return mStringTable->loadString(aId, aNullAvailable);
}

const xpr_tchar_t *WinApp::loadString(const xpr::string &aId, xpr_bool_t aNullAvailable)
{
    const xpr_tchar_t *sId = aId.c_str();

    if (XPR_IS_NULL(mStringTable))
        return sId;

    return mStringTable->loadString(sId, aNullAvailable);
}

const xpr_tchar_t *WinApp::loadFormatString(const xpr_tchar_t *aId, const xpr_tchar_t *aReplaceFormatSpecifier)
{
    if (XPR_IS_NULL(mFormatStringTable))
        return aId;

    return mFormatStringTable->loadString(aId, aReplaceFormatSpecifier);
}

const xpr_tchar_t *WinApp::loadFormatString(const xpr::string &aId, const xpr_tchar_t *aReplaceFormatSpecifier)
{
    const xpr_tchar_t *sId = aId.c_str();

    if (XPR_IS_NULL(mFormatStringTable))
        return sId;

    return mFormatStringTable->loadString(sId, aReplaceFormatSpecifier);
}

void WinApp::onChangedConfig(Option &aOption)
{
    // unregister shell registry for old language
    ShellRegistry::unregisterShell();

    // popup warning message about language change
    const fxfile::base::LanguagePack::Desc *sLoadedLanguagePackDesc = mLanguageTable->getLanguageDesc();
    if (sLoadedLanguagePackDesc->mLanguage.compare_case(aOption.mConfig.mLanguage) != 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.cfg.msg.apply_language_on_next_loading_time"));
        AfxMessageBox(sMsg, MB_OK | MB_ICONWARNING);
    }

    // load language on runtime
    //loadLanguage(aOption.mConfig.mLanguage);

    // set single process
    if (XPR_IS_TRUE(aOption.mConfig.mSingleProcess))
    {
        SingleProcess::lock();
    }
    else
    {
        SingleProcess::unlock();
    }

    // shell registry
    if (XPR_IS_TRUE(aOption.mConfig.mRegShellContextMenu))
    {
        ShellRegistry::registerShell();
    }
    else
    {
        ShellRegistry::unregisterShell();
    }

    // fxfile-launcher
    if (XPR_IS_TRUE(aOption.mConfig.mLauncher))
    {
        LauncherManager::startupProcess(aOption.mConfig.mLauncherGlobalHotKey, aOption.mConfig.mLauncherTray);
    }
    else
    {
        LauncherManager::shutdownProcess(aOption.mConfig.mLauncherGlobalHotKey, aOption.mConfig.mLauncherTray);
    }

    if (XPR_IS_TRUE(aOption.mConfig.mLauncherWinStartup))
    {
        LauncherManager::registerWinStartup();
    }
    else
    {
        LauncherManager::unregisterWinStartup();
    }

    // fxfile-updater
    UpdaterManager::writeUpdaterConf(aOption.mConfig);

    if (XPR_IS_TRUE(aOption.mConfig.mUpdateCheckEnable))
    {
        UpdaterManager::startupProcess();
        UpdaterManager::registerWinStartup();
    }
    else
    {
        UpdaterManager::shutdownProcess();
        UpdaterManager::unregisterWinStartup();
    }

    // notify changed options to main frame
    MainFrame *sMainFrame = (MainFrame *)GetMainWnd();

    sMainFrame->setChangedOption(aOption);
}

void WinApp::saveAllOptions(void)
{
    // save main frame options
    MainFrame *sMainFrame = (MainFrame *)GetMainWnd();
    sMainFrame->saveAllOptions();
}
} // namespace fxfile
