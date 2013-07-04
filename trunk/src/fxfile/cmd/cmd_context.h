//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_CONTEXT_H__
#define __FXFILE_CMD_CONTEXT_H__ 1
#pragma once

namespace fxfile
{
class MainFrame;
class FolderCtrl;
class ExplorerCtrl;
class SearchResultCtrl;
class FileScrapPane;

namespace cmd
{
class CommandParameters;

class CommandContext
{
public:
    CommandContext(void);
    virtual ~CommandContext(void);

public:
    xpr_uint_t getCommandId(void) const;
    MainFrame *getMainFrame(void) const;
    CWnd *getTargetWnd(void) const;
    CommandParameters *getParameters(void) const;
    void setCommandId(xpr_uint_t aCommandId);
    void setMainFrame(MainFrame *aMainFrame);
    void setTargetWnd(CWnd *aWnd);
    void setParameters(CommandParameters *aParameters);

public:
    static void getKnownCtrl(CommandContext &aContext, FolderCtrl *&aFolderCtrl, ExplorerCtrl *&aExplorerCtrl, SearchResultCtrl *&aSearchResultCtrl, FileScrapPane *&aFileScrapPane);
    static void castKnownCtrl(CWnd *aWnd, FolderCtrl *&aFolderCtrl, ExplorerCtrl *&aExplorerCtrl, SearchResultCtrl *&aSearchResultCtrl, FileScrapPane *&aFileScrapPane);

protected:
    xpr_uint_t         mCommandId;
    MainFrame         *mMainFrame;
    CWnd              *mTargetWnd;
    CommandParameters *mParameters;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_CONTEXT_H__
