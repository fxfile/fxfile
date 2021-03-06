//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ADDRESS_BAR_OBSERVER_H__
#define __FXFILE_ADDRESS_BAR_OBSERVER_H__ 1
#pragma once

namespace fxfile
{
class AddressBar;

class AddressBarObserver
{
public:
    virtual xpr_bool_t onExplore(AddressBar &aAddressBar, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy) = 0;
    virtual LPITEMIDLIST onFailExec(AddressBar &aAddressBar, const xpr_tchar_t *aExecPath) = 0;
    virtual void onMoveFocus(AddressBar &aAddressBar) = 0;
};
} // namespace fxfile

#endif // __FXFILE_ADDRESS_BAR_OBSERVER_H__
