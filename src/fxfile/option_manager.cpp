//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "option_manager.h"

#include "conf_file_ex.h"
#include "filter.h"
#include "program_ass.h"
#include "thumbnail.h"
#include "size_format.h"
#include "sys_img_list.h"
#include "recent_file_list.h"

#include "option.h"
#include "main_frame.h"
#include "explorer_view.h"
#include "explorer_pane.h"
#include "explorer_ctrl.h"
#include "folder_ctrl.h"
#include "path_bar.h"
#include "activate_bar.h"
#include "address_bar.h"
#include "picture_viewer.h"
#include "conf_dir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
OptionManager::OptionManager(void)
    : mOption(new Option)
{
}

OptionManager::~OptionManager(void)
{
    XPR_SAFE_DELETE(mOption);
}

void OptionManager::initDefault(void)
{
    FilterMgr     &sFilterMgr     = FilterMgr::instance();
    ProgramAssMgr &sProgramAssMgr = ProgramAssMgr::instance();
    SizeFormat    &sSizeFormat    = SizeFormat::instance();

    sFilterMgr.initDefault();
    sProgramAssMgr.initDefault();
    sSizeFormat.initDefault();
    mOption->initDefault();
}

void OptionManager::load(xpr_bool_t &aInitCfg)
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

xpr_bool_t OptionManager::save(void)
{
    // save main option
    saveMainOption();

    // save config option
    saveConfigOption();

    return XPR_TRUE;
}

xpr_bool_t OptionManager::loadMainOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getLoadPath(ConfDir::TypeMain, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);
    xpr_bool_t sResult = sConfFile.load();

    // load main options
    mOption->loadMainOption(sConfFile);

    // load recent file list
    loadRecentFileList(sConfFile);

    return sResult;
}

xpr_bool_t OptionManager::loadConfigOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getLoadPath(ConfDir::TypeConfig, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);
    xpr_bool_t sResult = sConfFile.load();

    // config options
    mOption->loadConfigOption(sConfFile);

    // load filter
    loadFilter(sConfFile);

    // load program association
    loadProgramAss(sConfFile);

    // load size format
    loadSizeFormat(sConfFile);

    return sResult;
}

xpr_bool_t OptionManager::saveMainOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getSavePath(ConfDir::TypeMain, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);
    sConfFile.setComment(XPR_STRING_LITERAL("fxfile main option file"));

    // save main options
    mOption->saveMainOption(sConfFile);

    // save recent file list
    saveRecentFileList(sConfFile);

    return sConfFile.save(xpr::CharSetUtf16);
}

xpr_bool_t OptionManager::saveConfigOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getSavePath(ConfDir::TypeConfig, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);
    sConfFile.setComment(XPR_STRING_LITERAL("fxfile configuration option file"));

    // save config options
    mOption->saveConfigOption(sConfFile);

    // save filter
    saveFilter(sConfFile);

    // save program association
    saveProgramAss(sConfFile);

    // save size format
    saveSizeFormat(sConfFile);

    return sConfFile.save(xpr::CharSetUtf16);
}

void OptionManager::loadFilter(fxfile::base::ConfFileEx &aConfFile)
{
    FilterMgr &sFilterMgr = FilterMgr::instance();

    xpr_bool_t sResult = sFilterMgr.load(aConfFile);
    if (XPR_IS_FALSE(sResult))
    {
        sFilterMgr.initDefault();
    }
}

void OptionManager::saveFilter(fxfile::base::ConfFileEx &aConfFile)
{
    FilterMgr &sFilterMgr = FilterMgr::instance();

    sFilterMgr.save(aConfFile);
}

void OptionManager::loadProgramAss(fxfile::base::ConfFileEx &aConfFile)
{
    ProgramAssMgr &sProgramAssMgr = ProgramAssMgr::instance();

    xpr_bool_t sResult = sProgramAssMgr.load(aConfFile);
    if (XPR_IS_FALSE(sResult))
    {
        sProgramAssMgr.initDefault();
    }
}

void OptionManager::saveProgramAss(fxfile::base::ConfFileEx &aConfFile)
{
    ProgramAssMgr &sProgramAssMgr = ProgramAssMgr::instance();

    sProgramAssMgr.save(aConfFile);
}

void OptionManager::loadSizeFormat(fxfile::base::ConfFileEx &aConfFile)
{
    SizeFormat &sSizeFormat = SizeFormat::instance();

    xpr_bool_t sResult = sSizeFormat.load(aConfFile);
    if (XPR_IS_FALSE(sResult))
    {
        sSizeFormat.initDefault();
    }
}

void OptionManager::saveSizeFormat(fxfile::base::ConfFileEx &aConfFile)
{
    SizeFormat &sSizeFormat = SizeFormat::instance();

    sSizeFormat.save(aConfFile);
}

void OptionManager::loadRecentFileList(fxfile::base::ConfFileEx &aConfFile)
{
    RecentFileList &sRecentFileList = RecentFileList::instance();

    sRecentFileList.load(aConfFile);
}

void OptionManager::saveRecentFileList(fxfile::base::ConfFileEx &aConfFile)
{
    RecentFileList &sRecentFileList = RecentFileList::instance();

    sRecentFileList.save(aConfFile);
}
} // namespace fxfile
