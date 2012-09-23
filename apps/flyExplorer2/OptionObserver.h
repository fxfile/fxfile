//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_OPTION_OBSERVER_H__
#define __FX_OPTION_OBSERVER_H__
#pragma once

class Option;

class OptionObserver
{
public:
    virtual onSaveOption(Option &aOption) = 0;
};

#endif // __FX_OPTION_OBSERVER_H__
