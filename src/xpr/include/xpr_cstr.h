//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_CSTR_H__
#define __XPR_CSTR_H__
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_std.h"

namespace xpr
{
XPR_DL_API xpr_size_t  xpr_strlen(const xpr_char_t *aStr);
XPR_DL_API xpr_char_t *xpr_strcpy(xpr_char_t *aDest, xpr_size_t aDestSize, const xpr_char_t *aSrc, xpr_size_t aCount);
XPR_DL_API xpr_char_t *xpr_strcat(xpr_char_t *aStr, xpr_size_t aSize, const xpr_char_t *aAppend, xpr_size_t aCount);
XPR_DL_API xpr_char_t *xpr_strchr(const xpr_char_t *aStr, xpr_char_t aChar);
XPR_DL_API xpr_char_t *xpr_strrchr(const xpr_char_t *aStr, xpr_char_t aChar);
XPR_DL_API xpr_sint_t  xpr_strcmp(const xpr_char_t *aStr1, const xpr_char_t *aStr2, xpr_size_t aLength);
XPR_DL_API xpr_sint_t  xpr_stricmp(const xpr_char_t *aStr1, const xpr_char_t *aStr2, xpr_size_t aLength);
XPR_DL_API xpr_char_t *xpr_strstr(const xpr_char_t *aStr, const xpr_char_t *aSubStr);
XPR_DL_API xpr_char_t *xpr_strnstr(const xpr_char_t *aStr, const xpr_char_t *aSubStr, xpr_size_t aLength);
XPR_DL_API xpr_char_t *xpr_strrstr(const xpr_char_t *aStr, const xpr_char_t *aSubStr);
XPR_DL_API xpr_size_t  xpr_strcspn(const xpr_char_t *aStr, const xpr_char_t *aCharSet);
XPR_DL_API xpr_char_t *xpr_strpbrk(const xpr_char_t *aStr, const xpr_char_t *aCharSet);
XPR_DL_API xpr_size_t  xpr_strspn(const xpr_char_t *aStr, const xpr_char_t *aCharSet);
XPR_DL_API xpr_char_t *xpr_strupr(xpr_char_t *aStr);
XPR_DL_API xpr_char_t *xpr_strlwr(xpr_char_t *aStr);
XPR_DL_API xpr_char_t *xpr_strtok(xpr_char_t *aStr, const xpr_char_t *aDelimit);
XPR_DL_API xpr_char_t *xpr_strdup(const xpr_char_t *aStr);

XPR_DL_API xpr_rcode_t xpr_itoa(xpr_sint32_t aValue, xpr_char_t *aStr, xpr_size_t aSize);
XPR_DL_API xpr_rcode_t xpr_i64toa(xpr_sint64_t aValue, xpr_char_t *aStr, xpr_size_t aSize);
XPR_DL_API xpr_rcode_t xpr_ftoa(xpr_float_t aValue, xpr_char_t *aStr, xpr_size_t aSize);
XPR_DL_API xpr_rcode_t xpr_dbtoa(xpr_double_t aValue, xpr_char_t *aStr, xpr_size_t aSize);
XPR_DL_API xpr_rcode_t xpr_atoi(const xpr_char_t *aStr, xpr_sint32_t *aValue);
XPR_DL_API xpr_rcode_t xpr_atoi64(const xpr_char_t *aStr, xpr_sint64_t *aValue);
XPR_DL_API xpr_rcode_t xpr_atof(const xpr_char_t *aStr, xpr_float_t *aValue);
XPR_DL_API xpr_rcode_t xpr_atodb(const xpr_char_t *aStr, xpr_double_t *aValue);

XPR_DL_API xpr_rcode_t xpr_scanf(const xpr_char_t *aFormat, ...);
XPR_DL_API xpr_rcode_t xpr_vscanf(const xpr_char_t *aFormat, va_list aArgs);
XPR_DL_API xpr_rcode_t xpr_fscanf(xpr_file_t *aFile, const xpr_char_t *aFormat, ...);
XPR_DL_API xpr_rcode_t xpr_vfscanf(xpr_file_t *aFile, const xpr_char_t *aFormat, va_list aArgs);
XPR_DL_API xpr_rcode_t xpr_snscanf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, ...);
XPR_DL_API xpr_rcode_t xpr_vsnscanf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, va_list aArgs);

XPR_DL_API xpr_rcode_t xpr_printf(const xpr_char_t *aFormat, ...);
XPR_DL_API xpr_rcode_t xpr_fprintf(xpr_file_t *aFile, const xpr_char_t *aFormat, ...);
XPR_DL_API xpr_rcode_t xpr_snprintf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, ...);
XPR_DL_API xpr_rcode_t xpr_vprintf(const xpr_char_t *aFormat, va_list aArgs);
XPR_DL_API xpr_rcode_t xpr_vfprintf(xpr_file_t *aFile, const xpr_char_t *aFormat, va_list aArgs);
XPR_DL_API xpr_rcode_t xpr_vsnprintf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, va_list aArgs);
} // namespace xpr

#endif // __XPR_STRING_H__
