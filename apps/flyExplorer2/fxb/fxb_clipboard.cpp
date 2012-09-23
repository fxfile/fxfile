//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_clipboard.h"

#include <afxole.h>
#include "fxb_file_op_thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
xpr_bool_t FillFormatEtc(LPFORMATETC aFormatEtc, CLIPFORMAT aClipFormat)
{
    if (XPR_IS_NULL(aFormatEtc) || aClipFormat != 0)
        return XPR_FALSE;

    aFormatEtc->cfFormat = aClipFormat;
    aFormatEtc->ptd      = XPR_NULL;
    aFormatEtc->dwAspect = DVASPECT_CONTENT;
    aFormatEtc->lindex   = -1;
    aFormatEtc->tymed    = (DWORD)-1;

    return XPR_TRUE;
}

void SetClipboard(LPDATAOBJECT aDataObject, DROPEFFECT aDropEffect, CLIPFORMAT *aClipFormat)
{
    if (XPR_IS_NULL(aDataObject))
        return;

    COleDataSource *sDataSource = new COleDataSource;
    if (XPR_IS_NULL(sDataSource))
        return;

    STGMEDIUM sStgMedium = {0};
    FORMATETC sFormatEtc = {0};

    sFormatEtc.ptd      = (DVTARGETDEVICE FAR *)XPR_NULL;
    sFormatEtc.dwAspect = DVASPECT_CONTENT;
    sFormatEtc.lindex   = -1;
    sFormatEtc.tymed    = TYMED_HGLOBAL;

    // ShellIDList
    sFormatEtc.cfFormat = aClipFormat[0];
    aDataObject->GetData(&sFormatEtc, &sStgMedium);
    sDataSource->CacheGlobalData(aClipFormat[0], sStgMedium.hGlobal);

    // Unknown Clipformat - HDROP
    memset(&sStgMedium, 0, sizeof(STGMEDIUM));
    sFormatEtc.cfFormat = CF_HDROP;
    aDataObject->GetData(&sFormatEtc, &sStgMedium);
    sDataSource->CacheGlobalData(CF_HDROP, sStgMedium.hGlobal);

    // DropEffect
    HGLOBAL sGlobal = ::GlobalAlloc(GHND | GMEM_SHARE, sizeof(DROPEFFECT));
    DROPEFFECT *sDropEffect = (DROPEFFECT *)::GlobalLock(sGlobal);
    *sDropEffect = aDropEffect;
    ::GlobalUnlock(sGlobal);
    sDataSource->CacheGlobalData(aClipFormat[1], sGlobal);

    // FileName
    memset(&sStgMedium, 0, sizeof(STGMEDIUM));
    sFormatEtc.cfFormat = aClipFormat[2];
    aDataObject->GetData(&sFormatEtc, &sStgMedium);
    sDataSource->CacheGlobalData(aClipFormat[2], sStgMedium.hGlobal);

    // FileNameW
    memset(&sStgMedium, 0, sizeof(STGMEDIUM));
    sFormatEtc.cfFormat = aClipFormat[3];
    aDataObject->GetData(&sFormatEtc, &sStgMedium);
    sDataSource->CacheGlobalData(aClipFormat[3], sStgMedium.hGlobal);

    sDataSource->SetClipboard();
}

xpr_bool_t DoPaste(HWND               aHwnd,
                   CLIPFORMAT         aShellIDListClipFormat,
                   CLIPFORMAT         aDropEffectClipFormat,
                   const xpr_tchar_t *aTarget,
                   const xpr_tchar_t *aLinkSuffix,
                   xpr_uint_t         aMsg,
                   xpr_bool_t         aCopy)
{
    COleDataObject sOleDataObject;
    if (sOleDataObject.AttachClipboard() == XPR_FALSE)
        return XPR_FALSE;

    DROPEFFECT sDropEffect = aDropEffectClipFormat;
    if (aDropEffectClipFormat > 4)
    {
        HGLOBAL sGlobal = sOleDataObject.GetGlobalData(aDropEffectClipFormat);
        if (XPR_IS_NULL(sGlobal))
            return XPR_FALSE;

        sDropEffect = *((DROPEFFECT *)::GlobalLock(sGlobal));

        ::GlobalUnlock(sGlobal);
        //::GlobalFree(sGlobal);
    }

    return DoPaste(aHwnd, &sOleDataObject, XPR_FALSE, aShellIDListClipFormat, sDropEffect, aTarget, aLinkSuffix, aMsg, aCopy);
}

