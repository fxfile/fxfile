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
#include "format.h"

namespace fxfile
{
namespace cmd
{
#define FXFILE_BATCH_RENAME_FORMAT_MAX_LENGTH          (1024)
#define FXFILE_BATCH_RENAME_FORMAT_DEF                 XPR_STRING_LITERAL("<n>")
#define FXFILE_BATCH_RENAME_FORMAT_NUMBER_MIN          (0)
#define FXFILE_BATCH_RENAME_FORMAT_NUMBER_DEF          (1)
#define FXFILE_BATCH_RENAME_FORMAT_NUMBER_MAX          (ksint32max)
#define FXFILE_BATCH_RENAME_FORMAT_COUNT_MIN           (1)
#define FXFILE_BATCH_RENAME_FORMAT_COUNT_DEF           (1)
#define FXFILE_BATCH_RENAME_FORMAT_COUNT_MAX           (kuint32max)
#define FXFILE_BATCH_RENAME_FORMAT_INCREASE_MIN        (1)
#define FXFILE_BATCH_RENAME_FORMAT_INCREASE_DEF        (1)
#define FXFILE_BATCH_RENAME_FORMAT_INCREASE_MAX        (kuint32max)
#define FXFILE_BATCH_RENAME_FORMAT_DIGIT_MIN           (1)
#define FXFILE_BATCH_RENAME_FORMAT_DIGIT_DEF           (1)
#define FXFILE_BATCH_RENAME_FORMAT_DIGIT_MAX           (XPR_MAX_PATH)
#define FXFILE_BATCH_RENAME_REPLACE_OLD_MAX_LENGTH     (XPR_MAX_PATH)
#define FXFILE_BATCH_RENAME_REPLACE_NEW_MAX_LENGTH     (XPR_MAX_PATH)
#define FXFILE_BATCH_RENAME_REPLACE_REPEAT_MIN         (0)
#define FXFILE_BATCH_RENAME_REPLACE_REPEAT_DEF         (1)
#define FXFILE_BATCH_RENAME_REPLACE_REPEAT_MAX         (ksint32max)
#define FXFILE_BATCH_RENAME_INSERT_MAX_LENGTH          (XPR_MAX_PATH)
#define FXFILE_BATCH_RENAME_INSERT_POS_MIN             (0)
#define FXFILE_BATCH_RENAME_INSERT_POS_DEF             (0)
#define FXFILE_BATCH_RENAME_INSERT_POS_MAX             (XPR_MAX_PATH)
#define FXFILE_BATCH_RENAME_DELETE_POS_MIN             (0)
#define FXFILE_BATCH_RENAME_DELETE_POS_DEF             (0)
#define FXFILE_BATCH_RENAME_DELETE_POS_MAX             (XPR_MAX_PATH)
#define FXFILE_BATCH_RENAME_DELETE_LENGTH_MIN          (1)
#define FXFILE_BATCH_RENAME_DELETE_LENGTH_DEF          (1)
#define FXFILE_BATCH_RENAME_DELETE_LENGTH_MAX          (XPR_MAX_PATH)
#define FXFILE_BATCH_RENAME_CASE_SKIP_CHARS_MAX_LENGTH (XPR_MAX_PATH)

class BatchRename
{
public:
    typedef struct Item
    {
        xpr::string mDir;
        xpr::string mOld;
        xpr::string mNew;
        xpr_bool_t  mFolder;
    } Item;

    typedef std::deque<Item *> ItemDeque;

    enum Result
    {
        ResultSuccess,
        ResultError,
        ResultWrongFormat,
    };

    enum
    {
        FlagNoChangeExt    = 0x00000001,
        FlagResultRename   = 0x00000002,
        FlagHistoryArchive = 0x00000004,
    };

public:
    BatchRename(void);
    virtual ~BatchRename(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setBackupName(const xpr_tchar_t *aBackup);
    static void setString(const xpr_tchar_t *aExcessMaxLengthString,
                          const xpr_tchar_t *aMaxPathLengthString,
                          const xpr_tchar_t *aInvalidFileNameString);
    xpr_bool_t isFlag(xpr_uint_t aFlag) const;
    xpr_uint_t getFlags(void) const;
    void setFlags(xpr_uint_t aFlags);

public:
    void addItem(const xpr_tchar_t *aDir, const xpr_tchar_t *aOldName, const xpr_tchar_t *aNewName, xpr_bool_t aFolder);
    void addItem(Item *aItem);
    xpr_size_t getCount(void) const;
    const Item *getItem(xpr_size_t aIndex) const;
    Item *getItem(xpr_size_t aIndex);
    xpr_bool_t setItemName(xpr_size_t aIndex, const xpr_tchar_t *aName);
    xpr_bool_t moveItem(xpr_size_t aIndex1, xpr_size_t aIndex2);
    void clear(void);
    void reset(void);

public:
    Result     renameFormat(const xpr_tchar_t *aFormat,
                            xpr_uint_t         aNumber,
                            xpr_uint_t         aIncrease);

    xpr_bool_t renameReplace(const xpr_tchar_t *aOld,
                             const xpr_tchar_t *aNew,
                             xpr_sint_t         aRepeat,
                             xpr_bool_t         aCaseSensitivity);

    xpr_bool_t renameInsert(Format::InsertPosType  aPosType,
                            xpr_sint_t             aPos,
                            const xpr_tchar_t     *aInsert);

    xpr_bool_t renameDelete(Format::DeletePosType aPosType,
                            xpr_sint_t            aPos,
                            xpr_sint_t            aLength);

    xpr_bool_t renameCase(Format::CaseTargetType  aTargetType,
                          Format::CaseType        aCaseType,
                          const xpr_tchar_t      *aSkipChars);

public:
    xpr_size_t getBackwardCount(void) const;
    xpr_size_t getForwardCount(void) const;
    void goBackward(void);
    void goForward(void);
    void clearHistory(void);
    void clearBackward(void);
    void clearForward(void);

public:
    xpr_bool_t load(const xpr::string &aFilePath);
    xpr_bool_t save(const xpr::string &aFilePath) const;

public:
    xpr_bool_t start(void);
    void stop(void);

    MultiRename::Status getStatus(xpr_size_t *aPreparedCount = XPR_NULL, xpr_size_t *aValidatedCount = XPR_NULL, xpr_size_t *aRenamedCount = XPR_NULL) const;
    MultiRename::Result getItemResult(xpr_size_t aIndex) const;
    xpr_sint_t getInvalidItem(void) const;

private:
    struct History
    {
    public:
        History(FileNameFormat *aFileNameFormat)
            : mFileNameFormat(aFileNameFormat)
        {
        }

        ~History(void)
        {
            XPR_SAFE_DELETE(mFileNameFormat);
        }

    public:
        FileNameFormat *mFileNameFormat;
    };

    typedef std::deque<History *> HistoryDeque;

    xpr_bool_t rename(const FileNameFormat &aFileNameFormat, xpr_bool_t aAddHistory = XPR_TRUE);
    xpr_bool_t rename(const HistoryDeque &aHistoryDeque);

    void getFileNameFormatFromHistory(FileNameFormat &aFileNameFormat, const HistoryDeque &aHistoryDeque) const;

    void addHistory(const FileNameFormat &aFileNameFormat);

private:
    HWND         mHwnd;
    xpr_uint_t   mMsg;
    xpr_uint_t   mFlags;
    xpr::string  mBackup;

    ItemDeque    mItemDeque;
    MultiRename *mMultiRename;

    HistoryDeque mBackwardHistory;
    HistoryDeque mForwardHistory;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_BATCH_RENAME_H__
