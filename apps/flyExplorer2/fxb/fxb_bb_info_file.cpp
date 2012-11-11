//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_bb_info_file.h"

#include <Sddl.h> // for Security Indentifier

// Windows vista below
//  - INFO/INFO2 File
// Windows vista higher
//  - R$XXXXXX.ext, I$XXXX.ext

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
struct BBInfoFile::BitBucketInfoHeader
{
    xpr_uint_t mReserved1;
    xpr_uint_t mReserved2;                // the # of items in this drive's recycle bin
    xpr_uint_t mReserved3;                // the current file number.
    xpr_uint_t mDataEntrySize;            // size of each entry
    xpr_uint_t mReserved4;                // total size of this recycle bin drive
};

struct BBInfoFile::BitBucketDataEntryA
{
    xpr_char_t mOriginal[260];            // original filename (if szOriginal[0] is 0, then it's a deleted entry)
    xpr_sint_t mIndex;                    // index (key to name)
    xpr_sint_t mDrive;                    // which drive bucket it's currently in
    FILETIME   mDeleteTime;
    DWORD      mSize;
    // shouldn't need file attributes because we did a move file
    // which should have preserved them.
};

struct BBInfoFile::BitBucketDataEntryW
{
    xpr_char_t  mShortName[260];          // original filename, shortened (if szOriginal[0] is 0, then it's a deleted entry)
    xpr_sint_t  mIndex;                   // index (key to name)
    xpr_sint_t  mDrive;                   // which drive bucket it's currently in
    FILETIME    mDeleteTime;
    DWORD       mSize;
    xpr_wchar_t mOriginal[260];           // original filename
};

struct BBInfoFile::Index
{
    xpr_bool_t  mInfo2Mode;
    xpr_sint_t  mInfo2Key;
    xpr_tchar_t mKey[7];
    FILETIME    mDeleteFileTime;
    DWORD       mFileSize;
    xpr_tchar_t mOriginalFilePath[MAX_PATH * 2 + 1];
};

static PTOKEN_USER GetUserToken(HANDLE aUserHandle)
{
    HANDLE sToCloseHandle = XPR_NULL;
    if (aUserHandle == XPR_NULL)
    {
        ::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &aUserHandle);
        sToCloseHandle = aUserHandle;
    }

    DWORD sSize = XPR_MAX_PATH;
    PTOKEN_USER sTokenUser = (PTOKEN_USER)::LocalAlloc(LPTR, sSize);
    if (XPR_IS_NOT_NULL(sTokenUser))
    {
        DWORD sNewSize;
        if (::GetTokenInformation(aUserHandle, TokenUser, sTokenUser, sSize, &sNewSize) == XPR_FALSE)
        {
            ::LocalFree((HLOCAL)sTokenUser);
            sTokenUser = XPR_NULL;
        }
    }

    if (XPR_IS_NOT_NULL(sToCloseHandle))
        ::CloseHandle(sToCloseHandle);

    return sTokenUser;
}

