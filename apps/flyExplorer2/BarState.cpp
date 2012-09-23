//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "BarState.h"

#include <afxpriv.h>

#include "fxb/fxb_ini_file.h"

#include "rgc/sizecbar.h"

#include "CfgPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BarState::BarState(void)
{
}

BarState::~BarState(void)
{
    mBarMap.clear();
}

static const xpr_tchar_t _afxVisible[]          = XPR_STRING_LITERAL("Visible");
static const xpr_tchar_t _afxBarSection[]       = XPR_STRING_LITERAL("%s-Bar%d");
static const xpr_tchar_t _afxSummarySection[]   = XPR_STRING_LITERAL("%s-Summary");
static const xpr_tchar_t _afxXPos[]             = XPR_STRING_LITERAL("XPos");
static const xpr_tchar_t _afxYPos[]             = XPR_STRING_LITERAL("YPos");
static const xpr_tchar_t _afxMRUWidth[]         = XPR_STRING_LITERAL("MRUWidth");
static const xpr_tchar_t _afxDocking[]          = XPR_STRING_LITERAL("Docking");
static const xpr_tchar_t _afxMRUDockID[]        = XPR_STRING_LITERAL("MRUDockID");
static const xpr_tchar_t _afxMRUDockLeftPos[]   = XPR_STRING_LITERAL("MRUDockLeftPos");
static const xpr_tchar_t _afxMRUDockRightPos[]  = XPR_STRING_LITERAL("MRUDockRightPos");
static const xpr_tchar_t _afxMRUDockTopPos[]    = XPR_STRING_LITERAL("MRUDockTopPos");
static const xpr_tchar_t _afxMRUDockBottomPos[] = XPR_STRING_LITERAL("MRUDockBottomPos");
static const xpr_tchar_t _afxMRUFloatStyle[]    = XPR_STRING_LITERAL("MRUFloatStyle");
static const xpr_tchar_t _afxMRUFloatXPos[]     = XPR_STRING_LITERAL("MRUFloatXPos");
static const xpr_tchar_t _afxMRUFloatYPos[]     = XPR_STRING_LITERAL("MRUFloatYPos");

static const xpr_tchar_t _afxBarID[]            = XPR_STRING_LITERAL("BarID");
static const xpr_tchar_t _afxHorz[]             = XPR_STRING_LITERAL("Horz");
static const xpr_tchar_t _afxFloating[]         = XPR_STRING_LITERAL("Floating");
static const xpr_tchar_t _afxBars[]             = XPR_STRING_LITERAL("Bars");
static const xpr_tchar_t _afxScreenCX[]         = XPR_STRING_LITERAL("ScreenCX");
static const xpr_tchar_t _afxScreenCY[]         = XPR_STRING_LITERAL("ScreenCY");
static const xpr_tchar_t _afxBar[]              = XPR_STRING_LITERAL("Bar#%d");

static const xpr_tchar_t kBarStateKey[]         = XPR_STRING_LITERAL("BarState");

