//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "OptionMgr.h"

#include "fxb/fxb_ini_file_ex.h"
#include "fxb/fxb_filter.h"
#include "fxb/fxb_program_ass.h"
#include "fxb/fxb_thumbnail.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_sys_img_list.h"
#include "fxb/fxb_recent_file_list.h"

#include "Option.h"
#include "MainFrame.h"
#include "ExplorerView.h"
#include "ExplorerPane.h"
#include "ExplorerCtrl.h"
#include "FolderCtrl.h"
#include "PathBar.h"
#include "ActivateBar.h"
#include "AddressBar.h"
#include "PicViewer.h"
#include "CfgPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

OptionMgr::OptionMgr(void)
    : mOption(new Option)
{
}

OptionMgr::~OptionMgr(void)
{
    XPR_SAFE_DELETE(mOption);
}

void OptionMgr::initDefault(void)
{
    fxb::FilterMgr     &sFilterMgr     = fxb::FilterMgr::instance();
    fxb::ProgramAssMgr &sProgramAssMgr = fxb::ProgramAssMgr::instance();
    fxb::SizeFormat    &sSizeFormat    = fxb::SizeFormat::instance();

    sFilterMgr.initDefault();
    sProgramAssMgr.initDefault();
    sSizeFormat.initDefault();
    mOption->initDefault();
}

void OptionMgr::load(xpr_bool_t &aInitCfg)
{
#if defined(XPR_CFG_BUILD_DEBUG)

    xpr_time_t sTime1 = xpr::timer_ms();

#endif

    aInitCfg = XPR_FALSE;

    // load main option
    if (loadMainOption() == XPR_FALSE)
    {
        saveMainOption();
    }

    // load config option
    if (loadConfigOption() == XPR_FALSE)
    {
        saveConfigOption();

        aInitCfg = XPR_TRUE;
    }

#if defined(XPR_CFG_BUILD_DEBUG)

    xpr_time_t sTime2 = xpr::timer_ms();

    XPR_TRACE(XPR_STRING_LITERAL("Loading time of configuration file = %I64dms\n"), sTime2 - sTime1);

#endif
}

xpr_bool_t OptionMgr::save(void)
{
    // save main option
    saveMainOption();

    // save config option
    saveConfigOption();

    return XPR_TRUE;
}

xpr_bool_t OptionMgr::loadMainOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeMain, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);
    xpr_bool_t sResult = sIniFile.readFile();

    // load main options
    mOption->loadMainOption(sIniFile);

    // load recent file list
    loadRecentFileList(sIniFile);

    return sResult;
}

xpr_bool_t OptionMgr::loadConfigOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeConfig, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);
    xpr_bool_t sResult = sIniFile.readFile();

    // config options
    mOption->loadConfigOption(sIniFile);

    // load filter
    loadFilter(sIniFile);

    // load program association
    loadProgramAss(sIniFile);

    // load size format
    loadSizeFormat(sIniFile);

    return sResult;
}

xpr_bool_t OptionMgr::saveMainOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeMain, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer main option file"));

    // save main options
    mOption->saveMainOption(sIniFile);

    // save recent file list
    saveRecentFileList(sIniFile);

    return sIniFile.writeFile(xpr::CharSetUtf16);
}

xpr_bool_t OptionMgr::saveConfigOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeConfig, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer configuration option file"));

    // save config options
    mOption->saveConfigOption(sIniFile);

    // save filter
    saveFilter(sIniFile);

    // save program association
    saveProgramAss(sIniFile);

    // save size format
    saveSizeFormat(sIniFile);

    return sIniFile.writeFile(xpr::CharSetUtf16);
}

void OptionMgr::loadFilter(fxb::IniFileEx &aIniFile)
{
    fxb::FilterMgr &sFilterMgr = fxb::FilterMgr::instance();

    xpr_bool_t sResult = sFilterMgr.load(aIniFile);
    if (XPR_IS_FALSE(sResult))
    {
        sFilterMgr.initDefault();
    }
}

void OptionMgr::saveFilter(fxb::IniFileEx &aIniFile)
{
    fxb::FilterMgr &sFilterMgr = fxb::FilterMgr::instance();

    sFilterMgr.save(aIniFile);
}

void OptionMgr::loadProgramAss(fxb::IniFileEx &aIniFile)
{
    fxb::ProgramAssMgr &sProgramAssMgr = fxb::ProgramAssMgr::instance();

    xpr_bool_t sResult = sProgramAssMgr.load(aIniFile);
    if (XPR_IS_FALSE(sResult))
    {
        sProgramAssMgr.initDefault();
    }
}

void OptionMgr::saveProgramAss(fxb::IniFileEx &aIniFile)
{
    fxb::ProgramAssMgr &sProgramAssMgr = fxb::ProgramAssMgr::instance();

    sProgramAssMgr.save(aIniFile);
}

void OptionMgr::loadSizeFormat(fxb::IniFileEx &aIniFile)
{
    fxb::SizeFormat &sSizeFormat = fxb::SizeFormat::instance();

    xpr_bool_t sResult = sSizeFormat.load(aIniFile);
    if (XPR_IS_FALSE(sResult))
    {
        sSizeFormat.initDefault();
    }
}

void OptionMgr::saveSizeFormat(fxb::IniFileEx &aIniFile)
{
    fxb::SizeFormat &sSizeFormat = fxb::SizeFormat::instance();

    sSizeFormat.save(aIniFile);
}

void OptionMgr::loadRecentFileList(fxb::IniFileEx &aIniFile)
{
    fxb::RecentFileList &sRecentFileList = fxb::RecentFileList::instance();

    sRecentFileList.load(aIniFile);
}

void OptionMgr::saveRecentFileList(fxb::IniFileEx &aIniFile)
{
    fxb::RecentFileList &sRecentFileList = fxb::RecentFileList::instance();

    sRecentFileList.save(aIniFile);
}
