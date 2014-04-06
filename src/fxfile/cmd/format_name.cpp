//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_name.h"
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
const xpr_sint_t kDefaultBeginPos = 0;
const xpr_sint_t kDefaultEndPos   = -1;

// short format
const xpr_tchar_t kShortFormatFileNameRegExpr       [] = XPR_STRING_LITERAL("[<][*]([0-9]+[-][0-9]+)*[>]");
const xpr_tchar_t kShortFormatBaseFileNameRegExpr   [] = XPR_STRING_LITERAL("[<]n([0-9]+[-][0-9]+)*[>]");
const xpr_tchar_t kShortFormatFileExtRegExpr        [] = XPR_STRING_LITERAL("[<]e([0-9]+[-][0-9]+)*[>]");
const xpr_tchar_t kShortFormatOrgFileNameRegExpr    [] = XPR_STRING_LITERAL("[<]g[*]([0-9]+[-][0-9]+)*[>]");
const xpr_tchar_t kShortFormatOrgBaseFileNameRegExpr[] = XPR_STRING_LITERAL("[<]gn([0-9]+[-][0-9]+)*[>]");
const xpr_tchar_t kShortFormatOrgFileExtRegExpr     [] = XPR_STRING_LITERAL("[<]ge([0-9]+[-][0-9]+)*[>]");

const xpr_tchar_t kXmlFormatBaseFileNameElement     [] = XPR_STRING_LITERAL("FormatBaseFileName");
const xpr_tchar_t kXmlFormatFileExtElement          [] = XPR_STRING_LITERAL("FormatFileExt");
const xpr_tchar_t kXmlFormatFileNameElement         [] = XPR_STRING_LITERAL("FormatFileName");
const xpr_tchar_t kXmlFormatOrgBaseFileNameElement  [] = XPR_STRING_LITERAL("FormatOrgBaseFileName");
const xpr_tchar_t kXmlFormatOrgFileExtElement       [] = XPR_STRING_LITERAL("FormatOrgFileExt");
const xpr_tchar_t kXmlFormatOrgFileNameElement      [] = XPR_STRING_LITERAL("FormatOrgFileName");
const xpr_tchar_t kXmlBeginPositionAttribute        [] = XPR_STRING_LITERAL("BeginPosition");
const xpr_tchar_t kXmlEndPositionAttribute          [] = XPR_STRING_LITERAL("EndPosition");
} // namespace anonymous

FormatName::FormatName(void)
    : mBeginPos(kDefaultBeginPos), mEndPos(kDefaultEndPos)
{
}

FormatName::FormatName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos)
    : mBeginPos(aBeginPos), mEndPos(aEndPos)
{
}

FormatName::~FormatName(void)
{
}

xpr_sint_t FormatName::getDefaultBeginPos(void)
{
    return kDefaultBeginPos;
}

xpr_sint_t FormatName::getDefaultEndPos(void)
{
    return kDefaultEndPos;
}

void FormatName::parseShort(const xpr::tstring &aFormatString, xpr_size_t aParseBeginPos, xpr_sint_t &aBeginPos, xpr_sint_t &aEndPos)
{
    aBeginPos = kDefaultBeginPos;
    aEndPos   = kDefaultEndPos;

    _stscanf(aFormatString.c_str() + aParseBeginPos, XPR_STRING_LITERAL("%d-%d"), &aBeginPos, &aEndPos);
}

xpr_bool_t FormatName::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::tstring sName;
    xpr::tstring sValue;
    xpr_sint_t   sBeginPos = kDefaultBeginPos;
    xpr_sint_t   sEndPos   = kDefaultEndPos;

    base::XmlReader::Attribute *sAttribute = aXmlReader.getFirstAttribute(aElement);
    while (XPR_IS_NOT_NULL(sAttribute))
    {
        if (aXmlReader.getAttribute(sAttribute, sName, sValue) == XPR_FALSE)
        {
            return XPR_FALSE;
        }

        if (sName.compare(kXmlBeginPositionAttribute) == 0)
        {
            sBeginPos = _ttoi(sValue.c_str());
        }
        else if (sName.compare(kXmlEndPositionAttribute) == 0)
        {
            sEndPos = _ttoi(sValue.c_str());
        }

        sAttribute = aXmlReader.nextAttribute(sAttribute);
    }

    mBeginPos = sBeginPos;
    mEndPos   = sEndPos;

    return XPR_TRUE;
}

FormatBaseFileName::FormatBaseFileName(void)
{
}

FormatBaseFileName::FormatBaseFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos)
    : FormatName(aBeginPos, aEndPos)
{
}

FormatBaseFileName::~FormatBaseFileName(void)
{
}

Format *FormatBaseFileName::newObject(void)
{
    return new FormatBaseFileName;
}

