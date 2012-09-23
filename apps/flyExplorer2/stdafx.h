//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_STDAFX_H__
#define __FX_STDAFX_H__
#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// windows
#include <process.h>

// xpr
#include "xpr.h"

// flyExplorer
#include "fxb/fxb.h"
#include "Explorer.h"
#include "ItemData.h"

// crash handler
#if defined(XPR_CFG_BUILD_RELEASE)
    #include "fxcrash.h"

    #ifdef XPR_CFG_UNICODE
        #pragma comment(lib, "fxcrashu.lib")
    #else
        #pragma comment(lib, "fxcrash.lib")
    #endif
#endif

#endif // __FX_STDAFX_H__