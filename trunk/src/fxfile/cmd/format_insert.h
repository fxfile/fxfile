//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_INSERT_H__
#define __FXFILE_FORMAT_INSERT_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatInsert : public Format
{
    typedef Format super;

public:
    FormatInsert(void);
    FormatInsert(InsertPosType aPosType, xpr_sint_t aPos, const xpr::string &aString);
    virtual ~FormatInsert(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    inline InsertPosType getPosType(void) const { return mPosType; }
    inline xpr_sint_t getPos(void) const { return mPos; }
    inline const xpr::string &getString(void) const { return mString; }

public:
    static xpr_bool_t canParseXml(const xpr::string &aElementName);

    static Format *newObject(void);

private:
    static void toString(const InsertPosType &aPosType, xpr::string &aString);
    static void toType(const xpr::string &aString, InsertPosType &aPosType);

private:
    InsertPosType mPosType;
    xpr_sint_t    mPos;
    xpr::string   mString;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_INSERT_H__
