//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_LINE_PARSER_H__
#define __FX_CMD_LINE_PARSER_H__
#pragma once

class CmdLineParser
{
public:
    CmdLineParser(void);
    ~CmdLineParser(void);

public:
    void parse(void);
    void clear(void);

    BOOL isExistArg(const std::tstring &aOption);
    BOOL getArg(const std::tstring &aOption, std::tstring &aValue);

protected:
    typedef std::map<std::tstring, std::tstring> ArgsMap;
    ArgsMap mArgsMap;
};

#endif // __FX_CMD_LINE_PARSER_H__
