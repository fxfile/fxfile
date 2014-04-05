//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_REPLACE_H__
#define __FXFILE_FORMAT_REPLACE_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatReplace : public Format
{
    typedef Format super;

public:
    FormatReplace(void);
    FormatReplace(const xpr::tstring &aFindString, const xpr::tstring &aReplaceString);
    FormatReplace(const xpr::tstring &aFindString, const xpr::tstring &aReplaceString, xpr_sint_t aRepeat, xpr_bool_t aCaseSensitivity);
    virtual ~FormatReplace(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    inline const xpr::tstring &getFindString(void) const { return mFindString; }
    inline const xpr::tstring &getReplaceString(void) const { return mReplaceString; }
    inline xpr_sint_t getRepeat(void) const { return mRepeat; }
    inline xpr_bool_t isCaseSensitivity(void) const { return mCaseSensitivity; }

    static xpr_sint_t getDefaultRepeat(void);
    static xpr_bool_t getDefaultCaseSensitivity(void);

public:
    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);

private:
    xpr::tstring mFindString;
    xpr::tstring mReplaceString;
    xpr_sint_t   mRepeat;
    xpr_bool_t   mCaseSensitivity;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_REPLACE_H__
