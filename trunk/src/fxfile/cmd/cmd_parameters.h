//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_PARAMETERS_H__
#define __FXFILE_CMD_PARAMETERS_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class CommandParameters
{
public:
    CommandParameters(void);
    ~CommandParameters(void);

public:
    void *get(xpr_sint_t aId);
    void *set(xpr_sint_t aId, void *aValue);

protected:
    typedef std::map<xpr_sint_t, void *> ParameterMap;
    ParameterMap mParameters;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_PARAMETERS_H__
