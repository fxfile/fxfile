//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_batch_rename.h"

#include "fxb_multi_rename.h"
#include "fxb_format_parser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
BatchRename::BatchRename(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mMultiRename(XPR_NULL)
    , mFlags(0)
    , mInitBatchFormat(XPR_TRUE)
{
}

BatchRename::~BatchRename(void)
{
    XPR_SAFE_DELETE(mMultiRename);

    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        XPR_SAFE_DELETE(sBatRenItem);
    }

    mBatRenDeque.clear();

    clearBackward();
    clearForward();
}

void BatchRename::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void BatchRename::setBackupName(const xpr_tchar_t *aBackup)
{
    if (XPR_IS_NOT_NULL(aBackup))
        mBackup = aBackup;
}

void BatchRename::setString(const xpr_tchar_t *aExcessMaxLengthString, const xpr_tchar_t *aMaxPathLengthString, const xpr_tchar_t *aWrongFormatString)
{
    if (XPR_IS_NULL(aExcessMaxLengthString)) mExcessMaxLengthString = aExcessMaxLengthString;
    if (XPR_IS_NULL(aMaxPathLengthString  )) mMaxPathLengthString   = aMaxPathLengthString;
    if (XPR_IS_NULL(aWrongFormatString    )) mWrongFormatString     = aWrongFormatString;
}

void BatchRename::addItem(const xpr_tchar_t *aDir, const xpr_tchar_t *aOldName, const xpr_tchar_t *aNewName, xpr_bool_t aFolder)
{
    if (XPR_IS_NULL(aDir) || XPR_IS_NULL(aOldName) || XPR_IS_NULL(aNewName))
        return;

    BatRenItem *sBatRenItem = new BatRenItem;
    if (XPR_IS_NULL(sBatRenItem))
        return;

    sBatRenItem->mDir    = aDir;
    sBatRenItem->mOld    = aOldName;
    sBatRenItem->mNew    = aNewName;
    sBatRenItem->mFolder = aFolder;

    addItem(sBatRenItem);
}

void BatchRename::addItem(BatRenItem *aBatRenItem)
{
    if (XPR_IS_NOT_NULL(aBatRenItem))
        mBatRenDeque.push_back(aBatRenItem);
}

xpr_size_t BatchRename::getCount(void)
{
    return mBatRenDeque.size();
}

BatRenItem *BatchRename::getItem(xpr_size_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mBatRenDeque))
        return XPR_NULL;

    return mBatRenDeque[aIndex];
}

xpr_bool_t BatchRename::setItemName(xpr_size_t aIndex, const xpr_tchar_t *aName)
{
    if (XPR_IS_NULL(aName))
        return XPR_FALSE;

    BatRenItem *sBatRenItem = getItem(aIndex);
    if (XPR_IS_NULL(sBatRenItem))
        return XPR_FALSE;

    sBatRenItem->mNew = aName;

    return XPR_TRUE;
}

xpr_bool_t BatchRename::moveItem(xpr_size_t aIndex1, xpr_size_t aIndex2)
{
    if (aIndex1 == aIndex2)
        return XPR_TRUE;

    BatRenItem *sBatRenItem1 = getItem(aIndex1);
    BatRenItem *sBatRenItem2 = getItem(aIndex2);

    if (XPR_IS_NULL(sBatRenItem1) || XPR_IS_NULL(sBatRenItem2))
        return XPR_FALSE;

    mBatRenDeque[aIndex1] = sBatRenItem2;
    mBatRenDeque[aIndex2] = sBatRenItem1;

    return XPR_TRUE;
}

