//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_scrap_mgr.h"

#include "fxb_file_scrap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
FileScrapMgr::FileScrapMgr(void)
{
}

FileScrapMgr::~FileScrapMgr(void)
{
}

void FileScrapMgr::add(LPSHELLFOLDER aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t aCount)
{
    xpr_uint_t sGroupId = FileScrap::instance().getCurGroupId();
    add(sGroupId, aShellFolder, aPidls, aCount);
}

void FileScrapMgr::add(xpr_uint_t aGroupId, LPSHELLFOLDER aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t aCount)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_sint_t i;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    LPITEMIDLIST sPidl = XPR_NULL;
    xpr_ulong_t sAttributes;

    for (i = 0; i < aCount; ++i)
    {
        sPidl = aPidls[i];

        sAttributes = SFGAO_FILESYSTEM;
        GetItemAttributes(aShellFolder, sPidl, sAttributes);

        if (!XPR_TEST_BITS(sAttributes, SFGAO_FILESYSTEM))
            continue;

        fxb::GetName(aShellFolder, sPidl, SHGDN_FORPARSING, sPath);

        sFileScrap.addItem(aGroupId, sPath);
    }
}

void FileScrapMgr::addPath(const xpr_tchar_t *aPath, xpr_sint_t aCount)
{
    xpr_uint_t sGroupId = FileScrap::instance().getCurGroupId();
    addPath(sGroupId, aPath, aCount);
}

void FileScrapMgr::addPath(xpr_uint_t aGroupId, const xpr_tchar_t *aPath, xpr_sint_t aCount)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_sint_t i;
    xpr_tchar_t *sPathEnum = (xpr_tchar_t *)aPath;

    for (i = 0; i < aCount; ++i)
    {
        sFileScrap.addItem(aGroupId, sPathEnum);
        sPathEnum += _tcslen(sPathEnum) + 1;
    }
}

void FileScrapMgr::doCopyOperation(const xpr_tchar_t *aTargetDir)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    sFileScrap.copyOperation(sGroupId, aTargetDir);
}

void FileScrapMgr::doMoveOperation(const xpr_tchar_t *aTargetDir)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    sFileScrap.moveOperation(sGroupId, aTargetDir);
}

void FileScrapMgr::doDeleteOperation(void)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.confirm_delete_or_trash"));
    xpr_sint_t sMsgId = AfxGetMainWnd()->MessageBox(sMsg, XPR_NULL, MB_OK | MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (sMsgId == IDYES)
    {
        sFileScrap.deleteOperation(sGroupId);
    }
    else if (sMsgId == IDNO)
    {
        sFileScrap.recycleBinOperation(sGroupId);
    }
}

void FileScrapMgr::removeList(void)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    sFileScrap.removeAllItems(sGroupId);
}

void FileScrapMgr::cutToClipboard(void)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    sFileScrap.setClipboardCut(sGroupId);
}

void FileScrapMgr::copyToClipboard(void)
{
    FileScrap &sFileScrap = FileScrap::instance();

    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    sFileScrap.setClipboardCopy(sGroupId);
}
} // namespace fxb
