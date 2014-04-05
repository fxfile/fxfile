//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_number.h"
#include "xpr_reg_expr.h"
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
const xpr_sint_t kDefaultStart      = 1;
const xpr_sint_t kDefaultStep       = 1;
const xpr_bool_t kDefaultZeroFilled = XPR_FALSE;
const xpr_size_t kDefaultDigit      = 0;

// short format
const xpr_tchar_t kShortFormatNumberRegExpr[] = XPR_STRING_LITERAL("[<][-+]?[0-9]+([-+][1-9][0-9]*)*[>]");

const xpr_tchar_t kXmlFormatNumberElement  [] = XPR_STRING_LITERAL("FormatNumber");
const xpr_tchar_t kXmlNumberElement        [] = XPR_STRING_LITERAL("Number");
const xpr_tchar_t kXmlStepElement          [] = XPR_STRING_LITERAL("Step");
const xpr_tchar_t kXmlZeroFilledAttribute  [] = XPR_STRING_LITERAL("ZeroFilled");
const xpr_tchar_t kXmlDigitAttribute       [] = XPR_STRING_LITERAL("Digit");
} // namespace anonymous

FormatNumber::FormatNumber(void)
    : mStart(kDefaultStart), mStep(kDefaultStep)
    , mZeroFilled(kDefaultZeroFilled), mDigit(kDefaultDigit)
{
}

FormatNumber::FormatNumber(xpr_sint_t aStart)
    : mStart(aStart), mStep(kDefaultStep)
    , mZeroFilled(kDefaultZeroFilled), mDigit(kDefaultDigit)
{
}

FormatNumber::FormatNumber(xpr_sint_t aStart, xpr_sint_t aStep)
    : mStart(aStart), mStep(aStep)
    , mZeroFilled(kDefaultZeroFilled), mDigit(kDefaultDigit)
{
}

FormatNumber::FormatNumber(xpr_sint_t aStart, xpr_sint_t aStep, xpr_bool_t aZeroFilled, xpr_size_t aDigit)
    : mStart(aStart), mStep(aStep)
    , mZeroFilled(aZeroFilled), mDigit(aDigit)
{
}

FormatNumber::~FormatNumber(void)
{
}

void FormatNumber::rename(RenameContext &aContext) const
{
    xpr_sint_t   sNumber = mStart + (xpr_sint_t)aContext.mIndex * mStep;
    xpr::tstring sStringFormat;

    if (XPR_IS_TRUE(mZeroFilled))
    {
        sStringFormat.format(XPR_STRING_LITERAL("%%0%dd"), mDigit);
    }
    else
    {
        sStringFormat.format(XPR_STRING_LITERAL("%%d"));
    }

    aContext.mNewFileName.append_format(sStringFormat.c_str(), sNumber);
}

xpr_bool_t FormatNumber::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatNumberElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatNumber::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::tstring sName;
    xpr::tstring sValue;
    xpr_sint_t   sStart      = kDefaultStart;
    xpr_sint_t   sStep       = kDefaultStep;
    xpr_bool_t   sZeroFilled = kDefaultZeroFilled;
    xpr_size_t   sDigit      = kDefaultDigit;

    base::XmlReader::Attribute *sFormatNumberAttribute = aXmlReader.getFirstAttribute(aElement);
    while (XPR_IS_NOT_NULL(sFormatNumberAttribute))
    {
        if (aXmlReader.getAttribute(sFormatNumberAttribute, sName, sValue) == XPR_FALSE)
        {
            return XPR_FALSE;
        }

        if (sName.compare(kXmlZeroFilledAttribute) == 0)
        {
            sZeroFilled = _ttoi(sValue.c_str());
        }
        else if (sName.compare(kXmlDigitAttribute) == 0)
        {
            sDigit = _ttoi(sValue.c_str());
        }

        sFormatNumberAttribute = aXmlReader.nextAttribute(sFormatNumberAttribute);
    }

    base::XmlReader::Element *sFormatElement = aXmlReader.childElement(aElement);
    while (XPR_IS_NOT_NULL(sFormatElement))
    {
        if (aXmlReader.getElementEntity(sFormatElement, sName, sValue) == XPR_FALSE)
        {
            return XPR_FALSE;
        }

        if (sName.compare(kXmlNumberElement) == 0)
        {
            sStart = _ttoi(sValue.c_str());
        }
        else if (sName.compare(kXmlStepElement) == 0)
        {
            sStep = _ttoi(sValue.c_str());
        }

        sFormatElement = aXmlReader.nextElement(sFormatElement);
    }

    mStart      = sStart;
    mStep       = sStep;
    mZeroFilled = sZeroFilled;
    mDigit      = sDigit;

    return XPR_TRUE;
}

xpr_bool_t FormatNumber::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatNumberElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlDigitAttribute,      mDigit);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlZeroFilledAttribute, mZeroFilled);

        FXFILE_XML_WRITE_ELEMENT_ENTITY_SINT32(aXmlWriter, kXmlNumberElement, mStart);
        FXFILE_XML_WRITE_ELEMENT_ENTITY_SINT32(aXmlWriter, kXmlStepElement,   mStep);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

Format *FormatNumber::newObject(void)
{
    return new FormatNumber;
}

Format *FormatNumber::newClone(void) const
{
    Format *sNewFormat = new FormatNumber(mStart, mStep, mZeroFilled, mDigit);

    return sNewFormat;
}

xpr_bool_t FormatNumber::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatNumberRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

// count format (e.g. <1> or <1+2> or <1-2> or <001+2> or <-001+2> or <-001-2>)
void FormatNumber::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr_sint_t sNumber          = 1;
    xpr_sint_t sStep            = 1;
    xpr_bool_t sZeroFilled      = XPR_FALSE;
    xpr_sint_t sZeroFilledIndex = 1;
    xpr_size_t sDigit           = 0;

    if (aFormatString[1] == XPR_STRING_LITERAL('-'))
    {
        sZeroFilledIndex = 2;
    }

    sZeroFilled = (aFormatString[sZeroFilledIndex] == XPR_STRING_LITERAL('0')) ? XPR_TRUE : XPR_FALSE;

    xpr_size_t sStepSign = aFormatString.find_first_of(XPR_STRING_LITERAL("+-"), 2);
    if (sStepSign != xpr::tstring::npos)
    {
        if (aFormatString[sStepSign] == XPR_STRING_LITERAL('-'))
        {
            _stscanf(aFormatString.c_str() + 1, XPR_STRING_LITERAL("%d-%d"), &sNumber, &sStep);
            sStep = -sStep;
        }
        else
        {
            _stscanf(aFormatString.c_str() + 1, XPR_STRING_LITERAL("%d+%d"), &sNumber, &sStep);
        }
    }
    else
    {
        _stscanf(aFormatString.c_str() + 1, XPR_STRING_LITERAL("%d"), &sNumber);
    }

    xpr::tstring sStringFormat;
    if (XPR_IS_TRUE(sZeroFilled))
    {
        if (sStepSign != xpr::tstring::npos)
        {
            sDigit = sStepSign - 1;

            if (sNumber < 0)
            {
                --sDigit;
            }
        }
        else
        {
            sDigit = aFormatString.length() - 2;
        }
    }

    aFormat = new FormatNumber(sNumber, sStep, sZeroFilled, sDigit);
}
} // namespace cmd
} // namespace fxfile
