//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_string.h"
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
const xpr_tchar_t kXmlFormatStringElement[] = XPR_STRING_LITERAL("FormatString");
const xpr_tchar_t kXmlStringElement      [] = XPR_STRING_LITERAL("String");
} // namespace anonymous

FormatString::FormatString(void)
{
}

FormatString::FormatString(const xpr::string &aString)
    : mString(aString)
{
}

FormatString::~FormatString(void)
{
}

Format *FormatString::newObject(void)
{
    return new FormatString;
}

Format *FormatString::newClone(void) const
{
    Format *sNewFormat = new FormatString(mString);

    return sNewFormat;
}

void FormatString::rename(RenameContext &aContext) const
{
    aContext.mNewFileName += mString;
}

xpr_bool_t FormatString::canParseXml(const xpr::string &aElementName)
{
    return (aElementName.compare(kXmlFormatStringElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatString::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::string sName;
    xpr::string sValue;
    xpr::string sString;

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

    mString = sString;

    return XPR_TRUE;
}

xpr_bool_t FormatString::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatStringElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ELEMENT_ENTITY_STRING(aXmlWriter, kXmlStringElement, mString);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatString::canParseShort(const xpr::string &aFormatString)
{
    return XPR_TRUE;
}

void FormatString::parseShort(const xpr::string &aFormatString, Format *&aFormat)
{
    aFormat = new FormatString(aFormatString);
}
} // namespace cmd
} // namespace fxfile
