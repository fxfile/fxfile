//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_DEF_WIN_H__
#define __FXFILE_BASE_DEF_WIN_H__ 1
#pragma once

//
// windows
//
#include <shlwapi.h>
#include <shlobj.h>

#define COM_RELEASE(aPtr) if ((aPtr) != XPR_NULL) { (aPtr)->Release(); (aPtr) = XPR_NULL; }
#define COM_FREE(aPtr) if ((aPtr) != XPR_NULL) { ::CoTaskMemFree((aPtr)); (aPtr) = XPR_NULL; }

#define GLOBAL_FREE(aPtr) if ((aPtr) != XPR_NULL) { if ((aPtr) != XPR_NULL) ::GlobalFree((aPtr)); (aPtr) = XPR_NULL; }
#define LOCAL_FREE(aPtr) if ((aPtr) != XPR_NULL) { if ((aPtr) != XPR_NULL) ::LocalFree((aPtr)); (aPtr) = XPR_NULL; }

#define DESTROY_DELETE(aWnd) if ((aWnd) != XPR_NULL) { (aWnd)->DestroyWindow(); delete (aWnd); (aWnd) = XPR_NULL; }

#define DELETE_OBJECT(aObject)  if ((aObject).m_hObject != XPR_NULL) { (aObject).DeleteObject(); }
#define DESTROY_ICON(aIcon)  if ((aIcon) != XPR_NULL) { ::DestroyIcon((aIcon)); (aIcon) = XPR_NULL; }
#define DESTROY_CURSOR(aCursor)  if ((aCursor) != XPR_NULL) { ::DestroyCursor((aCursor)); (aCursor) = XPR_NULL; }
#define DESTROY_WINDOW(aWnd) if ((aWnd).m_hWnd != XPR_NULL) { (aWnd).DestroyWindow(); }

#define CLOSE_HANDLE(aHandle) if ((aHandle) != XPR_NULL) { ::CloseHandle((aHandle)); (aHandle) = XPR_NULL; }

// for xp, server 2003, more
#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED 0x031A
#endif

#ifndef LPSHELLFOLDER2
typedef IShellFolder2 *LPSHELLFOLDER2;
#endif

#ifndef LPSHELLICONOVERLAY
typedef IShellIconOverlay *LPSHELLICONOVERLAY;
#endif

#ifndef LPSHELLICON
typedef IShellIcon *LPSHELLICON;
#endif

#define MAX_CLASS_NAME 256

#define WM_XBUTTON1DOWN 0x1020B
#define WM_XBUTTON2DOWN 0x2020B

#endif // __FXFILE_BASE_DEF_WIN_H__
