//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_clear.h"
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
const xpr_tchar_t kXmlFormatClearElement[] = XPR_STRING_LITERAL("FormatClear");
} // namespace anonymous

FormatClear::FormatClear(void)
{
}

FormatClear::~FormatClear(void)
{
}

void FormatClear::rename(RenameContext &aContext) const
{
    aContext.mNewFileName.clear();
}

xpr_bool_t FormatClear::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatClearElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatClear::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return XPR_TRUE;
}

xpr_bool_t FormatClear::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatClearElement) == XPR_TRUE)
    {
        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

Format *FormatClear::newObject(void)
{
    return new FormatClear;
}

Format *FormatClear::newClone(void) const
{
    Format *sNewFormat = new FormatClear;

    return sNewFormat;
}
} // namespace cmd
} // namespace fxfile
