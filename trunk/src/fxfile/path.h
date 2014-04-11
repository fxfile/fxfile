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
LPITEMIDLIST GetEnvPidl(const xpr::string &aEnv, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPidl(const xpr::string &aEnv, LPITEMIDLIST *aFullPidl, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPath(const xpr_tchar_t *aEnv, xpr_tchar_t *aPath, xpr_uint_t *aCSIDL = XPR_NULL);
xpr_bool_t   GetEnvPath(const xpr::string &aEnv, xpr::string &aPath, xpr_uint_t *aCSIDL = XPR_NULL);
void         GetEnvRealPath(const xpr::string &aEnvPath, xpr::string &aRealPath);
LPITEMIDLIST GetEnvRealPidl(const xpr::string &aEnvPath);
xpr_bool_t   GetEnvRealPidl(const xpr::string &aEnvPath, LPITEMIDLIST *aFullPidl);

xpr_bool_t   IsEqualFilter(const xpr_tchar_t *aFilter, const xpr_tchar_t *aExt);
xpr_bool_t   IsEqualFilterPath(const xpr_tchar_t *aFilter, const xpr_tchar_t *aPath);

xpr_bool_t   SetNewPath(xpr_tchar_t       *aPath,
                        const xpr_tchar_t *aDir,
                        const xpr_tchar_t *aFileName,
                        const xpr_tchar_t *aExt = XPR_NULL,
                        xpr_sint_t         aMaxNumber = 1000);

xpr_bool_t   SetNewPath(xpr::string       &aPath,
                        const xpr::string &aDir,
                        const xpr::string &aFileName,
                        const xpr::string &aExt,
                        xpr_sint_t         aMaxNumber = 1000);

void         CombinePath(xpr_tchar_t       *aPath,
                         const xpr_tchar_t *aDir,
                         const xpr_tchar_t *aFileName,
                         const xpr_tchar_t *aExt = XPR_NULL);

void         CombinePath(xpr::string       &aPath,
                         const xpr::string &aDir,
                         const xpr::string &aFileName,
                         const xpr_tchar_t *aExt = XPR_NULL);

void         SplitPath(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName);
void         SplitPathExt(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName, xpr_tchar_t *aExt);
void         SplitFileNameExt(xpr_tchar_t *aPath, xpr_tchar_t *aFileName, xpr_tchar_t *aExt);
void         RemoveLastSplit(xpr_tchar_t *aPath);
void         RemoveLastSplit(xpr::string &aPath);
xpr_bool_t   IsEqualPath(const xpr_tchar_t *aPath1, const xpr_tchar_t *aPath2, xpr_bool_t aCase = XPR_FALSE);
xpr_bool_t   IsEqualPath(const xpr::string &aPath1, const xpr::string &aPath2, xpr_bool_t aCase = XPR_FALSE);

void         ConvUrlPath(xpr::string &aPath);
void         ConvDevPath(xpr::string &aPath);

xpr_bool_t   VerifyFileName(const xpr_tchar_t *aFileName);
xpr_bool_t   VerifyFileName(const xpr::string &aFileName);

void         RemoveInvalidChar(xpr::string &aFileName);
} // namespace fxfile

#endif // __FXFILE_PATH_H__
