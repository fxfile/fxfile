//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_parser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
FormatParser::FormatParser(void)
    : mTime(CTime::GetCurrentTime())
{
}

FormatParser::~FormatParser(void)
{
}

FormatParser::Result FormatParser::parse(const xpr_tchar_t *aOrgPath,
                                         const xpr_tchar_t *aPath,
                                         const xpr_tchar_t *aFileName,
                                         const xpr_tchar_t *aExt,
                                         const xpr_tchar_t *aFormat,
                                         xpr::tstring      &aDest,
                                         xpr_uint_t         aNumber,
                                         xpr_size_t         aMaxDestLen)
{
    xpr::tstring sOrgPath;
    xpr::tstring sPath;
    xpr::tstring sFileName;
    xpr::tstring sExt;
    xpr::tstring sFormat;

    if (XPR_IS_NOT_NULL(aOrgPath))  sOrgPath  = aOrgPath;
    if (XPR_IS_NOT_NULL(aPath))     sPath     = aPath;
    if (XPR_IS_NOT_NULL(aFileName)) sFileName = aFileName;
    if (XPR_IS_NOT_NULL(aExt))      sExt      = aExt;
    if (XPR_IS_NOT_NULL(aFormat))   sFormat   = aFormat;

    Result sResult;
    sResult = parse(sOrgPath, sPath, sFileName, sExt, sFormat, aDest, aNumber, aMaxDestLen);

    return sResult;
}

FormatParser::Result FormatParser::parse(const xpr::tstring &aFormat,
                                         xpr::tstring       &aDest,
                                         xpr_uint_t          aNumber,
                                         xpr_size_t          aMaxDestLen)
{
    static const xpr::tstring sOrgPath;
    static const xpr::tstring sPath;
    static const xpr::tstring sFileName;
    static const xpr::tstring sExt;

    Result sResult;
    sResult = parse(sOrgPath, sPath, sFileName, sExt, aFormat, aDest, aNumber, aMaxDestLen);

    return sResult;
}

