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
const xpr_tchar_t kLanguagePackElement[] = XPR_STRING_LITERAL("LanguagePack");
const xpr_tchar_t kDescriptionElement[]  = XPR_STRING_LITERAL("Description");
const xpr_tchar_t kLanguageElement[]     = XPR_STRING_LITERAL("Language");
const xpr_tchar_t kAuthorElement[]       = XPR_STRING_LITERAL("Author");
const xpr_tchar_t kHomepageElement[]     = XPR_STRING_LITERAL("Homepage");
const xpr_tchar_t kEmailElement[]        = XPR_STRING_LITERAL("Email");
const xpr_tchar_t kStringTableElement[]  = XPR_STRING_LITERAL("StringTable");
const xpr_tchar_t kStringElement[]       = XPR_STRING_LITERAL("String");
const xpr_tchar_t kIDAttribute[]         = XPR_STRING_LITERAL("ID");

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

    XPR_TRACE(XPR_STRING_LITERAL("The \'%s\' language pack is loaded.\n"), mDesc.mLanguage);

    return XPR_TRUE;
}

xpr_bool_t LanguagePack::loadStringTable(StringTable *aStringTable)
{
    if (load(mPath, aStringTable) == XPR_FALSE)
        return XPR_FALSE;

    XPR_TRACE(XPR_STRING_LITERAL("The string table for \'%s\' language is loaded.\n"), mDesc.mLanguage);

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

    xpr_tchar_t sName[0xff] = {0};
    xpr_tchar_t sEntity[XPR_MAX_URL_LENGTH] = {0};
    xpr_tchar_t sValue[0xff] = {0};
    XmlReader::Attribute *sAttribute;
    xpr_tchar_t sId[StringTable::kMaxIdLength + 1] = {0};
    xpr_tchar_t sString[StringTable::kMaxStringLength + 1] = {0};

    XmlReader::Element *sInLanguagePackElement = sXmlReader.childElement(sLanguagePackElement);
    while (sInLanguagePackElement != XPR_NULL)
    {
        if (sXmlReader.getElement(sInLanguagePackElement, sName, 0xfe))
        {
            if (_tcscmp(sName, kDescriptionElement) == 0)
            {
                XmlReader::Element *sInDescriptionElement = sXmlReader.childElement(sInLanguagePackElement);
                while (sInDescriptionElement != XPR_NULL)
                {
                    if (sXmlReader.getElement(sInDescriptionElement, sName, 0xfe) == XPR_TRUE)
                    {
                        if (_tcscmp(sName, kLanguageElement) == 0)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mLanguage, kMaxLanguageLength);
                        }
                        else if (_tcscmp(sName, kAuthorElement) == 0)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mAuthor, kMaxAuthorLength);
                        }
                        else if (_tcscmp(sName, kHomepageElement) == 0)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mHomepage, kMaxHomepageLength);
                        }
                        else if (_tcscmp(sName, kEmailElement) == 0)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mEmail, kMaxEmailLength);
                        }
                        else if (_tcscmp(sName, kDescriptionElement) == 0)
                        {
                            sXmlReader.getEntity(sInDescriptionElement, mDesc.mDescription, kMaxDescriptionLength);
                        }
                    }

                    sInDescriptionElement = sXmlReader.nextElement(sInDescriptionElement);
                }
            }
            else if (XPR_IS_NOT_NULL(aStringTable) && _tcscmp(sName, kStringTableElement) == 0)
            {
                XmlReader::Element *sStringElement = sXmlReader.childElement(sInLanguagePackElement);
                while (sStringElement != XPR_NULL)
                {
                    if (sXmlReader.testElement(sStringElement, kStringElement) == XPR_TRUE)
                    {
                        sId[0] = 0;
                        sString[0] = 0;

                        sAttribute = sXmlReader.getFirstAttribute(sStringElement);
                        if (sAttribute != XPR_NULL)
                        {
                            if (sXmlReader.getAttribute(sAttribute, sName, 2, sValue, StringTable::kMaxIdLength) == XPR_TRUE)
                            {
                                if (_tcscmp(sName, kIDAttribute) == 0)
                                {
                                    _tcscpy(sId, sValue);
                                }
                            }
                        }

                        sXmlReader.getEntity(sStringElement, sString, StringTable::kMaxStringLength);

                        if (sId[0] != 0)
                        {
                            aStringTable->mStringTable[sId] = sString;
                        }
                    }

                    sStringElement = sXmlReader.nextElement(sStringElement);
                }
            }
        }

        sInLanguagePackElement = sXmlReader.nextElement(sInLanguagePackElement);
    }

    _tcscpy(mDesc.mFilePath, aPath);

    XPR_TRACE(XPR_STRING_LITERAL("The string table for \'%s\' language is loaded.\n"), mDesc.mLanguage);

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

    if (_tcsicmp(mDesc.mLanguage, aLanguage) != 0)
        return XPR_FALSE;

    return XPR_TRUE;
}
} // namespace base
} // namespace fxfile
