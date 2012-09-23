//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CmdLineParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

template <typename T>
void upper(T &str)
{
    transform(str.begin(), str.end(), str.begin(), toupper);
}

CmdLineParser::CmdLineParser(void)
{
}

CmdLineParser::~CmdLineParser(void)
{
    clear();
}

void CmdLineParser::parse(void)
{
    int i;
    LPCTSTR sParam;
    LPCTSTR sSplit;
    std::tstring sOption;
    std::tstring sValue;
    BOOL sFlag;
    BOOL sLast;

    for (i=1; i<__argc; i++)
    {
        sParam = __targv[i];
        sFlag = FALSE;
        sLast = ((i + 1) == __argc);
        if (sParam[0] == _T('-') || sParam[0] == _T('/'))
        {
            // remove flag specifier
            sFlag = TRUE;
            ++sParam;
        }

        if (i == 1 && !sFlag)
        {
            mArgsMap[_T("1")] = sParam;
        }
        else
        {
            sOption = sParam;
            sValue.clear();

            sSplit = _tcschr(sParam, _T('='));
            if (sSplit != NULL)
            {
                sOption.erase(sSplit-sParam);
                sValue = sSplit+1;
            }

            upper(sOption);

            if (sOption.length() == 1)
            {
                if (sOption[0] == _T('L')) sOption = _T("1");
                if (sOption[0] == _T('R')) sOption = _T("2");
            }

            mArgsMap[sOption] = sValue;
        }
    }
}

void CmdLineParser::clear(void)
{
    mArgsMap.clear();
}

BOOL CmdLineParser::isExistArg(const std::tstring &aOption)
{
    return (mArgsMap.find(aOption) != mArgsMap.end());
}

BOOL CmdLineParser::getArg(const std::tstring &aOption, std::tstring &aValue)
{
    ArgsMap::iterator sIterator = mArgsMap.find(aOption);
    if (sIterator == mArgsMap.end())
        return FALSE;

    aValue = sIterator->second;

    return TRUE;
}
