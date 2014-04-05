//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_DELETE_H__
#define __FXFILE_FORMAT_DELETE_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatDelete : public Format
{
    typedef Format super;

public:
    FormatDelete(void);
    FormatDelete(DeletePosType aPosType, xpr_sint_t aPos, xpr_sint_t aLength);
    virtual ~FormatDelete(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    inline DeletePosType getPosType(void) const { return mPosType; }
    inline xpr_sint_t getPos(void) const { return mPos; }
    inline xpr_sint_t getLength(void) const { return mLength; }

public:
    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);

private:
    static void toString(const DeletePosType &aPosType, xpr::tstring &aString);
    static void toType(const xpr::tstring &aString, DeletePosType &aPosType);

private:
    DeletePosType mPosType;
    xpr_sint_t    mPos;
    xpr_sint_t    mLength;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_DELETE_H__
