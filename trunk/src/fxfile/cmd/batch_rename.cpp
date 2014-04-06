//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename.h"
#include "multi_rename.h"
#include "format.h"
#include "format_short_parser.h"
#include "format_name.h"
#include "format_replace.h"
#include "format_insert.h"
#include "format_delete.h"
#include "format_case.h"
#include "format_ext_remove.h"
#include "format_clear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
namespace
{
void getLocalTime(SYSTEMTIME &aTimeNow)
{
    xpr::TimeExpr sNowTimeExpr;
    xpr::getLocalTimeNow(sNowTimeExpr);

    memset(&aTimeNow, 0, sizeof(aTimeNow));
    aTimeNow.wYear         = sNowTimeExpr.mYear;
    aTimeNow.wMonth        = sNowTimeExpr.mMonth;
    aTimeNow.wDay          = sNowTimeExpr.mDay;
    aTimeNow.wHour         = sNowTimeExpr.mHour;
    aTimeNow.wMinute       = sNowTimeExpr.mMinute;
    aTimeNow.wSecond       = sNowTimeExpr.mSecond;
    aTimeNow.wMilliseconds = sNowTimeExpr.getMillisecond();
}
} // namespace anonymous

BatchRename::BatchRename(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mMultiRename(XPR_NULL)
    , mFlags(0)
{
}

BatchRename::~BatchRename(void)
{
    clear();
}

void BatchRename::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void BatchRename::setBackupName(const xpr_tchar_t *aBackup)
{
    if (XPR_IS_NOT_NULL(aBackup))
        mBackup = aBackup;
}

void BatchRename::setString(const xpr_tchar_t *aExcessMaxLengthString,
                            const xpr_tchar_t *aMaxPathLengthString,
                            const xpr_tchar_t *aInvalidFileNameString)
{
    FileNameFormat::setString(aExcessMaxLengthString, aMaxPathLengthString, aInvalidFileNameString);
}

void BatchRename::addItem(const xpr_tchar_t *aDir, const xpr_tchar_t *aOldName, const xpr_tchar_t *aNewName, xpr_bool_t aFolder)
{
    XPR_ASSERT(aDir != XPR_NULL);
    XPR_ASSERT(aOldName != XPR_NULL);
    XPR_ASSERT(aNewName != XPR_NULL);

    Item *sItem    = new Item;
    sItem->mDir    = aDir;
    sItem->mOld    = aOldName;
    sItem->mNew    = aNewName;
    sItem->mFolder = aFolder;

    addItem(sItem);
}

void BatchRename::addItem(Item *aItem)
{
    XPR_ASSERT(aItem != XPR_NULL);

    mItemDeque.push_back(aItem);
}

xpr_size_t BatchRename::getCount(void) const
{
    return mItemDeque.size();
}

const BatchRename::Item *BatchRename::getItem(xpr_size_t aIndex) const
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mItemDeque))
        return XPR_NULL;

    return mItemDeque[aIndex];
}

BatchRename::Item *BatchRename::getItem(xpr_size_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mItemDeque))
        return XPR_NULL;

    return mItemDeque[aIndex];
}

xpr_bool_t BatchRename::setItemName(xpr_size_t aIndex, const xpr_tchar_t *aName)
{
    if (XPR_IS_NULL(aName))
        return XPR_FALSE;

    Item *sItem = getItem(aIndex);
    if (XPR_IS_NULL(sItem))
        return XPR_FALSE;

    sItem->mNew = aName;

    return XPR_TRUE;
}

xpr_bool_t BatchRename::moveItem(xpr_size_t aIndex1, xpr_size_t aIndex2)
{
    if (aIndex1 == aIndex2)
        return XPR_TRUE;

    Item *sItem1 = getItem(aIndex1);
    Item *sItem2 = getItem(aIndex2);

    if (XPR_IS_NULL(sItem1) || XPR_IS_NULL(sItem2))
        return XPR_FALSE;

    mItemDeque[aIndex1] = sItem2;
    mItemDeque[aIndex2] = sItem1;

    return XPR_TRUE;
}

