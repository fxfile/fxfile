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

namespace xpr
{
class XPR_DL_API XmlReader
{
    DISALLOW_COPY_AND_ASSIGN(XmlReader);

public:
    XmlReader(void);
    virtual ~XmlReader(void);

public:
    xpr_bool_t load(const xpr::string &aPath);
    xpr_bool_t load(xpr_byte_t *aBuffer, xpr_size_t aBufferSize);
    void close(void);

public:
    xpr_rcode_t getEncoding(xpr_char_t *aEncoding, xpr_size_t aMaxLength) const;
    CharSet getEncoding(void) const;

    struct Element;
    struct Attribute;

    virtual Element *getRootElement(void) const;
    Element *nextElement(Element *aElement) const;
    Element *nextElement(Element *aElement, xpr::string &aNextName) const;
    Element *childElement(Element *aElement) const;
    Element *childElement(Element *aElement, xpr::string &aChildName) const;
    xpr_bool_t getElement(Element *aElement, xpr::string &aName) const;
    xpr_bool_t testElement(Element *aElement, const xpr::string &aTestName) const;

    xpr_bool_t getEntity(Element *aElement, xpr::string &aEntity) const;
    xpr_bool_t getElementEntity(Element *aElement, xpr::string &aName, xpr::string &aEntity) const;

    Attribute *getFirstAttribute(Element *aElement) const;
    Attribute *nextAttribute(Attribute *aAttribute) const;
    xpr_bool_t getAttribute(Attribute *aAttribute, xpr::string &aName, xpr::string &aValue) const;

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
    xpr_bool_t beginElement(const xpr::string &aName);
    xpr_bool_t endElement(void);
    xpr_bool_t writeElement(const xpr::string &aName, const xpr::string &aValue);
    xpr_bool_t writeElementEntity(const xpr::string &aElement, const xpr::string &aEntity);
    xpr_bool_t writeEntity(const xpr::string &aEntity);
    xpr_bool_t writeAttribute(const xpr::string &aName, const xpr::string &aValue);
    xpr_bool_t writeComment(const xpr::string &aComment);

public:
    xpr_bool_t save(const xpr::string &aPath) const;
    xpr_byte_t *getXmlBuffer(xpr_size_t &aSize) const;
    virtual void close(void);

protected:
    class PrivateObject;
    PrivateObject *mObject;
};
} // namespace xpr

#endif // __XPR_XML_H__
