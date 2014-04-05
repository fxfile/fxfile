//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_ext_remove.h"
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
const xpr_tchar_t kXmlFormatExtRemoveElement[] = XPR_STRING_LITERAL("FormatExtRemove");
} // namespace anonymous

FormatExtRemove::FormatExtRemove(void)
{
}

FormatExtRemove::~FormatExtRemove(void)
{
}

void FormatExtRemove::rename(RenameContext &aContext) const
{
    if (aContext.mNewFileName.empty() == XPR_FALSE)
    {
        xpr::tstring sBaseFileName;
        xpr::tstring sFileExt;

        RenameContext::getBaseFileNameAndExt(aContext.mFolder, aContext.mNewFileName, sBaseFileName, sFileExt);

        aContext.mNewFileName = sBaseFileName;
    }
}

xpr_bool_t FormatExtRemove::canParseXml(const xpr::tstring &aElementName)
{
    return (aElementName.compare(kXmlFormatExtRemoveElement) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FormatExtRemove::parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    return XPR_TRUE;
}

xpr_bool_t FormatExtRemove::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatExtRemoveElement) == XPR_TRUE)
    {
        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

Format *FormatExtRemove::newObject(void)
{
    return new FormatExtRemove;
}

Format *FormatExtRemove::newClone(void) const
{
    Format *sNewFormat = new FormatExtRemove;

    return sNewFormat;
}
} // namespace cmd
} // namespace fxfile
