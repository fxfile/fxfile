//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_case.h"
#include "xpr_file_sys.h"
#include "xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
namespace
{
const Format::CaseTargetType kDefaultTargetType = Format::CaseTargetTypeBaseFileName;
const Format::CaseType       kDefaultCaseType   = Format::CaseTypeLower;

const xpr_tchar_t kXmlFormatCaseElement  [] = XPR_STRING_LITERAL("FormatCase");
const xpr_tchar_t kXmlTargetTypeAttribute[] = XPR_STRING_LITERAL("TargetType");
const xpr_tchar_t kXmlCaseTypeAttribute  [] = XPR_STRING_LITERAL("CaseType");
const xpr_tchar_t kXmlSkipCharsElement   [] = XPR_STRING_LITERAL("SkipChars");

const xpr_tchar_t kXmlBaseFileNameValue  [] = XPR_STRING_LITERAL("BaseFileName");
const xpr_tchar_t kXmlFileExtValue       [] = XPR_STRING_LITERAL("FileExt");
const xpr_tchar_t kXmlFileNameValue      [] = XPR_STRING_LITERAL("FileName");

const xpr_tchar_t kXmlLowerValue                      [] = XPR_STRING_LITERAL("Lower");
const xpr_tchar_t kXmlUpperValue                      [] = XPR_STRING_LITERAL("Upper");
const xpr_tchar_t kXmlUpperAtFirstCharValue           [] = XPR_STRING_LITERAL("UpperAtFirstChar");
const xpr_tchar_t kXmlUpperAtFirstCharOnEveryWordValue[] = XPR_STRING_LITERAL("UpperAtFirstCharOnEveryWord");
} // namespace anonymous

FormatCase::FormatCase(void)
    : mTargetType(kDefaultTargetType), mCaseType(kDefaultCaseType)
{
}

FormatCase::FormatCase(CaseTargetType aTargetType, CaseType aCaseType)
    : mTargetType(aTargetType), mCaseType(aCaseType)
{
}

FormatCase::FormatCase(CaseTargetType aTargetType, CaseType aCaseType, const xpr::string &aSkipChars)
    : mTargetType(aTargetType), mCaseType(aCaseType), mSkipChars(aSkipChars)
{
}

FormatCase::~FormatCase(void)
{
}

void FormatCase::rename(RenameContext &aContext) const
{
    xpr::string sBaseFileName;
    xpr::string sExtWithoutDot;

    RenameContext::getBaseFileNameAndExt(aContext.mFolder, aContext.mNewFileName, sBaseFileName, sExtWithoutDot);
    if (sExtWithoutDot.empty() == XPR_FALSE)
    {
        sExtWithoutDot.erase(0, 1);
    }

    switch (mTargetType)
    {
    case CaseTargetTypeBaseFileName:
        aContext.mNewFileName = sBaseFileName;
        break;

    case CaseTargetTypeFileExt:
        aContext.mNewFileName = sExtWithoutDot;
        break;

    default:
        break;
    }

    switch (mCaseType)
    {
    case CaseTypeLower:
        aContext.mNewFileName.lower_case();
        break;

    case CaseTypeUpper:
        aContext.mNewFileName.upper_case();
        break;

    case CaseTypeUpperAtFirstChar:
        convertUpperAtFirstChar(aContext.mNewFileName, mSkipChars);
        break;

    case CaseTypeUpperAtFirstCharOnEveryWord:
        convertUpperAtFirstCharOnEveryWord(aContext.mNewFileName, mSkipChars);
        break;

    default:
        break;
    }

    switch (mTargetType)
    {
    case CaseTargetTypeBaseFileName:
        if (sExtWithoutDot.empty() == XPR_FALSE)
        {
            aContext.mNewFileName += XPR_FILE_EXT_SEPARATOR;
            aContext.mNewFileName += sExtWithoutDot;
        }
        break;

    case CaseTargetTypeFileExt:
        if (aContext.mNewFileName.empty() == XPR_FALSE)
        {
            aContext.mNewFileName.insert(0, 1, XPR_FILE_EXT_SEPARATOR);
        }
        aContext.mNewFileName.insert(0, sBaseFileName);
        break;

    default:
        break;
    }
}

void FormatCase::convertUpperAtFirstChar(xpr::string &aName, const xpr::string &aSkipChars)
{
    if (aName.empty() == XPR_TRUE)
    {
        return;
    }

    aName.lower_case();

    xpr_size_t sOffset = aName.find_first_not_of(aSkipChars, 0);
    if (sOffset == xpr::string::npos)
    {
        return;
    }

    if (aName.length() == sOffset)
    {
        return;
    }

    xpr::string sChar(1, aName[sOffset]);
    sChar.upper_case();

    aName.replace(sOffset, 1, sChar);
}

void FormatCase::convertUpperAtFirstCharOnEveryWord(xpr::string &aName, const xpr::string &aSkipChars)
{
    if (aName.empty() == XPR_TRUE)
    {
        return;
    }

    xpr_size_t  sOffset = 0;
    xpr::string sSkipChars(aSkipChars);
    xpr::string sChar;

    sSkipChars += XPR_MBCS_STRING_LITERAL(' ');

    aName.lower_case();

    while (true)
    {
        sOffset = aName.find_first_not_of(sSkipChars, sOffset);
        if (sOffset == xpr::string::npos)
        {
            break;
        }

        sChar = aName[sOffset];
        sChar.upper_case();

        aName.replace(sOffset, 1, sChar);

        sOffset = aName.find_first_of(sSkipChars, sOffset);
        if (sOffset == xpr::string::npos)
        {
            break;
        }
    }
}