xpr_bool_t BatchRename::renameFormat(const xpr_tchar_t *aFormat,
                                     xpr_uint_t         aNumber,
                                     xpr_uint_t         aIncrease,
                                     xpr_bool_t         aNotReplaceExt,
                                     xpr_bool_t         aByResult)
{
    if (XPR_IS_NULL(aFormat))
        return XPR_FALSE;

    std::tstring sFormat = aFormat;
    if (XPR_IS_TRUE(aNotReplaceExt))
    {
        sFormat += XPR_STRING_LITERAL("/e");
    }

    std::tstring sDir;
    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    CTime sTime = CTime::GetCurrentTime();

    std::tstring sOrgPath;
    std::tstring sOrgFile;

    std::tstring sOldPath;
    std::tstring sOldFile;
    std::tstring sNewFile;
    std::tstring sExt;
    xpr_tchar_t *sExtractExt;
    xpr_sint_t sMaxNewLen;
    FormatParser sFormatParser;
    FormatParser::Result sParseResult;
    xpr_tchar_t sError[0xff] = {0};

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        if (XPR_IS_NULL(sBatRenItem))
            continue;

        sOldFile = XPR_IS_TRUE(aByResult) ? sBatRenItem->mNew : sBatRenItem->mOld;
        sOrgFile = sBatRenItem->mOld;

        sOldPath = sOldFile;

        sDir = sBatRenItem->mDir;

        sExt.clear();
        if (XPR_IS_FALSE(sBatRenItem->mFolder))
        {
            sExtractExt = (xpr_tchar_t *)GetFileExt(sOldFile.c_str());
            if (XPR_IS_NOT_NULL(sExtractExt))
            {
                sExt = sExtractExt;
                sOldFile.erase(sOldFile.length()-sExt.length(), sExt.length());
            }
        }

        sOrgPath  = sDir;
        sOrgPath += sOrgFile;
        sMaxNewLen = (xpr_sint_t)(XPR_MAX_PATH - sDir.length());

        sParseResult = sFormatParser.parse(
            sOrgPath,
            sOldPath,
            sOldFile,
            sExt,
            sFormat,
            sNewFile,
            aNumber,
            sMaxNewLen);

        switch (sParseResult)
        {
        case FormatParser::PARSING_FORMAT_EXCEED_LENGTH:
            {
                _stprintf(sError, XPR_STRING_LITERAL("<error> - %s (%s: %d)"), mExcessMaxLengthString.c_str(), mMaxPathLengthString.c_str(), sMaxNewLen);
                sNewFile = sError;
                break;
            }

        case FormatParser::PARSING_FORMAT_INCORRECT_FORMAT:
        case FormatParser::PARSING_FORMAT_INVALID_FILENAME:
            {
                _stprintf(sError, XPR_STRING_LITERAL("<error> - %s(\\ / : * ? \" < > |)"), mWrongFormatString.c_str());
                sNewFile = sError;
                break;
            }
        }

        sBatRenItem->mNew = sNewFile;

        aNumber += aIncrease;
    }

    return XPR_TRUE;
}

