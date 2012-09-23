//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_CRC_CHECKSUM_H__
#define __FXB_CRC_CHECKSUM_H__
#pragma once

namespace fxb
{
#define CRC_ERR_NO                 0
#define CRC_ERR_FILE_READ         -1
#define CRC_ERR_READ_LENGTH       -2
#define CRC_ERR_INVALID_PARAMETER -3

xpr_sint_t getFileCrcSfv(const xpr_tchar_t *aPath, xpr_ulong_t *aCrcVal);
xpr_sint_t getFileCrcSfv(const xpr_tchar_t *aPath, xpr_char_t *aCrcVal);
xpr_sint_t getFileCrcMd5(const xpr_tchar_t *aPath, xpr_char_t *aCrcVal);
} // namespace fxb

#endif // __FXB_CRC_CHECKSUM_H__
