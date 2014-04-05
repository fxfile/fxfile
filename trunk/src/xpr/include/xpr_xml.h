//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_XML_H__
#define __XPR_XML_H__ 1
#pragma once

#include "xpr_dlsym.h"
#include "xpr_guid.h"
#include "xpr_char_set.h"
#include "xpr_string.h"
#include "xpr_wide_string.h"

namespace xpr
{
class XPR_DL_API XmlReader
{
    DISALLOW_COPY_AND_ASSIGN(XmlReader);

public:
    XmlReader(void);
    virtual ~XmlReader(void);

public:
    xpr_bool_t load(const xpr_char_t *aPath);
    xpr_bool_t load(const xpr_wchar_t *aPath);
    xpr_bool_t load(const xpr::string &aPath);
    xpr_bool_t load(const xpr::wstring &aPath);
    xpr_bool_t load(xpr_byte_t *aBuffer, xpr_size_t aBufferSize);
    void close(void);

public:
    xpr_rcode_t getEncoding(xpr_char_t *aEncoding, xpr_size_t aMaxLength) const;
    CharSet getEncoding(void) const;

    struct Element;
    struct Attribute;

    virtual Element *getRootElement(void) const;
    Element *nextElement(Element *aElement) const;
    Element *nextElement(Element *aElement, xpr_char_t *aNextName, xpr_size_t aMaxNextNameLength) const;
    Element *nextElement(Element *aElement, xpr_wchar_t *aNextName, xpr_size_t aMaxNextNameLength) const;
    Element *nextElement(Element *aElement, xpr::string &aNextName) const;
    Element *nextElement(Element *aElement, xpr::wstring &aNextName) const;
    Element *childElement(Element *aElement) const;
    Element *childElement(Element *aElement, xpr_char_t *aChildName, xpr_size_t aMaxChildNameLength) const;
    Element *childElement(Element *aElement, xpr_wchar_t *aChildName, xpr_size_t aMaxChildNameLength) const;
    Element *childElement(Element *aElement, xpr::string &aChildName) const;
    Element *childElement(Element *aElement, xpr::wstring &aChildName) const;
    xpr_bool_t getElement(Element *aElement, xpr_char_t *aName, xpr_size_t aMaxNameLength) const;
    xpr_bool_t getElement(Element *aElement, xpr_wchar_t *aName, xpr_size_t aMaxNameLength) const;
    xpr_bool_t getElement(Element *aElement, xpr::string &aName) const;
    xpr_bool_t getElement(Element *aElement, xpr::wstring &aName) const;
    xpr_bool_t testElement(Element *aElement, const xpr_char_t *aTestName) const;
    xpr_bool_t testElement(Element *aElement, const xpr_wchar_t *aTestName) const;
    xpr_bool_t testElement(Element *aElement, const xpr::string &aTestName) const;
    xpr_bool_t testElement(Element *aElement, const xpr::wstring &aTestName) const;

    xpr_bool_t getEntity(Element *aElement, xpr_char_t *aEntity, xpr_size_t aMaxEntityLength) const;
    xpr_bool_t getEntity(Element *aElement, xpr_wchar_t *aEntity, xpr_size_t aMaxEntityLength) const;
    xpr_bool_t getEntity(Element *aElement, xpr::string &aEntity) const;
    xpr_bool_t getEntity(Element *aElement, xpr::wstring &aEntity) const;
    xpr_bool_t getElementEntity(Element *aElement, xpr_char_t *aName, xpr_size_t aMaxNameLength, xpr_char_t *aEntity, xpr_size_t aMaxEntityLength) const;
    xpr_bool_t getElementEntity(Element *aElement, xpr_wchar_t *aName, xpr_size_t aMaxNameLength, xpr_wchar_t *aEntity, xpr_size_t aMaxEntityLength) const;
    xpr_bool_t getElementEntity(Element *aElement, xpr::string &aName, xpr::string &aEntity) const;
    xpr_bool_t getElementEntity(Element *aElement, xpr::wstring &aName, xpr::wstring &aEntity) const;

