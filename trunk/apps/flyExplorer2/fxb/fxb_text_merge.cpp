//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_text_merge.h"

#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
TextMerge::TextMerge(void)
    : mHwnd(NULL), mMsg(NULL)
    , mStatus(StatusNone)
    , mEncoding(XPR_FALSE)
{
}

TextMerge::~TextMerge(void)
{
    Stop();

    mPathDeque.clear();
}

void TextMerge::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void TextMerge::setEncoding(xpr_bool_t bEncoding)
{
    mEncoding = bEncoding;
}

void TextMerge::setTextFile(const xpr_tchar_t *aTextFile)
{
    if (XPR_IS_NOT_NULL(aTextFile))
       mTextFile = aTextFile;
}

void TextMerge::addPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        mPathDeque.push_back(aPath);
}

xpr_bool_t TextMerge::OnPreEntry(void)
{
    mSucceededCount = 0;
    mStatus = StatusMerging;

    return XPR_TRUE;
}

unsigned TextMerge::OnEntryProc(void)
{
    FILE *sFile, *sMergeFile;
    xpr_char_t *sTextA = new xpr_char_t[XPR_MAX_PATH * 6 + 1];
    xpr_wchar_t *sTextW = new xpr_wchar_t[XPR_MAX_PATH * 6 + 1];
    xpr_size_t sLen;
    xpr_bool_t sFileUnicode;
    xpr_ushort_t sCode;

    sMergeFile = _tfopen(mTextFile.c_str(), XPR_STRING_LITERAL("wt"));

    if (XPR_IS_TRUE(mEncoding))
    {
        _setmode(_fileno(sMergeFile), _O_BINARY);

        sCode = 0xFEFF;
        fwrite(&sCode, 2, 1, sMergeFile);
    }

    PathDeque::iterator sIterator;
    std::tstring sPath;

    sIterator = mPathDeque.begin();
    for (; sIterator != mPathDeque.end(); ++sIterator)
    {
        if (IsStop() == XPR_TRUE)
            break;

        sPath = *sIterator;

        sFile = _tfopen(sPath.c_str(), XPR_STRING_LITERAL("rt"));
        if (XPR_IS_NULL(sFile))
            continue;

        fread(&sCode, 2, 1, sFile);

        sFileUnicode = XPR_FALSE;
        if (sCode == 0xFEFF)
            sFileUnicode = XPR_TRUE;
        else
            fseek(sFile, 0, SEEK_SET);

        while (true)
        {
            if (IsStop() == XPR_TRUE)
                break;

            if (feof(sFile) != 0)
                break;

            if (XPR_IS_FALSE(mEncoding)) // ANSI
            {
                if (XPR_IS_FALSE(sFileUnicode))
                {
                    if (fgets(sTextA, XPR_MAX_PATH * 6, sFile) == NULL)
                        break;
                }
                else
                {
                    sLen = fread(sTextW, 1, XPR_MAX_PATH, sFile);
                    if (sLen <= 0)
                        break;

                    sLen /= 2;
                    sTextW[sLen] = '\0';

                    WideCharToMultiByte(CP_ACP, 0, sTextW, -1, sTextA, XPR_MAX_PATH * 6, NULL, NULL);

                    while (true)
                    {
                        xpr_char_t *s0D = strchr(sTextA, 0x0D);
                        if (XPR_IS_NULL(s0D))
                            break;

                        memmove(sTextA, sTextA+1, strlen(sTextA)+1);
                    }
                }

                fprintf(sMergeFile, "%s", sTextA);
            }
            else // UNICODE
            {
                if (XPR_IS_FALSE(sFileUnicode))
                {
                    if (fgets(sTextA, XPR_MAX_PATH, sFile) == NULL)
                        break;

                    MultiByteToWideChar(CP_ACP, 0, sTextA, -1, sTextW, XPR_MAX_PATH * 6);
                    xpr_wchar_t *s0D = sTextW;
                    while (true)
                    {
                        s0D = wcschr(s0D, 0x000A);
                        if (XPR_IS_NULL(s0D))
                            break;

                        memmove(s0D+1, s0D, (wcslen(s0D)+1) * sizeof(xpr_wchar_t));
                        s0D[0] = 0x000D;
                        s0D += 2;
                    }
                }
                else
                {
                    sLen = fread(sTextW, 1, XPR_MAX_PATH, sFile);
                    if (sLen <= 0)
                        break;

                    sLen /= 2;
                    sTextW[sLen] = '\0';
                }

                fwrite(sTextW, wcslen(sTextW) * sizeof(xpr_wchar_t), 1, sMergeFile);
            }
        }

        fclose(sFile);

        {
            CsLocker sLocker(mCs);
            mSucceededCount++;
        }
    }

    fclose(sMergeFile);

    XPR_SAFE_DELETE_ARRAY(sTextA);
    XPR_SAFE_DELETE_ARRAY(sTextW);

    {
        CsLocker sLocker(mCs);
        mStatus = (IsStop() == XPR_TRUE) ? StatusStopped : StatusMergeCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)0, (LPARAM)0);

    return 0;
}

TextMerge::Status TextMerge::getStatus(xpr_size_t *aSucceededCount)
{
    CsLocker sLocker(mCs);

    if (XPR_IS_NOT_NULL(aSucceededCount)) *aSucceededCount = mSucceededCount;

    return mStatus;
}
} // namespace fxb
