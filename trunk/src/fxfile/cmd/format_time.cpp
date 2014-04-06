//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_time.h"
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
const xpr_tchar_t kDefaultTimeFormat      [] = XPR_STRING_LITERAL("yyyy-MM-dd HH.mm.ss");

// short format
const xpr_tchar_t kShortFormatTimeRegExpr [] = XPR_STRING_LITERAL("[<]time([:][^:]*)*[>]");
const xpr_tchar_t kShortFormatCTimeRegExpr[] = XPR_STRING_LITERAL("[<]ctime([:][^:]*)*[>]");
const xpr_tchar_t kShortFormatMTimeRegExpr[] = XPR_STRING_LITERAL("[<]mtime([:][^:]*)*[>]");

// xml format
const xpr_tchar_t kXmlFormatNowTimeElement[] = XPR_STRING_LITERAL("FormatNowTime");
const xpr_tchar_t kXmlFormatCTimeElement  [] = XPR_STRING_LITERAL("FormatCTime");
const xpr_tchar_t kXmlFormatMTimeElement  [] = XPR_STRING_LITERAL("FormatMTime");
const xpr_tchar_t kXmlFormatElement       [] = XPR_STRING_LITERAL("Format");
} // namespace anonymous

FormatTime::FormatTime(void)
{
}

FormatTime::FormatTime(const xpr::tstring &aFormat)
    : mFormat(aFormat)
{
}

FormatTime::~FormatTime(void)
{
}

void FormatTime::toString(const xpr::tstring &aFormat, const SYSTEMTIME &aSystemTime, xpr::tstring &aString)
{
    xpr_sint_t sNumber;

         if (aFormat == XPR_STRING_LITERAL("y"   )) { sNumber = aSystemTime.wYear / 100;                             aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("yy"  )) { sNumber = aSystemTime.wYear / 100;                             aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("yyy" )) { sNumber = aSystemTime.wYear;                                   aString.append_format(XPR_STRING_LITERAL("%03d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("yyyy")) { sNumber = aSystemTime.wYear;                                   aString.append_format(XPR_STRING_LITERAL("%04d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("M"   )) { sNumber = aSystemTime.wMonth;                                  aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("MM"  )) { sNumber = aSystemTime.wMonth;                                  aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("d"   )) { sNumber = aSystemTime.wDay;                                    aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("dd"  )) { sNumber = aSystemTime.wDay;                                    aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("h"   )) { sNumber = aSystemTime.wHour; if (sNumber > 12) sNumber -= 12;  aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("hh"  )) { sNumber = aSystemTime.wHour; if (sNumber > 12) sNumber -= 12;  aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("H"   )) { sNumber = aSystemTime.wHour;                                   aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("HH"  )) { sNumber = aSystemTime.wHour;                                   aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("m"   )) { sNumber = aSystemTime.wMinute;                                 aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("mm"  )) { sNumber = aSystemTime.wMinute;                                 aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("s"   )) { sNumber = aSystemTime.wSecond;                                 aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("ss"  )) { sNumber = aSystemTime.wSecond;                                 aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("f"   )) { sNumber = aSystemTime.wMilliseconds / 100;                     aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("ff"  )) { sNumber = aSystemTime.wMilliseconds / 10;                      aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); }
    else if (aFormat == XPR_STRING_LITERAL("fff" )) { sNumber = aSystemTime.wMilliseconds;                           aString.append_format(XPR_STRING_LITERAL("%03d"), sNumber); }

    else if (aFormat == XPR_STRING_LITERAL("t"   )) { sNumber = aSystemTime.wHour < 12;                              aString += sNumber ? XPR_STRING_LITERAL("A" ) : XPR_STRING_LITERAL("P" ); }
    else if (aFormat == XPR_STRING_LITERAL("tt"  )) { sNumber = aSystemTime.wHour < 12;                              aString += sNumber ? XPR_STRING_LITERAL("AM") : XPR_STRING_LITERAL("PM"); }

    else if (aFormat == XPR_STRING_LITERAL("F"   )) { sNumber = aSystemTime.wMilliseconds / 100; if (sNumber != 0) { aString.append_format(XPR_STRING_LITERAL("%d"  ), sNumber); } }
    else if (aFormat == XPR_STRING_LITERAL("FF"  )) { sNumber = aSystemTime.wMilliseconds / 10;  if (sNumber != 0) { aString.append_format(XPR_STRING_LITERAL("%02d"), sNumber); } }
    else if (aFormat == XPR_STRING_LITERAL("FFF" )) { sNumber = aSystemTime.wMilliseconds;       if (sNumber != 0) { aString.append_format(XPR_STRING_LITERAL("%03d"), sNumber); } }

    else
    {
        aString += aFormat;
    }
}

void FormatTime::resolveTimeFormat(const xpr::tstring &aFormat, const SYSTEMTIME &aSystemTime, xpr::tstring &aString)
{
    xpr::tstring sBuffer;
    xpr::tstring::const_iterator it;

    it = aFormat.begin();
    for (; it != aFormat.end(); ++it)
    {
        const xpr_tchar_t &sChar = *it;

        switch (sChar)
        {
        case XPR_STRING_LITERAL('d'):
        case XPR_STRING_LITERAL('f'):
        case XPR_STRING_LITERAL('F'):
        case XPR_STRING_LITERAL('h'):
        case XPR_STRING_LITERAL('H'):
        case XPR_STRING_LITERAL('m'):
        case XPR_STRING_LITERAL('M'):
        case XPR_STRING_LITERAL('s'):
        case XPR_STRING_LITERAL('S'):
        case XPR_STRING_LITERAL('t'):
        case XPR_STRING_LITERAL('T'):
        case XPR_STRING_LITERAL('y'):
        case XPR_STRING_LITERAL('Y'):
            sBuffer += sChar;
            break;

        default:
            toString(sBuffer, aSystemTime, aString);
            sBuffer.clear();

            aString += sChar;
            break;
        }
    }

    toString(sBuffer, aSystemTime, aString);
}

void FormatTime::parseShort(const xpr::tstring &aFormatString, xpr::tstring &aFormat)
{
    xpr_size_t sColon = aFormatString.find(XPR_STRING_LITERAL(':'));
    if (sColon != xpr::tstring::npos)
    {
        aFormat = aFormatString.substr(sColon + 1, aFormatString.length() - sColon - 2);
    }
    else
    {
        aFormat.clear();
    }
}

xpr_bool_t FormatTime::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::tstring sName;
    xpr::tstring sValue;
    xpr::tstring sFormat;

    base::XmlReader::Element *sFormatElement = aXmlReader.childElement(aElement);
    while (XPR_IS_NOT_NULL(sFormatElement))
    {
        if (aXmlReader.getElementEntity(sFormatElement, sName, sValue) == XPR_FALSE)
        {
            return XPR_FALSE;
        }

        if (sName.compare(kXmlFormatElement) == 0)
        {
            sFormat = sValue;
        }

        sFormatElement = aXmlReader.nextElement(sFormatElement);
    }

    mFormat = sFormat;

    return XPR_TRUE;
}

