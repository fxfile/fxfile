//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_EVN_PATH_H__
#define __FXB_EVN_PATH_H__
#pragma once

#include "xpr_pattern.h"

namespace fxb
{
#define CSIDL_NONE          0xffffffff
#define CSIDL_FLYEXPLORER   0x80000000
#define CSIDL_FXDRIVE       0x80000001
#define CSIDL_WINDRIVE      0x80000002
#define CSIDL_LIBRARIES     0x80000003

class EnvPath : public xpr::Singleton<EnvPath>
{
    friend class xpr::Singleton<EnvPath>;

protected: EnvPath(void);
public:   ~EnvPath(void);

protected:
    void addSpec(const xpr_tchar_t *aSpec, xpr_uint_t aCSIDL);

public:
    LPITEMIDLIST getPidl(const std::tstring &aSpec, xpr_uint_t *aCSIDL = XPR_NULL);
    xpr_bool_t getPidl(const std::tstring &aSpec, LPITEMIDLIST *aFullPidl, xpr_uint_t *aCSIDL = XPR_NULL);
    xpr_bool_t getPath(const std::tstring &aSpec, std::tstring &aPath, xpr_uint_t *aCSIDL = XPR_NULL);

protected:
    typedef std::map<std::tstring, xpr_uint_t> SpecMap;
    SpecMap mSpecMap;
};
} // namespace fxb

#endif // __FXB_EVN_PATH_H__
