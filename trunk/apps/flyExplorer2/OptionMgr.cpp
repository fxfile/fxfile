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

#include "fxb/fxb_ini_file.h"
#include "fxb/fxb_filter.h"
#include "fxb/fxb_program_ass.h"
#include "fxb/fxb_thumbnail.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_sys_img_list.h"

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
    fxb::FilterMgr::instance().initDefault();
    fxb::ProgramAssMgr::instance().initDefault();
    fxb::SizeFormat::instance().initDefault();
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

    // load filter
    if (loadFilter() == XPR_FALSE)
    {
        saveFilter();
    }

    // load program association
    if (loadProgramAss() == XPR_FALSE)
    {
        saveProgramAss();
    }

    // load size format
    if (loadSizeFormat() == XPR_FALSE)
    {
        saveSizeFormat();
    }

#if defined(XPR_CFG_BUILD_DEBUG)

    xpr_time_t sTime2 = xpr::timer_ms();

    XPR_TRACE(XPR_STRING_LITERAL("Loading time of configuration file = %I64dms\n"), sTime2 - sTime1);

#endif
}

xpr_bool_t OptionMgr::save(xpr_bool_t aOnlyConfig)
{
    if (XPR_IS_TRUE(aOnlyConfig))
    {
        // save main option
        saveMainOption();
    }

    // save config option
    saveConfigOption();

    // save filter
    saveFilter();

    // save program association
    saveProgramAss();

    // save size format
    saveSizeFormat();

    return XPR_TRUE;
}

xpr_bool_t OptionMgr::loadMainOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeMain, sPath, XPR_MAX_PATH);

    return mOption->loadMainOption(sPath);
}

xpr_bool_t OptionMgr::loadConfigOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeConfig, sPath, XPR_MAX_PATH);

    return mOption->loadConfigOption(sPath);
}

xpr_bool_t OptionMgr::saveMainOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeMain, sPath, XPR_MAX_PATH);

    return mOption->saveMainOption(sPath);
}

xpr_bool_t OptionMgr::saveConfigOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeConfig, sPath, XPR_MAX_PATH);

    return mOption->saveConfigOption(sPath);
}

xpr_bool_t OptionMgr::loadFilter(void)
{
    fxb::FilterMgr &sFilterMgr = fxb::FilterMgr::instance();

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeFilter, sPath, XPR_MAX_PATH);

    xpr_bool_t sResult = sFilterMgr.loadFromFile(sPath);
    if (XPR_IS_FALSE(sResult))
        sFilterMgr.initDefault();

    return sResult;
}

xpr_bool_t OptionMgr::saveFilter(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeFilter, sPath, XPR_MAX_PATH);

    return fxb::FilterMgr::instance().saveToFile(sPath);
}

xpr_bool_t OptionMgr::loadProgramAss(void)
{
    fxb::ProgramAssMgr &sProgramAssMgr = fxb::ProgramAssMgr::instance();

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeProgramAss, sPath, XPR_MAX_PATH);

    xpr_bool_t sResult = sProgramAssMgr.loadFromFile(sPath);
    if (XPR_IS_FALSE(sResult))
        sProgramAssMgr.initDefault();

    return sResult;
}

xpr_bool_t OptionMgr::saveProgramAss(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeProgramAss, sPath, XPR_MAX_PATH);

    return fxb::ProgramAssMgr::instance().saveToFile(sPath);
}

xpr_bool_t OptionMgr::loadSizeFormat(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeSizeFmt, sPath, XPR_MAX_PATH);

    return fxb::SizeFormat::instance().loadFromFile(sPath);
}

xpr_bool_t OptionMgr::saveSizeFormat(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeSizeFmt, sPath, XPR_MAX_PATH);

    return fxb::SizeFormat::instance().saveToFile(sPath);
}
