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

#include "xpr_file_path.h"

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

    enum
    {
        AttrbitueNone         = 0x00000000,
        AttributeFileSystem   = 0x00000001,
        AttributeHidden       = 0x00000002,
        AttributeReadOnly     = 0x00000004,
        AttributeSystem       = 0x00000008,

        AttributeCanCopy      = 0x00000100,
        AttributeCanMove      = 0x00000200,
        AttributeCanDelete    = 0x00000400,
        AttributeCanRename    = 0x00000800,
        AttributeCanLink      = 0x00001000,

        AttributeCompressed   = 0x00010000,
        AttributeEncrypted    = 0x00020000,
        AttributeShared       = 0x00040000,
    };

    enum
    {
        FileTypeUnknown,
        FileTypeRegular,
        FileTypeDirectory,
        FileTypeCharacterDevice,
        FileTypeBlockDevice,
        FileTypeNamedPipe,
        FileTypeSymblicLink,
        FileTypeSocket,
        FileTypeShortcut = 100, // only for Microsoft Windows
    };

    typedef xpr_uint_t Attribute;
    typedef xpr_uint_t FileType;

public:
    ShellItem(void);
    explicit ShellItem(ShellItem &aShellItem);
    explicit ShellItem(xpr_sint_t aSpecialFolder);
    explicit ShellItem(const xpr_tchar_t *aFilePath);
    explicit ShellItem(const xpr::string &aFilePath);
    explicit ShellItem(const xpr::FilePath &aFilePath);
    virtual ~ShellItem(void);

public:
    void clear(void);

public:
    xpr_bool_t assign(const ShellItem &aShellItem);
    xpr_bool_t assign(const xpr_tchar_t *aFilePath);
    xpr_bool_t assign(const xpr::string &aFilePath);
    xpr_bool_t assign(const xpr::FilePath &aFilePath);
    xpr_bool_t assign(xpr_sint_t aSpecialFolder);

