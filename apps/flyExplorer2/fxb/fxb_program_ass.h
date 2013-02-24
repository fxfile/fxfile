//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_PROGRAM_ASS_H__
#define __FXB_PROGRAM_ASS_H__
#pragma once

namespace fxb
{
#define MAX_PROGRAM_ASS      20

#define MAX_PROGRAM_ASS_NAME 40
#define MAX_PROGRAM_ASS_EXTS 260

class IniFileEx;

enum
{
    ProgramAssTypeNone   = 0x00000000,
    ProgramAssTypeViewer = 0x00000001,
    ProgramAssTypeEditor = 0x00000002,
};

enum
{
    ProgramAssMethodExt = 0,
    ProgramAssMethodFilter,
};

class ProgramAssItem
{
public:
    ProgramAssItem(void)
        : mType(ProgramAssTypeNone)
        , mMethod(ProgramAssMethodExt)
    {
    }

    void clear(void)
    {
        mName.clear();
        mType = ProgramAssTypeNone;
        mMethod = ProgramAssMethodExt;
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

typedef std::deque<ProgramAssItem *> ProgramAssDeque;

class ProgramAss
{
public:
    ProgramAss(void);
    virtual ~ProgramAss(void);

public:
    void addItem(ProgramAssItem *aProgramAssItem);
    ProgramAssItem *getItem(xpr_size_t aIndex);
    ProgramAssItem *getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType);
    ProgramAssItem *getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType);
    xpr_size_t indexFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType);
    xpr_size_t indexFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType);
    xpr_size_t getCount(void);
    void clear(void);

    xpr_bool_t getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType, ProgramAssDeque &aProgramAssDeque);
    xpr_bool_t getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType, ProgramAssDeque &aProgramAssDeque);

public:
    xpr_bool_t load(fxb::IniFileEx &aIniFile);
    void save(fxb::IniFileEx &aIniFile) const;

public:
    void initDefault(void);

protected:
    ProgramAssDeque mProgramAssDeque;
};

class ProgramAssMgr : public ProgramAss, public xpr::Singleton<ProgramAssMgr>
{
    friend class xpr::Singleton<ProgramAssMgr>;

protected: ProgramAssMgr(void);
public:   ~ProgramAssMgr(void);
};
} // namespace fxb

#endif // __FXB_PROGRAM_ASS_H__
