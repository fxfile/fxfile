//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "desktop_background.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
DesktopBackground::DesktopBackground(void)
{
}

DesktopBackground::~DesktopBackground(void)
{
}

xpr_bool_t DesktopBackground::apply(Style aStyle, const xpr_tchar_t *aPath)
{
    if (IsExistFile(aPath) == XPR_FALSE)
        return XPR_FALSE;

    // setup
    HKEY sKey;
    xpr_slong_t sResult;
    xpr_tchar_t sText[0xff] = {0};

    sResult = RegCreateKeyEx(HKEY_CURRENT_USER, XPR_STRING_LITERAL("Control Panel\\Desktop"), 0L,
        XPR_NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, XPR_NULL, &sKey, XPR_NULL);
    if (sResult != ERROR_SUCCESS)
        return XPR_FALSE;

    RegSetValueEx(sKey, XPR_STRING_LITERAL("Wallpaper"), XPR_NULL, REG_SZ, (const xpr_byte_t *)aPath, (DWORD)(_tcslen(aPath)+1)*sizeof(xpr_tchar_t));

    _stprintf(sText, XPR_STRING_LITERAL("%d"), aStyle);
    RegSetValueEx(sKey, XPR_STRING_LITERAL("WallpaperStyle"), XPR_NULL, REG_SZ, (const xpr_byte_t *)&sText, (DWORD)(_tcslen(sText)+1)*sizeof(xpr_tchar_t));

    _stprintf(sText, XPR_STRING_LITERAL("%d"), (aStyle == 1) ? 1 : 0);
    RegSetValueEx(sKey, XPR_STRING_LITERAL("TileWallpaper"), XPR_NULL, REG_SZ, (const xpr_byte_t *)&sText, (DWORD)(_tcslen(sText)+1)*sizeof(xpr_tchar_t));

    RegCloseKey(sKey);
    sKey = XPR_NULL;

    // apply
    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, XPR_NULL, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

    return XPR_TRUE;
}
} // namespace cmd
} // namespace fxfile
