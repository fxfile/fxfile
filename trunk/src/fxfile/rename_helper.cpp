//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "rename_helper.h"
#include "file_op_undo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
RenameHelper::RenameHelper(RenameStyle aRenameStyle)
    : mRenameStyle(aRenameStyle)
    , mSelItem0(0)
    , mSelItem1(-1)
    , mShellFolder(XPR_NULL)
    , mPidl(XPR_NULL)
    , mFullPidl(XPR_NULL)
    , mNewPidl(XPR_NULL)
    , mNewFullPidl(XPR_NULL)
    , mShellAttributes(0)
{
}

RenameHelper::~RenameHelper(void)
{
    COM_FREE(mFullPidl);
    COM_FREE(mNewPidl);
    COM_FREE(mNewFullPidl);
}

void RenameHelper::setRenameStyle(RenameStyle eRenameStyle)
{
    mRenameStyle = eRenameStyle;
}

void RenameHelper::setItem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, xpr_ulong_t aShellAttributes)
{
    mShellFolder     = aShellFolder;
    mPidl            = aPidl;
    mFullPidl        = fxfile::base::Pidl::clone(aFullPidl);
    mShellAttributes = aShellAttributes;

    mEditName.clear();

    xpr_bool_t sFileSystem = XPR_TEST_BITS(mShellAttributes, SFGAO_FILESYSTEM);
    xpr_bool_t sFolder     = XPR_TEST_BITS(mShellAttributes, SFGAO_FOLDER);

    if (XPR_IS_TRUE(sFileSystem) && XPR_IS_FALSE(sFolder))
        GetName(mShellFolder, mPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, mEditName);
    else
        GetName(mShellFolder, mPidl, SHGDN_INFOLDER | SHGDN_FOREDITING, mEditName);

    if (XPR_IS_FALSE(sFolder))
    {
        if (XPR_IS_TRUE(sFileSystem) && mRenameStyle != RenameStyleNone)
        {
            xpr_tchar_t sFullName[XPR_MAX_PATH + 1] = {0};
            GetName(mShellFolder, mPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sFullName);

            if (_tcsicmp(mEditName.c_str(), sFullName) == 0)
            {
                xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(mEditName.c_str());
                if (XPR_IS_NOT_NULL(sExt))
                {
                    if (mRenameStyle == RenameStyleKeepExt)
                    {
                        mKeepExt = sExt;
                        sExt[0] = '\0';
                    }
                    else if (mRenameStyle == RenameStyleSelExceptForExt)
                    {
                        mSelItem1 = (xpr_sint_t)(sExt - mEditName.c_str());
                    }
                }
            }
        }
    }
}

const xpr_tchar_t *RenameHelper::getEditName(void)
{
    return mEditName.c_str();
}

void RenameHelper::getEditSel(xpr_sint_t *aSelItem0, xpr_sint_t *aSelItem1)
{
    if (XPR_IS_NOT_NULL(aSelItem0)) *aSelItem0 = mSelItem0;
    if (XPR_IS_NOT_NULL(aSelItem1)) *aSelItem1 = mSelItem1;
}

xpr_sint_t RenameHelper::getIconIndex(void)
{
    return GetItemIconIndex(mShellFolder, mPidl);
}

