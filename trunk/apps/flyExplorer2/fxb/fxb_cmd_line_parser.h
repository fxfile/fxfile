//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_CMD_LINE_PARSER_H__
#define __FXB_CMD_LINE_PARSER_H__
#pragma once

namespace fxb
{
class CmdLineParser : public xpr::Singleton<CmdLineParser>
{
    friend class xpr::Singleton<CmdLineParser>;

protected: CmdLineParser(void);
public:   ~CmdLineParser(void);

public:
    void parse(void);
    void clear(void);

    xpr_bool_t isExistArg(const std::tstring &aOption);
    xpr_bool_t getArg(const std::tstring &aOption, std::tstring &aValue);

protected:
    typedef std::map<std::tstring, std::tstring> ArgsMap;
    ArgsMap mArgsMap;
};
} // namespace fxb

#endif // __FXB_CMD_LINE_PARSER_H__
