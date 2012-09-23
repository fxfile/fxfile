//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_BB_INFO_FILE_H__
#define __FXB_BB_INFO_FILE_H__
#pragma once

namespace fxb
{
// BitBucket Info/Info2 File
class BBInfoFile
{
public:
    BBInfoFile(void);
    virtual ~BBInfoFile(void);

public:
    xpr_bool_t open(xpr_tchar_t aDrive);
    void close(void);

    xpr_size_t getCount(void);
    void getBBPath(xpr_tchar_t *aPath);
    xpr_bool_t isUnicode(void);
    xpr_tchar_t getDrive(void);

    xpr_bool_t findOriginalPath(xpr_tchar_t *aFind, FILETIME aCreatedTime, xpr_tchar_t *aBBIndexPath);
    xpr_bool_t getBBIndexPath(xpr_size_t aIndex, xpr_tchar_t *aBBIndexPath);

protected:
    xpr_bool_t getDataEntryAll(void);

protected:
    FILE        *mFile;
    xpr_tchar_t  mPath[XPR_MAX_PATH + 1];
    xpr_bool_t   mUnicode;

    struct BitBucketInfoHeader;
    struct BitBucketDataEntryA;
    struct BitBucketDataEntryW;

    std::deque<BitBucketDataEntryA *> mListA;
    std::deque<BitBucketDataEntryW *> mListW;
};
} // namespace fxb

#endif // __FXB_BB_INFO_FILE_H__
