//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FORMAT_SHORT_PARSER_H__
#define __FXFILE_FORMAT_SHORT_PARSER_H__ 1
#pragma once

#include "format.h"
#include "singleton_manager.h"

namespace fxfile
{
namespace cmd
{
class ShortFormatParser : public SingletonInstance
{
    friend class SingletonManager;

protected: ShortFormatParser(void);
public:   ~ShortFormatParser(void);

public:
    xpr_bool_t parse(const xpr::string &aFormat, FormatSequence &aFormatSequence) const;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FORMAT_SHORT_PARSER_H__
