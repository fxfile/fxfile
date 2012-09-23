//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_XML_H__
#define __FXB_XML_H__
#pragma once

#include "xpr_xml.h"

namespace fxb
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
    const std::string mEncoding;

    xpr_bool_t mDocument;
    xpr_bool_t mTopElement;
};
} // namespace fxb

#endif // __FXB_XML_H__
