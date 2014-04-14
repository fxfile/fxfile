//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "language_pack.h"

#include "string_table.h"
#include "xml.h"

namespace fxfile
{
namespace base
{
namespace
{
const xpr_tchar_t kLanguagePackElement[] = XPR_STRING_LITERAL("LanguagePack");
const xpr_tchar_t kDescriptionElement[]  = XPR_STRING_LITERAL("Description");
const xpr_tchar_t kLanguageElement[]     = XPR_STRING_LITERAL("Language");
const xpr_tchar_t kAuthorElement[]       = XPR_STRING_LITERAL("Author");
const xpr_tchar_t kHomepageElement[]     = XPR_STRING_LITERAL("Homepage");
const xpr_tchar_t kEmailElement[]        = XPR_STRING_LITERAL("Email");
const xpr_tchar_t kStringTableElement[]  = XPR_STRING_LITERAL("StringTable");
const xpr_tchar_t kStringElement[]       = XPR_STRING_LITERAL("String");
const xpr_tchar_t kIDAttribute[]         = XPR_STRING_LITERAL("ID");
} // namespace anonymous

LanguagePack::LanguagePack(void)
{
    mPath[0] = 0;
    memset(&mDesc, 0, sizeof(mDesc));
}

LanguagePack::~LanguagePack(void)
{
}

xpr_bool_t LanguagePack::load(const xpr_tchar_t *aPath)
{
    if (load(aPath, XPR_NULL) == XPR_FALSE)
        return XPR_FALSE;

    _tcscpy(mPath, aPath);

    XPR_TRACE(XPR_STRING_LITERAL("The \'%s\' language pack is loaded.\n"), mDesc.mLanguage.c_str());

    return XPR_TRUE;
}

xpr_bool_t LanguagePack::loadStringTable(StringTable *aStringTable)
{
    if (load(mPath, aStringTable) == XPR_FALSE)
        return XPR_FALSE;

    XPR_TRACE(XPR_STRING_LITERAL("The string table for \'%s\' language is loaded.\n"), mDesc.mLanguage.c_str());

    return XPR_TRUE;
}

xpr_bool_t LanguagePack::load(const xpr_tchar_t *aPath, StringTable *aStringTable)
{
    XmlReader sXmlReader;
    if (sXmlReader.load(aPath) != XPR_TRUE)
        return XPR_FALSE;

    XmlReader::Element *sLanguagePackElement = sXmlReader.getRootElement();
    if (sLanguagePackElement == XPR_NULL)
        return XPR_FALSE;

    if (sXmlReader.testElement(sLanguagePackElement, kLanguagePackElement) == XPR_FALSE)
        return XPR_FALSE;

    xpr::string  sName;
    xpr::string  sEntity;
    xpr::string  sValue;
    XmlReader::Attribute *sAttribute;
    xpr::string *sId;
    xpr::string  sString;

    XmlReader::Element *sInLanguagePackElement = sXmlReader.childElement(sLanguagePackElement);
    while (sInLanguagePackElement != XPR_NULL)
    {
        if (sXmlReader.getElement(sInLanguagePackElement, sName))
        {
            if (sName == kDescriptionElement)
            {
                XmlReader::Element *sInDescriptionElement = sXmlReader.childElement(sInLanguagePackElement);
                while (sInDescriptionElement != XPR_NULL)
                {
                    if (sXmlReader.getElement(sInDescriptionElement, sName) == XPR_TRUE)
                    {
                        if (sName == kLanguageElement)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mLanguage);
                        }
                        else if (sName == kAuthorElement)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mAuthor);
                        }
                        else if (sName == kHomepageElement)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mHomepage);
                        }
                        else if (sName == kEmailElement)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mEmail);
                        }
                        else if (sName == kDescriptionElement)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mDescription);
                        }
                    }

                    sInDescriptionElement = sXmlReader.nextElement(sInDescriptionElement);
                }
            }
            else if (XPR_IS_NOT_NULL(aStringTable) && sName == kStringTableElement)
            {
                XmlReader::Element *sStringElement = sXmlReader.childElement(sInLanguagePackElement);
                while (sStringElement != XPR_NULL)
                {
                    if (sXmlReader.testElement(sStringElement, kStringElement) == XPR_TRUE)
                    {
                        sId = XPR_NULL;

                        sAttribute = sXmlReader.getFirstAttribute(sStringElement);
                        if (sAttribute != XPR_NULL)
                        {
                            if (sXmlReader.getAttribute(sAttribute, sName, sValue) == XPR_TRUE)
                            {
                                if (sName == kIDAttribute)
                                {
                                    sId = &sValue;
                                    sString.clear();
                                }
                            }
                        }

                        sXmlReader.getEntity(sStringElement, sString);

                        if (XPR_IS_NOT_NULL(sId))
                        {
                            aStringTable->mStringTable[*sId] = sString;
                        }
                    }

                    sStringElement = sXmlReader.nextElement(sStringElement);
                }
            }
        }

        sInLanguagePackElement = sXmlReader.nextElement(sInLanguagePackElement);
    }

    mDesc.mFilePath = aPath;

    XPR_TRACE(XPR_STRING_LITERAL("The string table for \'%s\' language is loaded.\n"), mDesc.mLanguage.c_str());

    return XPR_TRUE;
}

const LanguagePack::Desc *LanguagePack::getLanguageDesc(void) const
{
    return &mDesc;
}

xpr_bool_t LanguagePack::equalLanguage(const xpr_tchar_t *aLanguage) const
{
    if (XPR_IS_NULL(aLanguage))
        return XPR_FALSE;

    if (mDesc.mLanguage.compare_case(aLanguage) != 0)
        return XPR_FALSE;

    return XPR_TRUE;
}
} // namespace base
} // namespace fxfile
