//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_PARAMETERS_H__
#define __FX_CMD_PARAMETERS_H__
#pragma once

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

#endif // __FX_CMD_PARAMETERS_H__