xpr_bool_t BatchRename::renameReplace(xpr_sint_t         aRepeat,
                                      xpr_bool_t         aNotCase,
                                      const xpr_tchar_t *aOld,
                                      const xpr_tchar_t *aNew,
                                      xpr_bool_t         aNotReplaceExt,
                                      xpr_bool_t         aByResult)
{
    if (XPR_IS_NULL(aOld) || XPR_IS_NULL(aNew))
        return XPR_FALSE;

    if (aRepeat <= 0)
        aRepeat = XPR_MAX_PATH;

    std::tstring sOld = aOld;
    std::tstring sNew = aNew;

    std::tstring sDir;
    std::tstring sPath;
    std::tstring sExt;
    std::tstring sCase;
    std::tstring sTemp;
    xpr_sint_t sLenDiff = (xpr_sint_t)(sNew.length() - sOld.length());
    xpr_size_t sOffset, sFind;
    xpr_bool_t sFolder;
    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;
    xpr_sint_t sMaxNewLen;
    xpr_tchar_t sError[0xff] = {0};

    if (XPR_IS_TRUE(aNotCase))
        fxb::upper(sOld);

    xpr_sint_t j;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;

        sPath = XPR_IS_TRUE(aByResult) ? sBatRenItem->mNew : sBatRenItem->mOld;

        if (VerifyFileName(sPath) == XPR_TRUE)
        {
            sFolder = sBatRenItem->mFolder;
            sDir = sBatRenItem->mDir;
            sMaxNewLen = (xpr_sint_t)(XPR_MAX_PATH - sDir.length());

            beginExtProc(aNotReplaceExt, sPath, sExt, sFolder);

            sCase = sPath;
            if (XPR_IS_TRUE(aNotCase))
                fxb::upper(sCase);

            sOffset = 0;
            for (j = 0; j < aRepeat; ++j)
            {
                sFind = sCase.find(sOld, sOffset);
                if (sFind == std::tstring::npos)
                    break;

                sTemp = sPath.substr(sFind + j*sLenDiff + sOld.length());
                sPath.erase(sFind + j*sLenDiff);
                sPath += sNew;
                sPath += sTemp;

                sOffset = sFind + sOld.length();
            }

            endExtProc(sPath, sExt);

            if ((xpr_sint_t)sPath.length() >= sMaxNewLen)
            {
                _stprintf(sError, XPR_STRING_LITERAL("<error> - %s (%s: %d)"), mExcessMaxLengthString.c_str(), mMaxPathLengthString.c_str(), sMaxNewLen);
                sPath = sError;
            }
        }
        else
        {
            _stprintf(sError, XPR_STRING_LITERAL("<error> - %s(\\ / : * ? \" < > |)"), mWrongFormatString.c_str());
            sPath = sError;
        }

        sBatRenItem->mNew = sPath;
    }

    return XPR_TRUE;
}

xpr_bool_t BatchRename::renameInsert(xpr_sint_t         aType,
                                     xpr_sint_t         aPos,
                                     const xpr_tchar_t *aInsert,
                                     xpr_bool_t         aNotReplaceExt,
                                     xpr_bool_t         aByResult)
{
    if (XPR_IS_NULL(aInsert))
        return XPR_FALSE;

    std::tstring sDir;
    std::tstring sPath;
    std::tstring sExt;
    xpr_bool_t sFolder;
    xpr_sint_t sMaxNewLen;
    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        if (XPR_IS_NULL(sBatRenItem))
            continue;

        sPath = XPR_IS_TRUE(aByResult) ? sBatRenItem->mNew : sBatRenItem->mOld;

        if (VerifyFileName(sPath) == XPR_TRUE)
        {
            sFolder = sBatRenItem->mFolder;
            sDir = sBatRenItem->mDir;
            sMaxNewLen = (xpr_sint_t)(XPR_MAX_PATH - sDir.length());

            beginExtProc(aNotReplaceExt, sPath, sExt, sFolder);

            switch (aType)
            {
            case 0: // at first position
                sPath.insert(0, aInsert);
                break;

            case 1: // at last postition
                sPath.insert(sPath.length(), aInsert);
                break;

            case 3: // from last position
                sPath.insert(sPath.length() - aPos, aInsert);
                break;

            default: // from first position
            case 2:
                if (aPos < (xpr_sint_t)sPath.length())
                    sPath.insert(aPos, aInsert);
                break;
            }

            endExtProc(sPath, sExt);

            if ((xpr_sint_t)sPath.length() >= sMaxNewLen)
            {
                xpr_tchar_t sError[0xff] = {0};
                _stprintf(sError, XPR_STRING_LITERAL("<error> - %s (%s: %d)"), mExcessMaxLengthString.c_str(), mMaxPathLengthString.c_str(), sMaxNewLen);
                sPath = sError;
            }
        }
        else
        {
            xpr_tchar_t sError[0xff] = {0};
            _stprintf(sError, XPR_STRING_LITERAL("<error> - %s(\\ / : * ? \" < > |)"), mWrongFormatString.c_str());
            sPath = sError;
        }

        sBatRenItem->mNew = sPath;
    }

    return XPR_TRUE;
}