    Attribute *getFirstAttribute(Element *aElement) const;
    Attribute *nextAttribute(Attribute *aAttribute) const;
    xpr_bool_t getAttribute(Attribute *aAttribute, xpr_char_t *aName, xpr_size_t aMaxNameLength, xpr_char_t *aValue, xpr_size_t aMaxValueLength) const; 
    xpr_bool_t getAttribute(Attribute *aAttribute, xpr_wchar_t *aName, xpr_size_t aMaxNameLength, xpr_wchar_t *aValue, xpr_size_t aMaxValueLength) const;
    xpr_bool_t getAttribute(Attribute *aAttribute, xpr::string &aName, xpr::string &aValue) const;
    xpr_bool_t getAttribute(Attribute *aAttribute, xpr::wstring &aName, xpr::wstring &aValue) const;

protected:
    class PrivateObject;
    PrivateObject *mObject;
};

class XPR_DL_API XmlWriter
{
    DISALLOW_COPY_AND_ASSIGN(XmlWriter);

public:
    XmlWriter(void);
    virtual ~XmlWriter(void);

public:
    virtual xpr_bool_t beginDocument(const xpr_char_t *aEncoding);
    virtual xpr_bool_t endDocument(void);
    xpr_bool_t beginElement(const xpr_char_t *aName);
    xpr_bool_t beginElement(const xpr_wchar_t *aName);
    xpr_bool_t beginElement(const xpr::string &aName);
    xpr_bool_t beginElement(const xpr::wstring &aName);
    xpr_bool_t endElement(void);
    xpr_bool_t writeElement(const xpr_char_t *aName, const xpr_char_t *aValue);
    xpr_bool_t writeElement(const xpr_wchar_t *aName, const xpr_wchar_t *aValue);
    xpr_bool_t writeElement(const xpr::string &aName, const xpr::string &aValue);
    xpr_bool_t writeElement(const xpr::wstring &aName, const xpr::wstring &aValue);
    xpr_bool_t writeElementEntity(const xpr_char_t *aElement, const xpr_char_t *aEntity);
    xpr_bool_t writeElementEntity(const xpr_wchar_t *aElement, const xpr_wchar_t *aEntity);
    xpr_bool_t writeElementEntity(const xpr::string &aElement, const xpr::string &aEntity);
    xpr_bool_t writeElementEntity(const xpr::wstring &aElement, const xpr::wstring &aEntity);
    xpr_bool_t writeEntity(const xpr_char_t *aEntity);
    xpr_bool_t writeEntity(const xpr_wchar_t *aEntity);
    xpr_bool_t writeEntity(const xpr::string &aEntity);
    xpr_bool_t writeEntity(const xpr::wstring &aEntity);
    xpr_bool_t writeAttribute(const xpr_char_t *aName, const xpr_char_t *aValue);
    xpr_bool_t writeAttribute(const xpr_wchar_t *aName, const xpr_wchar_t *aValue);
    xpr_bool_t writeAttribute(const xpr::string &aName, const xpr::string &aValue);
    xpr_bool_t writeAttribute(const xpr::wstring &aName, const xpr::wstring &aValue);
    xpr_bool_t writeComment(const xpr_char_t *aComment);
    xpr_bool_t writeComment(const xpr_wchar_t *aComment);
    xpr_bool_t writeComment(const xpr::string &aComment);
    xpr_bool_t writeComment(const xpr::wstring &aComment);

public:
    xpr_bool_t save(const xpr_char_t *aPath) const;
    xpr_bool_t save(const xpr_wchar_t *aPath) const;
    xpr_bool_t save(const xpr::string &aPath) const;
    xpr_bool_t save(const xpr::wstring &aPath) const;
    xpr_byte_t *getXmlBuffer(xpr_size_t &aSize) const;
    virtual void close(void);

protected:
    class PrivateObject;
    PrivateObject *mObject;
};
} // namespace xpr

#endif // __XPR_XML_H__
