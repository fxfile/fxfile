//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SHELL_H__
#define __FXFILE_SHELL_H__ 1
#pragma once

namespace fxfile
{
#define MAX_SHORTCUT_DESC INFOTIPSIZE

//
// path <-> pidl
//
xpr_bool_t   Path2Pidl(const xpr_tchar_t *aPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST Path2Pidl(const xpr_tchar_t *aPath);
xpr_bool_t   Path2Pidl(const xpr::tstring &aPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST Path2Pidl(const xpr::tstring &aPath);
xpr_bool_t   Pidl2Path(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aPath, xpr_bool_t aOnlyFileSystemPath = XPR_FALSE);
xpr_bool_t   Pidl2Path(LPCITEMIDLIST aFullPidl, xpr::tstring &aPath, xpr_bool_t aOnlyFileSystemPath = XPR_FALSE);

//
// folder path (resolved, if pidl is link file)
//
xpr_bool_t GetFolderPath(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aPath, xpr_ulong_t aShellAttributes = 0);
xpr_bool_t GetFolderPath(LPCITEMIDLIST aFullPidl, xpr::tstring &aPath, xpr_ulong_t aShellAttributes = 0);
xpr_bool_t GetFolderPath(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aPath, xpr_ulong_t aShellAttributes = 0);
xpr_bool_t GetFolderPath(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr::tstring &aPath, xpr_ulong_t aShellAttributes = 0);

//
// full path
//
void         GetFullPath(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aFullPath);
void         GetFullPath(LPCITEMIDLIST aFullPidl, xpr::tstring &aFullPath);
xpr_bool_t   GetFullPidl(const xpr_tchar_t *aFullPath, LPITEMIDLIST *aFullPidl);
xpr_bool_t   GetFullPidl(const xpr::tstring &aFullPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST GetFullPidl(const xpr_tchar_t *aFullPath);
LPITEMIDLIST GetFullPidl(const xpr::tstring &aFullPath);

void         GetDispFullPath(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aPath);
void         GetDispFullPath(LPCITEMIDLIST aFullPidl, xpr::tstring &aPath);
void         GetDispFullPath(LPCITEMIDLIST aFullPidl, CString &aPath);
CString      GetDispFullPath(LPCITEMIDLIST aFullPidl);
xpr_bool_t   GetDispFullPidl(const xpr_tchar_t *aFullPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST GetDispFullPidl(const xpr_tchar_t *aFullPath);

//
// name
//
xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, CString &aFriendlyName);
xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr::tstring &aFriendlyName);
xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr_tchar_t *aFriendlyName);
xpr_bool_t GetName(LPCITEMIDLIST aFullPidl, DWORD aFlags, CString &aFriendlyName);
xpr_bool_t GetName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr::tstring &aFriendlyName);
xpr_bool_t GetName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr_tchar_t *aFriendlyName);
void       PathToNormal(xpr_tchar_t *aPath);
void       PathToNormal(const xpr_tchar_t *aPath, xpr_tchar_t *aNormal);

//
// file time
//
xpr_bool_t GetFileTime(const xpr_tchar_t *aPath, xpr_tchar_t *aLastWriteDateTime);
xpr_bool_t GetFileTime(const FILETIME &aFileTime, xpr_tchar_t *aFileTimeText);
xpr_bool_t GetFileTime(const xpr_tchar_t *aPath, FILETIME *aModifiedFileTime);
xpr_bool_t SetFileTime(const xpr_tchar_t *aPath,
                       FILETIME          *aCreatedFileTime = XPR_NULL,
                       FILETIME          *aLastAccessFileTime = XPR_NULL,
                       FILETIME          *aModifiedFileTime = XPR_NULL,
                       xpr_bool_t         aForce = XPR_TRUE);

//
// file type
//
xpr_bool_t   GetFileType(const xpr_tchar_t *aPath, xpr_tchar_t *aTypeName);

//
// file size
//
xpr_uint64_t GetFileSize(const xpr::tstring &aPath);
xpr_uint64_t GetFileSize(const xpr_tchar_t *aPath);
xpr_bool_t   GetFileSize(const xpr_tchar_t *aPath, xpr_tchar_t *aFileSize, xpr_size_t aMaxLen);
xpr_bool_t   GetFileSize(xpr_uint64_t aFileSize, xpr_tchar_t *aFileSizeText, xpr_size_t aMaxLen);

