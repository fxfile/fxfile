//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_EVN_PATH_H__
#define __FXFILE_EVN_PATH_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
#define CSIDL_NONE          0xffffffff
#define CSIDL_FXFILE        0x80000000
#define CSIDL_FXDRIVE       0x80000001
#define CSIDL_WINDRIVE      0x80000002
#define CSIDL_LIBRARIES     0x80000004

class EnvPath : public fxfile::base::Singleton<EnvPath>
{
    friend class fxfile::base::Singleton<EnvPath>;

protected: EnvPath(void);
public:   ~EnvPath(void);

protected:
    void addSpec(const xpr_tchar_t *aSpec, xpr_uint_t aCSIDL);

public:
    LPITEMIDLIST getPidl(const xpr::tstring &aSpec, xpr_uint_t *aCSIDL = XPR_NULL);
    xpr_bool_t getPidl(const xpr::tstring &aSpec, LPITEMIDLIST *aFullPidl, xpr_uint_t *aCSIDL = XPR_NULL);
    xpr_bool_t getPath(const xpr::tstring &aSpec, xpr::tstring &aPath, xpr_uint_t *aCSIDL = XPR_NULL);

protected:
    typedef std::map<xpr::tstring, xpr_uint_t> SpecMap;
    SpecMap mSpecMap;
};
} // namespace fxfile

#endif // __FXFILE_EVN_PATH_H__
