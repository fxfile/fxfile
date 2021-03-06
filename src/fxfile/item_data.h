//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ITEM_DATA_H__
#define __FXFILE_ITEM_DATA_H__ 1
#pragma once

namespace fxfile
{
typedef struct LVITEMDATA
{
    xpr::string    mName;
    xpr_uint_t     mSignature;
    DWORD          mItemType;
    LPSHELLFOLDER  mShellFolder;
    LPSHELLFOLDER2 mShellFolder2;
    LPITEMIDLIST   mPidl;
    xpr_ulong_t    mShellAttributes;
    DWORD          mFileAttributes;
    xpr_uint_t     mThumbImageId;
} LVITEMDATA, *LPLVITEMDATA;

typedef struct TVITEMDATA
{
    LPSHELLFOLDER mShellFolder;
    LPITEMIDLIST  mPidl;
    LPITEMIDLIST  mFullPidl;
    xpr_ulong_t   mShellAttributes;
    DWORD         mFileAttributes;
    //WatchId     mWatchId;
    //AdvWatchId  mAdvWatchId;
    xpr_bool_t    mExpandPartial;
} TVITEMDATA, *LPTVITEMDATA;

typedef struct ABITEMDATA
{
    LPSHELLFOLDER mShellFolder;
    LPITEMIDLIST  mPidl;
    LPITEMIDLIST  mFullPidl;
    xpr_ulong_t   mShellAttributes;
    xpr_uint_t    mLevel;
    xpr_uint_t    mType;
} ABITEMDATA, *LPABITEMDATA;
} // namespace fxfile

#endif // __FXFILE_ITEM_DATA_H__
