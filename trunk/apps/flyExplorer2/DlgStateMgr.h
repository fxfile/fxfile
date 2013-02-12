//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DLG_STATE_MGR_H__
#define __FX_DLG_STATE_MGR_H__
#pragma once

#include "xpr_pattern.h"

class DlgState;

class DlgStateMgr : public xpr::Singleton<DlgStateMgr>
{
    friend class xpr::Singleton<DlgStateMgr>;

protected: DlgStateMgr(void);
public:   ~DlgStateMgr(void);

public:
    void setPath(const xpr_tchar_t *aPath);

    xpr_bool_t load(void);
    void save(void) const;

public:
    DlgState *getDlgState(const xpr_tchar_t *aSection, xpr_bool_t aCreateIfNotExist = XPR_TRUE);

    void clear(void);

protected:
    std::tstring mPath;
    xpr_bool_t   mLoaded;

    typedef std::tr1::unordered_map<std::tstring, DlgState *> DlgStateMap;
    DlgStateMap mDlgStateMap;
};

#endif // __FX_DLG_STATE_MGR_H__
