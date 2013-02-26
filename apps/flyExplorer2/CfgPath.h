//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_PATH_H__
#define __FX_CFG_PATH_H__
#pragma once

#include "xpr_pattern.h"

class CfgPath : public xpr::Singleton<CfgPath>
{
    friend class xpr::Singleton<CfgPath>;

protected: CfgPath(void);
public:   ~CfgPath(void);

public:
    enum
    {
        TypeNone = 0,

        TypeAll,
        TypeBegin,

        TypeMain = TypeBegin,
        TypeConfig,
        TypeBookmark,
        TypeFileScrap,
        TypeSearchDir,
        TypeViewSet,
        TypeDlgState,
        TypeAccel,
        TypeCoolBar,
        TypeToolBar,
        TypeThumbnail,
        TypeEnd,
    };

public:
    xpr_bool_t load(void);
    xpr_bool_t save(void) const;

    const xpr_tchar_t *getRootDir(void) const;
    void setRootDir(const xpr_tchar_t *aDir);

    xpr_bool_t setBackup(xpr_bool_t aOldBackup);
    xpr_bool_t checkChangedCfgPath(void);
    void moveToNewCfgPath(void);

public:
    xpr_bool_t getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL) const;
    xpr_bool_t getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL) const;
    xpr_bool_t getLoadDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getSaveDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;

    static const xpr_tchar_t *getDefRootDir(void);

protected:
    xpr_bool_t getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL) const;
    xpr_bool_t getDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;

    static const xpr_tchar_t *getFileName(xpr_sint_t aType);

    void clear(void);

protected:
    typedef std::map<xpr_sint_t, std::tstring> CfgPathMap;
    std::tstring mDir;
    CfgPathMap   mBakOldCfgPathMap;
    CfgPathMap   mBakNewCfgPathMap;
};

#endif // __FX_CFG_PATH_H__
