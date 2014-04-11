//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "xml.h"

namespace fxfile
{
namespace base
{
namespace
{
const xpr_char_t kEncoding[] = XPR_MBCS_STRING_LITERAL("utf-8");

const xpr_tchar_t kTopElementName[] = XPR_STRING_LITERAL("fxfile");
} // namespace anonymous

XmlReader::XmlReader(void)
{
}

XmlReader::~XmlReader(void)
{
}

xpr::XmlReader::Element *XmlReader::getRootElement(void)
{
    XmlReader::Element *sTopElement = super::getRootElement();
    if (sTopElement == XPR_NULL)
        return XPR_NULL;

    xpr::string sName;

    if (getElement(sTopElement, sName) == XPR_FALSE)
        return XPR_NULL;

    if (sName.compare(kTopElementName) != 0)
        return XPR_NULL;

    return super::childElement(sTopElement);
}

XmlWriter::XmlWriter(void)
    : mDocument(XPR_FALSE)
    , mTopElement(XPR_FALSE)
{
}

XmlWriter::~XmlWriter(void)
{
}

xpr_bool_t XmlWriter::beginDocument(void)
{
    mDocument = super::beginDocument(kEncoding);
    if (mDocument == XPR_TRUE)
    {
        mTopElement = beginElement(kTopElementName);
        if (mTopElement == XPR_TRUE)
            return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t XmlWriter::endDocument(void)
{
    if (mTopElement == XPR_TRUE)
    {
        if (endElement() == XPR_FALSE)
            return XPR_FALSE;
    }

    if (mDocument == XPR_TRUE)
    {
        if (super::endDocument() == XPR_FALSE)
            return XPR_FALSE;
    }

    mDocument = XPR_FALSE;
    mTopElement = XPR_FALSE;

    return XPR_TRUE;
}

void XmlWriter::close(void)
{
    super::close();

    mDocument = XPR_FALSE;
    mTopElement = XPR_FALSE;
}
} // namespace base
} // namespace fxfile
