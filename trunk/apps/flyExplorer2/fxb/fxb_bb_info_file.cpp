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
    : mFile(XPR_NULL), mUnicode(XPR_FALSE)
{
    mPath[0] = '\0';
}

BBInfoFile::~BBInfoFile(void)
{
    close();

    {
        BitBucketDataEntryA *sBitBucketDataEntryA;
        std::deque<BitBucketDataEntryA *>::iterator sIterator;

        sIterator = mListA.begin();
        for (; sIterator != mListA.end(); ++sIterator)
        {
            sBitBucketDataEntryA = *sIterator;
            XPR_SAFE_DELETE(sBitBucketDataEntryA);
        }
    }

    {
        BitBucketDataEntryW *sBitBucketDataEntryW;
        std::deque<BitBucketDataEntryW *>::iterator sIterator;

        sIterator = mListW.begin();
        for (; sIterator != mListW.end(); ++sIterator)
        {
            sBitBucketDataEntryW = *sIterator;
            XPR_SAFE_DELETE(sBitBucketDataEntryW);
        }
    }

    mListA.clear();
    mListW.clear();
}

xpr_bool_t BBInfoFile::open(xpr_tchar_t aDrive)
{
    mPath[0] = '\0';
    if (IsDriveSecure(aDrive))
    {
        xpr_tchar_t sUserSid[XPR_MAX_PATH + 1] = {0};
        if (GetUserSid(XPR_NULL, sUserSid, XPR_MAX_PATH))
            _stprintf(mPath, XPR_STRING_LITERAL("%c:\\RECYCLER\\%s"), aDrive, sUserSid);
    }
    else
    {
        _stprintf(mPath, XPR_STRING_LITERAL("%c:\\RECYCLED"), aDrive);
    }

    if (mPath[0] == '\0')
        return XPR_FALSE;

    xpr_tchar_t sInfo[XPR_MAX_PATH + 1] = {0};
    _stprintf(sInfo, XPR_STRING_LITERAL("%s\\INFO2"), mPath);

    mFile = _tfopen(sInfo, XPR_STRING_LITERAL("rb"));
    if (XPR_IS_NULL(mFile))
        return XPR_FALSE;

    getDataEntryAll();

    return XPR_TRUE;
}

void BBInfoFile::close(void)
{
    if (XPR_IS_NOT_NULL(mFile))
    {
        fclose(mFile);
        mFile = XPR_NULL;
    }
}

xpr_size_t BBInfoFile::getCount(void)
{
    return (isUnicode() == XPR_TRUE) ? (xpr_sint_t)mListW.size() : (xpr_sint_t)mListA.size();
}

xpr_tchar_t BBInfoFile::getDrive(void)
{
    return mPath[0];
}

void BBInfoFile::getBBPath(xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        _tcscpy(aPath, mPath);
}

xpr_bool_t BBInfoFile::isUnicode(void)
{
    return mUnicode;
}

xpr_bool_t BBInfoFile::getDataEntryAll(void)
{
    xpr_size_t sRead;
    xpr_size_t sSize;

    BitBucketInfoHeader sBitBucketInfoHeader = {0};
    sRead = fread(&sBitBucketInfoHeader, sizeof(BitBucketInfoHeader), 1, mFile);
    if (sRead != 1)
        return XPR_TRUE;

    mUnicode = XPR_FALSE;
    sSize = sizeof(BitBucketDataEntryA);
    if (sBitBucketInfoHeader.mDataEntrySize == sizeof(BitBucketDataEntryW))
    {
        mUnicode = XPR_TRUE;
        sSize = sizeof(BitBucketDataEntryW);
    }

    BitBucketDataEntryA *sBitBucketDataEntryA = XPR_NULL;
    BitBucketDataEntryW *sBitBucketDataEntryW = XPR_NULL;

    do
    {
        if (XPR_IS_TRUE(mUnicode))
        {
            sBitBucketDataEntryW = new BitBucketDataEntryW;
            sRead = fread(sBitBucketDataEntryW, sSize, 1, mFile);

            mListW.push_back(sBitBucketDataEntryW);
        }
        else
        {
            sBitBucketDataEntryA = new BitBucketDataEntryA;
            sRead = fread(sBitBucketDataEntryA, sSize, 1, mFile);

            mListA.push_back(sBitBucketDataEntryA);
        }
    }
    while (sRead == 1);

    return XPR_TRUE;
}

