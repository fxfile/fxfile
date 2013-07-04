//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BATCH_RENAME_H__
#define __FXFILE_BATCH_RENAME_H__ 1
#pragma once

#include "multi_rename.h"

namespace fxfile
{
namespace cmd
{
#define MAX_FORMAT       1024
#define MAX_BATCH_FORMAT 4096

typedef struct BatRenItem
{
    xpr::tstring mDir;
    xpr::tstring mOld;
    xpr::tstring mNew;
    xpr_bool_t   mFolder;
} BatRenItem;

typedef std::deque<BatRenItem *> BatRenDeque;

class BatchRename
{
public:
    enum
    {
        FlagsNoChangeExt        = 0x00000001,
        FlagsResultRename       = 0x00000002,
        FlagsBatchFormatArchive = 0x00000004,
    };

public:
    BatchRename(void);
    virtual ~BatchRename(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setBackupName(const xpr_tchar_t *aBackup);
    void setString(const xpr_tchar_t *aExcessMaxLengthString, const xpr_tchar_t *aMaxPathLengthString, const xpr_tchar_t *aWrongFormatString);

    void addItem(const xpr_tchar_t *aDir, const xpr_tchar_t *aOldName, const xpr_tchar_t *aNewName, xpr_bool_t aFolder);
    void addItem(BatRenItem *aBatRenItem);
    xpr_size_t getCount(void);
    BatRenItem *getItem(xpr_size_t aIndex);
    xpr_bool_t setItemName(xpr_size_t aIndex, const xpr_tchar_t *aName);
    xpr_bool_t moveItem(xpr_size_t aIndex1, xpr_size_t aIndex2);

    xpr_bool_t renameFormat(
        const xpr_tchar_t *aFormat,
        xpr_uint_t         aNumber,
        xpr_uint_t         aIncrease,
        xpr_bool_t         aNotReplaceExt,
        xpr_bool_t         aByResult);
    xpr_bool_t renameReplace(
        xpr_sint_t         aRepeat,
        xpr_bool_t         aCase,
        const xpr_tchar_t *aOld,
        const xpr_tchar_t *aNew,
        xpr_bool_t         aNotReplaceExt,
        xpr_bool_t         aByResult);
    xpr_bool_t renameInsert(
        xpr_sint_t         aType,
        xpr_sint_t         aPos,
        const xpr_tchar_t *aInsert,
        xpr_bool_t         aNotReplaceExt,
        xpr_bool_t         aByResult);
    void renameDelete(
        xpr_sint_t         aType,
        xpr_size_t         aPos,
        xpr_size_t         aLength,
        xpr_bool_t         aNotReplaceExt,
        xpr_bool_t         aByResult);
    void renameCase(
        xpr_sint_t         aType,
        xpr_sint_t         aCase,
        const xpr_tchar_t *aSkipSpecChar,
        xpr_bool_t         aByResult);

    xpr_bool_t addBatchFormat(const xpr_tchar_t *aBatchFormat);
    xpr_bool_t setBatchFormat(const xpr_tchar_t *aBatchFormat, xpr_bool_t aHistory = XPR_TRUE);
    xpr_bool_t setBatchFormat(const xpr::tstring &aBatchFormat, xpr_bool_t aHistory = XPR_TRUE);
    const xpr_tchar_t *getBatchFormat(void);
    void getBatchFormat(xpr::tstring &aBatchFormat);
    xpr_bool_t renameBatchFormat(const xpr_tchar_t *aFormat);
    void getBatchFormatFlags(xpr_uint_t aFlags, xpr::tstring &aBatchFormat);
    void setBatchFormatFlags(xpr_uint_t aFlags = 0xffffffff);
    void clearBatchFormat(void);

    xpr_bool_t isFlag(xpr_uint_t aFlag);
    xpr_uint_t getFlags(void);
    void setFlags(xpr_uint_t aFlags, xpr_bool_t aWriteBatchFormat = XPR_TRUE);

    void init(void);

    xpr_size_t getBackwardCount(void);
    xpr_size_t getForwardCount(void);
    xpr_bool_t goBackward(const xpr_tchar_t *aBatchFormat, xpr::tstring &aNewBatchFormat);
    xpr_bool_t goForward(const xpr_tchar_t *aBatchFormat, xpr::tstring &aNewBatchFormat);
    void clearBackward(void);
    void clearForward(void);

public:
    xpr_bool_t start(void);
    void stop(void);

    MultiRename::Status getStatus(xpr_size_t *aPreparedCount = XPR_NULL, xpr_size_t *aValidatedCount = XPR_NULL, xpr_size_t *aRenamedCount = XPR_NULL);
    MultiRename::Result getItemResult(xpr_size_t aIndex);
    xpr_sint_t getInvalidItem(void);

protected:
    void beginExtProc(xpr_bool_t aNotReplaceExt, xpr::tstring &aPath, xpr::tstring &aExt, xpr_bool_t aFolder);
    void endExtProc(xpr::tstring &aPath, const xpr::tstring &aExt);

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;

    xpr::tstring mExcessMaxLengthString;
    xpr::tstring mMaxPathLengthString;
    xpr::tstring mWrongFormatString;

    MultiRename *mMultiRename;

    BatRenDeque mBatRenDeque;

    xpr::tstring mBackup;
    xpr_uint_t mFlags;
    xpr_bool_t mInitBatchFormat;
    xpr::tstring mBatchFormat;

    typedef std::deque<xpr::tstring> HistoryDeque;
    HistoryDeque mBackwardDeque;
    HistoryDeque mForwardDeque;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_BATCH_RENAME_H__