void BatchRename::beginExtProc(xpr_bool_t aNotReplaceExt, std::tstring &aPath, std::tstring &aExt, xpr_bool_t aFolder)
{
    aExt.clear();

    if (XPR_IS_TRUE(aNotReplaceExt) && XPR_IS_FALSE(aFolder))
    {
        const xpr_tchar_t *sFind = GetFileExt(aPath.c_str());
        if (XPR_IS_NOT_NULL(sFind))
        {
            aExt = sFind;
            aPath = aPath.substr(0, aPath.length() - _tcslen(sFind));
        }
    }
}

void BatchRename::endExtProc(std::tstring &aPath, const std::tstring &aExt)
{
    aPath += aExt;
}

void BatchRename::renameDelete(xpr_sint_t aType,
                               xpr_size_t aPos,
                               xpr_size_t aLength,
                               xpr_bool_t aNotReplaceExt,
                               xpr_bool_t aByResult)
{
    xpr_size_t sTextLen;
    xpr_bool_t sFolder;
    std::tstring sExt;
    std::tstring sText;

    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        if (XPR_IS_NULL(sBatRenItem))
            continue;

        sText = XPR_IS_TRUE(aByResult) ? sBatRenItem->mNew : sBatRenItem->mOld;

        sFolder = sBatRenItem->mFolder;

        beginExtProc(aNotReplaceExt, sText, sExt, sFolder);

        switch (aType)
        {
        case 0: // at first position
            sTextLen = sText.length();
            if (sTextLen >= aLength)
                sText.erase(0, aLength);
            break;

        case 1: // at last position
            sTextLen = sText.length();
            if (sTextLen >= aLength)
                sText.erase(sTextLen - aLength, aLength);
            break;

        case 3: // from last position
            sTextLen = sText.length();
            if ((sTextLen - aPos) >= (sTextLen - aLength))
                sText.erase(sTextLen - aPos, aLength);
            break;

        default:
        case 2: // from first position
            sTextLen = sText.length();
            if (sTextLen < (aPos + aLength))
            {
                if (sTextLen < aPos)
                    continue;
                sText.erase(aPos, sTextLen - aPos);
            }
            else
            {
                sText.erase(aPos, aLength);
            }
            break;
        }

        endExtProc(sText, sExt);

        sBatRenItem->mNew = sText;
    }
}

static inline xpr_bool_t isSpecChar(xpr_tchar_t aChar, const xpr_tchar_t *aSpecChar, xpr_sint_t aSpecCharLen)
{
    if (XPR_IS_NULL(aSpecChar) || aSpecCharLen <= 0)
        return XPR_FALSE;

    xpr_sint_t i;
    for (i = 0; i < aSpecCharLen; ++i)
    {
        if (aChar == aSpecChar[i])
            return XPR_TRUE;
    }

    return XPR_FALSE;
}