RenameHelper::Result RenameHelper::rename(HWND aHwnd, const xpr_tchar_t *aNewName)
{
    if (XPR_IS_NULL(aNewName))
        return ResultInvalidParameter;

    mNewName = aNewName;

    if (mNewName.empty() == XPR_TRUE)
        return ResultSucceeded;

    xpr_size_t nNewNameLen = mNewName.length();
    if (mNewName[nNewNameLen-1] == '.')
        mNewName.erase(nNewNameLen-1);

    mNewName += mKeepExt.c_str();

    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;
    xpr_wchar_t sUtf16NewName[XPR_MAX_PATH + 1] = {0};
    sInputBytes = mNewName.length() * sizeof(xpr_tchar_t);
    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
    XPR_TCS_TO_UTF16(mNewName.c_str(), &sInputBytes, sUtf16NewName, &sOutputBytes);
    sUtf16NewName[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

    xpr::tstring sOldPath;
    xpr::tstring sNewPath;
    GetName(mShellFolder, mPidl, SHGDN_FORPARSING, sOldPath);
    GetName(mShellFolder, mPidl, SHGDN_FORPARSING, sNewPath);

    xpr_size_t sFind = sNewPath.rfind(XPR_STRING_LITERAL('\\'));
    if (sFind != xpr::tstring::npos)
    {
        sNewPath.erase(sFind+1);

        sNewPath += mNewName;
        sNewPath += mParsingExt;
    }

    HRESULT sHResult = E_FAIL;

    xpr_bool_t sFileSystem =  XPR_TEST_BITS(mShellAttributes, SFGAO_FILESYSTEM);
    xpr_bool_t sFile       = !XPR_TEST_BITS(mShellAttributes, SFGAO_FOLDER);

    // [remark]
    // Some file name can rename using MoveFile API function, but it can not rename using IShellFolder::SetNameOf method.
    // This result code of IShellFolder::SetNameOf is 0x800704c7.
    // This bug is confirmed to WinXP sp2 or higher
    // ex) 'test (test,1981) - test.ogg' -> '@test (test,1981) - test.ogg'

    xpr_bool_t sMoveFile = XPR_FALSE;
    if (XPR_IS_TRUE(sFileSystem))
    {
        if (VerifyFileName(mNewName) == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.rename.msg.wrong_filename"));
            CWnd::FromHandle(aHwnd)->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            return ResultInvalidName;
        }

        xpr_sint_t sMsgId;
        DWORD sFileAttributes = ::GetFileAttributes(sOldPath.c_str());

        if (sOldPath.length() <= 3)
        {
            xpr_tchar_t sMsg[XPR_MAX_PATH * 3 + 1] = {0};
            _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.rename.msg.confirm_drive_rename"), XPR_STRING_LITERAL("%c,%s,%s")), sOldPath[0], mEditName.c_str(), mNewName.c_str());
            sMsgId = ::MessageBox(aHwnd, sMsg, gApp.loadString(XPR_STRING_LITERAL("popup.rename.msg.title")), MB_YESNO | MB_ICONQUESTION);
            if (sMsgId == IDNO)
                return ResultUserCancel;
        }
        else if (XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_SYSTEM) || XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_READONLY))
        {
            xpr::tstring sAttributes;
            if (XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_READONLY))
                sAttributes = gApp.loadString(XPR_STRING_LITERAL("popup.rename.msg.confirm_file_rename.read_only_file"));
            else
                sAttributes = gApp.loadString(XPR_STRING_LITERAL("popup.rename.msg.confirm_file_rename.system_file"));

            xpr::tstring sOldFileName = sOldPath.substr(sOldPath.rfind(XPR_STRING_LITERAL('\\'))+1);
            xpr::tstring sNewFileName = sNewPath.substr(sNewPath.rfind(XPR_STRING_LITERAL('\\'))+1);

            xpr_tchar_t sMsg[XPR_MAX_PATH * 3 + 1] = {0};
            _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.rename.msg.confirm_file_rename"), XPR_STRING_LITERAL("%s,%s,%s")), sOldFileName.c_str(), sAttributes.c_str(), sNewFileName.c_str());
            sMsgId = ::MessageBox(aHwnd, sMsg, gApp.loadString(XPR_STRING_LITERAL("popup.rename.msg.title")), MB_YESNO | MB_ICONQUESTION);
            if (sMsgId == IDNO)
                return ResultUserCancel;
        }

        if (::MoveFile(sOldPath.c_str(), sNewPath.c_str()) == XPR_TRUE)
        {
            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sNewPath.c_str());
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                LPSHELLFOLDER sShellFolder = XPR_NULL;
                LPCITEMIDLIST sPidl        = XPR_NULL;
                fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);

                mNewPidl = fxfile::base::Pidl::clone(sPidl);

                COM_RELEASE(sShellFolder);
                COM_FREE(sFullPidl);
            }

            sMoveFile = XPR_TRUE;
            sHResult = S_OK;
        }
    }

    if (FAILED(sHResult))
    {
        xpr_uint_t sFlags = SHGDN_INFOLDER;
        if (XPR_IS_TRUE(sFileSystem) && XPR_IS_TRUE(sFile))
            sFlags |= SHGDN_FORPARSING;

        sHResult = mShellFolder->SetNameOf(aHwnd, mPidl, sUtf16NewName, sFlags, &mNewPidl);
    }

    if (FAILED(sHResult))
        return ResultUnknownError;

    mNewFullPidl = fxfile::base::Pidl::getFullPidl(mShellFolder, mNewPidl);

    xpr_slong_t sEventId = SHCNE_RENAMEITEM;
    xpr_uint_t sFlags = SHCNF_IDLIST;
    const void *sItem1 = mPidl;
    const void *sItem2 = mNewPidl;

    if (XPR_IS_FALSE(sFile))
        sEventId = SHCNE_RENAMEFOLDER;

    if (XPR_IS_TRUE(sMoveFile))
    {
        sFlags = SHCNF_PATH;
        sItem1 = sOldPath.c_str();
        sItem2 = sNewPath.c_str();
    }

    sFlags |= SHCNF_FLUSH;
    ::SHChangeNotify(sEventId, sFlags, sItem1, sItem2);

    if (XPR_IS_TRUE(sFileSystem))
    {
        FileOpUndo sFileOpUndo;
        sFileOpUndo.addRename(sOldPath, sNewPath);
    }

    return ResultSucceeded;
}

LPITEMIDLIST RenameHelper::getNewItemIDList(xpr_bool_t aNull)
{
    LPITEMIDLIST sNewPidl = mNewPidl;

    if (XPR_IS_TRUE(aNull))
        mNewPidl = XPR_NULL;

    return sNewPidl;
}

const xpr_tchar_t *RenameHelper::getNewName(void)
{
    return mNewName.c_str();
}
} // namespace fxfile