static xpr_bool_t loadControlBarInfo(CControlBarInfo *aControlBarInfo,
                                     CDockState      *aDockState,
                                     fxb::IniFile    *aIniFile,
                                     xpr_sint_t       aIndex,
                                     BarMap          &aBarMap)
{
    if (XPR_IS_NULL(aIniFile))
        return XPR_FALSE;

    xpr_tchar_t sEntry[0xff] = {0};
    _stprintf(sEntry, XPR_STRING_LITERAL("%s #%d"), kBarStateKey, aIndex);

    ASSERT(aDockState != NULL);

    aControlBarInfo->m_nBarID    = aIniFile->getValueI(sEntry, _afxBarID,    0);
    aControlBarInfo->m_bVisible  = aIniFile->getValueB(sEntry, _afxVisible,  XPR_TRUE);
    aControlBarInfo->m_bHorz     = aIniFile->getValueB(sEntry, _afxHorz,     XPR_TRUE);
    aControlBarInfo->m_bFloating = aIniFile->getValueB(sEntry, _afxFloating, XPR_FALSE);
    aControlBarInfo->m_pointPos  = CPoint(
        aIniFile->getValueI(sEntry, _afxXPos, -1),
        aIniFile->getValueI(sEntry, _afxYPos, -1));
    aDockState->ScalePoint(aControlBarInfo->m_pointPos);
    if (aControlBarInfo->m_bFloating)
    {
        // multi-monitor support only available under Win98/ME/Win2000 or greater
        OSVERSIONINFO sOSVersionInfo;
        ZeroMemory(&sOSVersionInfo, sizeof(OSVERSIONINFO));
        sOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        ::GetVersionEx(&sOSVersionInfo);
        if ((sOSVersionInfo.dwMajorVersion > 4) || (sOSVersionInfo.dwMajorVersion == 4 && sOSVersionInfo.dwMinorVersion != 0))
        {
            if (aControlBarInfo->m_pointPos.x - GetSystemMetrics(SM_CXFRAME) < GetSystemMetrics(SM_XVIRTUALSCREEN))
                aControlBarInfo->m_pointPos.x = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_XVIRTUALSCREEN);
            if (aControlBarInfo->m_pointPos.x + GetSystemMetrics(SM_CXFRAME) > GetSystemMetrics(SM_CXVIRTUALSCREEN))
                aControlBarInfo->m_pointPos.x = - GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXVIRTUALSCREEN);
            if (aControlBarInfo->m_pointPos.y - GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYSMCAPTION) < GetSystemMetrics(SM_YVIRTUALSCREEN))
                aControlBarInfo->m_pointPos.y = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_YVIRTUALSCREEN);
            if (aControlBarInfo->m_pointPos.y + GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSMCAPTION) > GetSystemMetrics(SM_CYVIRTUALSCREEN))
                aControlBarInfo->m_pointPos.y = - GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYVIRTUALSCREEN);

            HMODULE sDll = ::GetModuleHandle(XPR_STRING_LITERAL("user32.dll"));
            ASSERT(sDll);
            if (sDll != NULL)
            {
                // in case of multiple monitors check if point is in one of monitors
                typedef HMONITOR (WINAPI *MonitorFromPointFunc)(POINT, DWORD);
                MonitorFromPointFunc sMonitorFromPointFunc = (MonitorFromPointFunc)::GetProcAddress(sDll, "MonitorFromPoint");
                if (XPR_IS_NOT_NULL(sMonitorFromPointFunc) && sMonitorFromPointFunc(aControlBarInfo->m_pointPos, MONITOR_DEFAULTTONULL) == XPR_FALSE)
                {
                    aControlBarInfo->m_pointPos.x = GetSystemMetrics(SM_CXFRAME);
                    aControlBarInfo->m_pointPos.y = GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYFRAME);
                }
            }
        }
        else
        {
            if (aControlBarInfo->m_pointPos.x - GetSystemMetrics(SM_CXFRAME) < 0)
                aControlBarInfo->m_pointPos.x = GetSystemMetrics(SM_CXFRAME) + 0;
            if (aControlBarInfo->m_pointPos.y - GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYSMCAPTION) < 0)
                aControlBarInfo->m_pointPos.y = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSMCAPTION) + 0;
        }
    }

    aControlBarInfo->m_nMRUWidth = aIniFile->getValueU(sEntry, _afxMRUWidth, 32767);
    aControlBarInfo->m_bDocking  = aIniFile->getValueB(sEntry, _afxDocking, 0);
    if (XPR_IS_TRUE(aControlBarInfo->m_bDocking))
    {
        aControlBarInfo->m_uMRUDockID = aIniFile->getValueU(sEntry, _afxMRUDockID, 0);

        aControlBarInfo->m_rectMRUDockPos = CRect(
            aIniFile->getValueI(sEntry, _afxMRUDockLeftPos, 0),
            aIniFile->getValueI(sEntry, _afxMRUDockTopPos, 0),
            aIniFile->getValueI(sEntry, _afxMRUDockRightPos, 0),
            aIniFile->getValueI(sEntry, _afxMRUDockBottomPos, 0));
        aDockState->ScaleRectPos(aControlBarInfo->m_rectMRUDockPos);

        aControlBarInfo->m_dwMRUFloatStyle = aIniFile->getValueU(sEntry, _afxMRUFloatStyle, 0);

        aControlBarInfo->m_ptMRUFloatPos = CPoint(
            aIniFile->getValueI(sEntry, _afxMRUFloatXPos, 0),
            aIniFile->getValueI(sEntry, _afxMRUFloatYPos, 0));
        aDockState->ScalePoint(aControlBarInfo->m_ptMRUFloatPos);
    }

    xpr_sint_t i;
    xpr_sint_t sBars = aIniFile->getValueI(sEntry, _afxBars, 0);
    for (i = 0; i < sBars; ++i)
    {
        xpr_tchar_t sBuf[16];
        _stprintf_s(sBuf, _countof(sBuf), _afxBar, i);
        aControlBarInfo->m_arrBarID.Add((xpr_uint_t)aIniFile->getValueI(sEntry, sBuf, 0));
    }

    xpr_bool_t sResult = (aControlBarInfo->m_nBarID > 0);
    if (XPR_IS_TRUE(sResult))
    {
        BarMap::iterator sIterator;
        CControlBar *sControlBar;

        sIterator = aBarMap.find(aControlBarInfo->m_nBarID);
        if (sIterator != aBarMap.end())
        {
            sControlBar = sIterator->second;
            if (sControlBar->IsKindOf(RUNTIME_CLASS(CSizingControlBar)))
                ((CSizingControlBar *)sControlBar)->LoadStateIni(aIniFile, sEntry);
        }
    }

    return sResult;
}

