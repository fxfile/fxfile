//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_OPTION_OBSERVER_H__
#define __FXFILE_OPTION_OBSERVER_H__ 1
#pragma once

namespace fxfile
{
class Option;

class OptionObserver
{
public:
    virtual void onChangedConfig(Option &aOption) = 0;
};
} // namespace fxfile

#endif // __FXFILE_OPTION_OBSERVER_H__
