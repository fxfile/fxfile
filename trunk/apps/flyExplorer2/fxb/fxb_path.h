//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_PATH_H__
#define __FXB_PATH_H__
#pragma once

namespace fxb
{
LPITEMIDLIST GetEnvPidl(const std::tstring &aEnv, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPidl(const std::tstring &aEnv, LPITEMIDLIST *aFullPidl, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPath(const xpr_tchar_t *aEnv, xpr_tchar_t *aPath, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPath(const std::tstring &aEnv, std::tstring &aPath, xpr_uint_t *aCSIDL = XPR_NULL);
void         GetEnvRealPath(const std::tstring &aEnvPath, std::tstring &aRealPath);
LPITEMIDLIST GetEnvRealPidl(const std::tstring &aEnvPath);
xpr_bool_t   GetEnvRealPidl(const std::tstring &aEnvPath, LPITEMIDLIST *aFullPidl);

xpr_bool_t   IsEqualFilter(const xpr_tchar_t *aFilter, const xpr_tchar_t *aExt);
xpr_bool_t   IsEqualFilterPath(const xpr_tchar_t *aFilter, const xpr_tchar_t *aPath);

xpr_bool_t   SetNewPath(xpr_tchar_t       *aPath,
                        const xpr_tchar_t *aDir,
                        const xpr_tchar_t *aFileName,
                        const xpr_tchar_t *aExt = XPR_NULL,
                        xpr_sint_t         aMaxNumber = 1000);

xpr_bool_t   SetNewPath(std::tstring       &aPath,
                        const std::tstring &aDir,
                        const std::tstring &aFileName,
                        const std::tstring &aExt,
                        xpr_sint_t          aMaxNumber = 1000);

void         CombinePath(xpr_tchar_t       *aPath,
                         const xpr_tchar_t *aDir,
                         const xpr_tchar_t *aFileName,
                         const xpr_tchar_t *aExt = XPR_NULL);

void         CombinePath(std::tstring       &aPath,
                         const std::tstring &aDir,
                         const std::tstring &aFileName,
                         const xpr_tchar_t  *aExt = XPR_NULL);

void         SplitPath(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName);
void         SplitPathExt(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName, xpr_tchar_t *aExt);
void         SplitFileNameExt(xpr_tchar_t *aPath, xpr_tchar_t *aFileName, xpr_tchar_t *aExt);
void         RemoveLastSplit(xpr_tchar_t *aPath);
void         RemoveLastSplit(std::tstring &aPath);
xpr_bool_t   IsEqualPath(const xpr_tchar_t *aPath1, const xpr_tchar_t *aPath2, xpr_bool_t aCase = XPR_FALSE);
xpr_bool_t   IsEqualPath(const std::tstring &aPath1, const std::tstring &aPath2, xpr_bool_t aCase = XPR_FALSE);

void         ConvUrlPath(std::tstring &aPath);
void         ConvDevPath(std::tstring &aPath);

xpr_bool_t   VerifyFileName(const xpr_tchar_t *aFileName);
xpr_bool_t   VerifyFileName(const std::tstring &aFileName);

void         RemoveInvalidChar(std::tstring &aFileName);
} // namespace fxb

#endif // __FXB_PATH_H__