void BatchRename::renameCase(xpr_sint_t         aType,
                             xpr_sint_t         aCase,
                             const xpr_tchar_t *aSkipSpecChar,
                             xpr_bool_t         aByResult)
{
    if (XPR_IS_NULL(aSkipSpecChar))
        return;

    xpr_sint_t j, sLen, sSkipSpecCharLen, sPos;
    xpr_tchar_t sULCase[2] = { '\0', '\0' };
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    xpr_tchar_t sTemp[XPR_MAX_PATH + 1];
    xpr_tchar_t *sDot;
    xpr_bool_t sFirstChar;

    sSkipSpecCharLen = (xpr_sint_t)_tcslen(aSkipSpecChar);

    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        if (XPR_IS_NULL(sBatRenItem))
            continue;

        if (XPR_IS_TRUE(sBatRenItem->mFolder) && aType == 1)
            continue;

        _tcscpy(sText, XPR_IS_TRUE(aByResult) ? sBatRenItem->mNew.c_str() : sBatRenItem->mOld.c_str());

        sTemp[0] = '\0';
        switch (aType)
        {
        case 0:
            sDot = (xpr_tchar_t *)GetFileExt(sText);
            if (XPR_IS_NOT_NULL(sDot))
            {
                _tcscpy(sTemp, sDot);
                *sDot = '\0';
            }
            break;

        case 1:
            _tcscpy(sTemp, sText);
            sDot = (xpr_tchar_t *)GetFileExt(sTemp);
            if (XPR_IS_NOT_NULL(sDot))
            {
                sDot++;
                _tcscpy(sText, sDot);
                *sDot = XPR_STRING_LITERAL('\0');
            }
            else
            {
                sText[0] = XPR_STRING_LITERAL('\0');
            }
            break;

        default:
            break;
        }

        if (aCase == 2 || aCase == 3)
            _tcslwr(sText);

        switch (aCase)
        {
        default:
        case 0: _tcslwr(sText); break;
        case 1: _tcsupr(sText); break;

        case 2:
            {
                sPos = -1;

                if (sSkipSpecCharLen > 0)
                {
                    sLen = (xpr_sint_t)_tcslen(sText);
                    for (j = 0; j < sLen; ++j)
                    {
                        if (isSpecChar(sText[j], aSkipSpecChar, sSkipSpecCharLen) == XPR_FALSE)
                        {
                            sPos = j;
                            break;
                        }
                    }

                    if (sPos == -1)
                        break;
                }

                if (sPos == -1)
                    sPos = 0;

                sULCase[0] = sText[sPos];
                _tcsupr(sULCase);
                sText[sPos] = sULCase[0];
                break;
            }

        case 3:
            {
                sFirstChar = XPR_TRUE;

                sLen = (xpr_sint_t)_tcslen(sText) - 1;
                for (j = 0; j < sLen; ++j)
                {
                    if (isSpecChar(sText[j], aSkipSpecChar, sSkipSpecCharLen))
                    {
                        sFirstChar = XPR_TRUE;
                        continue;
                    }

                    if (j != 0 && sText[j] == XPR_STRING_LITERAL(' '))
                    {
                        sFirstChar = XPR_TRUE;
                        continue;
                    }

                    if (XPR_IS_TRUE(sFirstChar))
                    {
                        sULCase[0] = sText[j];
                        _tcsupr(sULCase);
                        sText[j] = sULCase[0];
                        sFirstChar = XPR_FALSE;
                    }
                }
                break;
            }
        }

        switch (aType)
        {
        case 0:
            _tcscat(sText, sTemp);
            break;

        case 1:
            _tcscat(sTemp, sText);
            _tcscpy(sText, sTemp);
            break;

        default:
            break;
        }

        sBatRenItem->mNew = sText;
    }
}

