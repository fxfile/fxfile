//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_STRING_H__
#define __FXFILE_FORMAT_STRING_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatString : public Format
{
    typedef Format super;

public:
    FormatString(void);
    FormatString(const xpr::string &aString);
    virtual ~FormatString(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseShort(const xpr::string &aFormatString);
    static void       parseShort(const xpr::string &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::string &aElementName);

    static Format *newObject(void);

public:
    inline const xpr::string &getString(void) const { return mString; }

private:
    xpr::string mString;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_STRING_H__
