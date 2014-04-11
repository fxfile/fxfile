//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_XML_H__
#define __FXFILE_BASE_XML_H__ 1
#pragma once

#include "xpr_xml.h"

namespace fxfile
{
namespace base
{
class XmlReader : public xpr::XmlReader
{
    typedef xpr::XmlReader super;

public:
    XmlReader(void);
    virtual ~XmlReader(void);

public:
    virtual Element *getRootElement(void);
};

class XmlWriter : public xpr::XmlWriter
{
    typedef xpr::XmlWriter super;

public:
    XmlWriter(void);
    virtual ~XmlWriter(void);

public:
    virtual xpr_bool_t beginDocument(void);
    virtual xpr_bool_t endDocument(void);

public:
    virtual void close(void);

protected:
    xpr_bool_t mDocument;
    xpr_bool_t mTopElement;
};

#define FXFILE_XML_GET_ATTRIBUTE_SINT32(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%d"), &aValue); \
        } \
    } while (false)

#define FXFILE_XML_GET_ATTRIBUTE_UINT32(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%u"), &aValue); \
        } \
    } while (false)

#define FXFILE_XML_GET_ATTRIBUTE_SINT64(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%lld"), &aValue); \
        } \
    } while (false)

#define FXFILE_XML_GET_ATTRIBUTE_UINT64(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%llu"), &aValue); \
        } \
    } while (false)

#define FXFILE_XML_GET_ATTRIBUTE_DOUBLE(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%lf"), &aValue); \
        } \
    } while (false)

#define FXFILE_XML_GET_ATTRIBUTE_SIZE(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            _stscanf(sValue, XPR_STRING_LITERAL("%u"), &aValue); \
        } \
    } while (false)

#define FXFILE_XML_GET_ATTRIBUTE_GUID(aXmlReader, aAttribute, aName, aMaxNameLength, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        if (aXmlReader.getAttribute(aAttribute, aName, aMaxNameLength, sValue, 0xfe) == XPR_TRUE) \
        { \
            aGuid.fromString(sValue); \
        } \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_SINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_UINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_SINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lld"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_UINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%llu"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_DOUBLE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lf"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_SIZE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_GUID(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        aGuid.toString(sValue); \
        aXmlWriter.writeElementEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ELEMENT_ENTITY_STRING(aXmlWriter, aElement, aValue) \
    do \
    { \
        aXmlWriter.writeElementEntity(aElement, aValue.c_str()); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_SINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_UINT32(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_SINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lld"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_UINT64(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%llu"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_DOUBLE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lf"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_SIZE(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_GUID(aXmlWriter, aElement, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        aGuid.toString(sValue); \
        aXmlWriter.writeEntity(aElement, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ENTITY_STRING(aXmlWriter, aElement, aValue) \
    do \
    { \
        aXmlWriter.writeEntity(aElement, aValue.c_str()); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_SINT32(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_UINT32(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_SINT64(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lld"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_UINT64(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%llu"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_DOUBLE(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%lf"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_SIZE(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        _stprintf(sValue, XPR_STRING_LITERAL("%u"), aValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_GUID(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        xpr_tchar_t sValue[0xff] = {0}; \
        aGuid.toString(sValue); \
        aXmlWriter.writeAttribute(aAttribute, sValue); \
    } while (false)

#define FXFILE_XML_WRITE_ATTRIBUTE_STRING(aXmlWriter, aAttribute, aValue) \
    do \
    { \
        aXmlWriter.writeAttribute(aAttribute, aValue.c_str()); \
    } while (false)
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_XML_H__
