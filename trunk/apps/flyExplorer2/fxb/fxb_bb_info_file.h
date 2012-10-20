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
class BBInfoFile
{
public:
    BBInfoFile(void);
    virtual ~BBInfoFile(void);

public:
    xpr_bool_t open(xpr_tchar_t aDriveChar);
    void close(void);

    xpr_size_t getCount(void);
    void getDirPath(xpr_tchar_t *aDirPath);
    xpr_tchar_t getDrive(void);

    xpr_bool_t findOriginalPath(const xpr_tchar_t *aOriginalFilePath, FILETIME aCreatedTime, xpr_tchar_t *aRestoreFilePath);

protected:
    void readINFO2Index(void);
    void readVistaIndex(void);

protected:
    xpr_tchar_t mDirPath[XPR_MAX_PATH + 1];

    struct BitBucketInfoHeader;
    struct BitBucketDataEntryA;
    struct BitBucketDataEntryW;
    struct Index;

    typedef std::deque<Index *> IndexDeque;
    IndexDeque mIndexDeque;
};
} // namespace fxb

#endif // __FXB_BB_INFO_FILE_H__