xpr_bool_t DoPaste(HWND               aHwnd,
                   COleDataObject    *aOleDataObject,
                   xpr_bool_t         aDragDrop,
                   CLIPFORMAT         aShellIDListClipFormat,
                   DROPEFFECT         aDropEffect,
                   const xpr_tchar_t *aTarget,
                   const xpr_tchar_t *aLinkSuffix,
                   xpr_uint_t         aMsg,
                   xpr_bool_t         aCopy)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    CWaitCursor sWaitCursor;
    if (aOleDataObject->IsDataAvailable(aShellIDListClipFormat) == XPR_TRUE && aDropEffect == DROPEFFECT_LINK)
    {
        STGMEDIUM sStgMedium = {0};
        FORMATETC sFormatEtc = {aShellIDListClipFormat, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

        if (aOleDataObject->GetData(aShellIDListClipFormat, &sStgMedium, &sFormatEtc) == XPR_TRUE)
        {
            LPIDA sIda = (LPIDA)::GlobalLock(sStgMedium.hGlobal);
            xpr_sint_t sCount = sIda->cidl;

            xpr_tchar_t *sMultipleSource = XPR_NULL;
            xpr_tchar_t *sMultipleSourceEnum = XPR_NULL;
            if (XPR_IS_NOT_NULL(aHwnd) && aMsg >= WM_USER)
            {
                sMultipleSource = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sCount + 1];
                sMultipleSourceEnum = sMultipleSource;
            }

            std::tstring sName;
            xpr_tchar_t sLink[XPR_MAX_PATH + 1];
            xpr_size_t sColon;

            LPITEMIDLIST sFullPidl = XPR_NULL; // fully-qualified PIDL
            LPITEMIDLIST sFolderPidl = XPR_NULL; // folder PIDL
            LPITEMIDLIST sItemPidl = XPR_NULL; // item PIDL

            // get folder PIDL
            sFolderPidl = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

            LPSHELLFOLDER sDesktopShellFolder = XPR_NULL;
            ::SHGetDesktopFolder(&sDesktopShellFolder);

            LPSHELLFOLDER sShellFolder = XPR_NULL;
            HRESULT sHResult = sDesktopShellFolder->BindToObject(sFolderPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sShellFolder);
            if (FAILED(sHResult) || XPR_IS_NULL(sShellFolder))
            {
                sShellFolder = sDesktopShellFolder;
                sDesktopShellFolder = XPR_NULL;
            }

            std::tstring sLinkSuffix;
            if (XPR_IS_NOT_NULL(aLinkSuffix))
                sLinkSuffix = aLinkSuffix;
            else
                sLinkSuffix = XPR_STRING_LITERAL(" - Shortcut");

            xpr_sint_t i;
            for (i = 0; i < sCount; ++i)
            {
                // get item PIDL and get full-qualified PIDL
                sItemPidl = (LPITEMIDLIST)((xpr_byte_t *)sIda + sIda->aoffset[i+1]);
                sFullPidl = ConcatPidls(sFolderPidl, sItemPidl);

                fxb::GetName(sShellFolder, sItemPidl, SHGDN_INFOLDER, sName);

                sColon = sName.find(XPR_STRING_LITERAL(':'));
                if (sColon != std::tstring::npos)
                    sName.erase(sColon, 1);

                sName += sLinkSuffix;

                if (SetNewPath(sLink, aTarget, sName.c_str(), XPR_STRING_LITERAL(".lnk")) == XPR_TRUE)
                {
                    if (CreateShortcut(sLink, sFullPidl) == XPR_TRUE)
                    {
                        _tcscpy(sMultipleSourceEnum, sLink);
                        sMultipleSourceEnum += _tcslen(sMultipleSourceEnum) + 1;
                    }
                }

                COM_FREE(sFullPidl);
            }

            sMultipleSourceEnum[0] = '\0'; // double end null

            ::SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH | SHCNF_FLUSH, (LPCVOID)aTarget, XPR_NULL);

            if (XPR_IS_NOT_NULL(sMultipleSource) && XPR_IS_NOT_NULL(sMultipleSourceEnum) && XPR_IS_NOT_NULL(aHwnd) && aMsg >= WM_USER)
            {
                PasteSelItems sPasteSelItems = {0};
                sPasteSelItems.mSource = sMultipleSource;
                sPasteSelItems.mTarget = aTarget;
                ::SendMessage(aHwnd, aMsg, (WPARAM)&sPasteSelItems, 0);
            }

            XPR_SAFE_DELETE_ARRAY(sMultipleSource);
            COM_RELEASE(sShellFolder);
            COM_RELEASE(sDesktopShellFolder);
            ::GlobalUnlock(sStgMedium.hGlobal);
            //::GlobalFree(sStgMedium.hGlobal);
            ::ReleaseStgMedium(&sStgMedium);
        }

        return sResult;
    }

    xpr_uint_t sFiles;
    xpr_tchar_t *sMultipleSource = XPR_NULL;
    xpr_tchar_t *sMultipleSourceEnum = XPR_NULL;
    xpr_tchar_t *sTarget = XPR_NULL;

    if (aOleDataObject->IsDataAvailable(aShellIDListClipFormat) == XPR_TRUE)
    {
        STGMEDIUM sStgMedium = {0};
        FORMATETC sFormatEtc = {aShellIDListClipFormat, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

        if (aOleDataObject->GetData(aShellIDListClipFormat, &sStgMedium, &sFormatEtc) == XPR_TRUE)
        {
            LPIDA sIda = (LPIDA)::GlobalLock(sStgMedium.hGlobal);
            sFiles = sIda->cidl;

            LPITEMIDLIST sFullPidl = XPR_NULL; // fully-qualified PIDL
            LPITEMIDLIST sFolderPidl = XPR_NULL; // folder PIDL
            LPITEMIDLIST sItemPidl = XPR_NULL; // item PIDL

            // get folder PIDL
            sFolderPidl = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

            LPSHELLFOLDER sDesktopShellFolder = XPR_NULL;
            ::SHGetDesktopFolder(&sDesktopShellFolder);

            LPSHELLFOLDER sShellFolder = XPR_NULL;
            HRESULT sHResult = sDesktopShellFolder->BindToObject(sFolderPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sShellFolder);
            if (FAILED(sHResult) || XPR_IS_NULL(sShellFolder))
            {
                sShellFolder = sDesktopShellFolder;
                sDesktopShellFolder = XPR_NULL;
            }

            xpr_sint_t i;
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            sMultipleSource = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sFiles + 1];
            memset(sMultipleSource, 0, sizeof(xpr_tchar_t) * (XPR_MAX_PATH + 1) * sFiles + sizeof(xpr_tchar_t));
            sMultipleSourceEnum = sMultipleSource;

            for (i = 0; i < (xpr_sint_t)sFiles; ++i)
            {
                // get item PIDL and get full-qualified PIDL
                sItemPidl = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
                fxb::GetName(sShellFolder, sItemPidl, SHGDN_FORPARSING, sPath);

                _tcscpy(sMultipleSourceEnum, sPath);

                sMultipleSourceEnum += _tcslen(sMultipleSourceEnum) + 1;
            }

            sMultipleSourceEnum[0] = '\0'; // double null end

            COM_RELEASE(sShellFolder);
            COM_RELEASE(sDesktopShellFolder);
            ::GlobalUnlock(sStgMedium.hGlobal);
            ::ReleaseStgMedium(&sStgMedium);

            if (sFiles > 0)
                sResult = XPR_TRUE;
        }
    }

    if (XPR_IS_FALSE(sResult) && aOleDataObject->IsDataAvailable(CF_HDROP) == XPR_TRUE)
    {
        // First, Get Global Data(CF_HDROP)
        HDROP sDrop = (HDROP)aOleDataObject->GetGlobalData(CF_HDROP);

        sFiles = ::DragQueryFile(sDrop, 0xFFFFFFFF, XPR_NULL, 0); // 0xFFFFFFFF -1
        if (sFiles > 0)
        {
            xpr_sint_t i;
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            sMultipleSource = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sFiles + 1];
            sMultipleSourceEnum = sMultipleSource;

            for (i = 0; i < (xpr_sint_t)sFiles; ++i)
            {
                ::DragQueryFile(sDrop, i, sPath, XPR_MAX_PATH);

                _tcscpy(sMultipleSourceEnum, sPath);

                sMultipleSourceEnum += _tcslen(sMultipleSourceEnum) + 1;
            }

            sMultipleSourceEnum[0] = '\0'; // double end null
        }

        ::GlobalUnlock(sDrop);
        ::DragFinish(sDrop);

        if (sFiles > 0)
            sResult = XPR_TRUE;
    }

    if (XPR_IS_FALSE(sResult))
    {
        XPR_SAFE_DELETE_ARRAY(sMultipleSource);
        XPR_SAFE_DELETE_ARRAY(sTarget);
        return sResult;
    }

    // check target file name (double end null)
    // +---+---+---+---+---+---+---+---+---+---+---+
    // | P | A | T | H | \0| P | A | T | H | \0| \0|
    // +---+---+---+---+---+---+---+---+---+---+---+

    sTarget = new xpr_tchar_t[XPR_MAX_PATH + 1];
    _tcscpy(sTarget, aTarget);

    xpr_sint_t sLen = (xpr_sint_t)_tcslen(sTarget);
    if (sTarget[sLen - 1] == '\\')
        sTarget[sLen - 1] = '\0';
    sLen = (xpr_sint_t)_tcslen(sTarget);
    sTarget[sLen + 1] = '\0';

    xpr_uint_t sFunc = 0;
    WORD sFlags = 0;
    // Drop Effect
    // 1-Copy, 2-Move, 3-Link, 5-Copy+Link
    if (aDropEffect == DROPEFFECT_MOVE)
        sFunc = FO_MOVE;
    else
    {
        sFunc = FO_COPY;

        CString sCompare;
        AfxExtractSubString(sCompare, sMultipleSource, 0, '\0');
        sCompare = sCompare.Left(sCompare.ReverseFind('\\'));
        if (_tcscmp(sCompare, sTarget) == 0)
            sFlags |= FOF_RENAMEONCOLLISION;
    }

    // windows vista bug
    //if (sFiles > 1)
    //  sFlags |= FOF_MULTIDESTFILES;

    if (XPR_IS_TRUE(aCopy))
        sFlags |= FOF_RENAMEONCOLLISION;

    SHFILEOPSTRUCT *pshfo = new SHFILEOPSTRUCT;
    memset(pshfo, 0, sizeof(SHFILEOPSTRUCT));
    pshfo->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
    pshfo->wFunc  = sFunc;
    pshfo->fFlags = sFlags;
    pshfo->pFrom  = sMultipleSource;
    pshfo->pTo    = sTarget;

    if (XPR_IS_FALSE(aDragDrop) && sFunc == FO_MOVE)
    {
        // Empty clipboard - The first parameter of OleSetClipboard function is XPR_NULL
        ::OleSetClipboard(XPR_NULL);
    }

    FileOpThread *sFileOpThread = new FileOpThread;
    sFileOpThread->start(pshfo, aMsg >= WM_USER, aHwnd, aMsg);

    return sResult;
}

