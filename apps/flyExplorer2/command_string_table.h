//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_COMMAND_STRING_TABLE_H__
#define __FX_COMMAND_STRING_TABLE_H__
#pragma once

#include "xpr_pattern.h"

class CommandStringTable : public xpr::Singleton<CommandStringTable>
{
    friend class xpr::Singleton<CommandStringTable>;

protected: CommandStringTable(void);
public:   ~CommandStringTable(void);

public:
    void load(void);
    const xpr_tchar_t *loadString(xpr_uint_t aId);
    const xpr_tchar_t *loadButtonString(xpr_uint_t aId);

protected:
    void loadCommands(void);
    void loadCommandButtons(void);

protected:
    typedef std::map<xpr_uint_t, std::tstring> CommandStringMap;
    CommandStringMap mCommandString;
    CommandStringMap mCommandButtonString;
};

#endif // __FX_COMMAND_STRING_TABLE_H__
