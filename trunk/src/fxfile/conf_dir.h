//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CONF_DIR_H__
#define __FXFILE_CONF_DIR_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class ConfDir : public fxfile::base::Singleton<ConfDir>
{
    friend class fxfile::base::Singleton<ConfDir>;

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
        TypeThumbnailData,
        TypeThumbnailIndex,
        TypeLauncher,
        TypeUpdater,
        TypeEnd,
    };

public:
    xpr_bool_t load(void);
    xpr_bool_t save(void) const;

    const xpr_tchar_t *getConfDir(void) const;
    void setConfDir(const xpr_tchar_t *aConfDir, xpr_bool_t aReadOnly = XPR_FALSE);

    void setBackup(void);
    const xpr_tchar_t *getOldConfDir(void) const;
    xpr_bool_t checkChangedConfDir(void);
    xpr_bool_t moveToNewConfDir(void);

public:
    xpr_bool_t getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const;
    xpr_bool_t getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const;
    xpr_bool_t getLoadDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getSaveDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;

    static const xpr_tchar_t *getDefConfDir(void);
    static const xpr_tchar_t *getProgramConfDir(void);

protected:
    xpr_bool_t getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const;
    xpr_bool_t getPath(xpr_sint_t aType, const xpr_tchar_t *aDir, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const;
    xpr_bool_t getDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getDir(const xpr_tchar_t *aConfDir, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const;

    static const xpr_tchar_t *getFileName(xpr_sint_t aType);

    void clear(void);

protected:
    xpr::tstring mConfDir;
    xpr::tstring mOldConfDir;
    xpr_bool_t   mReadOnly;
};
} // namespace fxfile

#endif // __FXFILE_CONF_DIR_H__