xpr_sint_t GetDataObjectFormatCount(COleDataObject *aOleDataObject)
{
    if (XPR_IS_NULL(aOleDataObject))
        return 0;

    xpr_sint_t sFormatCount = 0;

    FORMATETC sFormatEtc = {0};
    aOleDataObject->BeginEnumFormats();
    while (aOleDataObject->GetNextFormat(&sFormatEtc))
        sFormatCount++;

    return sFormatCount;
}

xpr_bool_t DoPasteText(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aDataObject) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    FORMATETC sTextFormatEtc = { CF_TEXT, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    HRESULT sHResult;
    STGMEDIUM sStgMedium = {0};
    sHResult = aDataObject->GetData(&sTextFormatEtc, &sStgMedium);

    xpr_char_t *aText = (xpr_char_t *)::GlobalLock(sStgMedium.hGlobal);
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(aText))
    {
        xpr_sint_t sSize = (xpr_sint_t)::GlobalSize(sStgMedium.hGlobal);

        FILE *sFile = _tfopen(aPath, XPR_STRING_LITERAL("wb"));
        if (XPR_IS_NOT_NULL(sFile))
        {
            fwrite(aText, sSize - 1, 1, sFile);
            fclose(sFile);

            ::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, aPath, XPR_NULL);
            sResult = XPR_TRUE;
        }
    }

    if (XPR_IS_NOT_NULL(sStgMedium.hGlobal))
        ::GlobalUnlock(sStgMedium.hGlobal);

    return sResult;
}

