//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SHELL_H__
#define __FXB_SHELL_H__
#pragma once

namespace fxb
{
#define MAX_SHORTCUT_DESC INFOTIPSIZE

//
// path <-> pidl
//
LPITEMIDLIST IL_CreateFromPath(const xpr_tchar_t *aPath);
HRESULT      SHGetPidlFromPath(const xpr_tchar_t *aPath, LPITEMIDLIST *aPidl);
HRESULT      SHGetSimplePidlFromPath(const xpr_tchar_t *aPath, LPITEMIDLIST *aPidl);
LPITEMIDLIST SHGetPidlFromPath(const xpr_tchar_t *aPath);
xpr_bool_t   Path2Pidl(const xpr_tchar_t *aPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST Path2Pidl(const xpr_tchar_t *aPath);
xpr_bool_t   Path2Pidl(const std::tstring &aPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST Path2Pidl(const std::tstring &aPath);
xpr_bool_t   Pidl2Path(LPITEMIDLIST aFullPidl, xpr_tchar_t *aPath, xpr_bool_t aOnlyFileSystemPath = XPR_FALSE);
xpr_bool_t   Pidl2Path(LPITEMIDLIST aFullPidl, std::tstring &aPath, xpr_bool_t aOnlyFileSystemPath = XPR_FALSE);

//
// folder path (resolved, if pidl is link file)
//
xpr_bool_t GetFolderPath(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aPath, xpr_ulong_t aShellAttributes = 0);
xpr_bool_t GetFolderPath(LPCITEMIDLIST aFullPidl, std::tstring &aPath, xpr_ulong_t aShellAttributes = 0);
xpr_bool_t GetFolderPath(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_tchar_t *aPath, xpr_ulong_t aShellAttributes = 0);
xpr_bool_t GetFolderPath(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, std::tstring &aPath, xpr_ulong_t aShellAttributes = 0);

//
// full path
//
void         GetFullPath(LPITEMIDLIST aFullPidl, xpr_tchar_t *aFullPath);
void         GetFullPath(LPITEMIDLIST aFullPidl, std::tstring &aFullPath);
xpr_bool_t   GetFullPidl(const xpr_tchar_t *aFullPath, LPITEMIDLIST *aFullPidl);
xpr_bool_t   GetFullPidl(const std::tstring &aFullPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST GetFullPidl(const xpr_tchar_t *aFullPath);
LPITEMIDLIST GetFullPidl(const std::tstring &aFullPath);

void         GetDispFullPath(LPITEMIDLIST aFullPidl, xpr_tchar_t *aPath);
void         GetDispFullPath(LPITEMIDLIST aFullPidl, std::tstring &aPath);
void         GetDispFullPath(LPITEMIDLIST aFullPidl, CString &aPath);
CString      GetDispFullPath(LPITEMIDLIST aFullPidl);
xpr_bool_t   GetDispFullPidl(const xpr_tchar_t *aFullPath, LPITEMIDLIST *aFullPidl);
LPITEMIDLIST GetDispFullPidl(const xpr_tchar_t *aFullPath);

//
// aShellFolder + aPidl <-> aFullPidl
//
LPITEMIDLIST SH_GetRealIDL(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl);
HRESULT      SH_GetRealIDL(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, LPITEMIDLIST *aFullPidl);
HRESULT      SH_BindToParent(LPCITEMIDLIST aPidl, REFIID riid, LPVOID *ppv, LPCITEMIDLIST *aLastPidl);

//
// name
//
xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, DWORD aFlags, CString &aFriendlyName);
xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, DWORD aFlags, std::tstring &aFriendlyName);
xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, DWORD aFlags, xpr_tchar_t *aFriendlyName);
xpr_bool_t GetName(LPITEMIDLIST aFullPidl, DWORD aFlags, CString &aFriendlyName);
xpr_bool_t GetName(LPITEMIDLIST aFullPidl, DWORD aFlags, std::tstring &aFriendlyName);
xpr_bool_t GetName(LPITEMIDLIST aFullPidl, DWORD aFlags, xpr_tchar_t *aFriendlyName);
void       PathToNormal(xpr_tchar_t *aPath);
void       PathToNormal(const xpr_tchar_t *aPath, xpr_tchar_t *aNormal);

//
// file time
//
xpr_bool_t GetFileTime(const xpr_tchar_t *aPath, xpr_tchar_t *aLastWriteDateTime);
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
xpr_uint64_t GetFileSize(const std::tstring &aPath);
xpr_uint64_t GetFileSize(const xpr_tchar_t *aPath);
xpr_bool_t   GetFileSize(const xpr_tchar_t *aPath, xpr_tchar_t *aFileSize, xpr_size_t aMaxLen);

//
// info tip
//
xpr_bool_t GetInfotip(HWND aHwnd, LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_tchar_t *aInfotip);

//
// shell item and file icon
//
HICON      GetItemIcon(const xpr_tchar_t *aPath, xpr_bool_t aOpen = XPR_FALSE, xpr_bool_t aLarge = XPR_FALSE);
HICON      GetItemIcon(LPITEMIDLIST aFullPidl, xpr_bool_t aOpen = XPR_FALSE, xpr_bool_t aLarge = XPR_FALSE);
HICON      GetItemIcon(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_bool_t aOpen = XPR_FALSE, xpr_bool_t aLarge = XPR_FALSE);
xpr_sint_t GetItemIconIndex(const xpr_tchar_t *aPath, xpr_bool_t aOpen = XPR_FALSE);
xpr_sint_t GetItemIconIndex(LPITEMIDLIST aFullPidl, xpr_bool_t aOpen = XPR_FALSE);
xpr_sint_t GetItemIconIndex(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_bool_t aOpen = XPR_FALSE);
xpr_sint_t GetItemIconOverlayIndex(const xpr_tchar_t *aPath, xpr_bool_t aIconOverlay = XPR_FALSE);
xpr_sint_t GetItemIconOverlayIndex(LPITEMIDLIST aFullPidl, xpr_bool_t aIconOverlay = XPR_FALSE);
xpr_sint_t GetItemIconOverlayIndex(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_bool_t aIconOverlay = XPR_FALSE);
HICON      GetFileExtIcon(const xpr_tchar_t *aExt, xpr_bool_t aLarge);
xpr_sint_t GetFileExtIconIndex(const xpr_tchar_t *aExt);

//
// shell item and file attribues
//
xpr_ulong_t GetItemAttributes(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl);
xpr_bool_t  GetItemAttributes(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes);
xpr_ulong_t GetItemAttributes(LPITEMIDLIST aFullPidl);
xpr_bool_t  GetItemAttributes(LPITEMIDLIST aFullPidl, xpr_ulong_t &aShellAttributes);
void        SetItemAttribsFilter(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes);
xpr_bool_t  IsFileSystem(LPITEMIDLIST aFullPidl);
xpr_bool_t  IsFileSystem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl);
xpr_bool_t  IsFileSystem(const xpr_tchar_t *aPath);
xpr_bool_t  IsFileSystemFolder(LPITEMIDLIST aFullPidl);
xpr_bool_t  IsFileSystemFolder(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl);
xpr_bool_t  IsFileSystemFolder(const std::tstring &aPath);
xpr_bool_t  IsFileSystemFolder(const xpr_tchar_t *aPath);
xpr_bool_t  IsNetItem(LPITEMIDLIST aFullPidl);
xpr_bool_t  IsNetItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl);
xpr_bool_t  IsNetItem(const xpr_tchar_t *aPath);
xpr_bool_t  IsParentVirtualItem(LPITEMIDLIST aFullPidl, const std::tstring &aFullPath);
xpr_bool_t  IsParentVirtualItem(LPITEMIDLIST aFullPidl);
xpr_bool_t  IsVirtualItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl);
xpr_bool_t  IsVirtualItem(LPITEMIDLIST aFullPidl);
xpr_bool_t  IsExistFile(const xpr_tchar_t *aPath);
xpr_bool_t  IsExistFile(const std::tstring &aPath);

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
const xpr_tchar_t *GetFileExt(const std::tstring &aPath);
const xpr_tchar_t *GetFileExt(const xpr_tchar_t *aPath);
const xpr_tchar_t *GetPathToFileName(const xpr_tchar_t *aPath, xpr_sint_t *aFileNameLen = XPR_NULL);
const xpr_tchar_t *GetPathToExt(const xpr_tchar_t *aPath);
xpr_bool_t         GetPathFileNameExt(const xpr_tchar_t *aPath, xpr_tchar_t *aFileName, xpr_tchar_t *aExt);
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

xpr_bool_t ExecFile(LPITEMIDLIST       aFullPidl,
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
} // namespace fxb

#endif // __FXB_SHELL_H__
