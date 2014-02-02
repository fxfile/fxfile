//
// Copyright (c) 2001-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_GDI_H__
#define __FXFILE_GDI_H__ 1
#pragma once

COLORREF LightenColor(COLORREF aRgb, xpr_double_t aFactor);
COLORREF CalculateColor(COLORREF aFrontColor, COLORREF aBackColor, xpr_sint_t aAlpha);

void DrawHorzGradient(CDC *sDC, CRect aRect, COLORREF aBeginColor, COLORREF aEndColor);
void DrawVertGradient(CDC *sDC, CRect aRect, COLORREF aBeginColor, COLORREF aEndColor);

xpr_bool_t DuplicateImgList(CImageList *aSrcImgList, CImageList *aDstImgList);
xpr_bool_t LoadImgList(CImageList *aImgList, HBITMAP aBitmap, CSize aIconSize, xpr_sint_t aOverlayCount = 0);
xpr_bool_t LoadImgList(CImageList *aImgList, xpr_uint_t aBitmapId, CSize aIconSize, xpr_sint_t aOverlayCount = 0);
xpr_bool_t LoadImgList(CImageList *aImgList, const xpr_tchar_t *aPath, CSize aIconSize, xpr_sint_t aOverlayCount = 0);

xpr_bool_t getIconInfo(HICON sIcon, SIZE *aIconSize);

HFONT CreateFont(const xpr_tchar_t *aFaceName,
                 xpr_sint_t         aHeight,
                 xpr_bool_t         aBold = XPR_FALSE,
                 xpr_bool_t         aItalic = XPR_FALSE,
                 xpr_bool_t         aUnderLine = XPR_FALSE,
                 xpr_bool_t         aStrikeOut = XPR_FALSE,
                 xpr_sint_t         aWidth = -1);


xpr_bool_t FontDlgToString(const xpr_tchar_t *aFaceName,
                           xpr_sint_t         aSize,
                           xpr_sint_t         aWeight,
                           xpr_bool_t         aItalic,
                           xpr_bool_t         aStrikeOut,
                           xpr_bool_t         aUnderline,
                           xpr_tchar_t       *aFontText);

xpr_bool_t StringToLogFont(const xpr_tchar_t *aFontText, LOGFONT &aLogFont);

#endif // __FXFILE_GDI_H__
