//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_EVN_PATH_H__
#define __FXFILE_BASE_EVN_PATH_H__
#pragma once

#include "pattern.h"
#include "shell_item.h"

namespace fxfile
{
namespace base
{
#define CSIDL_NONE          0xffffffff
#define CSIDL_FXFILE        0x80000008
#define CSIDL_FXFILE_DRIVE  0x80000001
#define CSIDL_WINDRIVE      0x80000002
#define CSIDL_LIBRARIES     0x80000004

class EnvPath : public Singleton<EnvPath>
{
    friend class Singleton<EnvPath>;

protected: EnvPath(void);
public:   ~EnvPath(void);

public:
    xpr_bool_t getPidl(const xpr::tstring &aSpec, ShellItem *aShellItem, xpr_uint_t *aCSIDL = XPR_NULL) const;
    xpr_bool_t getPath(const xpr::tstring &aSpec, xpr::tstring &aPath, xpr_uint_t *aCSIDL = XPR_NULL) const;

    void resolve(const xpr::tstring &aEnvPath, xpr::tstring &aResolvedPath) const;

protected:
    void addSpec(const xpr_tchar_t *aSpec, xpr_uint_t aCSIDL);

protected:
    typedef std::tr1::unordered_map<xpr::tstring, xpr_uint_t> SpecMap;
    SpecMap mSpecMap;
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_EVN_PATH_H__
