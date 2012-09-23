//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_EXPLORER_H__
#define __FX_EXPLORER_H__
#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "rgc/WinAppEx.h"

namespace fxb
{
class LanguageTable;
class StringTable;
class FormatStringTable;
};

class Option;
class ExplorerApp;
class MainFrame;
class BCMenu;

extern ExplorerApp  theApp;
extern MainFrame   *gFrame;
extern Option      *gOpt;

class ExplorerApp : public CWinAppEx
{
    typedef CWinAppEx super;

public:
    ExplorerApp(void);
    ~ExplorerApp(void);

public:
    xpr_bool_t updateRecentMenu(BCMenu *aMenu, xpr_sint_t aInsert);
    xpr_bool_t getRecentFile(xpr_uint_t aId, xpr_tchar_t *aPath);
    void removeRecentFileList(void);

    void setSingleInstance(xpr_bool_t aSingleInstance);

public:
    xpr_bool_t loadLanguageTable(void);
    xpr_bool_t loadLanguage(const xpr_tchar_t *aLanguage);

    const xpr_tchar_t *loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable = XPR_FALSE);
    const xpr_tchar_t *loadFormatString(const xpr_tchar_t *aId, const xpr_tchar_t *aReplaceFormatSpecifier);

protected:
    void LoadStdProfileSettings(xpr_uint_t nMaxMRU = _AFX_MRU_COUNT);

protected:
    virtual xpr_bool_t InitInstance(void);
    virtual xpr_sint_t ExitInstance(void);

protected:
    HANDLE mSingleInstanceMutex;

    fxb::LanguageTable     *mLanguageTable;
    fxb::StringTable       *mStringTable;
    fxb::FormatStringTable *mFormatStringTable;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnFileNew(void);
};

#endif // __FX_EXPLORER_H__