xpr_bool_t BatchRename::renameBatchFormat(const xpr_tchar_t *aFormat)
{
    if (XPR_IS_NULL(aFormat))
        return XPR_FALSE;

    std::deque<std::tstring> sBackupDeque;

    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        if (XPR_IS_NULL(sBatRenItem))
            continue;

        sBackupDeque.push_back(sBatRenItem->mNew);
    }

    xpr_sint_t sStart, sIncrease;
    xpr_sint_t sRepeat, sType, sPos, sLength, sCase;
    xpr_bool_t sNotCase;
    xpr_bool_t sNotReplaceExt = XPR_FALSE;
    xpr_bool_t sByResult = XPR_FALSE;

    xpr_tchar_t *sFormatRef;
    xpr_tchar_t sParsing[XPR_MAX_PATH + 1];
    xpr_tchar_t sParsing2[XPR_MAX_PATH + 1];

    xpr_sint_t i, j;
    xpr_sint_t sFormat;
    xpr_bool_t sAloneFormat;
    xpr_bool_t sNotBatchFormat = XPR_FALSE;
    xpr_sint_t sLen = (xpr_sint_t)_tcslen(aFormat);
    for (i = 0; i < sLen; )
    {
        if (aFormat[i] == ' ')
        {
            i++;
            continue;
        }

        if (aFormat[i] != '<')
        {
            sNotBatchFormat = XPR_TRUE;
            break;
        }
        i++;

        sFormat = 0;
        sAloneFormat = XPR_FALSE;
        switch (aFormat[i])
        {
        case '1': sFormat = 1; break;
        case '2': sFormat = 2; break;
        case '3': sFormat = 3; break;
        case '4': sFormat = 4; break;
        case '5': sFormat = 5; break;
        case 'e':
        case 'E': sAloneFormat = XPR_TRUE; sNotReplaceExt = XPR_FALSE; break;
        case 'n':
        case 'N': sAloneFormat = XPR_TRUE; sNotReplaceExt = XPR_TRUE;  break;
        case 'o':
        case 'O': sAloneFormat = XPR_TRUE; sByResult = XPR_FALSE;      break;
        case 'r':
        case 'R': sAloneFormat = XPR_TRUE; sByResult = XPR_TRUE;       break;
        }
        i += 2;

        if (sAloneFormat)
        {
            if (aFormat[i-1] != '>')
            {
                sNotBatchFormat = XPR_TRUE;
                break;
            }
            continue;
        }

        if (sFormat == 0)
        {
            sNotBatchFormat = XPR_TRUE;
            break;
        }

        for (j = 0; ; ++j)
        {
            sFormatRef = sParsing;
            while (aFormat[i] != ':' && aFormat[i] != '>')
            {
                if (i >= sLen) { sNotBatchFormat = XPR_TRUE; break; }
                *sFormatRef++ = aFormat[i++];
            }
            *sFormatRef = '\0';
            i++;

            if (sNotBatchFormat)
                break;

            if (sFormat == 1)
            {
                switch (j)
                {
                case 0: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sStart);    break;
                case 1: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sIncrease); break;
                }

                if (j == 2)
                {
                    renameFormat(sParsing, sStart, sIncrease, sNotReplaceExt, sByResult);
                    break;
                }
            }
            else if (sFormat == 2)
            {
                switch (j)
                {
                case 0: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sRepeat);  break;
                case 1: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sNotCase); break;
                case 2: _tcscpy(sParsing2, sParsing);            break;
                }

                if (j == 3)
                {
                    renameReplace(sRepeat, sNotCase, sParsing2, sParsing, sNotReplaceExt, sByResult);
                    break;
                }
            }
            else if (sFormat == 3)
            {
                switch (j)
                {
                case 0: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sType); break;
                case 1: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sPos);  break;
                }

                if (j == 2)
                {
                    renameInsert(sType, sPos, sParsing, sNotReplaceExt, sByResult);
                    break;
                }
            }
            else if (sFormat == 4)
            {
                switch (j)
                {
                case 0: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sType);   break;
                case 1: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sPos);    break;
                case 2: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sLength); break;
                }

                if (j == 2)
                {
                    renameDelete(sType, sPos, sLength, sNotReplaceExt, sByResult);
                    break;
                }
            }
            else if (sFormat == 5)
            {
                switch (j)
                {
                case 0: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sType); break;
                case 1: _stscanf(sParsing, XPR_STRING_LITERAL("%d"), &sCase); break;
                case 2: _tcscpy(sParsing2, sParsing);         break;
                }

                if (j == 2)
                {
                    renameCase(sType, sCase, sParsing2, sByResult);
                    break;
                }
            }
        }
    }

    sBackupDeque.clear();

    if (XPR_IS_TRUE(sNotBatchFormat))
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t BatchRename::addBatchFormat(const xpr_tchar_t *aBatchFormat)
{
    if (XPR_IS_NULL(aBatchFormat))
        return XPR_FALSE;

    if (XPR_IS_TRUE(mInitBatchFormat))
    {
        clearBatchFormat();
        setBatchFormatFlags();
        mInitBatchFormat = XPR_FALSE;
    }

    std::tstring sBatchFormat;
    sBatchFormat  = mBatchFormat;
    sBatchFormat += aBatchFormat;

    return setBatchFormat(sBatchFormat);
}

