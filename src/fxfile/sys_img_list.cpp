//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "sys_img_list.h"

#include "gui/gdi.h"

#include <commoncontrols.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if (NTDDI_VERSION >= NTDDI_WINXP)
#define SHIL_LARGE          0   // normally 32x32
#define SHIL_SMALL          1   // normally 16x16
#define SHIL_EXTRALARGE     2
#define SHIL_SYSSMALL       3   // like SHIL_SMALL, but tracks system small icon metric correctly
#if (NTDDI_VERSION < NTDDI_LONGHORN)
#define SHIL_JUMBO          4   // normally 256x256
#define SHIL_LAST           SHIL_JUMBO
#else
#define SHIL_LAST           SHIL_SYSSMALL
#endif // (NTDDI_VERSION >= NTDDI_LONGHORN)
#endif // (NTDDI_VERSION < NTDDI_WINXP)

namespace fxfile
{
typedef WINSHELLAPI xpr_bool_t (WINAPI *Shell_GetImageListsFunc)(HIMAGELIST *aLargeImageList, HIMAGELIST *aSmallImageList);
typedef WINSHELLAPI HRESULT (WINAPI *SHGetImageListFunc)(xpr_sint_t, REFIID, void**);
typedef xpr_bool_t (WINAPI *FileIconInitFunc)(xpr_bool_t fFullInit);

SysImgList::SysImgList(void)
{
}

SysImgList::~SysImgList(void)
{
    Detach();
}

void SysImgList::init(ImgSize aImgSize)
{
    HIMAGELIST sSysImgList = XPR_NULL;

    switch (aImgSize)
    {
    case ImgSizeSmall: sSysImgList = SHGetImageList(SHIL_SYSSMALL  ); break;
    case ImgSizeLarge: sSysImgList = SHGetImageList(SHIL_LARGE     ); break;
    case ImgSizeExtra: sSysImgList = SHGetImageList(SHIL_EXTRALARGE); break;
    case ImgSizeJumbo: sSysImgList = SHGetImageList(SHIL_JUMBO     ); break;
    }

    if (XPR_IS_NULL(sSysImgList))
        return;

    Attach(sSysImgList);
}

HIMAGELIST SysImgList::SHGetImageList(xpr_sint_t aImageSize)
{
    HIMAGELIST sSysImgList = XPR_NULL;

    HMODULE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        HRESULT sHResult;
        SHGetImageListFunc sSHGetImageListFunc;

        sSHGetImageListFunc = (SHGetImageListFunc)GetProcAddress(sDll, "SHGetImageList");
        if (XPR_IS_NULL(sSHGetImageListFunc))
            sSHGetImageListFunc = (SHGetImageListFunc)GetProcAddress(sDll, MAKEINTRESOURCEA(727));  // see Q316931

        if (XPR_IS_NOT_NULL(sSHGetImageListFunc))
        {
            sHResult = sSHGetImageListFunc(aImageSize, IID_IImageList2, (void**)&sSysImgList);
            if (FAILED(sHResult))
            {
                sHResult = sSHGetImageListFunc(aImageSize, IID_IImageList, (void**)&sSysImgList);
                if (FAILED(sHResult))
                {
                    if (aImageSize == SHIL_SMALL || aImageSize == SHIL_LARGE || aImageSize == SHIL_SYSSMALL)
                    {
                        Shell_GetImageListsFunc sShell_GetImageListsFunc = (Shell_GetImageListsFunc)::GetProcAddress(sDll, (const xpr_char_t *)71);
                        if (XPR_IS_NOT_NULL(sShell_GetImageListsFunc))
                        {
                            xpr_bool_t sResult = sShell_GetImageListsFunc(
                                (aImageSize == SHIL_LARGE) ? &sSysImgList : XPR_NULL,
                                (aImageSize == SHIL_LARGE) ? XPR_NULL : &sSysImgList);
                        }
                    }
                }
            }
        }

        ::FreeLibrary(sDll);
        sDll = XPR_NULL;
    }

    return sSysImgList;
}

void SysImgList::release(void)
{
    Detach();
}
} // namespace fxfile