static xpr_bool_t saveControlBarInfo(CControlBar *aControlBar, CControlBarInfo *aControlBarInfo, fxb::IniFile *aIniFile, xpr_sint_t aIndex)
{
    if (XPR_IS_NULL(aIniFile))
        return XPR_FALSE;

    xpr_tchar_t sEntry[0xff] = {0};
    _stprintf(sEntry, XPR_STRING_LITERAL("%s #%d"), kBarStateKey, aIndex);

    if (XPR_IS_NOT_NULL(aControlBar) && aControlBar->IsKindOf(RUNTIME_CLASS(CSizingControlBar)))
    {
        ((CSizingControlBar *)aControlBar)->SaveStateIni(aIniFile, sEntry);
    }

    if (XPR_IS_TRUE(aControlBarInfo->m_bDockBar) &&
        XPR_IS_TRUE(aControlBarInfo->m_bVisible) &&
        XPR_IS_FALSE(aControlBarInfo->m_bFloating) &&
        aControlBarInfo->m_pointPos.x == -1 &&
        aControlBarInfo->m_pointPos.y == -1 &&
        aControlBarInfo->m_arrBarID.GetSize() <= 1)
    {
        return XPR_FALSE;
    }

    aIniFile->setValueI(sEntry, _afxBarID, aControlBarInfo->m_nBarID);

    if (XPR_IS_FALSE(aControlBarInfo->m_bVisible))
        aIniFile->setValueB(sEntry, _afxVisible, aControlBarInfo->m_bVisible);

    if (XPR_IS_TRUE(aControlBarInfo->m_bFloating))
    {
        aIniFile->setValueB(sEntry, _afxHorz, aControlBarInfo->m_bHorz);
        aIniFile->setValueB(sEntry, _afxFloating, aControlBarInfo->m_bFloating);
    }

    if (aControlBarInfo->m_pointPos.x != -1)
        aIniFile->setValueI(sEntry, _afxXPos, aControlBarInfo->m_pointPos.x);

    if (aControlBarInfo->m_pointPos.y != -1)
        aIniFile->setValueI(sEntry, _afxYPos, aControlBarInfo->m_pointPos.y);

    if (aControlBarInfo->m_nMRUWidth != 32767)
        aIniFile->setValueU(sEntry, _afxMRUWidth, aControlBarInfo->m_nMRUWidth);

    if (XPR_IS_TRUE(aControlBarInfo->m_bDocking))
    {
        aIniFile->setValueB(sEntry, _afxDocking,          aControlBarInfo->m_bDocking);
        aIniFile->setValueU(sEntry, _afxMRUDockID,        aControlBarInfo->m_uMRUDockID);
        aIniFile->setValueI(sEntry, _afxMRUDockLeftPos,   aControlBarInfo->m_rectMRUDockPos.left);
        aIniFile->setValueI(sEntry, _afxMRUDockTopPos,    aControlBarInfo->m_rectMRUDockPos.top);
        aIniFile->setValueI(sEntry, _afxMRUDockRightPos,  aControlBarInfo->m_rectMRUDockPos.right);
        aIniFile->setValueI(sEntry, _afxMRUDockBottomPos, aControlBarInfo->m_rectMRUDockPos.bottom);
        aIniFile->setValueU(sEntry, _afxMRUFloatStyle,    aControlBarInfo->m_dwMRUFloatStyle);
        aIniFile->setValueI(sEntry, _afxMRUFloatXPos,     aControlBarInfo->m_ptMRUFloatPos.x);
        aIniFile->setValueI(sEntry, _afxMRUFloatYPos,     aControlBarInfo->m_ptMRUFloatPos.y);
    }

    if (aControlBarInfo->m_arrBarID.GetSize() > 1) //if ==1 then still empty
    {
        aIniFile->setValueI(sEntry, _afxBars, (xpr_sint_t)aControlBarInfo->m_arrBarID.GetSize());

        xpr_sint_t i;
        for (i = 0; i < aControlBarInfo->m_arrBarID.GetSize(); ++i)
        {
            xpr_tchar_t sBuf[16];
            _stprintf_s(sBuf, _countof(sBuf), _afxBar, i);
            aIniFile->setValueI(sEntry, sBuf, (xpr_sint_t)(INT_PTR)aControlBarInfo->m_arrBarID[i]);
        }
    }

    return XPR_TRUE;
}

