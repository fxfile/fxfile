//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DLG_STATE_MANAGER_H__
#define __FXFILE_DLG_STATE_MANAGER_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class DlgState;

class DlgStateManager : public fxfile::base::Singleton<DlgStateManager>
{
    friend class fxfile::base::Singleton<DlgStateManager>;

protected: DlgStateManager(void);
public:   ~DlgStateManager(void);

public:
    void setPath(const xpr_tchar_t *aPath);

    xpr_bool_t load(void);
    void save(void) const;

public:
    DlgState *getDlgState(const xpr_tchar_t *aSection, xpr_bool_t aCreateIfNotExist = XPR_TRUE);

    void clear(void);

protected:
    xpr::string mPath;
    xpr_bool_t  mLoaded;

    typedef std::tr1::unordered_map<xpr::string, DlgState *> DlgStateMap;
    DlgStateMap mDlgStateMap;
};
} // namespace fxfile

#endif // __FXFILE_DLG_STATE_MANAGER_H__