xpr_bool_t BatchRename::setBatchFormat(const std::tstring &aBatchFormat, xpr_bool_t aHistory)
{
    return setBatchFormat(aBatchFormat.c_str());
}

xpr_bool_t BatchRename::setBatchFormat(const xpr_tchar_t *aBatchFormat, xpr_bool_t aHistory)
{
    if (XPR_IS_NULL(aBatchFormat))
        return XPR_FALSE;

    if (_tcslen(aBatchFormat) >= MAX_BATCH_FORMAT)
        return XPR_FALSE;

    if (mBatchFormat.empty())
        setBatchFormatFlags();

    if (XPR_IS_TRUE(aHistory))
    {
        std::tstring sBatchFormat = mBatchFormat;

        if (XPR_IS_TRUE(mInitBatchFormat))
            getBatchFormatFlags(0xffffffff, sBatchFormat);

        mBackwardDeque.push_back(sBatchFormat);
    }

    mBatchFormat = aBatchFormat;

    if (XPR_IS_TRUE(aHistory))
    {
        clearForward();
        mInitBatchFormat = XPR_FALSE;
    }

    return XPR_TRUE;
}

const xpr_tchar_t *BatchRename::getBatchFormat(void)
{
    return mBatchFormat.c_str();
}

void BatchRename::getBatchFormat(std::tstring &aBatchFormat)
{
    aBatchFormat = mBatchFormat;
}

void BatchRename::getBatchFormatFlags(xpr_uint_t aFlags, std::tstring &aBatchFormat)
{
    aBatchFormat.clear();

    if (!XPR_TEST_BITS(mFlags, FlagsBatchFormatArchive))
        return;

    if (XPR_TEST_BITS(aFlags, FlagsNoChangeExt))  aBatchFormat += isFlag(FlagsNoChangeExt)  ? XPR_STRING_LITERAL("<n>") : XPR_STRING_LITERAL("<e>");
    if (XPR_TEST_BITS(aFlags, FlagsResultRename)) aBatchFormat += isFlag(FlagsResultRename) ? XPR_STRING_LITERAL("<r>") : XPR_STRING_LITERAL("<o>");
}

void BatchRename::setBatchFormatFlags(xpr_uint_t aFlags)
{
    if (!XPR_TEST_BITS(mFlags, FlagsBatchFormatArchive))
        return;

    std::tstring sBatchFormat;
    getBatchFormatFlags(aFlags, sBatchFormat);

    mBatchFormat += sBatchFormat;
}

void BatchRename::clearBatchFormat(void)
{
    mBatchFormat.clear();
}

xpr_size_t BatchRename::getBackwardCount(void)
{
    return mBackwardDeque.size();
}

xpr_size_t BatchRename::getForwardCount(void)
{
    return mForwardDeque.size();
}

xpr_bool_t BatchRename::goBackward(const xpr_tchar_t *aBatchFormat, std::tstring &aNewBatchFormat)
{
    if (XPR_IS_NULL(aBatchFormat))
        return XPR_FALSE;

    if (mBackwardDeque.empty())
        return XPR_FALSE;

    mForwardDeque.push_back(aBatchFormat);

    HistoryDeque::iterator sIterator = mBackwardDeque.end() - 1;
    aNewBatchFormat = *sIterator;

    mBackwardDeque.erase(sIterator);

    init();
    renameBatchFormat(aNewBatchFormat.c_str());

    mBatchFormat = aNewBatchFormat;

    return XPR_TRUE;
}

