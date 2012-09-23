//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_cmd_line_parser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
CmdLineParser::CmdLineParser(void)
{
}

CmdLineParser::~CmdLineParser(void)
{
    clear();
}

void CmdLineParser::parse(void)
{
    xpr_sint_t i;
    const xpr_tchar_t *sParam;
    const xpr_tchar_t *sSplit;
    std::tstring sOption;
    std::tstring sValue;
    xpr_bool_t sFlag;
    xpr_bool_t sLast;

    for (i = 1; i < __argc; ++i)
    {
        sParam = __targv[i];
        sFlag = XPR_FALSE;
        sLast = ((i + 1) == __argc);
        if (sParam[0] == XPR_STRING_LITERAL('-') || sParam[0] == XPR_STRING_LITERAL('/'))
        {
            // remove flag specifier
            sFlag = XPR_TRUE;
            ++sParam;
        }

        if (i == 1 && XPR_IS_FALSE(sFlag))
        {
            mArgsMap[XPR_STRING_LITERAL("1")] = sParam;
        }
        else
        {
            sOption = sParam;
            sValue.clear();

            sSplit = _tcschr(sParam, XPR_STRING_LITERAL('='));
            if (XPR_IS_NOT_NULL(sSplit))
            {
                sOption.erase(sSplit-sParam);
                sValue = sSplit+1;
            }

            fxb::upper(sOption);

            if (sOption.length() == 1)
            {
                if (sOption[0] == XPR_STRING_LITERAL('L')) sOption = XPR_STRING_LITERAL("1");
                if (sOption[0] == XPR_STRING_LITERAL('R')) sOption = XPR_STRING_LITERAL("2");
            }

            mArgsMap[sOption] = sValue;
        }
    }
}

void CmdLineParser::clear(void)
{
    mArgsMap.clear();
}

xpr_bool_t CmdLineParser::isExistArg(const std::tstring &aOption)
{
    return (mArgsMap.find(aOption) != mArgsMap.end());
}

xpr_bool_t CmdLineParser::getArg(const std::tstring &aOption, std::tstring &aValue)
{
    ArgsMap::iterator it = mArgsMap.find(aOption);
    if (it == mArgsMap.end())
        return XPR_FALSE;

    aValue = it->second;

    return XPR_TRUE;
}
} // namespace fxb
