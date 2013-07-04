//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_LINE_PARSER_H__
#define __FXFILE_CMD_LINE_PARSER_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class CmdLineParser : public fxfile::base::Singleton<CmdLineParser>
{
    friend class fxfile::base::Singleton<CmdLineParser>;

protected: CmdLineParser(void);
public:   ~CmdLineParser(void);

public:
    void parse(void);
    void clear(void);

    xpr_bool_t isExistArg(const xpr::tstring &aOption);
    xpr_bool_t getArg(const xpr::tstring &aOption, xpr::tstring &aValue);

protected:
    typedef std::map<xpr::tstring, xpr::tstring> ArgsMap;
    ArgsMap mArgsMap;
};
} // namespace fxfile

#endif // __FXFILE_CMD_LINE_PARSER_H__
