//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_KEYBOARD_H__
#define __FXB_KEYBOARD_H__
#pragma once

namespace fxb
{
CString GetKeyName(xpr_uint_t aVirtKey);
void    ConvertStringToFormat(const xpr_tchar_t *aString, xpr_tchar_t *aFormat);
void    ConvertDoubleAmpersand(xpr_tchar_t *aText, xpr_tchar_t *aTemp);
} // namespace fxb

#endif // __FXB_KEYBOARD_H__
