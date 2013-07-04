//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CLIPBOARD_H__
#define __FXFILE_CLIPBOARD_H__ 1
#pragma once

namespace fxfile
{
xpr_bool_t FillFormatEtc(LPFORMATETC aFormatEtc, CLIPFORMAT aClipFormat);

void       SetClipboard(LPDATAOBJECT aDataObject, DROPEFFECT aDropEffect, CLIPFORMAT *aClipFormat);
void       SetTextClipboard(HWND aHwndOwner, const xpr_tchar_t *aText);
void       SetTextClipboardA(HWND aHwndOwner, const xpr_char_t *aText);
void       SetTextClipboardW(HWND aHwndOwner, const xpr_tchar_t *aText);

xpr_bool_t DoPaste(HWND               aHwnd,
                   CLIPFORMAT         aShellIDListClipFormat,
                   CLIPFORMAT         aDropEffectClipFormat,
                   const xpr_tchar_t *aTarget,
                   const xpr_tchar_t *aLinkSuffix,
                   xpr_uint_t         aMsg = 0,
                   xpr_bool_t         aCopy = XPR_FALSE);

xpr_bool_t DoPaste(HWND               aHwnd,
                   COleDataObject    *aOleDataObject,
                   xpr_bool_t         aDragDrop,
                   CLIPFORMAT         aShellIDListClipFormat,
                   DROPEFFECT         aDropEffectClipFormat,
                   const xpr_tchar_t *aTarget,
                   const xpr_tchar_t *aLinkSuffix,
                   xpr_uint_t         aMsg = 0,
                   xpr_bool_t         aCopy = XPR_FALSE);

xpr_bool_t DoPasteFileContents(LPDATAOBJECT  aDataObject,
                               xpr_tchar_t  *aDir,
                               CLIPFORMAT    aFileContentsClipFormat,
                               CLIPFORMAT    aFileDescriptorClipFormat,
                               xpr_bool_t    aUnicode);

xpr_bool_t DoPasteInetUrl(LPDATAOBJECT  aDataObject,
                          xpr_tchar_t  *aDir,
                          CLIPFORMAT    aInetUrlClipFormat,
                          CLIPFORMAT    aFileContentsClipFormat,
                          CLIPFORMAT    aFileDescriptorClipFormat,
                          xpr_bool_t    aUnicode);

xpr_bool_t DoPasteText(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath);
xpr_bool_t DoPasteUnicodeText(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath);
xpr_bool_t DoPasteBitmap(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath);
xpr_bool_t DoPasteDIB(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath);
xpr_bool_t GetPreferredDropEffect(CLIPFORMAT aPreferredDropEffectClipFormat, DROPEFFECT &aDropEffect);
xpr_bool_t GetPreferredDropEffect(COleDataObject *aOleDataObject, CLIPFORMAT aPreferredDropEffectClipFormat, DROPEFFECT &aDropEffect);
xpr_bool_t IsPasteFileContents(COleDataObject *aOleDataObject, CLIPFORMAT aFileContentsClipFormat, CLIPFORMAT aFileDescriptorClipFormat);
xpr_bool_t IsPasteInetUrl(COleDataObject *aOleDataObject, CLIPFORMAT aInetUrlClipFormat, CLIPFORMAT aFileContentsClipFormat, CLIPFORMAT aFileDescriptorClipFormat);
xpr_bool_t IsPasteText(COleDataObject *aOleDataObject);
xpr_bool_t IsPasteUnicodeText(COleDataObject *aOleDataObject);
xpr_bool_t IsPasteBitmap(COleDataObject *aOleDataObject);
xpr_bool_t IsPasteDIB(COleDataObject *aOleDataObject);
xpr_sint_t GetDataObjectFormatCount(COleDataObject *aOleDataObject);
} // namespace fxfile

#endif // __FXFILE_CLIPBOARD_H__
