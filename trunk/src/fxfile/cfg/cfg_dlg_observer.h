//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_DLG_OBSERVER_H__
#define __FXFILE_CFG_DLG_OBSERVER_H__ 1
#pragma once

namespace fxfile
{
namespace cfg
{
class CfgDlg;

class CfgDlgObserver
{
public:
    virtual xpr_bool_t onIsModified(CfgDlg &aCfgDlg) = 0;
    virtual void onSetModified(CfgDlg &aCfgDlg, xpr_bool_t aModified) = 0;
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_DLG_OBSERVER_H__