// ConvertSidToStringSid
// Win9x, NT 3.5 - not supported
static xpr_bool_t GetTextualSid(PSID aSid, xpr_tchar_t *aTextualSid, LPDWORD aBufferLen)
{
    PSID_IDENTIFIER_AUTHORITY sSidIdentifierAuthority;
    DWORD sSubAuthorities;
    DWORD sSidRev = SID_REVISION;
    DWORD sCounter;
    DWORD sSidSize;

    // Validate the binary SID.
    if (IsValidSid(aSid) == XPR_FALSE)
        return XPR_FALSE;

    // Get the identifier authority value from the SID.
    sSidIdentifierAuthority = GetSidIdentifierAuthority(aSid);

    // Get the number of subauthorities in the SID.
    sSubAuthorities = *GetSidSubAuthorityCount(aSid);

    // Compute the buffer length.
    // S-SID_REVISION- + IdentifierAuthority- + subauthorities- + XPR_NULL
    sSidSize = (15 + 12 + (12 * sSubAuthorities) + 1) * sizeof(xpr_tchar_t);

    // Check input buffer length.
    // If too small, indicate the proper size and set the last error.
    if (*aBufferLen < sSidSize)
    {
        *aBufferLen = sSidSize;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return XPR_FALSE;
    }

    // Add 'S' prefix and revision number to the string.
    sSidSize = wsprintf(aTextualSid, TEXT("S-%lu-"), sSidRev);

    // Add a SID identifier authority to the string.
    if ((sSidIdentifierAuthority->Value[0] != 0) || (sSidIdentifierAuthority->Value[1] != 0))
    {
        sSidSize += wsprintf(aTextualSid + lstrlen(aTextualSid),
            TEXT("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
            (xpr_ushort_t)sSidIdentifierAuthority->Value[0],
            (xpr_ushort_t)sSidIdentifierAuthority->Value[1],
            (xpr_ushort_t)sSidIdentifierAuthority->Value[2],
            (xpr_ushort_t)sSidIdentifierAuthority->Value[3],
            (xpr_ushort_t)sSidIdentifierAuthority->Value[4],
            (xpr_ushort_t)sSidIdentifierAuthority->Value[5]);
    }
    else
    {
        sSidSize += wsprintf(aTextualSid + lstrlen(aTextualSid),
            TEXT("%lu"),
            (xpr_ulong_t)(sSidIdentifierAuthority->Value[5]      )   +
            (xpr_ulong_t)(sSidIdentifierAuthority->Value[4] <<  8)   +
            (xpr_ulong_t)(sSidIdentifierAuthority->Value[3] << 16)   +
            (xpr_ulong_t)(sSidIdentifierAuthority->Value[2] << 24)   );
    }

    // Add SID subauthorities to the string.
    for (sCounter = 0; sCounter < sSubAuthorities; ++sCounter)
    {
        sSidSize += wsprintf(aTextualSid + sSidSize, TEXT("-%lu"), *GetSidSubAuthority(aSid, sCounter));
    }

    return XPR_TRUE;
}

// User Security Identifier
static xpr_bool_t GetUserSid(HANDLE aTokenHandle, xpr_tchar_t *aStringSid, DWORD aStringLen)
{
    PTOKEN_USER sTokenUser = fxb::GetUserToken(aTokenHandle);
    if (XPR_IS_NULL(sTokenUser))
        return XPR_FALSE;

    xpr_bool_t sResult = fxb::GetTextualSid(sTokenUser->User.Sid, aStringSid, &aStringLen);

    ::LocalFree((HLOCAL)sTokenUser);

    return sResult;
}

// You can get 'C:\\Recycler\\S-xx ... xx\\dc1.dat' from RecycledBin PIDL with SHGDN_FORPARSING flag.
// S-xx ... xx - SID(Security Identifier)

// BitBucket directory 2 type
// 1. c:\\Recycler\\S-xx ... xx\\Info2    - NTFS FileSystem
// 2. d:\\Recycled\\Info2                 - FAT  FileSystem

BBInfoFile::BBInfoFile(void)
{
    mDirPath[0] = '\0';
}

BBInfoFile::~BBInfoFile(void)
{
    close();

    Index *sIndex;
    IndexDeque::iterator sIterator;

    sIterator = mIndexDeque.begin();
    for (; sIterator != mIndexDeque.end(); ++sIterator)
    {
        sIndex = *sIterator;
        XPR_SAFE_DELETE(sIndex);
    }

    mIndexDeque.clear();
}

xpr_bool_t BBInfoFile::open(xpr_tchar_t aDriveChar)
{
    mDirPath[0] = '\0';

    xpr_uint_t sOsVer = xpr::getOsVer();

    if (IsDriveSecure(aDriveChar) == XPR_TRUE)
    {
        xpr_tchar_t sUserSid[XPR_MAX_PATH + 1] = {0};
        if (GetUserSid(XPR_NULL, sUserSid, XPR_MAX_PATH))
        {
            if (sOsVer >= xpr::kOsVerWinVista)
            {
                _stprintf(mDirPath, XPR_STRING_LITERAL("%c:\\$Recycle.Bin\\%s"), aDriveChar, sUserSid);
            }
            else
            {
                _stprintf(mDirPath, XPR_STRING_LITERAL("%c:\\RECYCLER\\%s"), aDriveChar, sUserSid);
            }
        }
    }
    else
    {
        _stprintf(mDirPath, XPR_STRING_LITERAL("%c:\\RECYCLED"), aDriveChar);
    }

    if (mDirPath[0] == '\0')
        return XPR_FALSE;

    if (sOsVer >= xpr::kOsVerWinVista)
    {
        readVistaIndex();
    }
    else
    {
        readINFO2Index();
    }

    return XPR_TRUE;
}

void BBInfoFile::close(void)
{
}

xpr_size_t BBInfoFile::getCount(void)
{
    return mIndexDeque.size();
}

xpr_tchar_t BBInfoFile::getDrive(void)
{
    return mDirPath[0];
}

void BBInfoFile::getDirPath(xpr_tchar_t *aDirPath)
{
    if (XPR_IS_NOT_NULL(aDirPath))
        _tcscpy(aDirPath, mDirPath);
}

void BBInfoFile::readINFO2Index(void)
{
    xpr_tchar_t sINFO2FilePath[XPR_MAX_PATH + 1] = {0};
    _stprintf(sINFO2FilePath, XPR_STRING_LITERAL("%s\\INFO2"), mDirPath);

    xpr_rcode_t sRcode;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(sINFO2FilePath, xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return;

    xpr_ssize_t sRead;
    xpr_size_t sSize;
    xpr_bool_t sUnicode;
    BitBucketInfoHeader sBitBucketInfoHeader = {0};

    sSize = sizeof(BitBucketInfoHeader);
    sRcode = sFileIo.read(&sBitBucketInfoHeader, sSize, &sRead);
    if (XPR_RCODE_IS_SUCCESS(sRcode) && sRead == sSize)
    {
        sUnicode = XPR_FALSE;

        sSize = sizeof(BitBucketDataEntryA);
        if (sBitBucketInfoHeader.mDataEntrySize == sizeof(BitBucketDataEntryW))
        {
            sUnicode = XPR_TRUE;
            sSize = sizeof(BitBucketDataEntryW);
        }

        Index *sIndex;
        BitBucketDataEntryA sBitBucketDataEntryA;
        BitBucketDataEntryW sBitBucketDataEntryW;
        xpr_size_t sOutputBytes;

        if (XPR_IS_TRUE(sUnicode))
        {
            do
            {
                sRcode = sFileIo.read(&sBitBucketDataEntryW, sSize, &sRead);
                if (XPR_RCODE_IS_SUCCESS(sRcode) && sRead == sSize)
                {
                    sIndex = new Index;
                    if (XPR_IS_NOT_NULL(sIndex))
                    {
                        sIndex->mInfo2Mode      = XPR_TRUE;
                        sIndex->mInfo2Key       = sBitBucketDataEntryW.mIndex;
                        sIndex->mKey[0]         = 0;
                        sIndex->mDeleteFileTime = sBitBucketDataEntryW.mDeleteTime;
                        sIndex->mFileSize       = sBitBucketDataEntryW.mSize;

                        sOutputBytes = sizeof(sIndex->mOriginalFilePath) - sizeof(xpr_tchar_t);
                        XPR_UTF16_TO_TCS(sBitBucketDataEntryW.mOriginal, wcslen(sBitBucketDataEntryW.mOriginal) * sizeof(xpr_wchar_t), sIndex->mOriginalFilePath, &sOutputBytes);
                        sIndex->mOriginalFilePath[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                        mIndexDeque.push_back(sIndex);
                    }
                }
            } while (XPR_RCODE_IS_SUCCESS(sRcode) && sRead == sSize);
        }
        else
        {
            do
            {
                sRcode = sFileIo.read(&sBitBucketDataEntryA, sSize, &sRead);
                if (XPR_RCODE_IS_SUCCESS(sRcode) && sRead == sSize)
                {
                    sIndex = new Index;
                    if (XPR_IS_NOT_NULL(sIndex))
                    {
                        sIndex->mInfo2Mode      = XPR_TRUE;
                        sIndex->mInfo2Key       = sBitBucketDataEntryA.mIndex;
                        sIndex->mKey[0]         = 0;
                        sIndex->mDeleteFileTime = sBitBucketDataEntryA.mDeleteTime;
                        sIndex->mFileSize       = sBitBucketDataEntryA.mSize;

                        sOutputBytes = sizeof(sIndex->mOriginalFilePath) - sizeof(xpr_tchar_t);
                        XPR_MBS_TO_TCS(sBitBucketDataEntryA.mOriginal, strlen(sBitBucketDataEntryA.mOriginal) * sizeof(xpr_char_t), sIndex->mOriginalFilePath, &sOutputBytes);
                        sIndex->mOriginalFilePath[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                        mIndexDeque.push_back(sIndex);
                    }
                }
            } while (XPR_RCODE_IS_SUCCESS(sRcode) && sRead == sSize);
        }
    }

    sFileIo.close();
}

void BBInfoFile::readVistaIndex(void)
{
    xpr_tchar_t sFindPath[XPR_MAX_PATH * 2 + 1] = {0};
    _tcscpy(sFindPath, mDirPath);
    _tcscat(sFindPath, XPR_STRING_LITERAL("\\*.*"));

    xpr_rcode_t sRcode;
    xpr_ssize_t sRead;
    xpr::FileIo sFileIo;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    xpr_sint64_t sFileSize;
    xpr_wchar_t sOriginalFile[XPR_MAX_PATH * 2 + 1] = {0};
    xpr_size_t sOutputBytes;
    Index *sIndex;

    HANDLE sFindFile;
    WIN32_FIND_DATA sWin32FindData = {0};

    sFindFile = ::FindFirstFile(sFindPath, &sWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (_tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL(".")) == 0 || _tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL("..")) == 0)
                continue;

            if (sWin32FindData.cFileName[0] != '$' || sWin32FindData.cFileName[1] != 'I')
                continue;

            _tcscpy(sPath, mDirPath);
            _tcscat(sPath, XPR_STRING_LITERAL("\\"));
            _tcscat(sPath, sWin32FindData.cFileName);

            sRcode = sFileIo.open(sPath, xpr::FileIo::OpenModeReadOnly);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                sIndex = new Index;
                if (XPR_IS_NOT_NULL(sIndex))
                {
                    sRcode = sFileIo.seekToEnd();
                    sFileSize = sFileIo.tell();
                    sRcode = sFileIo.seekFromBegin(8);

                    sRcode = sFileIo.read(&sIndex->mFileSize, (sFileSize == 544) ? 8 : 7, &sRead);
                    sRcode = sFileIo.read(&sIndex->mDeleteFileTime, sizeof(FILETIME), &sRead);
                    sRcode = sFileIo.read(&sOriginalFile, sFileSize - sFileIo.tell(), &sRead);

                    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                    XPR_UTF16_TO_TCS(sOriginalFile, wcslen(sOriginalFile) * sizeof(xpr_wchar_t), sIndex->mOriginalFilePath, &sOutputBytes);
                    sIndex->mOriginalFilePath[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                    sIndex->mInfo2Mode = XPR_FALSE;
                    _tcsncpy(sIndex->mKey, sWin32FindData.cFileName + 2, 6);
                    sIndex->mKey[6] = 0;

                    mIndexDeque.push_back(sIndex);
                }

                sFileIo.close();
            }
        }
        while (::FindNextFile(sFindFile, &sWin32FindData) == XPR_TRUE);

        ::FindClose(sFindFile);
        sFindFile = XPR_NULL;
    }
}

xpr_bool_t BBInfoFile::findOriginalPath(const xpr_tchar_t *aOriginalFilePath, FILETIME aCreatedTime, xpr_tchar_t *aRestoreFilePath)
{
    if (XPR_IS_NULL(aOriginalFilePath) || XPR_IS_NULL(aRestoreFilePath))
        return XPR_FALSE;

    HANDLE sFindFile;
    WIN32_FIND_DATA sWin32FindData = {0};
    IndexDeque::iterator sIterator;
    Index *sIndex;
    const xpr_tchar_t *sFileExt;

    sIterator = mIndexDeque.begin();
    for (; sIterator != mIndexDeque.end(); ++sIterator)
    {
        sIndex = *sIterator;
        if (XPR_IS_NULL(sIndex))
            continue;

        if (_tcsicmp(sIndex->mOriginalFilePath, aOriginalFilePath) == 0)
        {
            if (XPR_IS_TRUE(sIndex->mInfo2Mode))
            {
                // data file to restore
                //  - 'D' + drive + key + file ext.
                //  - ex) DC1.xml
                //        drive:     C
                //        key:       1
                //        file ext.: .xml

                _stprintf(aRestoreFilePath, XPR_STRING_LITERAL("%s\\D%c%d"), mDirPath, mDirPath[0], sIndex->mInfo2Key);

                sFileExt = fxb::GetFileExt(sIndex->mOriginalFilePath);
                if (XPR_IS_NOT_NULL(sFileExt))
                    _tcscat(aRestoreFilePath, sFileExt);
            }
            else
            {
                // data file to restore
                //  - R$' + key(6) + file ext.
                //  - ex) $RN0JW9J.xml
                //        key:       N0JW9J
                //        file ext.: .xml

                _stprintf(aRestoreFilePath, XPR_STRING_LITERAL("%s\\$R%s"), mDirPath, sIndex->mKey);

                sFileExt = fxb::GetFileExt(sIndex->mOriginalFilePath);
                if (XPR_IS_NOT_NULL(sFileExt))
                    _tcscat(aRestoreFilePath, sFileExt);
            }

            sFindFile = ::FindFirstFile(aRestoreFilePath, &sWin32FindData);
            if (sFindFile != INVALID_HANDLE_VALUE)
            {
                ::FindClose(sFindFile);

                if (::CompareFileTime(&aCreatedTime, &sWin32FindData.ftCreationTime) == 0)
                {
                    return XPR_TRUE;
                }
            }
        }
    }

    return XPR_FALSE;
}
} // namespace fxb
