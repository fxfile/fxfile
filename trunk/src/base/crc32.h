//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_CRC32_H__
#define __FXFILE_BASE_CRC32_H__
#pragma once

#include <inttypes.h>
#include "hash-internal.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef HASH_CTX CRC32_CTX;

void CRC32_init(CRC32_CTX* ctx);
void CRC32_update(CRC32_CTX* ctx, const void* data, int len);
const uint8_t* CRC32_final(CRC32_CTX* ctx);

// Convenience method. Returns digest address.
const uint8_t* CRC32(const void* data, int len, uint8_t* digest);

#define CRC32_DIGEST_SIZE 4

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __FXFILE_BASE_CRC32_H__
