//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CONF_DIR_H__
#define __FX_CONF_DIR_H__
#pragma once

#include "xpr_pattern.h"

class ConfDir : public xpr::Singleton<ConfDir>
{
    friend class xpr::Singleton<ConfDir>;

protected: ConfDir(void);
public:   ~ConfDir(void);

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
        TypeLauncher,
        TypeUpdater,
        TypeEnd,
    };

public:
    xpr_bool_t load(void);
    xpr_bool_t save(void) const;

    const xpr_tchar_t *getConfDir(void) const;
    void setConfDir(const xpr_tchar_t *aConfDir);

    void setBackup(void);
    const xpr_tchar_t *getOldConfDir(void) const;
    xpr_bool_t checkChangedConfDir(void);
    xpr_bool_t moveToNewConfDir(void);

public:
    xpr_bool_t getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL) const;
    xpr_bool_t getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL) const;
    xpr_bool_t getLoadDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getSaveDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;

    static const xpr_tchar_t *getDefConfDir(void);
    static const xpr_tchar_t *getProgramConfDir(void);

protected:
    xpr_bool_t getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL) const;
    xpr_bool_t getPath(xpr_sint_t aType, const xpr_tchar_t *aDir, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName = XPR_NULL) const;
    xpr_bool_t getDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getDir(const xpr_tchar_t *aConfDir, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;

    static const xpr_tchar_t *getFileName(xpr_sint_t aType);

    void clear(void);

protected:
    std::tstring mConfDir;
    std::tstring mOldConfDir;
};

#endif // __FX_CONF_DIR_H__