xpr_bool_t DoPasteUnicodeText(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aDataObject) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    FORMATETC sTextFormatEtc = { CF_UNICODETEXT, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    HRESULT sHResult;
    STGMEDIUM sStgMedium = {0};
    sHResult = aDataObject->GetData(&sTextFormatEtc, &sStgMedium);

    xpr_wchar_t *aText = (xpr_wchar_t *)::GlobalLock(sStgMedium.hGlobal);
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(aText))
    {
        xpr_sint_t sSize = (xpr_sint_t)::GlobalSize(sStgMedium.hGlobal);

        FILE *sFile = _tfopen(aPath, XPR_STRING_LITERAL("wb"));
        if (XPR_IS_NOT_NULL(sFile))
        {
            xpr_ushort_t sUnicodeBOM = 0xFEFF;
            fwrite(&sUnicodeBOM, 2, 1, sFile);
            fwrite(aText, sSize - 1, 1, sFile);
            fclose(sFile);

            ::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, aPath, XPR_NULL);
            sResult = XPR_TRUE;
        }
    }

    if (XPR_IS_NOT_NULL(sStgMedium.hGlobal))
        ::GlobalUnlock(sStgMedium.hGlobal);

    return sResult;
}

xpr_bool_t DoPasteBitmap(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aDataObject) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    FORMATETC sBitmapFormatEtc = { CF_BITMAP, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_GDI };

    HRESULT sHResult;
    STGMEDIUM sStgMedium = {0};
    sHResult = aDataObject->GetData(&sBitmapFormatEtc, &sStgMedium);

    HBITMAP sBitmap = sStgMedium.hBitmap;
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sBitmap))
    {
        HWND sHwnd = ::GetDesktopWindow();
        HDC sDC = ::GetDC(sHwnd);

        WriteBitmapFile((xpr_tchar_t *)aPath, sBitmap, sDC);

        ::ReleaseDC(sHwnd, sDC);

        ::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, aPath, XPR_NULL);
        sResult = XPR_TRUE;
    }

    ::DeleteObject(sBitmap);

    return sResult;
}

