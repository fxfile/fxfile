//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "shell_registry.h"

#include "winapi_ex.h"

#include <atlbase.h> // for CRegKey

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
static const xpr_tchar_t kKeyName[] = XPR_STRING_LITERAL("Folder\\Shell");
} // namespace anonymous

void ShellRegistry::registerShell(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CLASSES_ROOT, kKeyName) == ERROR_SUCCESS)
    {
        // HKEY_CLASSES_ROOT\Folder\shell\open with fxfile\command
        // (default) : "C:\Program Files\fxfile\fxfile.exe" "%1"

        xpr_tchar_t sCmd[XPR_MAX_PATH + 2 + 1] = {0};
        GetModuleDir(sCmd+1, XPR_MAX_PATH + 1);
        sCmd[0] = XPR_STRING_LITERAL('\"');
        _tcscat(sCmd, XPR_STRING_LITERAL("\\fxfile.exe\" \"%1\""));

        xpr_tchar_t sKey[0xff] = {0};
        _stprintf(sKey, XPR_STRING_LITERAL("%sfxfile%s\\command"), gApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_fxfile.prefix")), gApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_fxfile.suffix")));

        sRegKey.SetKeyValue(sKey, sCmd);
    }

    sRegKey.Close();
}

void ShellRegistry::unregisterShell(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CLASSES_ROOT, kKeyName) == ERROR_SUCCESS)
    {
        xpr_tchar_t sKey[0xff] = {0};
        _stprintf(sKey, XPR_STRING_LITERAL("%sfxfile%s"), gApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_fxfile.prefix")), gApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_fxfile.suffix")));

        sRegKey.RecurseDeleteKey(sKey);
    }

    sRegKey.Close();
}
} // namespace fxfile