Format *FormatBaseFileName::newClone(void) const
{
    Format *sNewFormat = new FormatBaseFileName(mBeginPos, mEndPos);

    return sNewFormat;
}

void FormatBaseFileName::rename(RenameContext &aContext) const
{
    xpr::tstring sBaseFileName;
    xpr::tstring sFileExt;

    RenameContext::getBaseFileNameAndExt(aContext.mFolder, aContext.mNewFileName, sBaseFileName, sFileExt);

    if (mEndPos == -1)
    {
        aContext.mNewFileName += sBaseFileName.substr(mBeginPos);
    }
    else
    {
        aContext.mNewFileName += sBaseFileName.substr(mBeginPos, mEndPos - mBeginPos);
    }
}

xpr_bool_t FormatBaseFileName::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatBaseFileNameElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatBaseFileName::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatBaseFileName::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatBaseFileNameElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlBeginPositionAttribute, mBeginPos);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlEndPositionAttribute,   mEndPos);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatBaseFileName::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatBaseFileNameRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatBaseFileName::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr_sint_t sBeginPos, sEndPos;
    FormatName::parseShort(aFormatString, 2, sBeginPos, sEndPos);

    aFormat = new FormatBaseFileName(sBeginPos, sEndPos);
}

FormatFileExt::FormatFileExt(void)
{
}

FormatFileExt::FormatFileExt(xpr_sint_t aBeginPos, xpr_sint_t aEndPos)
    : FormatName(aBeginPos, aEndPos)
{
}

FormatFileExt::~FormatFileExt(void)
{
}

Format *FormatFileExt::newObject(void)
{
    return new FormatFileExt;
}

Format *FormatFileExt::newClone(void) const
{
    Format *sNewFormat = new FormatFileExt(mBeginPos, mEndPos);

    return sNewFormat;
}

void FormatFileExt::rename(RenameContext &aContext) const
{
    xpr::tstring sBaseFileName;
    xpr::tstring sFileExt;

    RenameContext::getBaseFileNameAndExt(aContext.mFolder, aContext.mNewFileName, sBaseFileName, sFileExt);

    if (mEndPos == -1)
    {
        aContext.mNewFileName += sFileExt.substr(mBeginPos);
    }
    else
    {
        aContext.mNewFileName += sFileExt.substr(mBeginPos, mEndPos - mBeginPos);
    }
}

xpr_bool_t FormatFileExt::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatFileExtElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatFileExt::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatFileExt::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatFileExtElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlBeginPositionAttribute, mBeginPos);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlEndPositionAttribute,   mEndPos);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatFileExt::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatFileExtRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatFileExt::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr_sint_t sBeginPos, sEndPos;
    FormatName::parseShort(aFormatString, 2, sBeginPos, sEndPos);

    aFormat = new FormatFileExt(sBeginPos, sEndPos);
}

FormatFileName::FormatFileName(void)
{
}

FormatFileName::FormatFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos)
    : FormatName(aBeginPos, aEndPos)
{
}

FormatFileName::~FormatFileName(void)
{
}

Format *FormatFileName::newObject(void)
{
    return new FormatFileName;
}

Format *FormatFileName::newClone(void) const
{
    Format *sNewFormat = new FormatFileName(mBeginPos, mEndPos);

    return sNewFormat;
}

void FormatFileName::rename(RenameContext &aContext) const
{
    if (mEndPos == -1)
    {
        aContext.mNewFileName += aContext.mNewFileName.substr(mBeginPos);
    }
    else
    {
        aContext.mNewFileName += aContext.mNewFileName.substr(mBeginPos, mEndPos - mBeginPos);
    }
}

xpr_bool_t FormatFileName::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatFileNameElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatFileName::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatFileName::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatFileNameElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlBeginPositionAttribute, mBeginPos);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlEndPositionAttribute,   mEndPos);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatFileName::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatFileNameRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatFileName::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr_sint_t sBeginPos, sEndPos;
    FormatName::parseShort(aFormatString, 2, sBeginPos, sEndPos);

    aFormat = new FormatFileName(sBeginPos, sEndPos);
}

FormatOrgBaseFileName::FormatOrgBaseFileName(void)
{
}

FormatOrgBaseFileName::FormatOrgBaseFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos)
    : FormatName(aBeginPos, aEndPos)
{
}

FormatOrgBaseFileName::~FormatOrgBaseFileName(void)
{
}

Format *FormatOrgBaseFileName::newObject(void)
{
    return new FormatOrgBaseFileName;
}

Format *FormatOrgBaseFileName::newClone(void) const
{
    Format *sNewFormat = new FormatOrgBaseFileName(mBeginPos, mEndPos);

    return sNewFormat;
}