//
// info tip
//
xpr_bool_t GetInfotip(HWND aHwnd, LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aInfotip);

//
// shell item and file icon
//
HICON      GetItemIcon(const xpr_tchar_t *aPath, xpr_bool_t aOpen = XPR_FALSE, xpr_bool_t aLarge = XPR_FALSE);
HICON      GetItemIcon(LPCITEMIDLIST aFullPidl, xpr_bool_t aOpen = XPR_FALSE, xpr_bool_t aLarge = XPR_FALSE);
HICON      GetItemIcon(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_bool_t aOpen = XPR_FALSE, xpr_bool_t aLarge = XPR_FALSE);
xpr_sint_t GetItemIconIndex(const xpr_tchar_t *aPath, xpr_bool_t aOpen = XPR_FALSE);
xpr_sint_t GetItemIconIndex(LPCITEMIDLIST aFullPidl, xpr_bool_t aOpen = XPR_FALSE);
xpr_sint_t GetItemIconIndex(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_bool_t aOpen = XPR_FALSE);
xpr_sint_t GetItemIconOverlayIndex(const xpr_tchar_t *aPath, xpr_bool_t aIconOverlay = XPR_FALSE);
xpr_sint_t GetItemIconOverlayIndex(LPCITEMIDLIST aFullPidl, xpr_bool_t aIconOverlay = XPR_FALSE);
xpr_sint_t GetItemIconOverlayIndex(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_bool_t aIconOverlay = XPR_FALSE);
HICON      GetFileExtIcon(const xpr_tchar_t *aExt, xpr_bool_t aLarge);
xpr_sint_t GetFileExtIconIndex(const xpr_tchar_t *aExt);

