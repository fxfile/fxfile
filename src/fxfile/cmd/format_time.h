//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_TIME_H__
#define __FXFILE_FORMAT_TIME_H__ 1
#pragma once

#include "format.h"

namespace fxfile
{
namespace cmd
{
class FormatTime : public Format
{
    typedef Format super;

protected:
    FormatTime(void);
    FormatTime(const xpr::string &aFormat);

public:
    virtual ~FormatTime(void);

public:
    virtual xpr_bool_t parseXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement);

public:
    inline const xpr::string &getFormat(void) const { return mFormat; }

protected:
    static void parseShort(const xpr::string &aFormatString, xpr::string &aFormat);
    static void toString(const xpr::string &aFormat, const SYSTEMTIME &aSystemTime, xpr::string &aString);
    static void resolveTimeFormat(const xpr::string &aFormat, const SYSTEMTIME &aSystemTime, xpr::string &aString);

protected:
    xpr::string mFormat;
};

class FormatNowTime : public FormatTime
{
    typedef FormatTime super;

public:
    FormatNowTime(void);
    FormatNowTime(const xpr::string &aFormat);
    virtual ~FormatNowTime(void);

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
};

class FormatCTime : public FormatTime
{
    typedef FormatTime super;

public:
    FormatCTime(void);
    FormatCTime(const xpr::string &aFormat);
    virtual ~FormatCTime(void);

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
};

class FormatMTime : public FormatTime
{
    typedef FormatTime super;

public:
    FormatMTime(void);
    FormatMTime(const xpr::string &aFormat);
    virtual ~FormatMTime(void);

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
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_TIME_H__