xpr_bool_t DoPasteDIB(LPDATAOBJECT aDataObject, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aDataObject) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    FORMATETC sDibFormatEtc = { CF_DIB, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    HRESULT sHResult;
    STGMEDIUM sStgMedium = {0};
    sHResult = aDataObject->GetData(&sDibFormatEtc, &sStgMedium);

    LPBITMAPINFO sBitmapInfo = (LPBITMAPINFO)::GlobalLock(sStgMedium.hGlobal);
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sBitmapInfo))
    {
        WriteBitmapFile((xpr_tchar_t *)aPath, sBitmapInfo);

        ::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, aPath, XPR_NULL);
        sResult = XPR_TRUE;
    }

    if (XPR_IS_NOT_NULL(sStgMedium.hGlobal))
        ::GlobalUnlock(sStgMedium.hGlobal);

    return sResult;
}

xpr_bool_t GetPreferredDropEffect(CLIPFORMAT aPreferredDropEffectClipFormat, DROPEFFECT &aDropEffect)
{
    COleDataObject sOleDataObject;
    if (sOleDataObject.AttachClipboard() == XPR_FALSE)
        return XPR_FALSE;

    return GetPreferredDropEffect(&sOleDataObject, aPreferredDropEffectClipFormat, aDropEffect);
}

xpr_bool_t GetPreferredDropEffect(COleDataObject *aOleDataObject, CLIPFORMAT aPreferredDropEffectClipFormat, DROPEFFECT &aDropEffect)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    HGLOBAL sGlobal = aOleDataObject->GetGlobalData(aPreferredDropEffectClipFormat);
    if (XPR_IS_NULL(sGlobal))
        return XPR_FALSE;

    aDropEffect = *((DROPEFFECT *)::GlobalLock(sGlobal));

    ::GlobalUnlock(sGlobal);

    return XPR_TRUE;
}

