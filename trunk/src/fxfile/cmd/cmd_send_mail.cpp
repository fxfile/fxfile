//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_send_mail.h"

#include "winapi_ex.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t SendMailCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void SendMailCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sSelCount = sSearchResultCtrl->GetSelectedCount();
        if (sSelCount <= 0)
            return;

        xpr_sint_t sCount = 0;
        xpr_char_t **sPathes = new xpr_char_t*[sSelCount];
        memset(sPathes, 0, sizeof(xpr_char_t *)*sSelCount);

        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        if (XPR_IS_NOT_NULL(sPosition))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            xpr_char_t sPathA[XPR_MAX_PATH + 1];
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;
            SrItemData *sSrItemData;
            DWORD sFileAttributes;
            xpr_sint_t sIndex;

            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

                sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
                if (XPR_IS_NULL(sSrItemData))
                    continue;

                sSrItemData->getPath(sPath);

                sFileAttributes = ::GetFileAttributes(sPath);
                if (!XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                {
                    sInputBytes = _tcslen(sPath) * sizeof(xpr_tchar_t);
                    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
                    XPR_TCS_TO_MBS(sPath, &sInputBytes, sPathA, &sOutputBytes);
                    sPathA[sOutputBytes / sizeof(xpr_char_t)] = 0;

                    sPathes[sCount] = new xpr_char_t[XPR_MAX_PATH + 1];
                    strcpy(sPathes[sCount], sPathA);
                    sCount++;
                }
            }
        }

        if (sCount > 0)
        {
            SendMail(sPathes, sCount);
        }
        else
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.send_mail.msg.wrong_select"));
            sSearchResultCtrl->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        }

        xpr_sint_t i;
        for (i = 0; i < sSelCount; ++i)
        {
            XPR_SAFE_DELETE_ARRAY(sPathes[i]);
        }
        XPR_SAFE_DELETE_ARRAY(sPathes);

        sSearchResultCtrl->SetFocus();
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_sint_t sSelCount = sExplorerCtrl->GetSelectedCount();
        if (sSelCount <= 0)
            return;

        xpr_sint_t sCount = 0;
        xpr_char_t **sPathes = new xpr_char_t*[sSelCount];
        memset(sPathes, 0, sizeof(xpr_char_t *)*sSelCount);

        POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
        if (XPR_IS_NOT_NULL(sPosition))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            xpr_char_t sPathA[XPR_MAX_PATH + 1];
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;
            LPLVITEMDATA sLvItemData;
            xpr_sint_t sIndex;

            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (XPR_IS_NOT_NULL(sLvItemData))
                {
                    if ( XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) &&
                        !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                    {
                        GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                        sInputBytes = _tcslen(sPath) * sizeof(xpr_tchar_t);
                        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
                        XPR_TCS_TO_MBS(sPath, &sInputBytes, sPathA, &sOutputBytes);
                        sPathA[sOutputBytes / sizeof(xpr_char_t)] = 0;

                        sPathes[sCount] = new xpr_char_t[XPR_MAX_PATH + 1];
                        strcpy(sPathes[sCount], sPathA);
                        sCount++;
                    }
                }
            }
        }

        if (sCount > 0)
        {
            SendMail(sPathes, sCount);
        }
        else
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.send_mail.msg.wrong_select"));
            sExplorerCtrl->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        }

        xpr_sint_t i;
        for (i = 0; i < sSelCount; ++i)
        {
            XPR_SAFE_DELETE_ARRAY(sPathes[i]);
        }
        XPR_SAFE_DELETE_ARRAY(sPathes);
    }
}
} // namespace cmd
} // namespace fxfile