xpr_bool_t FormatCase::canParseXml(const xpr::string &aElementName)
{
    return (aElementName.compare(kXmlFormatCaseElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatCase::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::string    sName;
    xpr::string    sValue;
    CaseTargetType sTargetType = kDefaultTargetType;
    CaseType       sCaseType   = kDefaultCaseType;
    xpr::string    sSkipChars;

    base::XmlReader::Attribute *sAttribute = aXmlReader.getFirstAttribute(aElement);
    while (XPR_IS_NOT_NULL(sAttribute))
    {
        if (aXmlReader.getAttribute(sAttribute, sName, sValue) == XPR_FALSE)
        {
            return XPR_FALSE;
        }

        if (sName.compare(kXmlTargetTypeAttribute) == 0)
        {
            toType(sValue, sTargetType);
        }
        else if (sName.compare(kXmlCaseTypeAttribute) == 0)
        {
            toType(sValue, sCaseType);
        }

        sAttribute = aXmlReader.nextAttribute(sAttribute);
    }

    base::XmlReader::Element *sFormatElement = aXmlReader.childElement(aElement);
    while (XPR_IS_NOT_NULL(sFormatElement))
    {
        if (aXmlReader.getElementEntity(sFormatElement, sName, sValue) == XPR_FALSE)
        {
            return XPR_FALSE;
        }

        if (sName.compare(kXmlSkipCharsElement) == 0)
        {
            sSkipChars = sValue;
        }

        sFormatElement = aXmlReader.nextElement(sFormatElement);
    }

    mTargetType = sTargetType;
    mCaseType   = sCaseType;
    mSkipChars  = sSkipChars;

    return XPR_TRUE;
}

xpr_bool_t FormatCase::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatCaseElement) == XPR_TRUE)
    {
        xpr::string sTargetTypeString;
        xpr::string sCaseTypeString;

        toString(mTargetType, sTargetTypeString);
        toString(mCaseType,   sCaseTypeString);

        FXFILE_XML_WRITE_ATTRIBUTE_STRING(aXmlWriter, kXmlTargetTypeAttribute, sTargetTypeString);
        FXFILE_XML_WRITE_ATTRIBUTE_STRING(aXmlWriter, kXmlCaseTypeAttribute,   sCaseTypeString);

        FXFILE_XML_WRITE_ELEMENT_ENTITY_STRING(aXmlWriter, kXmlSkipCharsElement, mSkipChars);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

Format *FormatCase::newObject(void)
{
    return new FormatCase;
}

Format *FormatCase::newClone(void) const
{
    Format *sNewFormat = new FormatCase(mTargetType, mCaseType, mSkipChars);

    return sNewFormat;
}

void FormatCase::toString(const CaseTargetType &aTargetType, xpr::string &aString)
{
    switch (aTargetType)
    {
    case CaseTargetTypeBaseFileName: aString = kXmlBaseFileNameValue; break;
    case CaseTargetTypeFileExt:      aString = kXmlFileExtValue;      break;
    case CaseTargetTypeFileName:     aString = kXmlFileNameValue;     break;
    }
}

void FormatCase::toString(const CaseType &aCaseType, xpr::string &aString)
{
    switch (aCaseType)
    {
    case CaseTypeLower:                       aString = kXmlLowerValue;                       break;
    case CaseTypeUpper:                       aString = kXmlUpperValue;                       break;
    case CaseTypeUpperAtFirstChar:            aString = kXmlUpperAtFirstCharValue;            break;
    case CaseTypeUpperAtFirstCharOnEveryWord: aString = kXmlUpperAtFirstCharOnEveryWordValue; break;
    }
}

void FormatCase::toType(const xpr::string &aString, CaseTargetType &aTargetType)
{
    if (aString.compare(kXmlBaseFileNameValue) == 0)
    {
        aTargetType = CaseTargetTypeBaseFileName;
    }
    else if (aString.compare(kXmlFileExtValue) == 0)
    {
        aTargetType = CaseTargetTypeFileExt;
    }
    else if (aString.compare(kXmlFileNameValue) == 0)
    {
        aTargetType = CaseTargetTypeFileName;
    }
    else
    {
        aTargetType = kDefaultTargetType;
    }
}

void FormatCase::toType(const xpr::string &aString, CaseType &aCaseType)
{
    if (aString.compare(kXmlLowerValue) == 0)
    {
        aCaseType = CaseTypeLower;
    }
    else if (aString.compare(kXmlUpperValue) == 0)
    {
        aCaseType = CaseTypeUpper;
    }
    else if (aString.compare(kXmlUpperAtFirstCharValue) == 0)
    {
        aCaseType = CaseTypeUpperAtFirstChar;
    }
    else if (aString.compare(kXmlUpperAtFirstCharOnEveryWordValue) == 0)
    {
        aCaseType = CaseTypeUpperAtFirstCharOnEveryWord;
    }
    else
    {
        aCaseType = kDefaultCaseType;
    }
}
} // namespace cmd
} // namespace fxfile