FormatParser::Result FormatParser::parse(const xpr::tstring &aOrgPath,
                                         const xpr::tstring &aPath,
                                         const xpr::tstring &aFileName,
                                         const xpr::tstring &aExt,
                                         const xpr::tstring &aFormat,
                                         xpr::tstring       &aDest,
                                         xpr_uint_t          aNumber,
                                         xpr_size_t          aMaxDestLen)
{
    aDest.clear();

    static xpr_tchar_t sTemp[XPR_MAX_PATH * 2];
    sTemp[0] = '\0';

    xpr_sint_t i = 0;
    xpr_sint_t sFormatNumber = -1;
    xpr_sint_t sLen = (xpr_sint_t)aFormat.length();

    static xpr_tchar_t sTime[0xff] = {0};
    static FILETIME sCreatedFileTime = {0};
    static FILETIME sModifiedFileTime = {0};
    static SYSTEMTIME sCreatedSystemTime = {0};
    static SYSTEMTIME sModifiedSystemTime = {0};
    static HANDLE sFile = XPR_NULL;
    sCreatedFileTime.dwLowDateTime = 0;
    sModifiedFileTime.dwLowDateTime = 0;

    Result sResult = PARSING_FORMAT_SUCCEEDED;

    if (VerifyFileName(aFileName) == XPR_FALSE)
        sResult = PARSING_FORMAT_INVALID_FILENAME;

    if (VerifyFileName(aExt) == XPR_FALSE)
        sResult = PARSING_FORMAT_INVALID_FILENAME;

    if (sResult == PARSING_FORMAT_SUCCEEDED)
    {
        while (i < sLen)
        {
            if (aDest.length() > aMaxDestLen)
            {
                sResult = PARSING_FORMAT_EXCEED_LENGTH;
                break;
            }

            if (aFormat[i] == '/' && (i+1) < sLen)
            {
                if ((aFormat[i+1] == '0') && ((i+2) < sLen) && ('1' <= aFormat[i+2] && aFormat[i+2] <= '9'))
                {
                    sTemp[0] = aFormat[i+1];
                    sTemp[1] = aFormat[i+2];
                    sTemp[2] = '\0';
                    _stscanf(sTemp, XPR_STRING_LITERAL("%d"), &sFormatNumber);

                    sFormatNumber %= 10;

                    xpr_tchar_t fm[10];
                    _stprintf(fm, XPR_STRING_LITERAL("%%.%cd"), aFormat[i+2]);
                    _stprintf(sTemp, fm, aNumber);
                    aDest += sTemp;
                    i += 3;
                }
                else if ('1' <= aFormat[i+1] && aFormat[i+1] <= '9')
                {
                    sTemp[0] = aFormat[i+1];
                    sTemp[1] = '\0';
                    _stscanf(sTemp, XPR_STRING_LITERAL("%d"), &sFormatNumber);

                    sFormatNumber %= 10;

                    _stprintf(sTemp, XPR_STRING_LITERAL("%d"), aNumber);
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == '*')
                {
                    aDest += aPath.substr(aPath.find_last_of(XPR_STRING_LITERAL('\\'))+1);
                    i += 2;
                }
                else if (aFormat[i+1] == 'n')
                {
                    aDest += aFileName;
                    i += 2;
                }
                else if (aFormat[i+1] == 'N' && aFormat[i+2] == '(' && (i+6) < sLen)
                {
                    const xpr_tchar_t *sLastBrackets = _tcschr(aFormat.c_str() + i + 3, ')');
                    if (XPR_IS_NOT_NULL(sLastBrackets))
                    {
                        xpr_sint_t sLen = (xpr_sint_t)(sLastBrackets - (aFormat.c_str() + i) + 1);
                        const xpr_tchar_t *sTemp = _tcschr(aFormat.c_str() + i + 2, '-');
                        if (XPR_IS_NULL(sTemp))
                        {
                            sResult = PARSING_FORMAT_INCORRECT_FORMAT;
                            break;
                        }
                        sTemp++;
                        if (aFormat[i+3] < '0' || aFormat[i+3] > '9' || sTemp[0] < '0' || sTemp[0] > '9')
                        {
                            sResult = PARSING_FORMAT_INCORRECT_FORMAT;
                            break;
                        }

                        xpr_sint_t nStart = -1;
                        xpr_sint_t nEnd   = -1;
                        _stscanf(aFormat.c_str() + i + 3, XPR_STRING_LITERAL("%d-%d"), &nStart, &nEnd);
                        if (nStart <= 0 || nEnd <= 0 || nStart > nEnd)
                        {
                            sResult = PARSING_FORMAT_INCORRECT_FORMAT;
                            break;
                        }
                        nStart--;
                        nEnd--;

                        nEnd = min(nEnd, (xpr_sint_t)aFileName.length()-1);

                        aDest += aFileName.substr(nStart, nEnd-nStart+1);
                        i += sLen;
                    }
                    else
                    {
                        sResult = PARSING_FORMAT_INCORRECT_FORMAT;
                        break;
                    }
                }
                else if (aFormat[i+1] == 'e' || aFormat[i+1] == 'E')
                {
                    aDest += aExt;
                    i += 2;
                }
                else if (aFormat[i+1] == 'D') // current date
                {
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d-%d-%d"), mTime.GetYear(), mTime.GetMonth(), mTime.GetDay());
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == 'y')
                {
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d"), mTime.GetYear());
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == 'm')
                {
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d"), mTime.GetMonth());
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == 'd')
                {
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d"), mTime.GetDay());
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == 'T') // current time
                {
                    xpr_sint_t nHour = mTime.GetHour();
                    if (nHour > 12) nHour -= 12;
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d.%d.%d"), nHour, mTime.GetMinute(), mTime.GetSecond());
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == 'h')
                {
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d"), mTime.GetHour());
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == 'i')
                {
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d"), mTime.GetMinute());
                    aDest += sTemp;
                    i += 2;
                }
                else if (aFormat[i+1] == 's')
                {
                    _stprintf(sTemp, XPR_STRING_LITERAL("%d"), mTime.GetSecond());
                    aDest += sTemp;
                    i += 2;
                }
                else if ((!aOrgPath.empty()) && (
                    aFormat[i+1] == 'A' ||
                    aFormat[i+1] == 'a' ||
                    aFormat[i+1] == 'b' ||
                    aFormat[i+1] == 'c' ||
                    aFormat[i+1] == 'F' ||
                    aFormat[i+1] == 'f' ||
                    aFormat[i+1] == 'g' ||
                    aFormat[i+1] == 'k' ||
                    aFormat[i+1] == 'l' ||
                    aFormat[i+1] == 'r' ))
                {
                    if (sCreatedFileTime.dwLowDateTime == 0 && sModifiedFileTime.dwLowDateTime == 0)
                    {
                        sFile = ::CreateFile(aOrgPath.c_str(), 0, FILE_SHARE_READ, XPR_NULL, OPEN_ALWAYS, 0, XPR_NULL);
                        ::GetFileTime(sFile, &sCreatedFileTime, XPR_NULL, &sModifiedFileTime);
                        CLOSE_HANDLE(sFile);

                        ::FileTimeToLocalFileTime(&sCreatedFileTime, &sCreatedFileTime);
                        ::FileTimeToSystemTime(&sCreatedFileTime, &sCreatedSystemTime);

                        ::FileTimeToLocalFileTime(&sModifiedFileTime, &sModifiedFileTime);
                        ::FileTimeToSystemTime(&sModifiedFileTime, &sModifiedSystemTime);
                    }

                    if (aFormat[i+1] == 'A') // file created time
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("yyyy-MM-dd"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'a')
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("yyyy"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'b')
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("MM"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'c')
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("dd"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'F') // file modified time
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("tt hh.mm"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'f')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("tt"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'g')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("hh"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'k')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("HH"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'l')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("mm"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'r')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sCreatedSystemTime, XPR_STRING_LITERAL("ss"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                }
                else if ((!aOrgPath.empty()) && (
                    aFormat[i+1] == 'O' ||
                    aFormat[i+1] == 'o' ||
                    aFormat[i+1] == 'p' ||
                    aFormat[i+1] == 'q' ||
                    aFormat[i+1] == 'J' ||
                    aFormat[i+1] == 'j' ||
                    aFormat[i+1] == 'u' ||
                    aFormat[i+1] == 'w' ||
                    aFormat[i+1] == 'v' ||
                    aFormat[i+1] == 'z' ))
                {
                    if (sCreatedFileTime.dwLowDateTime == 0 && sModifiedFileTime.dwLowDateTime == 0)
                    {
                        sFile = ::CreateFile(aOrgPath.c_str(), 0, FILE_SHARE_READ, XPR_NULL, OPEN_ALWAYS, 0, XPR_NULL);
                        ::GetFileTime(sFile, &sCreatedFileTime, XPR_NULL, &sModifiedFileTime);
                        CLOSE_HANDLE(sFile);

                        ::FileTimeToLocalFileTime(&sCreatedFileTime, &sCreatedFileTime);
                        ::FileTimeToSystemTime(&sCreatedFileTime, &sCreatedSystemTime);

                        ::FileTimeToLocalFileTime(&sModifiedFileTime, &sModifiedFileTime);
                        ::FileTimeToSystemTime(&sModifiedFileTime, &sModifiedSystemTime);
                    }

                    if (aFormat[i+1] == 'O') // file modified date
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("yyyy-MM-dd"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'o')
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("yyyy"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'p')
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("MM"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'q')
                    {
                        ::GetDateFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("dd"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'J') // file modified time
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("tt hh.mm"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'j')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("tt"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'u')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("hh"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'w')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("HH"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'v')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("mm"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                    else if (aFormat[i+1] == 'z')
                    {
                        ::GetTimeFormat(XPR_NULL, 0, &sModifiedSystemTime, XPR_STRING_LITERAL("ss"), sTime, 0xfe);
                        aDest += sTime;
                        i += 2;
                    }
                }
                else
                {
                    sResult = PARSING_FORMAT_INCORRECT_FORMAT;
                    break;
                }
                continue;
            }
            else if (
                aFormat[i] == '\\' ||
                aFormat[i] == '/' ||
                aFormat[i] == ':' ||
                aFormat[i] == '*' ||
                aFormat[i] == '?' ||
                aFormat[i] == '\"' ||
                aFormat[i] == '<' ||
                aFormat[i] == '>' ||
                aFormat[i] == '|')
            {
                sResult = PARSING_FORMAT_INVALID_FILENAME;
                break;
            }
            aDest += aFormat[i++];
        }
    }

    return sResult;
}
} // namespace cmd
} // namespace fxfile