void BatchRename::clear(void)
{
    XPR_SAFE_DELETE(mMultiRename);

    Item *sItem;
    ItemDeque::iterator sIterator;

    sIterator = mItemDeque.begin();
    for (; sIterator != mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        XPR_SAFE_DELETE(sItem);
    }

    mItemDeque.clear();

    clearBackward();
    clearForward();
}

void BatchRename::reset(void)
{
    Item *sItem;
    ItemDeque::iterator sIterator;

    sIterator = mItemDeque.begin();
    for (; sIterator != mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        sItem->mNew = sItem->mOld;
    }
}

BatchRename::Result BatchRename::renameFormat(const xpr_tchar_t *aFormat,
                                              xpr_uint_t         aNumber,
                                              xpr_uint_t         aIncrease)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    FormatSequence *sFormatSequence;
    FileNameFormat  sFileNameFormat;
    ShortFormatParser &sShortFormatParser = SingletonManager::get<ShortFormatParser>();

    sFormatSequence = new FormatSequence;

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatExtRemove);
    }

    sFormatSequence->add(new FormatClear);

    if (sShortFormatParser.parse(aFormat, *sFormatSequence) == XPR_FALSE)
    {
        XPR_SAFE_DELETE(sFormatSequence);

        return ResultWrongFormat;
    }

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatOrgFileExt);
    }

    sFileNameFormat.add(sFormatSequence);

    xpr_bool_t sSuccess = rename(sFileNameFormat);

    return XPR_IS_TRUE(sSuccess) ? ResultSuccess : ResultError;
}

xpr_bool_t BatchRename::renameReplace(const xpr_tchar_t *aOld,
                                      const xpr_tchar_t *aNew,
                                      xpr_sint_t         aRepeat,
                                      xpr_bool_t         aCaseSensitivity)
{
    XPR_ASSERT(aOld != XPR_NULL);
    XPR_ASSERT(aNew != XPR_NULL);

    FormatSequence *sFormatSequence;
    FileNameFormat  sFileNameFormat;

    sFormatSequence = new FormatSequence;

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatExtRemove);
    }

    sFormatSequence->add(new FormatReplace(aOld, aNew, aRepeat, aCaseSensitivity));

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatOrgFileExt);
    }

    sFileNameFormat.add(sFormatSequence);

    return rename(sFileNameFormat);
}

xpr_bool_t BatchRename::renameInsert(Format::InsertPosType  aPosType,
                                     xpr_sint_t             aPos,
                                     const xpr_tchar_t     *aInsert)
{
    XPR_ASSERT(aInsert != XPR_NULL);

    FormatSequence *sFormatSequence;
    FileNameFormat  sFileNameFormat;

    sFormatSequence = new FormatSequence;

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatExtRemove);
    }

    sFormatSequence->add(new FormatInsert(aPosType, aPos, aInsert));

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatOrgFileExt);
    }

    sFileNameFormat.add(sFormatSequence);

    return rename(sFileNameFormat);
}

xpr_bool_t BatchRename::renameDelete(Format::DeletePosType aPosType,
                                     xpr_sint_t            aPos,
                                     xpr_sint_t            aLength)
{
    FormatSequence *sFormatSequence;
    FileNameFormat  sFileNameFormat;

    sFormatSequence = new FormatSequence;

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatExtRemove);
    }

    sFormatSequence->add(new FormatDelete(aPosType, aPos, aLength));

    if (XPR_IS_TRUE(isFlag(FlagNoChangeExt)))
    {
        sFormatSequence->add(new FormatOrgFileExt);
    }

    sFileNameFormat.add(sFormatSequence);

    return rename(sFileNameFormat);
}

