//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_EXT_REMOVE_H__
#define __FXFILE_FORMAT_EXT_REMOVE_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatExtRemove : public Format
{
    typedef Format super;

public:
    FormatExtRemove(void);
    virtual ~FormatExtRemove(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);

private:
    xpr_bool_t mExtChange;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_EXT_REMOVE_H__
