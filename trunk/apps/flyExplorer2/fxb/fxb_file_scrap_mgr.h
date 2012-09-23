//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_SCRAP_MGR_H__
#define __FXB_FILE_SCRAP_MGR_H__
#pragma once

#include "xpr_pattern.h"

namespace fxb
{
class FileScrapMgr : public xpr::Singleton<FileScrapMgr>
{
    friend class xpr::Singleton<FileScrapMgr>;

protected: FileScrapMgr(void);
public:   ~FileScrapMgr(void);

public:
    void add(LPSHELLFOLDER aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t aCount);
    void add(xpr_uint_t aGroupId, LPSHELLFOLDER aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t aCount);
    void addPath(const xpr_tchar_t *aPath, xpr_sint_t aCount);
    void addPath(xpr_uint_t aGroupId, const xpr_tchar_t *aPath, xpr_sint_t aCount);
    void doCopyOperation(const xpr_tchar_t *aTargetDir);
    void doMoveOperation(const xpr_tchar_t *aTargetDir);
    void doDeleteOperation(void);
    void removeList(void);
    void cutToClipboard(void);
    void copyToClipboard(void);
};
} // namespace fxb

#endif // __FXB_FILE_SCRAP_MGR_H__