xpr_bool_t BatchRename::renameCase(Format::CaseTargetType  aTargetType,
                                   Format::CaseType        aCaseType,
                                   const xpr_tchar_t      *aSkipChars)
{
    XPR_ASSERT(aSkipChars != XPR_NULL);

    FormatSequence *sFormatSequence;
    FileNameFormat  sFileNameFormat;

    sFormatSequence = new FormatSequence;
    sFormatSequence->add(new FormatCase(aTargetType, aCaseType, aSkipChars));

    sFileNameFormat.add(sFormatSequence);

    return rename(sFileNameFormat);
}

xpr_bool_t BatchRename::rename(const FileNameFormat &aFileNameFormat, xpr_bool_t aAddHistory)
{
    xpr_sint_t   i = 0;
    xpr_size_t   sMaxNewLen;
    xpr_bool_t   sByResult = isFlag(FlagResultRename);
    xpr_bool_t   sAtLeastOneError = XPR_FALSE;
    xpr::tstring sOrgFilePath;
    SYSTEMTIME   sNowTime = {0,};
    Item        *sItem;
    ItemDeque::iterator sIterator;

    getLocalTime(sNowTime);

    FXFILE_STL_FOR_EACH(sIterator, mItemDeque)
    {
        sItem = *sIterator;

        XPR_ASSERT(sItem != XPR_NULL);

        sOrgFilePath  = sItem->mDir;
        sOrgFilePath += XPR_FILE_SEPARATOR;
        sOrgFilePath += sItem->mOld;

        sMaxNewLen = XPR_MAX_PATH - sItem->mDir.length() - 1;

        RenameContext sContext(sItem->mFolder,
                               sOrgFilePath,
                               sItem->mOld,
                               XPR_IS_TRUE(sByResult) ? sItem->mNew : sItem->mOld,
                               i,
                               sMaxNewLen,
                               sNowTime);

        if (XPR_IS_FALSE(aFileNameFormat.rename(sContext)))
        {
            sAtLeastOneError = XPR_TRUE;
        }

        sItem->mNew = sContext.mNewFileName;

        ++i;
    }

    if (XPR_IS_TRUE(aAddHistory))
    {
        addHistory(aFileNameFormat);
    }

    return sAtLeastOneError;
}

xpr_bool_t BatchRename::rename(const HistoryDeque &aHistoryDeque)
{
    FileNameFormat sFileNameFormat;
    getFileNameFormatFromHistory(sFileNameFormat, aHistoryDeque);

    return rename(sFileNameFormat, XPR_FALSE);
}

void BatchRename::getFileNameFormatFromHistory(FileNameFormat &aFileNameFormat, const HistoryDeque &aHistoryDeque) const
{
    xpr_size_t      i, sCount;
    History        *sHistory;
    FormatSequence *sFormatSequence;
    FormatSequence *sNewFormatSequence;
    HistoryDeque::const_iterator sIterator;

    aFileNameFormat.clear();

    FXFILE_STL_FOR_EACH(sIterator, aHistoryDeque)
    {
        sHistory = *sIterator;

        XPR_ASSERT(sHistory != XPR_NULL);
        XPR_ASSERT(sHistory->mFileNameFormat!= XPR_NULL);

        sCount = sHistory->mFileNameFormat->getCount();
        for (i = 0; i < sCount; ++i)
        {
            sFormatSequence = sHistory->mFileNameFormat->get(i);

            XPR_ASSERT(sFormatSequence != XPR_NULL);

            sNewFormatSequence = sFormatSequence->newClone();

            aFileNameFormat.add(sNewFormatSequence);
        }
    }
}

void BatchRename::addHistory(const FileNameFormat &aFileNameFormat)
{
    FileNameFormat *sFileNameFormat = aFileNameFormat.newClone();

    mBackwardHistory.push_back(new History(sFileNameFormat));
}

xpr_size_t BatchRename::getBackwardCount(void) const
{
    return mBackwardHistory.size();
}

xpr_size_t BatchRename::getForwardCount(void) const
{
    return mForwardHistory.size();
}

void BatchRename::goBackward(void)
{
    xpr_size_t sCount = mBackwardHistory.size();
    if (sCount == 0)
    {
        return;
    }

    History *sHistory = mBackwardHistory.back();
    mBackwardHistory.pop_back();

    mForwardHistory.push_back(sHistory);

    reset();

    rename(mBackwardHistory);
}

