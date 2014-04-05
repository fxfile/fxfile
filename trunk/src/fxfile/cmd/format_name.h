//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_NAME_H__
#define __FXFILE_FORMAT_NAME_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatName : public Format
{
    typedef Format super;

protected:
    FormatName(void);
    FormatName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos);

public:
    virtual ~FormatName(void);

public:
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

public:
    inline xpr_sint_t getBeginPos(void) const { return mBeginPos; }
    inline xpr_sint_t getEndPos(void) const { return mEndPos; }
    static xpr_sint_t getDefaultBeginPos(void);
    static xpr_sint_t getDefaultEndPos(void);

protected:
    static void parseShort(const xpr::tstring &aFormatString,
                           xpr_size_t          aParseBeginPos,
                           xpr_sint_t         &aBeginPos,
                           xpr_sint_t         &aEndPos);

protected:
    xpr_sint_t mBeginPos;
    xpr_sint_t mEndPos;
};

class FormatBaseFileName : public FormatName
{
    typedef FormatName super;

public:
    FormatBaseFileName(void);
    FormatBaseFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos);
    virtual ~FormatBaseFileName(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseShort(const xpr::tstring &aFormatString);
    static void       parseShort(const xpr::tstring &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);
};

class FormatFileExt : public FormatName
{
    typedef FormatName super;

public:
    FormatFileExt(void);
    FormatFileExt(xpr_sint_t aBeginPos, xpr_sint_t aEndPos);
    virtual ~FormatFileExt(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseShort(const xpr::tstring &aFormatString);
    static void       parseShort(const xpr::tstring &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);
};

class FormatFileName : public FormatName
{
    typedef FormatName super;

public:
    FormatFileName(void);
    FormatFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos);
    virtual ~FormatFileName(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseShort(const xpr::tstring &aFormatString);
    static void       parseShort(const xpr::tstring &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);
};

class FormatOrgBaseFileName : public FormatName
{
    typedef FormatName super;

public:
    FormatOrgBaseFileName(void);
    FormatOrgBaseFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos);
    virtual ~FormatOrgBaseFileName(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseShort(const xpr::tstring &aFormatString);
    static void       parseShort(const xpr::tstring &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);
};

class FormatOrgFileExt : public FormatName
{
    typedef FormatName super;

public:
    FormatOrgFileExt(void);
    FormatOrgFileExt(xpr_sint_t aBeginPos, xpr_sint_t aEndPos);
    virtual ~FormatOrgFileExt(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseShort(const xpr::tstring &aFormatString);
    static void       parseShort(const xpr::tstring &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);
};

class FormatOrgFileName : public FormatName
{
    typedef FormatName super;

public:
    FormatOrgFileName(void);
    FormatOrgFileName(xpr_sint_t aBeginPos, xpr_sint_t aEndPos);
    virtual ~FormatOrgFileName(void);

public:
    virtual void rename(RenameContext &aContext) const;

    virtual xpr_bool_t write(base::XmlWriter &aXmlWriter) const;
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

    virtual Format *newClone(void) const;

public:
    static xpr_bool_t canParseShort(const xpr::tstring &aFormatString);
    static void       parseShort(const xpr::tstring &aFormatString, Format *&aFormat);

    static xpr_bool_t canParseXml(const xpr::tstring &aElementName);

    static Format *newObject(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_NAME_H__