xpr_bool_t BatchRename::goForward(const xpr_tchar_t *aBatchFormat, std::tstring &aNewBatchFormat)
{
    if (XPR_IS_NULL(aBatchFormat))
        return XPR_FALSE;

    if (mForwardDeque.empty())
        return XPR_FALSE;

    mBackwardDeque.push_back(aBatchFormat);

    HistoryDeque::iterator sIterator = mForwardDeque.end() - 1;
    aNewBatchFormat = *sIterator;

    mForwardDeque.erase(sIterator);

    init();
    renameBatchFormat(aNewBatchFormat.c_str());

    mBatchFormat = aNewBatchFormat;

    return XPR_TRUE;
}

void BatchRename::clearBackward(void)
{
    mBackwardDeque.clear();
}

void BatchRename::clearForward(void)
{
    mForwardDeque.clear();
}

void BatchRename::init(void)
{
    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        if (XPR_IS_NULL(sBatRenItem))
            continue;

        sBatRenItem->mNew = sBatRenItem->mOld;
    }

    clearBatchFormat();

    setBatchFormatFlags();
}

xpr_bool_t BatchRename::isFlag(xpr_uint_t aFlag)
{
    return XPR_TEST_BITS(mFlags, aFlag);
}

xpr_uint_t BatchRename::getFlags(void)
{
    return mFlags;
}

void BatchRename::setFlags(xpr_uint_t aFlags, xpr_bool_t aWriteBatchFormat)
{
    xpr_uint_t sChangedFlags = mFlags ^ aFlags;
    mFlags = aFlags;

    if (XPR_IS_TRUE(aWriteBatchFormat))
    {
        if (mBatchFormat.empty())
        {
            setBatchFormatFlags();
        }
        else
        {
            if (XPR_TEST_BITS(sChangedFlags, FlagsNoChangeExt))  setBatchFormatFlags(FlagsNoChangeExt);
            if (XPR_TEST_BITS(sChangedFlags, FlagsResultRename)) setBatchFormatFlags(FlagsResultRename);
        }
    }
}

xpr_bool_t BatchRename::start(void)
{
    if (XPR_IS_NOT_NULL(mMultiRename))
    {
        MultiRename::Status sStatus = mMultiRename->getStatus();
        if (sStatus == MultiRename::StatusValidating || sStatus == MultiRename::StatusRenaming)
            return XPR_FALSE;
    }

    XPR_SAFE_DELETE(mMultiRename);

    mMultiRename = new MultiRename;
    if (XPR_IS_NULL(mMultiRename))
        return XPR_FALSE;

    mMultiRename->setOwner(mHwnd, mMsg);

    BatRenItem *sBatRenItem;
    BatRenDeque::iterator sIterator;

    sIterator = mBatRenDeque.begin();
    for (; sIterator != mBatRenDeque.end(); ++sIterator)
    {
        sBatRenItem = *sIterator;
        if (XPR_IS_NULL(sBatRenItem))
            continue;

        mMultiRename->addPath(sBatRenItem->mDir, sBatRenItem->mOld, sBatRenItem->mNew);
    }

    mMultiRename->setBackupName(mBackup.c_str());

    return mMultiRename->Start();
}

void BatchRename::stop(void)
{
    if (XPR_IS_NOT_NULL(mMultiRename))
        mMultiRename->Stop();
}

MultiRename::Status BatchRename::getStatus(xpr_size_t *aPreparedCount, xpr_size_t *aValidatedCount, xpr_size_t *aRenamedCount)
{
    if (XPR_IS_NULL(mMultiRename))
        return MultiRename::StatusNone;

    return mMultiRename->getStatus(aPreparedCount, aValidatedCount, aRenamedCount);
}

MultiRename::Result BatchRename::getItemResult(xpr_size_t aIndex)
{
    if (XPR_IS_NULL(mMultiRename))
        return MultiRename::ResultNone;

    return mMultiRename->getItemResult(aIndex);
}

xpr_sint_t BatchRename::getInvalidItem(void)
{
    if (XPR_IS_NULL(mMultiRename))
        return -1;

    return mMultiRename->getInvalidItem();
}
} // namespace fxb
