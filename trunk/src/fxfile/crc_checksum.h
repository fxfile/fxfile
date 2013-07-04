//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CRC_CHECKSUM_H__
#define __FXFILE_CRC_CHECKSUM_H__ 1
#pragma once

namespace fxfile
{
xpr_rcode_t getFileCrcSfv(const xpr_tchar_t *aPath, xpr_char_t *aCrcVal);
xpr_rcode_t getFileCrcMd5(const xpr_tchar_t *aPath, xpr_char_t *aCrcVal);
} // namespace fxfile

#endif // __FXFILE_CRC_CHECKSUM_H__
