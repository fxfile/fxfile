//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_APP_VER_H__
#define __FXFILE_APP_VER_H__ 1
#pragma once

namespace fxfile
{
// minor version
//   - odd version : stable version
//   - even version: development version

#define FXFILE_MAJOR_VER 2
#define FXFILE_MINOR_VER 0
#define FXFILE_PATCH_VER 3

#define FXFILE_VER_STRING "2.0.3"

void getAppVer(xpr_tchar_t *aAppVer);
void getFullAppVer(xpr_tchar_t *aAppVer);
} // namespace fxfile

#endif // __FXFILE_APP_VER_H__
