//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_WIN_APP_H__
#define __FXFILE_WIN_APP_H__ 1
#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "gui/rebar/WinAppEx.h"

#include "option_observer.h"

namespace fxfile
{
namespace base
{
class LanguageTable;
class StringTable;
class FormatStringTable;
} // namespace base

class Option;
class WinApp;
class MainFrame;

extern WinApp    gApp;
extern MainFrame *gFrame;
extern Option    *gOpt;

class WinApp
    : public CWinAppEx
    , public OptionObserver
{
    typedef CWinAppEx super;

public:
    WinApp(void);
    ~WinApp(void);

public:
    void saveAllOptions(void);

public:
    xpr_bool_t loadLanguageTable(void);
    xpr_bool_t loadLanguage(const xpr_tchar_t *aLanguage);
    const fxfile::base::LanguageTable *getLanguageTable(void) const;

    const xpr_tchar_t *loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable = XPR_FALSE);
    const xpr_tchar_t *loadFormatString(const xpr_tchar_t *aId, const xpr_tchar_t *aReplaceFormatSpecifier);

protected:
    void LoadStdProfileSettings(xpr_uint_t aMaxMRU = _AFX_MRU_COUNT);

protected:
    virtual xpr_bool_t InitInstance(void);
    virtual xpr_sint_t ExitInstance(void);

protected:
    // from OptionObserver
    virtual void onChangedConfig(Option &aOption);

protected:
    fxfile::base::LanguageTable     *mLanguageTable;
    fxfile::base::StringTable       *mStringTable;
    fxfile::base::FormatStringTable *mFormatStringTable;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnFileNew(void);
};
} // namespace fxfile

#endif // __FXFILE_WIN_APP_H__
