//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_ASS_H__
#define __FXB_FILE_ASS_H__
#pragma once

namespace fxb
{
#define MAX_FILE_ASS      20

#define MAX_FILE_ASS_NAME 40
#define MAX_FILE_ASS_EXTS 260

enum
{
    FileAssTypeNone   = 0x00000000,
    FileAssTypeViewer = 0x00000001,
    FileAssTypeEditor = 0x00000002,
};

enum
{
    FileAssMethodExt = 0,
    FileAssMethodFilter,
};

class FileAssItem
{
public:
    FileAssItem(void)
        : mType(FileAssTypeNone)
        , mMethod(FileAssMethodExt)
    {
    }

    void clear(void)
    {
        mName.clear();
        mType = FileAssTypeNone;
        mMethod = FileAssMethodExt;
        mExts.clear();
        mFilterName.clear();
        mPath.clear();
    }

public:
    std::tstring mName;
    xpr_uint_t   mType;
    xpr_uint_t   mMethod;
    std::tstring mExts;
    std::tstring mFilterName;
    std::tstring mPath;
};

typedef std::deque<FileAssItem *> FileAssDeque;

class FileAss
{
public:
    FileAss(void);
    virtual ~FileAss(void);

public:
    void addItem(FileAssItem *aFileAssItem);
    FileAssItem *getItem(xpr_size_t aIndex);
    FileAssItem *getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType);
    FileAssItem *getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType);
    xpr_size_t indexFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType);
    xpr_size_t indexFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType);
    xpr_size_t getCount(void);
    void clear(void);

    xpr_bool_t getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType, FileAssDeque &aFileAssDeque);
    xpr_bool_t getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType, FileAssDeque &aFileAssDeque);

public:
    xpr_bool_t loadFromFile(const xpr_tchar_t *aPath);
    xpr_bool_t saveToFile(const xpr_tchar_t *aPath);

public:
    void initDefault(void);

protected:
    FileAssDeque mFileAssDeque;
};

class FileAssMgr : public FileAss, public xpr::Singleton<FileAssMgr>
{
    friend class xpr::Singleton<FileAssMgr>;

protected: FileAssMgr(void);
public:   ~FileAssMgr(void);
};
} // namespace fxb

#endif // __FXB_FILE_ASS_H__
