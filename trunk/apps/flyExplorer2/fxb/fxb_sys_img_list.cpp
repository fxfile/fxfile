//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_sys_img_list.h"

#include "Option.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
//#include <commoncontrols.h>
//#include <shellapi.h>
//#if (NTDDI_VERSION >= NTDDI_WINXP)
//#define SHIL_LARGE          0   // normally 32x32
//#define SHIL_SMALL          1   // normally 16x16
//#define SHIL_EXTRALARGE     2
//#define SHIL_SYSSMALL       3   // like SHIL_SMALL, but tracks system small icon metric correctly
//#if (NTDDI_VERSION >= NTDDI_LONGHORN)
//#define SHIL_JUMBO          4   // normally 256x256
//#define SHIL_LAST           SHIL_JUMBO
//#else
//#define SHIL_LAST           SHIL_SYSSMALL
//#endif // (NTDDI_VERSION >= NTDDI_LONGHORN)
//#endif // (NTDDI_VERSION >= NTDDI_WINXP)

typedef WINSHELLAPI xpr_bool_t (WINAPI *Shell_GetImageListsFunc)(HIMAGELIST *aLargeImageList, HIMAGELIST *aSmallImageList);
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
    case ImgSizeSmall: SHGetImageLists(XPR_NULL, &sSysImgList); break;
    case ImgSizeLarge: SHGetImageLists(&sSysImgList, XPR_NULL); break;
    case ImgSizeExtra: break;
    case ImgSizeJumbo: break;
    }

    if (XPR_IS_NULL(sSysImgList))
        return;

    Attach(sSysImgList);
}

//HIMAGELIST SysImgList::SHGetImageList()
//{
//    if (bLarge)
//    {
//        // Get the 48x48 system image list. 
//        HRESULT hr, (WINAPI* pfnGetImageList)(xpr_sint_t, REFIID, void**) = XPR_NULL;
//        HMODULE hmod = GetModuleHandle ( XPR_STRING_LITERAL("shell32") );
//
//        if ( XPR_NULL != hmod )
//            (FARPROC&) pfnGetImageList = GetProcAddress ( hmod, "SHGetImageList" );
//
//        if ( XPR_NULL == pfnGetImageList )
//            (FARPROC&) pfnGetImageList = GetProcAddress ( hmod, MAKEINTRESOURCEA(727) );  // see Q316931
//
//        if ( XPR_NULL != pfnGetImageList )
//        {
//
//            hSysImgList = XPR_NULL;
//            hr = pfnGetImageList ( SHIL_EXTRALARGE, IID_IImageList, (void**) &hSysImgList );
//
//            if ( SUCCEEDED(hr) )
//            {
//
//                // HIMAGELIST and IImageList* are interchangeable, so this cast is kosher.
//                //hSysImgList = (HIMAGELIST)(IImageList*) m_TileIml;
//            }
//        }
//    }
//}

xpr_bool_t SysImgList::SHGetImageLists(HIMAGELIST *aLargeImageList, HIMAGELIST *aSmallImageList)
{
    xpr_bool_t sResult = XPR_FALSE;
    HINSTANCE sDll;
    Shell_GetImageListsFunc sShell_GetImageListsFunc;

    sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        sShell_GetImageListsFunc = (Shell_GetImageListsFunc)::GetProcAddress(sDll, (const xpr_char_t *)71);
        if (XPR_IS_NOT_NULL(sShell_GetImageListsFunc))
            sResult = sShell_GetImageListsFunc(aLargeImageList, aSmallImageList);
        ::FreeLibrary(sDll);
    }

    // Initializes or reinitializes the system image list.
    //if (XPR_IS_NOT_NULL(aSmallImageList))
    //{
    //    FileIconInitFunc sFileIconInitFunc;
    //        sFileIconInitFunc = (FileIconInitFunc)    GetProcAddress(sDll, (const xpr_char_t *)660);
    //    if (XPR_IS_NOT_NULL(sFileIconInitFunc))
    //        sFileIconInitFunc(XPR_TRUE);
    //}

    return sResult;
}

void SysImgList::release(void)
{
    Detach();
}

SysImgListMgr::SysImgListMgr(void)
{
}

SysImgListMgr::~SysImgListMgr(void)
{
}

void SysImgListMgr::getSystemImgList(void)
{
    mSysImgList32.init(SysImgList::ImgSizeLarge);
    mSysImgList16.init(SysImgList::ImgSizeSmall);
}

void SysImgListMgr::getCustomImgList(void)
{
    CImageList *sImgList[] = { &mCusImgList16, &mCusImgList32, XPR_NULL };
    CSize sIconSize[] = { CSize(16,16), CSize(32,32), CSize(0,0) };

    xpr_sint_t i;
    std::tstring strPath;

    for (i = 0; sImgList[i]; ++i)
    {
        strPath = gOpt->mExplorerCustomIconFile[i];

        if (!strPath.empty() && strPath[0] == XPR_STRING_LITERAL('%'))
        {
            strPath.clear();
            GetEnvRealPath(gOpt->mExplorerCustomIconFile[i], strPath);
        }

        LoadImgList(sImgList[i], strPath.c_str(), sIconSize[i], 3);
    }
}

//SysImgList *SysImgListMgr::GetSysImgList(SysImgList::ImgSize nImgSize)
//{
//    switch (nImgSize)
//    {
//    case SysImgList::ImgSizeSmall: return &mSysImgList16;
//    case SysImgList::ImgSizeLarge: return &mSysImgList32;
//    }
//
//    return XPR_NULL;
//}
//
//SysImgList *SysImgListMgr::GetCusImgList(SysImgList::ImgSize nImgSize)
//{
//    switch (nImgSize)
//    {
//    case SysImgList::ImgSizeSmall: return &mCusImgList16;
//    case SysImgList::ImgSizeLarge: return &mCusImgList32;
//    }
//
//    return XPR_NULL;
//}
} // namespace fxb
