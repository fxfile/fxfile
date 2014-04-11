//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_insert.h"
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
const Format::InsertPosType kDefaultPosType = Format::InsertPosTypeAtLast;
const xpr_sint_t            kDefaultPos     = 0;

const xpr_tchar_t kXmlFormatInsertElement [] = XPR_STRING_LITERAL("FormatInsert");
const xpr_tchar_t kXmlPosTypeAttribute    [] = XPR_STRING_LITERAL("PositionType");
const xpr_tchar_t kXmlPosAttribute        [] = XPR_STRING_LITERAL("Position");
const xpr_tchar_t kXmlStringElement       [] = XPR_STRING_LITERAL("String");

const xpr_tchar_t kXmlAtFirstValue        [] = XPR_STRING_LITERAL("AtFirst");
const xpr_tchar_t kXmlAtLastValue         [] = XPR_STRING_LITERAL("AtLast");
const xpr_tchar_t kXmlFromFirstValue      [] = XPR_STRING_LITERAL("FromFirst");
const xpr_tchar_t kXmlFromLastValue       [] = XPR_STRING_LITERAL("FromLast");
} // namespace anonymous

FormatInsert::FormatInsert(void)
    : mPosType(kDefaultPosType), mPos(kDefaultPos)
{
}

FormatInsert::FormatInsert(InsertPosType aPosType, xpr_sint_t aPos, const xpr::string &aString)
    : mPosType(aPosType), mPos(aPos), mString(aString)
{
}

FormatInsert::~FormatInsert(void)
{
}

Format *FormatInsert::newObject(void)
{
    return new FormatInsert;
}

Format *FormatInsert::newClone(void) const
{
    Format *sNewFormat = new FormatInsert(mPosType, mPos, mString);

    return sNewFormat;
}

void FormatInsert::rename(RenameContext &aContext) const
{
    switch (mPosType)
    {
    case InsertPosTypeAtFirst:
        aContext.mNewFileName.insert(0, mString);
        break;

    case InsertPosTypeAtLast:
        aContext.mNewFileName += mString;
        break;

    case InsertPosTypeFromLast:
        {
            const xpr_size_t sLength = aContext.mNewFileName.length();
            xpr_size_t sOffset = sLength - mPos;

            if ((xpr_sint_t)sLength <= mPos)
            {
                sOffset = 0;
            }

            aContext.mNewFileName.insert(sOffset, mString);
            break;
        }

    default:
    case InsertPosTypeFromFirst:
        {
            xpr_sint_t sOffset = (mPos < 0) ? 0 : mPos;

            aContext.mNewFileName.insert(sOffset, mString);
        }
        break;
    }
}

xpr_bool_t FormatInsert::canParseXml(const xpr::string &aElementName)
{
    return (aElementName.compare(kXmlFormatInsertElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatInsert::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::string   sName;
    xpr::string   sValue;
    InsertPosType sPosType = kDefaultPosType;
    xpr_sint_t    sPos     = kDefaultPos;
    xpr::string   sString;

    base::XmlReader::Attribute *sAttribute = aXmlReader.getFirstAttribute(aElement);
    while (XPR_IS_NOT_NULL(sAttribute))
    {
        if (aXmlReader.getAttribute(sAttribute, sName, sValue) == XPR_FALSE)
        {
            return XPR_FALSE;
        }

        if (sName.compare(kXmlPosTypeAttribute) == 0)
        {
            toType(sValue, sPosType);
        }
        else if (sName.compare(kXmlPosAttribute) == 0)
        {
            sPos = _ttoi(sValue.c_str());
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

        if (sName.compare(kXmlStringElement) == 0)
        {
            sString = sValue;
        }

        sFormatElement = aXmlReader.nextElement(sFormatElement);
    }

    mPosType = sPosType;
    mPos     = sPos;
    mString  = sString;

    return XPR_TRUE;
}

xpr_bool_t FormatInsert::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatInsertElement) == XPR_TRUE)
    {
        xpr::string sPosTypeString;

        toString(mPosType, sPosTypeString);

        FXFILE_XML_WRITE_ATTRIBUTE_STRING(aXmlWriter, kXmlPosTypeAttribute, sPosTypeString);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlPosAttribute,     mPos);

        FXFILE_XML_WRITE_ELEMENT_ENTITY_STRING(aXmlWriter, kXmlStringElement, mString);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

void FormatInsert::toString(const InsertPosType &aPosType, xpr::string &aString)
{
    switch (aPosType)
    {
    case InsertPosTypeAtFirst:   aString = kXmlAtFirstValue;   break;
    case InsertPosTypeAtLast:    aString = kXmlAtLastValue;    break;
    case InsertPosTypeFromFirst: aString = kXmlFromFirstValue; break;
    case InsertPosTypeFromLast:  aString = kXmlFromLastValue;  break;
    }
}

void FormatInsert::toType(const xpr::string &aString, InsertPosType &aPosType)
{
    if (aString.compare(kXmlAtFirstValue) == 0)
    {
        aPosType = InsertPosTypeAtFirst;
    }
    else if (aString.compare(kXmlAtLastValue) == 0)
    {
        aPosType = InsertPosTypeAtLast;
    }
    else if (aString.compare(kXmlFromFirstValue) == 0)
    {
        aPosType = InsertPosTypeFromFirst;
    }
    else if (aString.compare(kXmlFromLastValue) == 0)
    {
        aPosType = InsertPosTypeFromLast;
    }
    else
    {
        aPosType = kDefaultPosType;
    }
}
} // namespace cmd
} // namespace fxfile
