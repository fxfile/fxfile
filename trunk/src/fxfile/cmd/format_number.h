//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_NUMBER_H__
#define __FXFILE_FORMAT_NUMBER_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatNumber : public Format
{
    typedef Format super;

public:
    FormatNumber(void);
    explicit FormatNumber(xpr_sint_t aStart);
    FormatNumber(xpr_sint_t aStart, xpr_sint_t aStep);
    FormatNumber(xpr_sint_t aStart, xpr_sint_t aStep, xpr_bool_t aZeroFilled, xpr_size_t aDigit);
    virtual ~FormatNumber(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    inline xpr_sint_t getStart(void) const { return mStart; }
    inline xpr_sint_t getStep(void) const { return mStep; }
    inline xpr_bool_t isZeroFilled(void) const { return mZeroFilled; }
    inline xpr_size_t getDigit(void) const { return mDigit; }

public:
    static xpr_bool_t canParseShort(const xpr::tstring &aFormatString);
    static void       parseShort(const xpr::tstring &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);

private:
    xpr_sint_t mStart;
    xpr_sint_t mStep;
    xpr_bool_t mZeroFilled;
    xpr_size_t mDigit;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_NUMBER_H__
