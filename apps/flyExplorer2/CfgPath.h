//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
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
        TypeFileAss,
        TypeFilter,
        TypeSearchDir,
        TypeSizeFmt,
        TypeViewSet,
        TypeSettings,
        TypeAccel,
        TypeCoolBar,
        TypeToolBar,
        TypeBarState,
        TypeThumbnail,
        TypeRecent,
        TypeHistory,
        TypeEnd,
    };

public:
    xpr_bool_t getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL);
    xpr_bool_t getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL);
    xpr_bool_t getLoadDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen);
    xpr_bool_t getSaveDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen);
    xpr_bool_t getLoadDir(xpr_sint_t aType, std::tstring &aDir);
    xpr_bool_t getSaveDir(xpr_sint_t aType, std::tstring &aDir);

public:
    void clear(void);
    xpr_bool_t getCfgPath(xpr_sint_t aType, std::tstring &aCfgPath);
    xpr_bool_t setCfgPath(xpr_sint_t aType, const xpr_tchar_t *aPath);

    xpr_bool_t setBackup(xpr_bool_t aOldBackup);
    xpr_bool_t isChangedCfgPath(void);
    void moveToNewCfgPath(void);

    xpr_bool_t isTypeAll(void);

    static const xpr_tchar_t *getDispName(xpr_sint_t aType);
    static const xpr_tchar_t *getFileName(xpr_sint_t aType);

public:
    xpr_bool_t load(void);
    xpr_bool_t save(void);

protected:
    xpr_bool_t getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL);
    xpr_bool_t getDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen);
    xpr_bool_t getDir(xpr_sint_t aType, std::tstring &aDir);

    static const xpr_tchar_t *getDefPath(xpr_sint_t aType);
    static const xpr_tchar_t *getDefSubPath(xpr_sint_t aType);
    static const xpr_tchar_t *getEntry(xpr_sint_t aType);

public:
    typedef std::map<xpr_sint_t, std::tstring> CfgPathMap;
    CfgPathMap mCfgPathMap;
    CfgPathMap mBakOldCfgPathMap;
    CfgPathMap mBakNewCfgPathMap;
};

#endif // __FX_CFG_PATH_H__
