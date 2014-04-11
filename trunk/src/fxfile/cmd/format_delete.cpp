//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_delete.h"
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
const Format::DeletePosType kDefaultPosType = Format::DeletePosTypeAtLast;
const xpr_sint_t            kDefaultPos     = 0;
const xpr_sint_t            kDefaultLength  = 0;

const xpr_tchar_t kXmlFormatDeleteElement [] = XPR_STRING_LITERAL("FormatDelete");
const xpr_tchar_t kXmlPosTypeAttribute    [] = XPR_STRING_LITERAL("PosisionType");
const xpr_tchar_t kXmlPosAttribute        [] = XPR_STRING_LITERAL("Posision");
const xpr_tchar_t kXmlLengthAttribute     [] = XPR_STRING_LITERAL("Length");

const xpr_tchar_t kXmlAtFirstValue        [] = XPR_STRING_LITERAL("AtFirst");
const xpr_tchar_t kXmlAtLastValue         [] = XPR_STRING_LITERAL("AtLast");
const xpr_tchar_t kXmlFromFirstValue      [] = XPR_STRING_LITERAL("FromFirst");
const xpr_tchar_t kXmlFromLastValue       [] = XPR_STRING_LITERAL("FromLast");
} // namespace anonymous

FormatDelete::FormatDelete(void)
    : mPosType(kDefaultPosType), mPos(kDefaultPos), mLength(kDefaultLength)
{
}

FormatDelete::FormatDelete(DeletePosType aPosType, xpr_sint_t aPos, xpr_sint_t aLength)
    : mPosType(aPosType), mPos(aPos), mLength(aLength)
{
}

FormatDelete::~FormatDelete(void)
{
}

Format *FormatDelete::newObject(void)
{
    return new FormatDelete;
}

Format *FormatDelete::newClone(void) const
{
    Format *sNewFormat = new FormatDelete(mPosType, mPos, mLength);

    return sNewFormat;
}

void FormatDelete::rename(RenameContext &aContext) const
{
    xpr_sint_t sOffset = mPos;
    xpr_sint_t sLength = mLength;
    const xpr_sint_t sNameLength = (xpr_sint_t)aContext.mNewFileName.length();

    if (sLength > 0)
    {
        switch (mPosType)
        {
        case DeletePosTypeAtFirst:
            sOffset = 0;
            if (sNameLength < sLength)
            {
                sLength = sNameLength;
            }

            aContext.mNewFileName.erase(sOffset, sLength);
            break;

        case DeletePosTypeAtLast:
            if (sNameLength < sLength)
            {
                sLength = sNameLength;
            }
            sOffset = sNameLength - sLength;

            aContext.mNewFileName.erase(sOffset, sLength);
            break;

        case DeletePosTypeFromLast:
            if (sOffset >= 0)
            {
                if (sOffset > sNameLength)
                {
                    sOffset = sNameLength;
                }

                if ((sOffset + sLength) > sNameLength)
                {
                    sLength = sNameLength - sOffset;
                }

                sOffset = sNameLength - sLength - sOffset;

                aContext.mNewFileName.erase(sOffset, sLength);
            }
            break;

        default:
        case DeletePosTypeFromFirst:
            if (sOffset >= 0)
            {
                if (sOffset > sNameLength)
                {
                    sOffset = sNameLength;
                }

                if ((sOffset + sLength) > sNameLength)
                {
                    sLength = sNameLength - sOffset;
                }

                aContext.mNewFileName.erase(sOffset, sLength);
            }
            break;
        }
    }
}

xpr_bool_t FormatDelete::canParseXml(const xpr::string &aElementName)
{
    return (aElementName.compare(kXmlFormatDeleteElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatDelete::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::string   sName;
    xpr::string   sValue;
    DeletePosType sPosType = kDefaultPosType;
    xpr_sint_t    sPos     = kDefaultPos;
    xpr_sint_t    sLength  = kDefaultLength;

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
        else if (sName.compare(kXmlLengthAttribute) == 0)
        {
            sLength = _ttoi(sValue.c_str());
        }

        sAttribute = aXmlReader.nextAttribute(sAttribute);
    }

    mPosType = sPosType;
    mPos     = sPos;
    mLength  = sLength;

    return XPR_TRUE;
}

xpr_bool_t FormatDelete::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatDeleteElement) == XPR_TRUE)
    {
        xpr::string sPosTypeString;

        toString(mPosType, sPosTypeString);

        FXFILE_XML_WRITE_ATTRIBUTE_STRING(aXmlWriter, kXmlPosTypeAttribute, sPosTypeString);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlPosAttribute,     mPos);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlLengthAttribute,  mLength);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

void FormatDelete::toString(const DeletePosType &aPosType, xpr::string &aString)
{
    switch (aPosType)
    {
    case DeletePosTypeAtFirst:   aString = kXmlAtFirstValue;   break;
    case DeletePosTypeAtLast:    aString = kXmlAtLastValue;    break;
    case DeletePosTypeFromFirst: aString = kXmlFromFirstValue; break;
    case DeletePosTypeFromLast:  aString = kXmlFromLastValue;  break;
    }
}

void FormatDelete::toType(const xpr::string &aString, DeletePosType &aPosType)
{
    if (aString.compare(kXmlAtFirstValue) == 0)
    {
        aPosType = DeletePosTypeAtFirst;
    }
    else if (aString.compare(kXmlAtLastValue) == 0)
    {
        aPosType = DeletePosTypeAtLast;
    }
    else if (aString.compare(kXmlFromFirstValue) == 0)
    {
        aPosType = DeletePosTypeFromFirst;
    }
    else if (aString.compare(kXmlFromLastValue) == 0)
    {
        aPosType = DeletePosTypeFromLast;
    }
    else
    {
        aPosType = kDefaultPosType;
    }
}
} // namespace cmd
} // namespace fxfile
