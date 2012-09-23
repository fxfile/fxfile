//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_MACROS_H__
#define __FXB_MACROS_H__
#pragma once

#define COM_RELEASE(ptr) if ((ptr)) { (ptr)->Release(); (ptr) = NULL; }
#define COM_FREE(ptr) if ((ptr)) { if ((ptr)) ::CoTaskMemFree((ptr)); (ptr) = NULL; }

#define GLOBAL_FREE(ptr) if ((ptr)) { if ((ptr)) ::GlobalFree((ptr)); (ptr) = NULL; }
#define LOCAL_FREE(ptr) if ((ptr)) { if ((ptr)) ::LocalFree((ptr)); (ptr) = NULL; }

#define DESTROY_DELETE(ptr) if ((ptr)) { (ptr)->DestroyWindow(); delete (ptr); (ptr) = NULL; }

#define DELETE_OBJECT(uiobj)  if ((uiobj).m_hObject) { (uiobj).DeleteObject(); }
#define DESTROY_ICON(icon)  if ((icon)) { ::DestroyIcon((icon)); (icon) = NULL; }
#define DESTROY_CURSOR(crs)  if ((crs)) { ::DestroyCursor((crs)); (crs) = NULL; }
#define DESTROY_WINDOW(wnd) if ((wnd).m_hWnd) { (wnd).DestroyWindow(); }

#define CLOSE_HANDLE(handle) if ((handle)) { ::CloseHandle((handle)); (handle) = NULL; }

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

#endif // __FXB_MACROS_H__
