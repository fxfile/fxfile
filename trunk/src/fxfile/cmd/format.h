//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_H__
#define __FXFILE_FORMAT_H__ 1
#pragma once

#include "xml.h"

namespace fxfile
{
namespace cmd
{
struct RenameContext
{
public:
    RenameContext(xpr_bool_t         aFolder,
                  const xpr::string &aOrgFilePath,
                  const xpr::string &aOrgFileName,
                  const xpr::string &aOldFileName,
                  xpr_sint_t         aIndex,
                  xpr_size_t         aMaxLen,
                  const SYSTEMTIME  &aNowTime)
        : mFolder(aFolder)
        , mOrgFilePath(aOrgFilePath)
        , mOrgFileName(aOrgFileName)
        , mOldFileName(aOldFileName)
        , mIndex(aIndex)
        , mMaxLen(aMaxLen)
        , mNowTime(aNowTime)
    {
        memset(&mCTime, 0, sizeof(SYSTEMTIME));
        memset(&mMTime, 0, sizeof(SYSTEMTIME));
    }

public:
    const SYSTEMTIME &getCTime(void) const
    {
        if (mCTime.wYear == 0)
        {
            getDate();
        }

        return mCTime;
    }

    const SYSTEMTIME &getMTime(void) const
    {
        if (mMTime.wYear == 0)
        {
            getDate();
        }

        return mMTime;
    }

    static void getBaseFileNameAndExt(xpr_bool_t         aFolder,
                                      const xpr::string &aFilePath,
                                      xpr::string       &aBaseFileName,
                                      xpr::string       &aFileExt)
    {
        if (XPR_IS_TRUE(aFolder))
        {
            aBaseFileName = aFilePath;
            aFileExt.clear();
        }
        else
        {
            xpr_size_t sOffset = aFilePath.rfind(XPR_FILE_EXT_SEPARATOR);
            if (sOffset != xpr::string::npos)
            {
                aBaseFileName = aFilePath.substr(0, sOffset);
                aFileExt      = aFilePath.substr(sOffset);
            }
            else
            {
                aBaseFileName = aFilePath;
                aFileExt.clear();
            }
        }
    }

private:
    void getDate(void) const
    {
        WIN32_FIND_DATA sWin32FindData = {0,};

        HANDLE sFindFile = ::FindFirstFile(mOrgFilePath.c_str(), &sWin32FindData);
        if (sFindFile != INVALID_HANDLE_VALUE)
        {
            FILETIME sLocalCreatedFileTime  = {0,};
            FILETIME sLocalModifiedFileTime = {0,};

            ::FileTimeToLocalFileTime(&sWin32FindData.ftCreationTime, &sLocalCreatedFileTime);
            ::FileTimeToSystemTime(&sLocalCreatedFileTime, &mCTime);

            ::FileTimeToLocalFileTime(&sWin32FindData.ftLastWriteTime, &sLocalModifiedFileTime);
            ::FileTimeToSystemTime(&sLocalModifiedFileTime, &mMTime);

            ::FindClose(sFindFile);
        }
    }

public:
    const xpr_bool_t   mFolder;
    const xpr::string &mOrgFilePath;   // original file path
    const xpr::string &mOrgFileName;   // original file name with extension
    xpr::string        mOldFileName;   // old file name with extension
    const xpr_uint_t   mIndex;
    xpr::string        mNewFileName;
    const xpr_size_t   mMaxLen;

    const SYSTEMTIME   &mNowTime;
    mutable SYSTEMTIME  mCTime;
    mutable SYSTEMTIME  mMTime;
};

class Format
{
public:
    enum InsertPosType
    {
        InsertPosTypeAtFirst,
        InsertPosTypeAtLast,
        InsertPosTypeFromFirst,
        InsertPosTypeFromLast,
    };

    enum DeletePosType
    {
        DeletePosTypeAtFirst,
        DeletePosTypeAtLast,
        DeletePosTypeFromFirst,
        DeletePosTypeFromLast,
    };

    enum CaseTargetType
    {
        CaseTargetTypeBaseFileName,
        CaseTargetTypeFileExt,
        CaseTargetTypeFileName,
    };

    enum CaseType
    {
        CaseTypeLower,
        CaseTypeUpper,
        CaseTypeUpperAtFirstChar,
        CaseTypeUpperAtFirstCharOnEveryWord,
    };

public:
    virtual void rename(RenameContext &aContext) const = 0;

public:
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement) = 0;
    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const = 0;

public:
    virtual Format *newClone(void) const = 0;
};

class FormatSequence
{
public:
    FormatSequence(void);
    virtual ~FormatSequence(void);

public:
    void add(Format *aFormat);
    void addFront(Format *aFormat);
    const Format *get(xpr_size_t aIndex) const;
    Format *get(xpr_size_t aIndex);
    xpr_bool_t isEmpty(void) const;
    xpr_size_t getCount(void) const;
    void clear(void);

public:
    void rename(RenameContext &aContext) const;
    void clone(FormatSequence &aFormatSequence) const;
    FormatSequence *newClone(void) const;

public:
    xpr_bool_t read(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);
    xpr_bool_t write(base::XmlWriter &aXmlWriter) const;

private:
    typedef std::deque<Format *> FormatDeque;
    FormatDeque mFormatDeque;
};

class FileNameFormat
{
public:
    FileNameFormat(void);
    virtual ~FileNameFormat(void);

public:
    void add(FormatSequence *aFormatSequence);
    const FormatSequence *get(xpr_size_t aIndex) const;
    FormatSequence *get(xpr_size_t aIndex);
    xpr_bool_t isEmpty(void) const;
    xpr_size_t getCount(void) const;
    FormatSequence *remove(xpr_size_t aIndex);
    void clear(void);

public:
    xpr_bool_t rename(RenameContext &aContext) const;
    void clone(FileNameFormat &aFileNameFormat) const;
    FileNameFormat *newClone(void) const;

public:
    xpr_bool_t load(const xpr::string &aFilePath);
    xpr_bool_t save(const xpr::string &aFilePath) const;

public:
    static void setString(const xpr_tchar_t *aExcessMaxLengthString,
                          const xpr_tchar_t *aMaxPathLengthString,
                          const xpr_tchar_t *aInvalidFileNameString);

private:
    typedef std::deque<FormatSequence *> FormatSequenceDeque;
    FormatSequenceDeque mFormatSequenceDeque;

    static const xpr_tchar_t *mExcessMaxLengthString;
    static const xpr_tchar_t *mMaxPathLengthString;
    static const xpr_tchar_t *mInvalidFileNameString;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_H__
