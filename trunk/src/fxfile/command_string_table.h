//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_COMMAND_STRING_TABLE_H__
#define __FXFILE_COMMAND_STRING_TABLE_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class CommandStringTable : public fxfile::base::Singleton<CommandStringTable>
{
    friend class fxfile::base::Singleton<CommandStringTable>;

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
    typedef std::map<xpr_uint_t, xpr::tstring> CommandStringMap;
    CommandStringMap mCommandString;
    CommandStringMap mCommandButtonString;
};
} // namespace fxfile

#endif // __FXFILE_COMMAND_STRING_TABLE_H__