xpr_bool_t BBInfoFile::findOriginalPath(xpr_tchar_t *aFind, FILETIME aCreatedTime, xpr_tchar_t *aBBIndexPath)
{
    xpr_size_t i, sCount;
    HANDLE sFindFile;
    WIN32_FIND_DATA sWin32FindData = {0};
    xpr_wchar_t sFindW[XPR_MAX_PATH + 1] = {0};
    xpr_char_t sFindA[XPR_MAX_PATH + 1] = {0};
    xpr_size_t sOutputBytes;

    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
    XPR_TCS_TO_UTF16(aFind, _tcslen(aFind) * sizeof(xpr_tchar_t), sFindW, &sOutputBytes);
    sFindW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
    XPR_TCS_TO_MBS(aFind, _tcslen(aFind) * sizeof(xpr_tchar_t), sFindA, &sOutputBytes);
    sFindA[sOutputBytes / sizeof(xpr_char_t)] = 0;

    sCount = getCount();
    for (i = 0; i < sCount; ++i)
    {
        if (XPR_IS_TRUE(mUnicode))
        {
            if (_wcsicmp(mListW[i]->mOriginal, sFindW) == 0)
            {
                getBBIndexPath(i, aBBIndexPath);

                sFindFile = ::FindFirstFile(aBBIndexPath, &sWin32FindData);
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
        else
        {
            if (_stricmp(mListA[i]->mOriginal, sFindA) == 0)
            {
                getBBIndexPath(i, aBBIndexPath);

                sFindFile = ::FindFirstFile(aBBIndexPath, &sWin32FindData);
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
    }

    return XPR_FALSE;
}

xpr_bool_t BBInfoFile::getBBIndexPath(xpr_size_t aIndex, xpr_tchar_t *aBBIndexPath)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mListW) || !aBBIndexPath)
        return XPR_FALSE;

    xpr_tchar_t sBBPath[XPR_MAX_PATH + 1] = {0};
    getBBPath(sBBPath);

    xpr_sint_t sIndexId;
    xpr_tchar_t sExt[XPR_MAX_PATH + 1] = {0};
    if (isUnicode() == XPR_TRUE)
    {
        sIndexId = mListW[aIndex]->mIndex;
        xpr_wchar_t *sSplit = wcsrchr(mListW[aIndex]->mOriginal, '\\');
        if (XPR_IS_NOT_NULL(sSplit))
        {
            sSplit++;
            xpr_wchar_t *sFindedExt = wcsrchr(sSplit, '.');
            if (XPR_IS_NOT_NULL(sFindedExt))
            {
                xpr_size_t sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                XPR_UTF16_TO_TCS(sFindedExt, wcslen(sFindedExt) * sizeof(xpr_wchar_t), sExt, &sOutputBytes);
                sExt[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
            }
        }
    }
    else
    {
        sIndexId = mListA[aIndex]->mIndex;
        xpr_char_t *sSplit = strrchr(mListA[aIndex]->mOriginal, '\\');
        if (XPR_IS_NOT_NULL(sSplit))
        {
            sSplit++;
            xpr_char_t *sFindedExt = strrchr(sSplit, '.');
            if (XPR_IS_NOT_NULL(sFindedExt))
            {
                xpr_size_t sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                XPR_MBS_TO_TCS(sFindedExt, strlen(sFindedExt) * sizeof(xpr_char_t), sExt, &sOutputBytes);
                sExt[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
            }
        }
    }

    _stprintf(aBBIndexPath, XPR_STRING_LITERAL("%s\\D%c%d%s"), sBBPath, sBBPath[0], sIndexId, sExt);

    return XPR_TRUE;
}
} // namespace fxb