xpr_bool_t IsPasteDIB(COleDataObject *aOleDataObject)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    if (aOleDataObject->IsDataAvailable((CLIPFORMAT)CF_DIB) == XPR_TRUE)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t IsPasteBitmap(COleDataObject *aOleDataObject)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    if (aOleDataObject->IsDataAvailable((CLIPFORMAT)CF_BITMAP) == XPR_TRUE)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t IsPasteText(COleDataObject *aOleDataObject)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    if (aOleDataObject->IsDataAvailable((CLIPFORMAT)CF_TEXT) == XPR_TRUE)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t IsPasteUnicodeText(COleDataObject *aOleDataObject)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    if (aOleDataObject->IsDataAvailable((CLIPFORMAT)CF_UNICODETEXT) == XPR_TRUE)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t IsPasteInetUrl(COleDataObject *aOleDataObject, CLIPFORMAT aInetUrlClipFormat, CLIPFORMAT aFileContentsClipFormat, CLIPFORMAT aFileDescriptorClipFormat)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    if (aOleDataObject->IsDataAvailable((CLIPFORMAT)aInetUrlClipFormat)        == XPR_TRUE &&
        aOleDataObject->IsDataAvailable((CLIPFORMAT)aFileContentsClipFormat)   == XPR_TRUE &&
        aOleDataObject->IsDataAvailable((CLIPFORMAT)aFileDescriptorClipFormat) == XPR_TRUE &&
        GetDataObjectFormatCount(aOleDataObject) >= 3)
    {
        return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t DoPasteInetUrl(LPDATAOBJECT aDataObject, xpr_tchar_t *aDir, CLIPFORMAT aInetUrlClipFormat, CLIPFORMAT aFileContentsClipFormat, CLIPFORMAT aFileDescriptorClipFormat, xpr_bool_t aUnicode)
{
    if (XPR_IS_NULL(aDataObject) || XPR_IS_NULL(aDir))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    FORMATETC sInetUrlFormatEtc    = { aInetUrlClipFormat,        XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    FORMATETC sDescriptorFormatEtc = { aFileDescriptorClipFormat, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    FORMATETC sContentsFormatEtc   = { aFileContentsClipFormat,   XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    STGMEDIUM sInetUrlStgMedium = {0};
    STGMEDIUM sDescriptorStgMedium = {0};

    HRESULT sHResult, sHResult2;
    sHResult  = aDataObject->GetData(&sInetUrlFormatEtc, &sInetUrlStgMedium);
    sHResult2 = aDataObject->GetData(&sDescriptorFormatEtc, &sDescriptorStgMedium);

    if (SUCCEEDED(sHResult) && SUCCEEDED(sHResult2) && XPR_IS_NOT_NULL(sInetUrlStgMedium.hGlobal) && XPR_IS_NOT_NULL(sDescriptorStgMedium.hGlobal))
    {
        xpr_tchar_t *sUrl;
        FILEDESCRIPTORA *sFileDescA = XPR_NULL;
        FILEDESCRIPTORW *sFileDescW = XPR_NULL;
        FILEGROUPDESCRIPTORA *sFileGroupDescA = XPR_NULL;
        FILEGROUPDESCRIPTORW *sFileGroupDescW = XPR_NULL;

        sUrl = (xpr_tchar_t *)::GlobalLock(sInetUrlStgMedium.hGlobal);

        if (XPR_IS_TRUE(aUnicode))
            sFileGroupDescW = (FILEGROUPDESCRIPTORW *)::GlobalLock(sDescriptorStgMedium.hGlobal);
        else
            sFileGroupDescA = (FILEGROUPDESCRIPTORA *)::GlobalLock(sDescriptorStgMedium.hGlobal);

        if (XPR_IS_NOT_NULL(sUrl) && (XPR_IS_NOT_NULL(sFileGroupDescA) || XPR_IS_NOT_NULL(sFileGroupDescW)))
        {
            RemoveLastSplit(aDir);

            FILE *sFile;
            xpr_char_t *aText;
            STGMEDIUM sStgMedium = {0};

            xpr_sint_t i, sCount;
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
            xpr_tchar_t sExt[XPR_MAX_PATH + 1] = {0};
            xpr_size_t sOutputBytes;

            sCount = XPR_IS_TRUE(aUnicode) ? sFileGroupDescW->cItems : sFileGroupDescA->cItems;
            for (i = 0; i < sCount; ++i)
            {
                if (XPR_IS_TRUE(aUnicode))
                    sFileDescW = &sFileGroupDescW->fgd[i];
                else
                    sFileDescA = &sFileGroupDescA->fgd[i];

                sContentsFormatEtc.lindex = i;

                sHResult = aDataObject->GetData(&sContentsFormatEtc, &sStgMedium);
                if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sStgMedium.hGlobal))
                {
                    if (XPR_IS_TRUE(aUnicode))
                    {
                        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                        XPR_UTF16_TO_TCS(sFileDescW->cFileName, wcslen(sFileDescW->cFileName) * sizeof(xpr_wchar_t), sFileName, &sOutputBytes);
                        sFileName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
                    }
                    else
                    {
                        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                        XPR_MBS_TO_TCS(sFileDescA->cFileName, strlen(sFileDescA->cFileName) * sizeof(xpr_char_t), sFileName, &sOutputBytes);
                        sFileName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
                    }

                    SplitFileNameExt(sFileName, sFileName, sExt);

                    SetNewPath(sPath, aDir, sFileName, sExt);

                    if (sStgMedium.tymed == TYMED_HGLOBAL)
                    {
                        aText = (xpr_char_t *)::GlobalLock(sStgMedium.hGlobal);
                        if (XPR_IS_NOT_NULL(aText))
                        {
                            sFile = _tfopen(sPath, XPR_STRING_LITERAL("wb"));
                            if (XPR_IS_NOT_NULL(sFile))
                            {
                                xpr_sint_t sLen = XPR_IS_TRUE(aUnicode) ? sFileDescW->nFileSizeLow : sFileDescA->nFileSizeLow;
                                if (sLen == 0)
                                    sLen = (xpr_sint_t)::GlobalSize(sStgMedium.hGlobal);

                                fwrite(aText, sLen, 1, sFile);
                                fclose(sFile);

                                ::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, sPath, XPR_NULL);
                                sResult = XPR_TRUE;
                            }

                            ::GlobalUnlock(sStgMedium.hGlobal);
                        }
                    }
                    else if (sStgMedium.tymed == TYMED_ISTREAM)
                    {
                        sHResult = StreamToFile(sStgMedium.pstm, sPath);
                        if (SUCCEEDED(sHResult))
                        {
                            ::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, sPath, XPR_NULL);
                            sResult = XPR_TRUE;
                        }
                    }
                }
            }
        }

        if (XPR_IS_NOT_NULL(sUrl))
            ::GlobalUnlock(sInetUrlStgMedium.hGlobal);

        if (XPR_IS_NOT_NULL(sFileGroupDescA) || XPR_IS_NOT_NULL(sFileGroupDescW))
            ::GlobalFree(sDescriptorStgMedium.hGlobal);
    }

    return sResult;
}