void BatchRename::goForward(void)
{
    xpr_size_t sCount = mForwardHistory.size();
    if (sCount == 0)
    {
        return;
    }

    History *sHistory = mForwardHistory.back();
    mForwardHistory.pop_back();

    mBackwardHistory.push_back(sHistory);

    reset();

    rename(mBackwardHistory);
}

void BatchRename::clearHistory(void)
{
    clearBackward();
    clearForward();
}

void BatchRename::clearBackward(void)
{
    History *sHistory;
    HistoryDeque::const_iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mBackwardHistory)
    {
        sHistory = *sIterator;

        XPR_ASSERT(sHistory != XPR_NULL);

        XPR_SAFE_DELETE(sHistory);
    }

    mBackwardHistory.clear();
}

void BatchRename::clearForward(void)
{
    History *sHistory;
    HistoryDeque::const_iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mForwardHistory)
    {
        sHistory = *sIterator;

        XPR_ASSERT(sHistory != XPR_NULL);

        XPR_SAFE_DELETE(sHistory);
    }

    mForwardHistory.clear();
}

xpr_bool_t BatchRename::isFlag(xpr_uint_t aFlag) const
{
    return XPR_TEST_BITS(mFlags, aFlag);
}

xpr_uint_t BatchRename::getFlags(void) const
{
    return mFlags;
}

void BatchRename::setFlags(xpr_uint_t aFlags)
{
    mFlags = aFlags;
}

xpr_bool_t BatchRename::start(void)
{
    if (XPR_IS_NOT_NULL(mMultiRename))
    {
        MultiRename::Status sStatus = mMultiRename->getStatus();
        if (sStatus == MultiRename::StatusValidating || sStatus == MultiRename::StatusRenaming)
            return XPR_FALSE;
    }

    XPR_SAFE_DELETE(mMultiRename);

    mMultiRename = new MultiRename;
    if (XPR_IS_NULL(mMultiRename))
        return XPR_FALSE;

    mMultiRename->setOwner(mHwnd, mMsg);

    Item *sItem;
    ItemDeque::iterator sIterator;

    sIterator = mItemDeque.begin();
    for (; sIterator != mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        mMultiRename->addPath(sItem->mDir, sItem->mOld, sItem->mNew);
    }

    mMultiRename->setBackupName(mBackup.c_str());

    return mMultiRename->Start();
}

void BatchRename::stop(void)
{
    if (XPR_IS_NOT_NULL(mMultiRename))
        mMultiRename->Stop();
}

MultiRename::Status BatchRename::getStatus(xpr_size_t *aPreparedCount, xpr_size_t *aValidatedCount, xpr_size_t *aRenamedCount) const
{
    if (XPR_IS_NULL(mMultiRename))
        return MultiRename::StatusNone;

    return mMultiRename->getStatus(aPreparedCount, aValidatedCount, aRenamedCount);
}

MultiRename::Result BatchRename::getItemResult(xpr_size_t aIndex) const
{
    if (XPR_IS_NULL(mMultiRename))
        return MultiRename::ResultNone;

    return mMultiRename->getItemResult(aIndex);
}

xpr_sint_t BatchRename::getInvalidItem(void) const
{
    if (XPR_IS_NULL(mMultiRename))
        return -1;

    return mMultiRename->getInvalidItem();
}

xpr_bool_t BatchRename::load(const xpr::tstring &aFilePath)
{
    FileNameFormat sFileNameFormat;
    if (sFileNameFormat.load(aFilePath) == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    rename(sFileNameFormat);

    return XPR_TRUE;
}

xpr_bool_t BatchRename::save(const xpr::tstring &aFilePath) const
{
    FileNameFormat sFileNameFormat;
    getFileNameFormatFromHistory(sFileNameFormat, mBackwardHistory);

    return sFileNameFormat.save(aFilePath);
}
} // namespace cmd
} // namespace fxfile
