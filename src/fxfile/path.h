//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PATH_H__
#define __FXFILE_PATH_H__ 1
#pragma once

namespace fxfile
{
LPITEMIDLIST GetEnvPidl(const xpr::tstring &aEnv, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPidl(const xpr::tstring &aEnv, LPITEMIDLIST *aFullPidl, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPath(const xpr_tchar_t *aEnv, xpr_tchar_t *aPath, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPath(const xpr::tstring &aEnv, xpr::tstring &aPath, xpr_uint_t *aCSIDL = XPR_NULL);
void         GetEnvRealPath(const xpr::tstring &aEnvPath, xpr::tstring &aRealPath);
LPITEMIDLIST GetEnvRealPidl(const xpr::tstring &aEnvPath);
xpr_bool_t   GetEnvRealPidl(const xpr::tstring &aEnvPath, LPITEMIDLIST *aFullPidl);

xpr_bool_t   IsEqualFilter(const xpr_tchar_t *aFilter, const xpr_tchar_t *aExt);
xpr_bool_t   IsEqualFilterPath(const xpr_tchar_t *aFilter, const xpr_tchar_t *aPath);

xpr_bool_t   SetNewPath(xpr_tchar_t       *aPath,
                        const xpr_tchar_t *aDir,
                        const xpr_tchar_t *aFileName,
                        const xpr_tchar_t *aExt = XPR_NULL,
                        xpr_sint_t         aMaxNumber = 1000);

xpr_bool_t   SetNewPath(xpr::tstring       &aPath,
                        const xpr::tstring &aDir,
                        const xpr::tstring &aFileName,
                        const xpr::tstring &aExt,
                        xpr_sint_t          aMaxNumber = 1000);

void         CombinePath(xpr_tchar_t       *aPath,
                         const xpr_tchar_t *aDir,
                         const xpr_tchar_t *aFileName,
                         const xpr_tchar_t *aExt = XPR_NULL);

void         CombinePath(xpr::tstring       &aPath,
                         const xpr::tstring &aDir,
                         const xpr::tstring &aFileName,
                         const xpr_tchar_t  *aExt = XPR_NULL);

void         SplitPath(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName);
void         SplitPathExt(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName, xpr_tchar_t *aExt);
void         SplitFileNameExt(xpr_tchar_t *aPath, xpr_tchar_t *aFileName, xpr_tchar_t *aExt);
void         RemoveLastSplit(xpr_tchar_t *aPath);
void         RemoveLastSplit(xpr::tstring &aPath);
xpr_bool_t   IsEqualPath(const xpr_tchar_t *aPath1, const xpr_tchar_t *aPath2, xpr_bool_t aCase = XPR_FALSE);
xpr_bool_t   IsEqualPath(const xpr::tstring &aPath1, const xpr::tstring &aPath2, xpr_bool_t aCase = XPR_FALSE);

void         ConvUrlPath(xpr::tstring &aPath);
void         ConvDevPath(xpr::tstring &aPath);

xpr_bool_t   VerifyFileName(const xpr_tchar_t *aFileName);
xpr_bool_t   VerifyFileName(const xpr::tstring &aFileName);

void         RemoveInvalidChar(xpr::tstring &aFileName);
} // namespace fxfile

#endif // __FXFILE_PATH_H__
