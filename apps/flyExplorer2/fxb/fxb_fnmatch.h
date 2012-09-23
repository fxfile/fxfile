//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FNMATCH_H__
#define __FXB_FNMATCH_H__
#pragma once

#define FNM_PATHNAME    (1 << 0) /* No wildcard can ever match `/'.  */
#define FNM_NOESCAPE    (1 << 1) /* Backslashes don't quote special chars.  */
#define FNM_PERIOD      (1 << 2) /* Leading `.' is matched only explicitly.  */

#define FNM_FILE_NAME   FNM_PATHNAME /* Preferred GNU name.  */
#define FNM_LEADING_DIR (1 << 3) /* Ignore `/...' after a match.  */
#define FNM_CASEFOLD    (1 << 4) /* Compare without regard to case.  */

#define FNM_MATCH   0
#define FNM_NOMATCH 1

extern "C" inline xpr_sint_t fnmatch(const xpr_tchar_t *pattern, const xpr_tchar_t *string, xpr_sint_t flags);

#endif // __FXB_FNMATCH_H__
