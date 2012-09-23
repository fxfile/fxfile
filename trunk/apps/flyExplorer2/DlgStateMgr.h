//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DLG_STATE_MGR_H__
#define __FX_DLG_STATE_MGR_H__
#pragma once

#include "xpr_pattern.h"

class DlgStateMgr : public xpr::Singleton<DlgStateMgr>
{
    friend class xpr::Singleton<DlgStateMgr>;

protected: DlgStateMgr(void);
public:   ~DlgStateMgr(void);

public:
    enum
    {
        TypeNone = 0,
        TypeBegin,
        TypeAttrTime = TypeBegin,
        TypeBatchCreate,
        TypeCrc,
        TypeDosCmd,
        TypeDriveSel,
        TypeFileScrap,
        TypeFileCombine,
        TypeFileMerge,
        TypeFileSplit,
        TypeHistory,
        TypeGoPath,
        TypeBookmarkEdit,
        TypeParamExec,
        TypePicConv,
        TypePicViewer,
        TypeRename,
        TypeSearch,
        TypeSelFilter,
        TypeSelName,
        TypeSharedFile,
        TypeSyncDirs,
        TypeTextMerge,
        TypeTextOut,
        TypeEnd,
    };

    enum
    {
        SubTypeNone = 0,

        SubTypeBatchCreateBegin = 10,
        SubTypeBatchCreate = SubTypeBatchCreateBegin,
        SubTypeBatchCreateFormat,
        SubTypeBatchCreateEnd,

        SubTypeCrcBegin,
        SubTypeCrcCreate = SubTypeCrcBegin,
        SubTypeCrcCheck,
        SubTypeCrcEnd,

        SubTypeFileScrapBegin,
        SubTypeFileScrap = SubTypeFileScrapBegin,
        SubTypeFileScrapDrop,
        SubTypeFileScrapEnd,

        SubTypeRenameBegin,
        SubTypeRename = SubTypeRenameBegin,
        SubTypeRename1,
        SubTypeRename2,
        SubTypeRename3,
        SubTypeRename4,
        SubTypeRename5,
        SubTypeRenameDirectInput,
        SubTypeRenameEdit,
        SubTypeRenameEnd,

        SubTypeSearchBegin,
        SubTypeSearch = SubTypeSearchBegin,
        SubTypeSchLoc,
        SubTypeSearchEnd,

        SubTypeSelFilterBegin,
        SubTypeSelFilter = SubTypeSelFilterBegin,
        SubTypeUnSelFilter,
        SubTypeSelFilterEnd,

        SubTypeSelNameBegin,
        SubTypeSelName = SubTypeSelNameBegin,
        SubTypeUnSelName,
        SubTypeSelNameEnd,
    };

    typedef std::deque<std::tstring> PathDeque;

public:
    xpr_bool_t getName(xpr_uint_t aType, std::tstring &aName);
    xpr_bool_t getPath(xpr_uint_t aType, xpr_uint_t aSubType, std::tstring &aPath);
    xpr_bool_t getPathList(xpr_uint_t aType, PathDeque &aPathDeque);

    xpr_bool_t clear(xpr_uint_t aType);
    void clearAll(void);
};

#endif // __FX_DLG_STATE_MGR_H__
