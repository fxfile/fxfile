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
    Element *childElement(Element *aElement) const;
    Element *childElement(Element *aElement, xpr_char_t *aChildName, xpr_size_t aMaxChildNameLength) const;
    Element *childElement(Element *aElement, xpr_wchar_t *aChildName, xpr_size_t aMaxChildNameLength) const;
    xpr_bool_t getElement(Element *aElement, xpr_char_t *aName, xpr_size_t aMaxNameLength) const;
    xpr_bool_t getElement(Element *aElement, xpr_wchar_t *aName, xpr_size_t aMaxNameLength) const;
    xpr_bool_t testElement(Element *aElement, const xpr_char_t *aTestName) const;
    xpr_bool_t testElement(Element *aElement, const xpr_wchar_t *aTestName) const;

    xpr_bool_t getEntity(Element *aElement, xpr_char_t *aEntity, xpr_size_t aMaxEntityLength) const;
    xpr_bool_t getEntity(Element *aElement, xpr_wchar_t *aEntity, xpr_size_t aMaxEntityLength) const;

    Attribute *getFirstAttribute(Element *aElement) const;
    Attribute *nextAttribute(Attribute *aAttribute) const;
    xpr_bool_t getAttribute(Attribute *aAttribute, xpr_char_t *aName, xpr_size_t aMaxNameLength, xpr_char_t *aValue, xpr_size_t aMaxValueLength) const; 
    xpr_bool_t getAttribute(Attribute *aAttribute, xpr_wchar_t *aName, xpr_size_t aMaxNameLength, xpr_wchar_t *aValue, xpr_size_t aMaxValueLength) const;

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
    xpr_bool_t endElement(void);
    xpr_bool_t writeElement(const xpr_char_t *aName, const xpr_char_t *aValue);
    xpr_bool_t writeElement(const xpr_wchar_t *aName, const xpr_wchar_t *aValue);
    xpr_bool_t writeElementEntity(const xpr_char_t *aElement, const xpr_char_t *aEntity);
    xpr_bool_t writeElementEntity(const xpr_wchar_t *aElement, const xpr_wchar_t *aEntity);
    xpr_bool_t writeEntity(const xpr_char_t *aEntity);
    xpr_bool_t writeEntity(const xpr_wchar_t *aEntity);
    xpr_bool_t writeAttribute(const xpr_char_t *aName, const xpr_char_t *aValue);
    xpr_bool_t writeAttribute(const xpr_wchar_t *aName, const xpr_wchar_t *aValue);
    xpr_bool_t writeComment(const xpr_char_t *aComment);
    xpr_bool_t writeComment(const xpr_wchar_t *aComment);

public:
    xpr_bool_t save(const xpr_char_t *aPath) const;
    xpr_bool_t save(const xpr_wchar_t *aPath) const;
    xpr_byte_t *getXmlBuffer(xpr_size_t &aSize) const;
    virtual void close(void);

protected:
    class PrivateObject;
    PrivateObject *mObject;
};

#define XPR_XML_GET_ATTRIBUTE_SINT32(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%d"), &aValue); \
        } \
    } while (false)

#define XPR_XML_GET_ATTRIBUTE_UINT32(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%u"), &aValue); \
        } \
    } while (false)

#define XPR_XML_GET_ATTRIBUTE_SINT64(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%lld"), &aValue); \
        } \
    } while (false)

#define XPR_XML_GET_ATTRIBUTE_UINT64(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%llu"), &aValue); \
        } \
    } while (false)

#define XPR_XML_GET_ATTRIBUTE_DOUBLE(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%lf"), &aValue); \
        } \
    } while (false)

#define XPR_XML_GET_ATTRIBUTE_SIZE(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%u"), &aValue); \
        } \
    } while (false)

#define XPR_XML_GET_ATTRIBUTE_GUID(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            aGuid.fromString(sValue); \
        } \
    } while (false)

#define XPR_XML_WRITE_ELEMENT_ENTITY_SINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%d"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ELEMENT_ENTITY_UINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ELEMENT_ENTITY_SINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%lld"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ELEMENT_ENTITY_UINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%llu"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ELEMENT_ENTITY_DOUBLE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lf"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ELEMENT_ENTITY_SIZE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ELEMENT_ENTITY_GUID(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        aGuid.toString(sValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ENTITY_SINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%d"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ENTITY_UINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ENTITY_SINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%lld"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ENTITY_UINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%llu"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ENTITY_DOUBLE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lf"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ENTITY_SIZE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ENTITY_GUID(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        aGuid.toString(sValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define XPR_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%d"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define XPR_XML_WRITE_ATTRIBUTE_UINT32(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define XPR_XML_WRITE_ATTRIBUTE_SINT64(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%lld"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define XPR_XML_WRITE_ATTRIBUTE_UINT64(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%llu"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define XPR_XML_WRITE_ATTRIBUTE_DOUBLE(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lf"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define XPR_XML_WRITE_ATTRIBUTE_SIZE(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(aValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define XPR_XML_WRITE_ATTRIBUTE_GUID(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        aGuid.toString(sValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)
} // namespace xpr

#endif // __XPR_XML_H__