xpr_bool_t IsPasteFileContents(COleDataObject *aOleDataObject, CLIPFORMAT aFileContentsClipFormat, CLIPFORMAT aFileDescriptorClipFormat)
{
    if (XPR_IS_NULL(aOleDataObject))
        return XPR_FALSE;

    if (aOleDataObject->IsDataAvailable((CLIPFORMAT)aFileContentsClipFormat)   == XPR_TRUE &&
        aOleDataObject->IsDataAvailable((CLIPFORMAT)aFileDescriptorClipFormat) == XPR_TRUE &&
        GetDataObjectFormatCount(aOleDataObject) >= 2)
    {
        return XPR_TRUE;
    }

    return XPR_FALSE;
}

//
// outlook
//
xpr_bool_t DoPasteFileContents(LPDATAOBJECT aDataObject, xpr_tchar_t *aDir, CLIPFORMAT aFileContentsClipFormat, CLIPFORMAT aFileDescriptorClipFormat, xpr_bool_t aUnicode)
{
    if (XPR_IS_NULL(aDataObject) || XPR_IS_NULL(aDir))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    // CFSTR_FILEDESCRIPTORW("FileGroupDescriptorW") - Not Supported in Unicode
    // CFSTR_FILEDESCRIPTOR("FileGroupDescriptor") - Only Supported in Ansi/Unicode

    FILEDESCRIPTORA *sFileDescA = XPR_NULL;
    FILEDESCRIPTORW *sFileDescW = XPR_NULL;
    FILEGROUPDESCRIPTORA *sFileGroupDescA = XPR_NULL;
    FILEGROUPDESCRIPTORW *sFileGroupDescW = XPR_NULL;

    FORMATETC sDescriptorFormatEtc = { aFileDescriptorClipFormat, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    FORMATETC sContentsFormatEtc   = { aFileContentsClipFormat,   XPR_NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM | TYMED_ISTORAGE };

    HRESULT sHResult;
    STGMEDIUM sStgMedium = {0};
    sHResult = aDataObject->GetData(&sDescriptorFormatEtc, &sStgMedium);

    if (XPR_IS_TRUE(aUnicode))
        sFileGroupDescW = (FILEGROUPDESCRIPTORW *)::GlobalLock(sStgMedium.hGlobal);
    else
        sFileGroupDescA = (FILEGROUPDESCRIPTORA *)::GlobalLock(sStgMedium.hGlobal);

    if ((SUCCEEDED(sHResult)) && (XPR_IS_NOT_NULL(sFileGroupDescA) || XPR_IS_NOT_NULL(sFileGroupDescW)))
    {
        RemoveLastSplit(aDir);

        xpr_sint_t i, sCount;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sExt[XPR_MAX_PATH + 1] = {0};
        xpr_size_t sOutputBytes;

        sCount = XPR_IS_TRUE(aUnicode) ? sFileGroupDescW->cItems : sFileGroupDescA->cItems;
        for (i = 0; i < sCount; ++i)
        {
            if (XPR_IS_TRUE(aUnicode))
                sFileDescW = &sFileGroupDescW->fgd[i];
            else
                sFileDescA = &sFileGroupDescA->fgd[i];

            sContentsFormatEtc.lindex = i;

            sHResult = aDataObject->GetData(&sContentsFormatEtc, &sStgMedium);
            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sStgMedium.pstm))
            {
                if (XPR_IS_TRUE(aUnicode))
                {
                    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                    XPR_UTF16_TO_TCS(sFileDescW->cFileName, wcslen(sFileDescW->cFileName) * sizeof(xpr_wchar_t), sFileName, &sOutputBytes);
                    sFileName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
                }
                else
                {
                    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                    XPR_MBS_TO_TCS(sFileDescA->cFileName, strlen(sFileDescA->cFileName) * sizeof(xpr_char_t), sFileName, &sOutputBytes);
                    sFileName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
                }

                SplitFileNameExt(sFileName, sFileName, sExt);

                SetNewPath(sPath, aDir, sFileName, sExt);

                switch (sStgMedium.tymed)
                {
                case TYMED_ISTORAGE:
                    {
                        sHResult = StorageToFile(sStgMedium.pstg, sPath);
                        break;
                    }

                case TYMED_ISTREAM:
                    {
                        sHResult = StreamToFile(sStgMedium.pstm, sPath);
                        break;
                    }
                }

                if (SUCCEEDED(sHResult))
                {
                    ::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, sPath, XPR_NULL);
                    sResult = XPR_TRUE;
                }
            }
        }
    }

    if (XPR_IS_NOT_NULL(sStgMedium.hGlobal))
        ::GlobalUnlock(sStgMedium.hGlobal);

    return sResult;
}

