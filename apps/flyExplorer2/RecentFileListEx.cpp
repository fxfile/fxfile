//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "RecentFileListEx.h"

#include <atlbase.h>
#include <afxpriv.h>

#include "fxb/fxb_ini_file.h"
#include "rgc/BCMenu.h"

#include "Option.h"
#include "CfgPath.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kRecentKey[] = XPR_STRING_LITERAL("Recent");

RecentFileListIni::RecentFileListIni(xpr_uint_t         aStart,
                                     const xpr_tchar_t *aSection,
                                     const xpr_tchar_t *aEntryFormat,
                                     xpr_sint_t         aSize,
                                     xpr_sint_t         aMaxDispLen)
    : super(aStart, aSection, aEntryFormat, aSize, aMaxDispLen)
{
}

RecentFileListIni::~RecentFileListIni(void)
{
}

void RecentFileListIni::WriteList(void)
{
    ASSERT(m_arrNames != XPR_NULL);
    ASSERT(!m_strSectionName.IsEmpty());
    ASSERT(!m_strEntryFormat.IsEmpty());

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeRecent, sPath, XPR_MAX_PATH);

    fxb::IniFile sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer Recent File List"));

    xpr_sint_t sMRU;
    xpr_sint_t sLen = m_strEntryFormat.GetLength() + 10;
    xpr_tchar_t *sEntry = new xpr_tchar_t[sLen];

    for (sMRU = 0; sMRU < m_nSize; ++sMRU)
    {
        _stprintf_s(sEntry, sLen, m_strEntryFormat, sMRU + 1);
        if (m_arrNames[sMRU].IsEmpty() == XPR_FALSE)
        {
            sIniFile.setValueS(kRecentKey, sEntry, m_arrNames[sMRU]);
        }
    }

    delete[] sEntry;

    sIniFile.writeFile(xpr::CharSetUtf16);
}

void RecentFileListIni::ReadList(void)
{
    ASSERT(m_arrNames != XPR_NULL);
    ASSERT(!m_strSectionName.IsEmpty());
    ASSERT(!m_strEntryFormat.IsEmpty());

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeRecent, sPath, XPR_MAX_PATH);

    fxb::IniFile sIniFile(sPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return;

    xpr_sint_t sMRU;
    xpr_sint_t sLen = m_strEntryFormat.GetLength() + 10;
    xpr_tchar_t *sEntry = new xpr_tchar_t[sLen];
    CWinApp *sApp = AfxGetApp();

    for (sMRU = 0; sMRU < m_nSize; ++sMRU)
    {
        _stprintf_s(sEntry, sLen, m_strEntryFormat, sMRU + 1);
        m_arrNames[sMRU] = sIniFile.getValueS(kRecentKey, sEntry, XPR_STRING_LITERAL(""));
    }

    delete[] sEntry;
}

RecentFileListEx::RecentFileListEx(xpr_uint_t         aStart,
                                   const xpr_tchar_t *aSection,
                                   const xpr_tchar_t *aEntryFormat,
                                   xpr_sint_t         aSize,
                                   xpr_sint_t         aMaxDispLen)
    : RecentFileListIni(aStart, aSection, aEntryFormat, aSize, aMaxDispLen)
{
}

RecentFileListEx::~RecentFileListEx(void)
{
}

void RecentFileListEx::getRecentFile(xpr_sint_t aIndex, xpr_tchar_t *aPath)
{
    _tcscpy(aPath, (*this)[aIndex]);
}

void RecentFileListEx::UpdateMenu(CCmdUI *aCmdUI)
{
}

void RecentFileListEx::updateMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return;

    ASSERT(m_arrNames != XPR_NULL);

    if (gOpt->mRecentFile == XPR_FALSE)
    {
        const xpr_tchar_t *sText = theApp.loadString(XPR_STRING_LITERAL("cmd.recent_files.popup.not_used"));

        aPopupMenu->InsertMenu(aInsert, MF_STRING | MF_BYPOSITION, ID_FILE_RECENT_DYNAMIC_MENU, (xpr_tchar_t *)sText);
        aPopupMenu->EnableMenuItem(aInsert, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
        return;
    }

    if (m_arrNames[0].IsEmpty() == XPR_TRUE)
    {
        const xpr_tchar_t *sText = theApp.loadString(XPR_STRING_LITERAL("cmd.recent_files.popup.none"));

        aPopupMenu->InsertMenu(aInsert, MF_STRING | MF_BYPOSITION, ID_FILE_RECENT_DYNAMIC_MENU, (xpr_tchar_t *)sText);
        aPopupMenu->EnableMenuItem(aInsert, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
        return;
    }

    xpr_tchar_t sCurDir[XPR_MAX_PATH + 1];
    GetCurrentDirectory(XPR_MAX_PATH, sCurDir);

    xpr_sint_t sCurDirLen = lstrlen(sCurDir);
    ASSERT(sCurDirLen >= 0);
    sCurDir[sCurDirLen] = '\\';
    sCurDir[++sCurDirLen] = '\0';

    xpr_sint_t i;
    CString sName;
    CString sTemp;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];

    for (i = 0; i < m_nSize; ++i)
    {
        if (GetDisplayName(sName, i, sCurDir, sCurDirLen) == XPR_FALSE)
            break;

        // double up any '&' characters so they are not underlined
        const xpr_tchar_t *sSrc = sName;
        xpr_tchar_t *sDest = sTemp.GetBuffer(sName.GetLength()*2);
        while (*sSrc != 0)
        {
            if (*sSrc == '&')
                *sDest++ = '&';
            if (_istlead(*sSrc))
                *sDest++ = *sSrc++;
            *sDest++ = *sSrc++;
        }
        *sDest = 0;
        sTemp.ReleaseBuffer();

        // insert mnemonic + the file name
        wsprintf(sText, XPR_STRING_LITERAL("&%d %s"), (i + 1 + m_nStart) % 10, sTemp);
        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_FILE_RECENT_FIRST+i, sText);
    }
}
