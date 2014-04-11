//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_WINAPI_EX_H__
#define __FXFILE_WINAPI_EX_H__ 1
#pragma once

namespace fxfile
{
xpr_bool_t SetForceForegroundWindow(HWND aHwnd);

//
// multi-monitor
//
void  GetDesktopRect(const POINT &aPoint, RECT &aDesktopRect, xpr_bool_t aWorkArea = XPR_TRUE);
void  GetDesktopRect(const RECT &aRect, RECT &aDesktopRect, xpr_bool_t aWorkArea = XPR_TRUE);
void  GetPrimaryDesktopRect(RECT &aDesktopRect, xpr_bool_t aWorkArea = XPR_TRUE);
RECT *GetDesktopListRect(xpr_size_t *aCount, xpr_bool_t aWorkArea = XPR_TRUE);

//
// e-mail
//
xpr_bool_t SendMail(xpr_char_t **aPath, xpr_sint_t aCount);

//
// string manipulation
//
void _tcstrimleft(xpr_tchar_t *aStr);
void _tcstrimright(xpr_tchar_t *aStr);

//
// file
//
xpr_bool_t GetFileLastWriteTime(const xpr_tchar_t *aPath, LPFILETIME aModifiedFileTime);

//
// message
//
LRESULT SendNotifyCodeMessage(HWND aHwnd, LPNMHDR aNmHdr);
LRESULT SendNotifyCodeMessage(CWnd *aWnd, LPNMHDR aNmHdr);

//
// module
//
void GetModulePath(xpr_tchar_t *aPath, xpr_size_t aMaxLen);
void GetModulePath(xpr::string &aPath);
void GetModuleDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen);
void GetModuleDir(xpr::string &aDir);

//
// message box
//
xpr_uint_t MsgBox(const xpr_tchar_t *aFormat, ...);

//
// stream
//
HRESULT StreamToFile(LPSTREAM aStream, xpr_tchar_t *aFile, const xpr_sint_t aBlockSize = 4096);
HRESULT StorageToFile(LPSTORAGE aStorage, xpr_tchar_t *aFile);

//
// UAC
//
xpr_bool_t IsUACRequiredWindows(void);
HRESULT    IsUACElevated(xpr_bool_t *aElevated);
} // namespace fxfile

#endif // __FXFILE_WINAPI_EX_H__