//
// shell item and file attribues
//
xpr_ulong_t GetItemAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
xpr_bool_t  GetItemAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes);
xpr_ulong_t GetItemAttributes(LPCITEMIDLIST aFullPidl);
xpr_bool_t  GetItemAttributes(LPCITEMIDLIST aFullPidl, xpr_ulong_t &aShellAttributes);
void        SetItemAttribsFilter(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes);
xpr_bool_t  IsFileSystem(LPCITEMIDLIST aFullPidl);
xpr_bool_t  IsFileSystem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
xpr_bool_t  IsFileSystem(const xpr_tchar_t *aPath);
xpr_bool_t  IsFileSystemFolder(LPCITEMIDLIST aFullPidl);
xpr_bool_t  IsFileSystemFolder(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
xpr_bool_t  IsFileSystemFolder(const xpr::tstring &aPath);
xpr_bool_t  IsFileSystemFolder(const xpr_tchar_t *aPath);
xpr_bool_t  IsNetItem(LPCITEMIDLIST aFullPidl);
xpr_bool_t  IsNetItem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
xpr_bool_t  IsNetItem(const xpr_tchar_t *aPath);
xpr_bool_t  IsParentVirtualItem(LPCITEMIDLIST aFullPidl, const xpr::tstring &aFullPath);
xpr_bool_t  IsParentVirtualItem(LPCITEMIDLIST aFullPidl);
xpr_bool_t  IsVirtualItem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
xpr_bool_t  IsVirtualItem(LPCITEMIDLIST aFullPidl);
xpr_bool_t  IsExistFile(const xpr_tchar_t *aPath);
xpr_bool_t  IsExistFile(const xpr::tstring &aPath);

//
// disk and drive related
//
enum
{
    DriveTypeUnknown = 0,
    DriveType8InchFloppyDisk,
    DriveType35InchFloppyDisk,
    DriveType525InchFloppyDisk,
    DriveTypeRemovableDisk,
    DriveTypeLocal,
    DriveTypeNetwork,
    DriveTypeCdRom,
    DriveTypeRam,
};

xpr_bool_t GetDiskFreeSize(xpr_tchar_t *aPath, xpr_uint64_t &aFreeSize);
xpr_bool_t GetDiskTotalSize(xpr_tchar_t *aPath, xpr_uint64_t &aTotalSize);
xpr_bool_t GetDiskSize(xpr_tchar_t *aPath, xpr_uint64_t &aTotalSize, xpr_uint64_t &aUsedSize, xpr_uint64_t &aFreeSize);
xpr_sint_t GetRootDriveType(xpr_tchar_t aDriveChar);
void       GetDriveStrings(xpr_tchar_t *aDrive, xpr_sint_t aMaxLen = -1);
xpr_bool_t GetDriveFileSystem(const xpr_tchar_t *aDrive, xpr_tchar_t *aFileSystem, xpr_size_t aMaxLen);
xpr_bool_t IsDriveSecure(xpr_tchar_t aDriveChar);

//
// file extension
//
xpr_bool_t         IsEqualFileExt(const xpr_tchar_t *aPath, const xpr_tchar_t *aExt);
const xpr_tchar_t *GetFileExt(const xpr::tstring &aPath);
const xpr_tchar_t *GetFileExt(const xpr_tchar_t *aPath);
xpr_bool_t         IsFileKnownExt(const xpr_tchar_t *aExt);

//
// directory
//
xpr_bool_t CreateDirectoryLevel(const xpr_tchar_t *aDir, xpr_size_t aOffset = 0, xpr_bool_t aLastDir = XPR_TRUE);

//
// file execution
//
xpr_bool_t ExecFile(const xpr_tchar_t *aPath,
                    const xpr_tchar_t *aStartup = XPR_NULL,
                    const xpr_tchar_t *aParameter = XPR_NULL,
                    xpr_sint_t         aShowCmd = -1);

xpr_bool_t ExecFile(LPCITEMIDLIST      aFullPidl,
                    const xpr_tchar_t *aStartLocation = XPR_NULL,
                    const xpr_tchar_t *aParameter = XPR_NULL,
                    xpr_sint_t         aShowCmd = -1);

xpr_bool_t OpenAsFile(const xpr_tchar_t *aPath,
                      const xpr_tchar_t *aStartup = XPR_NULL,
                      const xpr_tchar_t *aParameter = XPR_NULL,
                      xpr_sint_t         aShowCmd = -1);

void NavigateURL(xpr_tchar_t *aUrl);

//
// file operation
//
void SHSilentDeleteFile(const xpr_tchar_t *aSource);
void SHSilentCopyFile(const xpr_tchar_t *aSource, const xpr_tchar_t *aTarget);
void SHSilentMoveFile(const xpr_tchar_t *aSource, const xpr_tchar_t *aTarget);

//
// shortcut
//
xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile, const xpr_tchar_t *aTarget);
xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile, LPCITEMIDLIST aPidl);
xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile,
                          LPCITEMIDLIST      aPidl,
                          const xpr_tchar_t *aStartup,
                          WORD               aHotkey,
                          xpr_sint_t         aShowCmd,
                          const xpr_tchar_t *aDesc);
xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile,
                          const xpr_tchar_t *aTarget,
                          const xpr_tchar_t *aStartup,
                          WORD               aHotkey,
                          xpr_sint_t         aShowCmd,
                          const xpr_tchar_t *aDesc);
xpr_bool_t ResolveShortcut(HWND         aHwnd,
                           xpr_tchar_t *aLinkFile,
                           xpr_tchar_t *aPath,
                           xpr_tchar_t *aParameter = XPR_NULL,
                           xpr_tchar_t *aStartup = XPR_NULL,
                           xpr_tchar_t *aIcon = XPR_NULL,
                           xpr_sint_t  *aIconIndex = XPR_NULL) ;
xpr_bool_t ResolveShortcut(HWND aHwnd,
                           xpr_tchar_t  *aLinkFile,
                           LPITEMIDLIST *aFullPidl,
                           xpr_tchar_t  *aParameter = XPR_NULL,
                           xpr_tchar_t  *aStartup = XPR_NULL,
                           xpr_tchar_t  *aIcon = XPR_NULL,
                           xpr_sint_t   *aIconIndex = XPR_NULL);
} // namespace fxfile

#endif // __FXFILE_SHELL_H__
