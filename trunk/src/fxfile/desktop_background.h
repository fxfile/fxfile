//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DESKTOP_BACKGROUND_H__
#define __FXFILE_DESKTOP_BACKGROUND_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class DesktopBackground
{
public:
    enum Style
    {
        StyleNone,
        StyleCenter,
        StyleTile,
        StyleStretch,
    };

public:
    DesktopBackground(void);
    virtual ~DesktopBackground(void);

public:
    xpr_bool_t apply(Style aStyle, const xpr_tchar_t *aPath);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_DESKTOP_BACKGROUND_H__
