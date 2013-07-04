//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_checksum.h"

#include "functors.h"
#include "crc_create_dlg.h"
#include "crc_verify_dlg.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t ChecksumCreateCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
        {
            sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
        }
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ChecksumCreateCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->GetSelectedCount() <= 0)
            return;

        FileSysItemDeque sFileSysItemDeque;
        sExplorerCtrl->getSelFileSysItems(sFileSysItemDeque, XPR_FALSE, SFGAO_FILESYSTEM, SFGAO_FOLDER);

        if (sFileSysItemDeque.empty() == XPR_TRUE)
            return;

        CrcCreateDlg sDlg;

        FileSysItemDeque::iterator sIterator = sFileSysItemDeque.begin();
        for (; sIterator != sFileSysItemDeque.end(); ++sIterator)
        {
            FileSysItem *sFileSysItem = *sIterator;
            sDlg.addPath(sFileSysItem->mPath.c_str());
        }

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

        if (sFileSysItemDeque.size() > 1)
        {
            sExplorerCtrl->getCurPath(sPath);

            xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
            xpr_sint_t nLen = (xpr_sint_t)_tcslen(sPath);
            if (nLen <= 3)
                _stprintf(sFileName, XPR_STRING_LITERAL("%c.sfv"), sPath[0]);
            else
            {
                xpr_tchar_t *sSplit = _tcsrchr(sPath, XPR_STRING_LITERAL('\\'));
                _stprintf(sFileName, XPR_STRING_LITERAL("%s.sfv"), sSplit);
            }

            _tcscat(sPath, sFileName);
        }
        else
        {
            sIterator = sFileSysItemDeque.begin();

            FileSysItem *sFileSysItem = *sIterator;
            _stprintf(sPath, XPR_STRING_LITERAL("%s.sfv"), sFileSysItem->mPath.c_str());
        }

        sDlg.saveDefaultPath(sPath);
        sDlg.DoModal();

        clear(sFileSysItemDeque);
    }
}

xpr_sint_t ChecksumVerifyCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
        {
            sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
        }
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ChecksumVerifyCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->GetSelectedCount() <= 0)
            return;

        FileSysItemDeque sFileSysItemDeque;
        sExplorerCtrl->getSelFileSysItems(sFileSysItemDeque, XPR_FALSE, SFGAO_FILESYSTEM, SFGAO_FOLDER);

        if (sFileSysItemDeque.empty() == XPR_TRUE)
            return;

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        sExplorerCtrl->getCurPath(sPath);

        CrcVerifyDlg sDlg(sPath);

        FileSysItemDeque::iterator sIterator = sFileSysItemDeque.begin();
        for (; sIterator != sFileSysItemDeque.end(); ++sIterator)
        {
            FileSysItem *sFileSysItem = *sIterator;

            if (IsEqualFileExt(sFileSysItem->mPath.c_str(), XPR_STRING_LITERAL(".sfv")) ||
                IsEqualFileExt(sFileSysItem->mPath.c_str(), XPR_STRING_LITERAL(".md5")))
            {
                sDlg.addPath(sFileSysItem->mPath.c_str());
            }
        }

        sDlg.DoModal();

        clear(sFileSysItemDeque);
    }
}
} // namespace cmd
} // namespace fxfile