void SetTextClipboard(HWND aHwndOwner, const xpr_tchar_t *aText)
{
#ifdef _UNICODE
    SetTextClipboardW(aHwndOwner, aText);
#else
    SetTextClipboardA(aHwndOwner, aText);
#endif
}

void SetTextClipboardA(HWND aHwndOwner, const xpr_char_t *aText)
{
    if (XPR_IS_NULL(aText))
        return;

    if (!::OpenClipboard(aHwndOwner))
        return;

    xpr_sint_t sSize = (xpr_sint_t)strlen(aText) + 1;

    HANDLE sGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sSize);
    if (XPR_IS_NOT_NULL(sGlobal))
    {
        xpr_byte_t *sData = (xpr_byte_t *)::GlobalLock(sGlobal);
        if (XPR_IS_NOT_NULL(sData))
        {
            ::EmptyClipboard();

            memcpy(sData, aText, sSize);

            ::GlobalUnlock(sGlobal);
            ::SetClipboardData(CF_TEXT, sGlobal);
        }
        else
        {
            ::GlobalFree(sGlobal);
        }
    }

    ::CloseClipboard();
}

void SetTextClipboardW(HWND aHwndOwner, const xpr_tchar_t *aText)
{
    if (XPR_IS_NULL(aText))
        return;

    if (::OpenClipboard(aHwndOwner) == XPR_FALSE)
        return;

    xpr_sint_t sSize = (xpr_sint_t)((_tcslen(aText) + 1) * sizeof(xpr_wchar_t));

    HANDLE sGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sSize);
    if (XPR_IS_NOT_NULL(sGlobal))
    {
        xpr_byte_t *sData = (xpr_byte_t *)::GlobalLock(sGlobal);
        if (XPR_IS_NOT_NULL(sData))
        {
            ::EmptyClipboard();

            memcpy(sData, aText, sSize);

            ::GlobalUnlock(sGlobal);
            ::SetClipboardData(CF_UNICODETEXT, sGlobal);
        }
        else
        {
            ::GlobalFree(sGlobal);
        }
    }

    ::CloseClipboard();
}
} // namespace fxb
