//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_SHELL_ITEM_H__
#define __FXFILE_BASE_SHELL_ITEM_H__ 1
#pragma once

namespace fxfile
{
namespace base
{
class ShellItem
{
public:
#if defined(XPR_CFG_OS_WINDOWS)
    enum
    {
        SpecialFolderNone,
        SpecialFolderDesktop,
        SpecialFolderWindows,
        SpecialFolderWindowsDrive,
        SpecialFolderWindowsSystem32,
        SpecialFolderProgramFiles,
        SpecialFolderControlPanels,
        SpecialFolderMyDocuments,
        SpecialFolderMyPictures,
        SpecialFolderMyMusic,
        SpecialFolderMyVideos,
        SpecialFolderLibraries,
        SpecialFolderLibraryDocuments,
        SpecialFolderLibraryPictures,
        SpecialFolderLibraryMusic,
        SpecialFolderLibraryVideos,
        // 0x80000000 ~ 0x8000ffff               : reserved CSIDL value in <shlobj.h>
    };
#else // not XPR_CFG_OS_WINDOWS
#error Please, support your OS. <shell_item.h>
#endif

public:
    ShellItem(void);
    explicit ShellItem(ShellItem &aShellItem);
    explicit ShellItem(xpr_sint_t aSpecialFolder);
    explicit ShellItem(const xpr_tchar_t *aFilePath);
    virtual ~ShellItem(void);

public:
    void reset(void);

public:
    xpr_bool_t setShellItem(const ShellItem &aShellItem);
    xpr_bool_t setFilePath(const xpr_tchar_t *aPath);
    xpr_bool_t setSpecialFolder(xpr_sint_t aSpecialFolder);

    xpr_bool_t getFilePath(xpr_tchar_t *aPath, xpr_size_t aMaxLen) const;
    xpr_bool_t isFileSystem(void) const;
    xpr_bool_t isFileSystemFolder(void) const;

protected:
#if defined(XPR_CFG_OS_WINDOWS)
    LPSHELLFOLDER mShellFolder;
    LPITEMIDLIST  mPidl;
#else // not XPR_CFG_OS_WINDOWS
#error Please, support your OS. <shell_item.h>
#endif // XPR_CFG_OS_WINDOWS
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_SHELL_ITEM_H__
