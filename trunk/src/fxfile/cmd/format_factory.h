//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_FACTORY_H__
#define __FXFILE_FORMAT_FACTORY_H__ 1
#pragma once

#include "format.h"
#include "singleton_manager.h"

namespace fxfile
{
namespace cmd
{
class FormatFactory : public SingletonInstance
{
    friend class SingletonManager;

protected: FormatFactory(void);
public:   ~FormatFactory(void);

public:
    xpr_bool_t parseShort(const xpr::string &aFormatString, Format *&aFormat) const;
    void parseShortAsString(const xpr::string &aFormatString, Format *&aFormat) const;

public:
    xpr_bool_t parseFromXml(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement, Format *&aFormat) const;

private:
    template <typename T> void addShortParseFuncTable(void);
    template <typename T> void addXmlParseFuncTable(void);

private:
    typedef xpr_bool_t (*canParseShortFunc)(const xpr::string &aFormatString);
    typedef void       (*parseShortFunc   )(const xpr::string &aFormatString, Format *&aFormat);
    typedef xpr_bool_t (*canParseXmlFunc  )(const xpr::string &aElementName);
    typedef Format *   (*newObjectFunc    )(void);

    struct ShortParseFunc
    {
        ShortParseFunc(canParseShortFunc aCanParseShortFunc, parseShortFunc aParseShortFunc)
            : mCanParseShortFunc(aCanParseShortFunc), mParseShortFunc(aParseShortFunc)
        {
        }

        canParseShortFunc mCanParseShortFunc;
        parseShortFunc    mParseShortFunc;
    };

    struct XmlParseFunc
    {
        XmlParseFunc(canParseXmlFunc aCanParseXmlFunc, newObjectFunc aNewObjectFunc)
            : mCanParseXmlFunc(aCanParseXmlFunc), mNewObjectFunc(aNewObjectFunc)
        {
        }

        canParseXmlFunc mCanParseXmlFunc;
        newObjectFunc   mNewObjectFunc;
    };

    typedef std::list<ShortParseFunc> ShortParseFuncTable;
    typedef std::list<XmlParseFunc  > XmlParseFuncTable;

    ShortParseFuncTable mShortParseFuncTable;
    XmlParseFuncTable   mXmlParseFuncTable;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_FACTORY_H__