static xpr_bool_t loadDockState(CDockState *aDockState, BarMap &aBarMap)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeBarState, sPath, XPR_MAX_PATH);

    fxb::IniFile sIniFile(sPath);
    if (sIniFile.readFileW() == XPR_FALSE)
        return XPR_FALSE;

    CSize sSize;
    sSize.cx = sIniFile.getValueI(kBarStateKey, _afxScreenCX, 0);
    sSize.cy = sIniFile.getValueI(kBarStateKey, _afxScreenCY, 0);
    aDockState->SetScreenSize(sSize);

    xpr_sint_t sBars = sIniFile.getValueI(kBarStateKey, _afxBars, 0);

    {
        xpr_sint_t i;
        for (i = 0; i < sBars; ++i)
        {
            CControlBarInfo *aControlBarInfo = new CControlBarInfo;
            if (::loadControlBarInfo(aControlBarInfo, aDockState, &sIniFile, i, aBarMap) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(aControlBarInfo);
                continue;
            }

            aDockState->m_arrBarInfo.Add(aControlBarInfo);
        }
    }

    return XPR_TRUE;
}

static xpr_bool_t saveDockState(CDockState *aDockState, BarMap &aBarMap)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeBarState, sPath, XPR_MAX_PATH);

    fxb::IniFile sIniFile(sPath);

    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    BarMap::iterator sIterator;
    CControlBar *sControlBar;

    sIndex = 0;

    for (i = 0; i < aDockState->m_arrBarInfo.GetSize(); ++i)
    {
        CControlBarInfo *sControlBarInfo = (CControlBarInfo*)aDockState->m_arrBarInfo[i];
        ASSERT(sControlBarInfo != NULL);

        sControlBar = NULL;

        sIterator = aBarMap.find(sControlBarInfo->m_nBarID);
        if (sIterator != aBarMap.end())
            sControlBar = sIterator->second;

        if (::saveControlBarInfo(sControlBar, sControlBarInfo, &sIniFile, sIndex) == XPR_TRUE)
        {
            sIndex++;
        }
    }

    sCount = sIndex;

    sIniFile.setValueI(kBarStateKey, _afxBars, sCount);

    CSize sSize = aDockState->GetScreenSize();
    sIniFile.setValueI(kBarStateKey, _afxScreenCX, sSize.cx);
    sIniFile.setValueI(kBarStateKey, _afxScreenCY, sSize.cy);

    sIniFile.setSortKey(XPR_STRING_LITERAL("BarState"), -2);
    sIniFile.sortKey();

    sIniFile.writeFileW();

    return XPR_TRUE;
}

void BarState::addBar(CControlBar *aControlBar)
{
    if (XPR_IS_NULL(aControlBar))
        return;

    xpr_uint_t sBarId = aControlBar->GetDlgCtrlID();

    mBarMap[sBarId] = aControlBar;
}

xpr_bool_t BarState::loadBarState(CFrameWnd *aFrameWnd)
{
    CDockState sDocState;
    loadDockState(&sDocState, mBarMap);
    aFrameWnd->SetDockState(sDocState);

    return XPR_TRUE;
}

xpr_bool_t BarState::saveBarState(CFrameWnd *aFrameWnd)
{
    CDockState sDocState;
    aFrameWnd->GetDockState(sDocState);
    saveDockState(&sDocState, mBarMap);

    return XPR_TRUE;
}
