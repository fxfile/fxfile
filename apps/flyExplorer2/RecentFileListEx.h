//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_RECENT_FILE_LIST_EX_H__
#define __FX_RECENT_FILE_LIST_EX_H__
#pragma once

#include <afxadv.h>

class BCMenu;

class RecentFileListIni : public CRecentFileList
{
    typedef CRecentFileList super;

public:
    RecentFileListIni(xpr_uint_t         aStart,
                      const xpr_tchar_t *aSection,
                      const xpr_tchar_t *aEntryFormat,
                      xpr_sint_t         aSize,
                      xpr_sint_t         aMaxDispLen = AFX_ABBREV_FILENAME_LEN);
    virtual ~RecentFileListIni(void);

public:
    virtual void ReadList(void);    // reads from registry or ini file
    virtual void WriteList(void);   // writes to registry or ini file
};

class RecentFileListEx : public RecentFileListIni
{
public:
    RecentFileListEx(xpr_uint_t         aStart,
                     const xpr_tchar_t *aSection,
                     const xpr_tchar_t *aEntryFormat,
                     xpr_sint_t         aSize,
                     xpr_sint_t         aMaxDispLen = AFX_ABBREV_FILENAME_LEN);
    virtual ~RecentFileListEx(void);

public:
    void getRecentFile(xpr_sint_t aIndex, xpr_tchar_t *aPath);
    void updateMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);

protected:
    virtual void UpdateMenu(CCmdUI *aCmdUI);
};

#endif // __FX_RECENT_FILE_LIST_EX_H__
