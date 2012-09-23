//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_attr_time.h"

#include "cmd_context.h"
#include "ExplorerCtrl.h"
#include "FolderCtrl.h"

#include "AttrTimeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t AttrTimeCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = sFolderCtrl->isFileSystem();
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void AttrTimeCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        std::tstring sPath;
        sFolderCtrl->getCurPath(sPath);

        std::tstring sDir;
        std::tstring::size_type sOffset = sPath.rfind(XPR_STRING_LITERAL('\\'));
        if (sOffset != std::tstring::npos)
            sDir = sPath.substr(0, sOffset);

        AttrTimeDlg::PathList sPathList;
        sPathList.push_back(sPath);

        AttrTimeDlg sDlg;
        sDlg.setDir(sDir);
        sDlg.setItemList(sPathList);
        xpr_sintptr_t sId = sDlg.DoModal();
        if (sId == IDOK)
        {
            ::SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH | SHCNF_FLUSH, (LPCVOID)sDir.c_str(), XPR_NULL);
        }
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        std::tstring sDir;
        sExplorerCtrl->getCurPath(sDir);

        FileSysItemDeque sFileSysItemDeque;
        if (sExplorerCtrl->getSelFileSysItems(sFileSysItemDeque, XPR_FALSE) == XPR_FALSE)
            sExplorerCtrl->getAllFileSysItems(sFileSysItemDeque, XPR_FALSE);

        if (sFileSysItemDeque.empty() == XPR_TRUE)
            return;

        AttrTimeDlg::PathList sPathList;

        FileSysItemDeque::const_iterator sIterator = sFileSysItemDeque.begin();
        for (; sIterator != sFileSysItemDeque.end(); ++sIterator)
        {
            const FileSysItem *sItem = *sIterator;
            sPathList.push_back(sItem->mPath);
        }

        fxb::clear(sFileSysItemDeque);

        AttrTimeDlg sDlg;
        sDlg.setDir(sDir);
        sDlg.setItemList(sPathList);
        xpr_sintptr_t sId = sDlg.DoModal();
        if (sId == IDOK)
        {
            ::SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH | SHCNF_FLUSH, (LPCVOID)sDir.c_str(), XPR_NULL);
        }

        fxb::clear(sFileSysItemDeque);
    }
}
} // namespace cmd
