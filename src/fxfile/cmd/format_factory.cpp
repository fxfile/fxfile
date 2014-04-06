//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_factory.h"
#include "format_string.h"
#include "format_name.h"
#include "format_number.h"
#include "format_time.h"
#include "format_replace.h"
#include "format_insert.h"
#include "format_delete.h"
#include "format_case.h"
#include "format_ext_remove.h"
#include "format_clear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
FormatFactory::FormatFactory(void)
{
    addShortParseFuncTable<FormatNumber         >();
    addShortParseFuncTable<FormatBaseFileName   >();
    addShortParseFuncTable<FormatFileExt        >();
    addShortParseFuncTable<FormatFileName       >();
    addShortParseFuncTable<FormatOrgBaseFileName>();
    addShortParseFuncTable<FormatOrgFileExt     >();
    addShortParseFuncTable<FormatOrgFileName    >();
    addShortParseFuncTable<FormatNowTime        >();
    addShortParseFuncTable<FormatCTime          >();
    addShortParseFuncTable<FormatMTime          >();

    addXmlParseFuncTable<FormatNumber         >();
    addXmlParseFuncTable<FormatBaseFileName   >();
    addXmlParseFuncTable<FormatFileExt        >();
    addXmlParseFuncTable<FormatFileName       >();
    addXmlParseFuncTable<FormatOrgBaseFileName>();
    addXmlParseFuncTable<FormatOrgFileExt     >();
    addXmlParseFuncTable<FormatOrgFileName    >();
    addXmlParseFuncTable<FormatNowTime        >();
    addXmlParseFuncTable<FormatCTime          >();
    addXmlParseFuncTable<FormatMTime          >();
    addXmlParseFuncTable<FormatString         >();
    addXmlParseFuncTable<FormatReplace        >();
    addXmlParseFuncTable<FormatInsert         >();
    addXmlParseFuncTable<FormatDelete         >();
    addXmlParseFuncTable<FormatCase           >();
    addXmlParseFuncTable<FormatExtRemove      >();
    addXmlParseFuncTable<FormatClear          >();
}

FormatFactory::~FormatFactory(void)
{
}

xpr_bool_t FormatFactory::parseShort(const xpr::tstring &aFormatString, Format *&aFormat) const
{
    ShortParseFuncTable::const_iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mShortParseFuncTable)
    {
        const ShortParseFunc &sFunc = *sIterator;

        if (sFunc.mCanParseShortFunc(aFormatString) == XPR_TRUE)
        {
            sFunc.mParseShortFunc(aFormatString, aFormat);
            return XPR_TRUE;
        }
    }

    return XPR_FALSE;
}

void FormatFactory::parseShortAsString(const xpr::tstring &aFormatString, Format *&aFormat) const
{
    FormatString::parseShort(aFormatString, aFormat);
}

xpr_bool_t FormatFactory::parseFromXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement, Format *&aFormat) const
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::tstring sName;

    if (aXmlReader.getElement(aElement, sName) == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    XmlParseFuncTable::const_iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mXmlParseFuncTable)
    {
        const XmlParseFunc &sFunc = *sIterator;

        if (sFunc.mCanParseXmlFunc(sName) == XPR_TRUE)
        {
            aFormat = sFunc.mNewObjectFunc();

            XPR_ASSERT(aFormat != XPR_NULL);

            return aFormat->parseXml(aXmlReader, aElement);
        }
    }

    return XPR_FALSE;
}

template <typename T>
void FormatFactory::addShortParseFuncTable(void)
{
    mShortParseFuncTable.push_back(ShortParseFunc(T::canParseShort, T::parseShort));
}

template <typename T>
void FormatFactory::addXmlParseFuncTable(void)
{
    mXmlParseFuncTable.push_back(XmlParseFunc(T::canParseXml, T::newObject));
}
} // namespace cmd
} // namespace fxfile
