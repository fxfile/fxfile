//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_OP_H__
#define __FXB_FILE_OP_H__
#pragma once

namespace fxb
{
class FileOp
{
public:
    FileOp(void);
    virtual ~FileOp(void);

public:

protected:
    xpr_size_t mRefIndex;
    static xpr_size_t mRefCount;
};
} // namespace fxb

#endif // __FXB_FILE_OP_H__
