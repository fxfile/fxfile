//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BITMAP_H__
#define __FXFILE_BITMAP_H__ 1
#pragma once

namespace fxfile
{
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP aBitmap);
xpr_bool_t WriteBitmapFile(const xpr_tchar_t *aFile, PBITMAPINFO aBitmapInfo);
xpr_bool_t WriteBitmapFile(const xpr_tchar_t *aFile, PBITMAPINFO aBitmapInfo, HBITMAP aBitmap, HDC aDC);
xpr_bool_t WriteBitmapFile(const xpr_tchar_t *aFile, HBITMAP aBitmap, HDC aDC);
xpr_bool_t WriteBitmapFile(const xpr_tchar_t *aFile, HDC aDC, LPCRECT aRect);
} // namespace fxfile

#endif // __FXFILE_BITMAP_H__