void FormatOrgBaseFileName::rename(RenameContext &aContext) const
{
    xpr::tstring sBaseFileName;
    xpr::tstring sFileExt;

    RenameContext::getBaseFileNameAndExt(aContext.mFolder, aContext.mOrgFileName, sBaseFileName, sFileExt);

    if (mEndPos == -1)
    {
        aContext.mNewFileName += sBaseFileName.substr(mBeginPos);
    }
    else
    {
        aContext.mNewFileName += sBaseFileName.substr(mBeginPos, mEndPos - mBeginPos);
    }
}

xpr_bool_t FormatOrgBaseFileName::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatOrgBaseFileNameElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatOrgBaseFileName::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatOrgBaseFileName::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatOrgBaseFileNameElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlBeginPositionAttribute, mBeginPos);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlEndPositionAttribute,   mEndPos);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatOrgBaseFileName::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatOrgBaseFileNameRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatOrgBaseFileName::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr_sint_t sBeginPos, sEndPos;
    FormatName::parseShort(aFormatString, 3, sBeginPos, sEndPos);

    aFormat = new FormatOrgBaseFileName(sBeginPos, sEndPos);
}

FormatOrgFileExt::FormatOrgFileExt(void)
{
}

FormatOrgFileExt::FormatOrgFileExt(xpr_sint_t aBeginPos, xpr_sint_t aEndPos)
    : FormatName(aBeginPos, aEndPos)
{
}

FormatOrgFileExt::~FormatOrgFileExt(void)
{
}

Format *FormatOrgFileExt::newObject(void)
{
    return new FormatOrgFileExt;
}

Format *FormatOrgFileExt::newClone(void) const
{
    Format *sNewFormat = new FormatOrgFileExt(mBeginPos, mEndPos);

    return sNewFormat;
}

void FormatOrgFileExt::rename(RenameContext &aContext) const
{
    xpr::tstring sBaseFileName;
    xpr::tstring sFileExt;

    RenameContext::getBaseFileNameAndExt(aContext.mFolder, aContext.mOrgFileName, sBaseFileName, sFileExt);

    if (mEndPos == -1)
    {
        aContext.mNewFileName += sFileExt.substr(mBeginPos);
    }
    else
    {
        aContext.mNewFileName += sFileExt.substr(mBeginPos, mEndPos - mBeginPos);
    }
}

xpr_bool_t FormatOrgFileExt::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatOrgFileExtElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatOrgFileExt::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatOrgFileExt::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatOrgFileExtElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlBeginPositionAttribute, mBeginPos);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlEndPositionAttribute,   mEndPos);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatOrgFileExt::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatOrgFileExtRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatOrgFileExt::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr_sint_t sBeginPos, sEndPos;
    FormatName::parseShort(aFormatString, 3, sBeginPos, sEndPos);

    aFormat = new FormatOrgFileExt(sBeginPos, sEndPos);
}

FormatOrgFileName::FormatOrgFileName(void)
{
}

FormatOrgFileName::FormatOrgFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos)
    : FormatName(aBeginPos, aEndPos)
{
}

FormatOrgFileName::~FormatOrgFileName(void)
{
}

Format *FormatOrgFileName::newObject(void)
{
    return new FormatOrgFileName;
}

Format *FormatOrgFileName::newClone(void) const
{
    Format *sNewFormat = new FormatOrgFileName(mBeginPos, mEndPos);

    return sNewFormat;
}

void FormatOrgFileName::rename(RenameContext &aContext) const
{
    if (mEndPos == -1)
    {
        aContext.mNewFileName += aContext.mOrgFileName.substr(mBeginPos);
    }
    else
    {
        aContext.mNewFileName += aContext.mOrgFileName.substr(mBeginPos, mEndPos - mBeginPos);
    }
}

xpr_bool_t FormatOrgFileName::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatOrgFileNameElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatOrgFileName::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return super::parseXml(aXmlReader, aElement);
}

xpr_bool_t FormatOrgFileName::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatOrgFileNameElement) == XPR_TRUE)
    {
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlBeginPositionAttribute, mBeginPos);
        FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, kXmlEndPositionAttribute,   mEndPos);

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FormatOrgFileName::canParseShort(const xpr::tstring &aFormatString)
{
    const std::tr1::tregex sRegEx(kShortFormatOrgFileNameRegExpr);

    return (std::tr1::regex_match(aFormatString.c_str(), sRegEx) == true) ? XPR_TRUE : XPR_FALSE;
}

void FormatOrgFileName::parseShort(const xpr::tstring &aFormatString, Format *&aFormat)
{
    xpr_sint_t sBeginPos, sEndPos;
    FormatName::parseShort(aFormatString, 3, sBeginPos, sEndPos);

    aFormat = new FormatOrgFileName(sBeginPos, sEndPos);
}
} // namespace cmd
} // namespace fxfile