FormatNowTime::FormatNowTime(void)
{
}

FormatNowTime::FormatNowTime(const xpr::tstring &aFormat)
    : FormatTime(aFormat)
{
}

FormatNowTime::~FormatNowTime(void)
{
}

Format *FormatNowTime::newObject(void)
{
    return new FormatNowTime;
}

Format *FormatNowTime::newClone(void) const
{
    Format *sNewFormat = new FormatNowTime(mFormat);

    return sNewFormat;
}

void FormatNowTime::rename(RenameContext &aContext) const
{
    const xpr::tstring &sFormat = getFormat();

    if (sFormat.empty() == XPR_TRUE)
    {
        resolveTimeFormat(kDefaultTimeFormat, aContext.mNowTime, aContext.mNewFileName);
    }
    else
    {
        resolveTimeFormat(sFormat, aContext.mNowTime, aContext.mNewFileName);
    }
}

xpr_bool_t FormatNowTime::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatNowTimeElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatNowTime::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatNowTime::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatNowTimeElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ELEMENT_ENTITY_STRING(aXmlWriter, kXmlFormatElement, mFormat);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatNowTime::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatTimeRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatNowTime::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr::tstring sFormat;
    FormatTime::parseShort(aFormatString, sFormat);

    aFormat = new FormatNowTime(sFormat);
}

FormatCTime::FormatCTime(void)
{
}

FormatCTime::FormatCTime(const xpr::tstring &aFormat)
    : FormatTime(aFormat)
{
}

FormatCTime::~FormatCTime(void)
{
}

Format *FormatCTime::newObject(void)
{
    return new FormatCTime;
}

Format *FormatCTime::newClone(void) const
{
    Format *sNewFormat = new FormatCTime(mFormat);

    return sNewFormat;
}

void FormatCTime::rename(RenameContext &aContext) const
{
    const xpr::tstring &sFormat = getFormat();

    // get creation time of the file
    aContext.getCTime();

    if (sFormat.empty() == XPR_TRUE)
    {
        resolveTimeFormat(kDefaultTimeFormat, aContext.mCTime, aContext.mNewFileName);
    }
    else
    {
        resolveTimeFormat(sFormat, aContext.mCTime, aContext.mNewFileName);
    }
}

xpr_bool_t FormatCTime::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatCTimeElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatCTime::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatCTime::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatCTimeElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ELEMENT_ENTITY_STRING(aXmlWriter, kXmlFormatElement, mFormat);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatCTime::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatCTimeRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatCTime::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr::tstring sFormat;
    FormatTime::parseShort(aFormatString, sFormat);

    aFormat = new FormatCTime(sFormat);
}

FormatMTime::FormatMTime(void)
{
}

FormatMTime::FormatMTime(const xpr::tstring &aFormat)
    : FormatTime(aFormat)
{
}

FormatMTime::~FormatMTime(void)
{
}

Format *FormatMTime::newObject(void)
{
    return new FormatMTime;
}

Format *FormatMTime::newClone(void) const
{
    Format *sNewFormat = new FormatMTime(mFormat);

    return sNewFormat;
}

void FormatMTime::rename(RenameContext &aContext) const
{
    const xpr::tstring &sFormat = getFormat();

    // get last written time of the file
    aContext.getMTime();

    if (sFormat.empty() == XPR_TRUE)
    {
        resolveTimeFormat(kDefaultTimeFormat, aContext.mMTime, aContext.mNewFileName);
    }
    else
    {
        resolveTimeFormat(sFormat, aContext.mMTime, aContext.mNewFileName);
    }
}

xpr_bool_t FormatMTime::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatMTimeElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatMTime::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatMTime::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatMTimeElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ELEMENT_ENTITY_STRING(aXmlWriter, kXmlFormatElement, mFormat);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatMTime::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatMTimeRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatMTime::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr::tstring sFormat;
    FormatTime::parseShort(aFormatString, sFormat);

    aFormat = new FormatMTime(sFormat);
}
} // namespace cmd
} // namespace fxfile