//    xpr_bool_t clone(ShellItem &aShellItem) const;
//
//    void       swap(ShellItem &aShellItem);
//
//    xpr_bool_t rename(const xpr_tchar_t *aName) const;
//    xpr_bool_t rename(const xpr::string &aName) const;
//
//    xpr_bool_t getName(xpr_tchar_t *aName, xpr_size_t aMaxLen, xpr_bool_t aForEditing = XPR_FALSE) const;
//    xpr_bool_t getName(xpr::string &aName, xpr_bool_t aForEditing = XPR_FALSE) const;
//    xpr_bool_t getExt(xpr_tchar_t *aExt, xpr_size_t aMaxLen) const;
//    xpr_bool_t getExt(xpr::string &aExt) const;
//    xpr_bool_t getPath(xpr_tchar_t *aPath, xpr_size_t aMaxLen) const;
//    xpr_bool_t getPath(xpr::string &aPath) const;
//    xpr_bool_t getLastPath(xpr_tchar_t *aPath, xpr_size_t aMaxLen) const;
//    xpr_bool_t getLastPath(xpr::string &aPath) const;
//    xpr_bool_t getTreePath(xpr_tchar_t *aTreePath, xpr_size_t aMaxLen, xpr_bool_t aByName = XPR_FALSE) const;
//    xpr_bool_t getTreePath(const xpr::string &aTreePath, xpr_bool_t aByName = XPR_FALSE) const;
//
//    xpr_bool_t isFile(void) const;
//    xpr_bool_t isFolder(void) const;
//    xpr_bool_t isFileSystem(void) const;
//    xpr_bool_t isFileSystemFolder(void) const;
//    xpr_bool_t isNetwork(void) const;
//    xpr_bool_t isLink(void) const;
//    Attribute  getAttributes(void) const;
//    xpr_bool_t hasAttributes(Attribute aAttributes) const;
//    xpr_bool_t hasSubFolder(void) const;
//    xpr_bool_t existFile(void) const;
//    xpr_bool_t isKnownExt(void) const;
//    xpr_bool_t equalExt(const xpr_tchar_t *aExt) const;
//    xpr_bool_t equalExt(xpr::string &aExt) const;
//
//    xpr_bool_t getFileType(FileType &aFileType) const;
//    xpr_bool_t getFileTime(xpr_time_t *aAccessTime, xpr_time_t *aModifyTime, xpr_time_t *aChangeTime) const;
//    xpr_bool_t getFileAccessTime(xpr_time_t &aAccessTime) const;
//    xpr_bool_t getFileModifyTime(xpr_time_t &aModifyTime) const;
//    xpr_bool_t getFileChangeTime(xpr_time_t &aChangeTime) const;
//    xpr_bool_t getFileSize(xpr_sint64_t &aFileSize) const;
//    xpr_bool_t getInfoTip(xpr_tchar_t *aInfoTip, xpr_size_t aMaxLen) const;
//    xpr_bool_t getInfoTip(xpr::string &aInfoTip) const;
//
//    xpr_bool_t makeLink(const xpr_tchar_t *aLinkFile, xpr_bool_t aSymbolicLink) const;
//    xpr_bool_t makeLink(const xpr::string &aLinkFile, xpr_bool_t aSymbolicLink) const;
//    xpr_bool_t makeShortcut(const xpr_tchar_t *aLinkFile, const xpr_tchar_t *aStartupDir, xpr_ushort_t aHotkey, xpr_sint_t aShowCommand) const;
//    xpr_bool_t makeShortcut(const xpr::string &aLinkFile, const xpr::string *aStartupDir, xpr_ushort_t aHotkey, xpr_sint_t aShowCommand) const;
//    xpr_bool_t getLinkTarget(xpr_tchar_t *aTargetPath, xpr_size_t aMaxLen) const;
//    xpr_bool_t getLinkTarget(xpr::string &aTargetPath) const;
//    xpr_bool_t getShortcutTarget(xpr_tchar_t *aTargetPath, xpr_size_t aMaxTargetPathLen,
//                                 xpr_tchar_t *aParameters, xpr_size_t aMaxParametersLen,
//                                 xpr_tchar_t *aStartupDir, xpr_size_t aMaxStartupDirLen,
//                                 xpr_tchar_t *aIconPath,   xpr_size_t aMaxIconPathLen,
//                                 xpr_sint_t  *aIconIndex) const;
//    xpr_bool_t getShortcutTarget(xpr::string &aTargetPath,
//                                 xpr::string &aParameters,
//                                 xpr::string &aStartupDir,
//                                 xpr::string &aIconPath,
//                                 xpr_size_t   &aIconIndex) const;
//
//    xpr_bool_t execute(void) const;
//    xpr_bool_t execute(const xpr_tchar_t *aStartupDir, const xpr_tchar_t *aParameters, xpr_sint_t aShowCommand) const;
//    xpr_bool_t openAs(void) const;
//    xpr_bool_t openAs(const xpr_tchar_t *aStartupDir, const xpr_tchar_t *aParameters, xpr_sint_t aShowCommand) const;
//    static xpr_bool_t navigateUri(const xpr_tchar_t *aUri);
//
//#if defined(XPR_CFG_OS_WINDOWS)
//    struct ColumnId
//    {
//        xpr::Guid  mFormatId;
//        xpr_sint_t mPropertyId;
//    };
//#else // not XPR_CFG_OS_WINDOWS
//#error Please, support your OS. <shell_item.h>
//#endif // XPR_CFG_OS_WINDOWS
//
//    struct ColumnMetaData
//    {
//        ColumnId     mColumnId;
//        xpr::string  mColumnName;
//        xpr_sint_t   mTextAlign;
//        xpr_sint_t   mDefaultWidth;
//        xpr_bool_t   mDefaultUse;
//        xpr_bool_t   mSlow;
//    };
//
//    xpr_size_t getColumnCount(void) const;
//    xpr_bool_t getColumnMetaData(const ColumnId &aColumnId, ColumnMetaData &aColumnMetaData) const;
//    xpr_bool_t getColumnText(const ColumnId &aColumnId, xpr_tchar_t *aColumnText, xpr_size_t aMaxLen) const;
//    xpr_bool_t getColumnText(const ColumnId &aColumnId, xpr::string &aColumnText) const;

protected:
#if defined(XPR_CFG_OS_WINDOWS)
    LPSHELLFOLDER mShellFolder;
    LPITEMIDLIST  mPidl;        // simple PIDL (if mShellFolder is not NULL) or full-qualified PIDL (if mShellFolder is NULL)
    Attribute     mAttributes;
    FileType      mFileType;
#else // not XPR_CFG_OS_WINDOWS
#error Please, support your OS. <shell_item.h>
#endif // XPR_CFG_OS_WINDOWS
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_SHELL_ITEM_H__
