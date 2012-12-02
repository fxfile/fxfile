//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "Option.h"

#include "fxb/fxb_ini_file.h"
#include "fxb/fxb_bookmark.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_thumbnail.h"

#include "AppVer.h"
#include "PathBar.h"
#include "CfgPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Option::Option(void)
{
}

Option::~Option(void)
{
}

void Option::initDefault(void)
{
    loadFromFile(XPR_STRING_LITERAL("default"));
}

static const xpr_tchar_t CFG_MAIN[]      = XPR_STRING_LITERAL("Main");
static const xpr_tchar_t CFG_CONFIG[]    = XPR_STRING_LITERAL("Config");
static const xpr_tchar_t CFG_GENERAL[]   = XPR_STRING_LITERAL("General");
static const xpr_tchar_t CFG_FOLDER[]    = XPR_STRING_LITERAL("Folder");
static const xpr_tchar_t CFG_CONTENTS[]  = XPR_STRING_LITERAL("Contents");
static const xpr_tchar_t CFG_EXPLORER[]  = XPR_STRING_LITERAL("Explorer");
static const xpr_tchar_t CFG_THUMBNAIL[] = XPR_STRING_LITERAL("Thumbnail");
static const xpr_tchar_t CFG_ADVANCED[]  = XPR_STRING_LITERAL("Advanced");

xpr_bool_t Option::loadFromFile(const xpr_tchar_t *aFileName)
{
    xpr_bool_t sResult = XPR_TRUE;

    xpr_sint_t i, j;
    xpr_tchar_t sKey[0xff] = {0};
    xpr_tchar_t sEntry[0xff] = {0};
    const xpr_tchar_t *sValue;

    // main.ini
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        CfgPath::instance().getLoadPath(CfgPath::TypeMain, sPath, XPR_MAX_PATH);

        fxb::IniFile sIniFile(sPath);
        if (sIniFile.readFile() == XPR_FALSE)
            sResult = XPR_FALSE;

        {
            // [Main]
            mWindow                   = sIniFile.getValueR(CFG_MAIN, XPR_STRING_LITERAL("Window"),                     CRect(-1,-1,-1,-1));
            mCmdShow                  = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Window Status"),              -1);
            _tcscpy(mLanguage,          sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Language"),                   XPR_STRING_LITERAL("Korean")));
            mSingleFolderPaneMode     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Mode"),    XPR_FALSE);
            mShowEachFolderPane[0]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #1"),        XPR_TRUE);
            mShowEachFolderPane[1]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #2"),        XPR_TRUE);
            mShowEachFolderPane[2]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #3"),        XPR_TRUE);
            mShowEachFolderPane[3]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #4"),        XPR_TRUE);
            mShowEachFolderPane[4]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #5"),        XPR_TRUE);
            mShowEachFolderPane[5]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #6"),        XPR_TRUE);
            mShowSingleFolderPane     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Single Folder Pane"),    XPR_TRUE);
            mSingleFolderPaneSize     = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Size"),    DEF_FOLDER_PANE_SIZE);
            mEachFolderPaneSize[0]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #1"),        DEF_FOLDER_PANE_SIZE);
            mEachFolderPaneSize[1]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #2"),        DEF_FOLDER_PANE_SIZE);
            mEachFolderPaneSize[2]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #3"),        DEF_FOLDER_PANE_SIZE);
            mEachFolderPaneSize[3]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #4"),        DEF_FOLDER_PANE_SIZE);
            mEachFolderPaneSize[4]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #5"),        DEF_FOLDER_PANE_SIZE);
            mEachFolderPaneSize[5]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #6"),        DEF_FOLDER_PANE_SIZE);
            mLeftSingleFolderPane     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Single Folder Pane"),    XPR_TRUE);
            mLeftEachFolderPane[0]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #1"),        XPR_TRUE);
            mLeftEachFolderPane[1]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #2"),        XPR_TRUE);
            mLeftEachFolderPane[2]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #3"),        XPR_TRUE);
            mLeftEachFolderPane[3]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #4"),        XPR_TRUE);
            mLeftEachFolderPane[4]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #5"),        XPR_TRUE);
            mLeftEachFolderPane[5]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #6"),        XPR_TRUE);
            mActivateBar[0]           = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #1"),             XPR_TRUE);
            mActivateBar[1]           = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #2"),             XPR_TRUE);
            mActivateBar[2]           = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #3"),             XPR_TRUE);
            mActivateBar[3]           = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #4"),             XPR_TRUE);
            mActivateBar[4]           = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #5"),             XPR_TRUE);
            mActivateBar[5]           = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #6"),             XPR_TRUE);
            mAddressBar[0]            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #1"),              XPR_TRUE);
            mAddressBar[1]            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #2"),              XPR_TRUE);
            mAddressBar[2]            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #3"),              XPR_TRUE);
            mAddressBar[3]            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #4"),              XPR_TRUE);
            mAddressBar[4]            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #5"),              XPR_TRUE);
            mAddressBar[5]            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #6"),              XPR_TRUE);
            mPathBar[0]               = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #1"),                 XPR_FALSE);
            mPathBar[1]               = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #2"),                 XPR_FALSE);
            mPathBar[2]               = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #3"),                 XPR_FALSE);
            mPathBar[3]               = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #4"),                 XPR_FALSE);
            mPathBar[4]               = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #5"),                 XPR_FALSE);
            mPathBar[5]               = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #6"),                 XPR_FALSE);
            mBookmarkBarText          = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Text"),           XPR_TRUE);
            mBookmarkBarMultiLine     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Multi Line"),     XPR_FALSE);
            mDriveBar                 = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar"),                   XPR_TRUE);
            mDriveBarShortText        = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Short Text"));
            mDriveBarViewSplit        = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar ViewSplit"));
            mLeftDriveBarViewSplit    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar ViewSplit Left"));
            mDriveBarMultiLine        = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Multi Line"),        XPR_FALSE);
            mViewSplitRowCount        = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Row Count"),        DEF_VIEW_SPLIT_ROW);
            mViewSplitColumnCount     = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Column Count"),     DEF_VIEW_SPLIT_COLUMN);
            mViewSplitRatio[0]        = sIniFile.getValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #1"),         0.0);
            mViewSplitRatio[1]        = sIniFile.getValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #2"),         0.0);
            mViewSplitRatio[2]        = sIniFile.getValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #3"),         0.0);
            mViewSplitSize[0]         = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #1"),          0);
            mViewSplitSize[1]         = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #2"),          0);
            mViewSplitSize[2]         = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #3"),          0);
            mViewStyle[0]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #1"),              LVS_REPORT);
            mViewStyle[1]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #2"),              LVS_REPORT);
            mViewStyle[2]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #3"),              LVS_REPORT);
            mViewStyle[3]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #4"),              LVS_REPORT);
            mViewStyle[4]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #5"),              LVS_REPORT);
            mViewStyle[5]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #6"),              LVS_REPORT);
            _tcscpy(mLastFolder[0],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #1"),             XPR_STRING_LITERAL("")));
            _tcscpy(mLastFolder[1],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #2"),             XPR_STRING_LITERAL("")));
            _tcscpy(mLastFolder[2],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #3"),             XPR_STRING_LITERAL("")));
            _tcscpy(mLastFolder[3],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #4"),             XPR_STRING_LITERAL("")));
            _tcscpy(mLastFolder[4],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #5"),             XPR_STRING_LITERAL("")));
            _tcscpy(mLastFolder[5],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #6"),             XPR_STRING_LITERAL("")));
            _tcscpy(mWorkingFolder[0],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #1"),          XPR_STRING_LITERAL("")));
            _tcscpy(mWorkingFolder[1],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #2"),          XPR_STRING_LITERAL("")));
            _tcscpy(mWorkingFolder[2],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #3"),          XPR_STRING_LITERAL("")));
            _tcscpy(mWorkingFolder[3],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #4"),          XPR_STRING_LITERAL("")));
            _tcscpy(mWorkingFolder[4],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #5"),          XPR_STRING_LITERAL("")));
            mFileScrapDrop            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("FileScrapDrop"));
            mPicViewer                = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PicViewer"));
            mTipOfTheToday            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Tip Of The Today"),           XPR_TRUE);

            for (i = 0; i < MAX_VIEW_SPLIT; ++i)
            {
                if (_tcslen(mLastFolder[i]) == 2 && mLastFolder[i][1] == XPR_STRING_LITERAL(':'))
                    _tcscat(mLastFolder[i], XPR_STRING_LITERAL("\\"));
            }

            for (i = 0; i < MAX_WORKING_FOLDER; ++i)
            {
                if (_tcslen(mWorkingFolder[i]) == 2 && mWorkingFolder[i][1] == XPR_STRING_LITERAL(':'))
                    _tcscat(mWorkingFolder[i], XPR_STRING_LITERAL("\\"));
            }

            for (i = 0; i < MAX_VIEW_SPLIT; ++i)
            {
                _stprintf(sKey, XPR_STRING_LITERAL("TabViewSplit #%d"), i + 1);

                mViewSplitTab[i].mCurTab = sIniFile.getValueI(sKey, XPR_STRING_LITERAL("Current Tab"));

                for (j = 0; ; ++j)
                {
                    _stprintf(sEntry, XPR_STRING_LITERAL("Tab #%d"), j + 1);

                    sValue = sIniFile.getValueS(sKey, sEntry);
                    if (sValue == XPR_NULL)
                    {
                        break;
                    }

                    mViewSplitTab[i].mTabPathDeque.push_back(sValue);
                }
            }
        }
    }

    // config.ini
    {
        fxb::IniFile sIniFile(aFileName);
        if (sIniFile.readFile() == XPR_FALSE)
            sResult = XPR_FALSE;

        xpr_uint_t sMajorVer = 0;
        xpr_uint_t sMinorVer = 0;
        xpr_uint_t sBuildNumber = 0;

        const xpr_tchar_t *sAppVer = sIniFile.getValueS(CFG_CONFIG, XPR_STRING_LITERAL("version"));
        if (XPR_IS_NOT_NULL(sAppVer))
            _stscanf(sAppVer, XPR_STRING_LITERAL("%u.%u.%u"), &sMajorVer, &sMinorVer, &sBuildNumber);

        // [General]
        mMouseClick                   = sIniFile.getValueI(CFG_GENERAL, XPR_STRING_LITERAL("Mouse"),                   MOUSE_DOUBLE_CLICK);
        mShowSystemAttribute          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("System Attribute"));
        mTitleFullPath                = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Title Full Path"));
        mAddressFullPath              = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Address Full Path"));
        mTooltip                      = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip"),                 XPR_TRUE);
        mTooltipWithFileName          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip File Name"));
        mGridLines                    = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("GridLines"));
        mFullRowSelect                = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("FullRowSelect"));
        mFileExtType                  = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Extension"));
        mBatchRenameMultiSel          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Multi-Sel, Batch Rename"), XPR_TRUE);
        mBatchRenameWithFolder        = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Batch Rename With Folder"));
        mRenameByMouse                = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Mouse Rename"),            XPR_TRUE);
        mNewItemDlg                   = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("New Item Dlg"),            XPR_TRUE);
        mSingleRenameType             = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Single Rename Type"),      SINGLE_RENAME_TYPE_DEFAULT);
        mRenameExtType                = sIniFile.getValueI(CFG_GENERAL, XPR_STRING_LITERAL("Rename Extension Type"),   RENAME_EXT_TYPE_DEFAULT);
        mShowHiddenAttribute          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Hidden Attribute"));

        // [Folder]
        mFolderTreeCustomFolder[0]    = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #1"),         FOLDER_TREE_CUSTOM_NONE);
        mFolderTreeCustomFolder[1]    = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #2"),         FOLDER_TREE_CUSTOM_NONE);
        mFolderTreeCustomFolder[2]    = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #3"),         FOLDER_TREE_CUSTOM_NONE);
        mFolderTreeCustomFolder[3]    = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #4"),         FOLDER_TREE_CUSTOM_NONE);
        mFolderTreeCustomFolder[4]    = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #5"),         FOLDER_TREE_CUSTOM_NONE);
        mFolderTreeCustomFolder[5]    = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #6"),         FOLDER_TREE_CUSTOM_NONE);
        mFolderTreeBkgndColor[0]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #1"),            GetSysColor(COLOR_WINDOW));
        mFolderTreeBkgndColor[1]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #2"),            GetSysColor(COLOR_WINDOW));
        mFolderTreeBkgndColor[2]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #3"),            GetSysColor(COLOR_WINDOW));
        mFolderTreeBkgndColor[3]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #4"),            GetSysColor(COLOR_WINDOW));
        mFolderTreeBkgndColor[4]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #5"),            GetSysColor(COLOR_WINDOW));
        mFolderTreeBkgndColor[5]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #6"),            GetSysColor(COLOR_WINDOW));
        mFolderTreeTextColor[0]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #1"),            GetSysColor(COLOR_WINDOWTEXT));
        mFolderTreeTextColor[1]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #2"),            GetSysColor(COLOR_WINDOWTEXT));
        mFolderTreeTextColor[2]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #3"),            GetSysColor(COLOR_WINDOWTEXT));
        mFolderTreeTextColor[3]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #4"),            GetSysColor(COLOR_WINDOWTEXT));
        mFolderTreeTextColor[4]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #5"),            GetSysColor(COLOR_WINDOWTEXT));
        mFolderTreeTextColor[5]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #6"),            GetSysColor(COLOR_WINDOWTEXT));
        mFolderTreeIsItemHeight       = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Item Height"));
        mFolderTreeItemHeight         = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Item Height Size"));
        mFolderTreeHighlight[0]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #1"));
        mFolderTreeHighlight[1]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #2"));
        mFolderTreeHighlight[2]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #3"));
        mFolderTreeHighlight[3]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #4"));
        mFolderTreeHighlight[4]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #5"));
        mFolderTreeHighlight[5]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #6"));
        mFolderTreeHighlightColor[0]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #1"), GetSysColor(COLOR_HIGHLIGHT));
        mFolderTreeHighlightColor[1]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #2"), GetSysColor(COLOR_HIGHLIGHT));
        mFolderTreeHighlightColor[2]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #3"), GetSysColor(COLOR_HIGHLIGHT));
        mFolderTreeHighlightColor[3]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #4"), GetSysColor(COLOR_HIGHLIGHT));
        mFolderTreeHighlightColor[4]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #5"), GetSysColor(COLOR_HIGHLIGHT));
        mFolderTreeHighlightColor[5]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #6"), GetSysColor(COLOR_HIGHLIGHT));
        mSingleFolderTreeLinkage      = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Single Folder Pane Linkage"));
        mFolderTreeCustomFont         = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font"));
        _tcscpy(mFolderTreeCustomFontText, sIniFile.getValueS(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font Name"),       XPR_STRING_LITERAL("")));
        mFolderTreeInitNoExpand       = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Init, No Expand"));
        mFolderTreeSelDelay           = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay"));
        mFolderTreeSelDelayTime       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay Time"),            DEF_FLD_SEL_DELAY_MSEC);

        if (mFolderTreeItemHeight < MIN_FOLDER_TREE_HEIGHT) mFolderTreeItemHeight = MIN_FOLDER_TREE_HEIGHT;
        if (mFolderTreeItemHeight > MAX_FOLDER_TREE_HEIGHT) mFolderTreeItemHeight = MAX_FOLDER_TREE_HEIGHT;

        if (mFolderTreeSelDelayTime < MIN_FLD_SEL_DELAY_MSEC) mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;
        if (mFolderTreeSelDelayTime > MAX_FLD_SEL_DELAY_MSEC) mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;

        //
        // [Contents]
        //
        mContentsStyle                = sIniFile.getValueI(CFG_CONTENTS, CFG_CONTENTS,                 CONTENTS_EXPLORER);
        mContentsBookmark             = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link"),                   XPR_TRUE);
        mContentsTooltip              = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Popup"),                  XPR_TRUE);
        mContentsBookmarkColor        = sIniFile.getValueC(CFG_CONTENTS, XPR_STRING_LITERAL("Link Color"),             RGB(0,0,255));
        mContentsBookmarkExpandFolder = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Expand Folder"));
        mContentsBookmarkRealPath     = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Real Path"));
        mContentsBookmarkFastNetIcon  = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Fast Network Icon"), XPR_TRUE);
        mContentsNoDispInfo           = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("No Disp Info"));
        mContentsARHSAttribute        = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Attributes ARHS Style"));

        //
        // [Explorer]
        //
        mExplorerCustomFolder[0]      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #1"));
        mExplorerCustomFolder[1]      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #2"));
        mExplorerCustomFolder[2]      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #3"));
        mExplorerCustomFolder[3]      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #4"));
        mExplorerCustomFolder[4]      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #5"));
        mExplorerCustomFolder[5]      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #6"));
        mExplorerBkgndColor[0]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #1"),          GetSysColor(COLOR_WINDOW));
        mExplorerBkgndColor[1]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #2"),          GetSysColor(COLOR_WINDOW));
        mExplorerBkgndColor[2]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #3"),          GetSysColor(COLOR_WINDOW));
        mExplorerBkgndColor[3]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #4"),          GetSysColor(COLOR_WINDOW));
        mExplorerBkgndColor[4]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #5"),          GetSysColor(COLOR_WINDOW));
        mExplorerBkgndColor[5]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #6"),          GetSysColor(COLOR_WINDOW));
        mExplorerTextColor[0]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #1"),          GetSysColor(COLOR_WINDOWTEXT));
        mExplorerTextColor[1]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #2"),          GetSysColor(COLOR_WINDOWTEXT));
        mExplorerTextColor[2]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #3"),          GetSysColor(COLOR_WINDOWTEXT));
        mExplorerTextColor[3]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #4"),          GetSysColor(COLOR_WINDOWTEXT));
        mExplorerTextColor[4]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #5"),          GetSysColor(COLOR_WINDOWTEXT));
        mExplorerTextColor[5]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #6"),          GetSysColor(COLOR_WINDOWTEXT));
        mExplorerInitFolderType[0]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #1"),         INIT_TYPE_NONE);
        mExplorerInitFolderType[1]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #2"),         INIT_TYPE_NONE);
        mExplorerInitFolderType[2]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #3"),         INIT_TYPE_NONE);
        mExplorerInitFolderType[3]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #4"),         INIT_TYPE_NONE);
        mExplorerInitFolderType[4]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #5"),         INIT_TYPE_NONE);
        mExplorerInitFolderType[5]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #6"),         INIT_TYPE_NONE);
        mExplorerSizeUnit             = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Size Unit"),              SIZE_UNIT_DEFAULT);
        mExplorerListType             = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("List Type"),              LIST_TYPE_ALL);
        mExplorerParentFolder         = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Parent Folder"));
        mPathBarRealPath              = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar RealPath"));
        mActiveViewColor              = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Activate Color"),         RGB(48,192,48));//GetSysColor(COLOR_ACTIVECAPTION));
        mPathBarIcon                  = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Icon"));
        mPathBarHighlight             = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight"),      XPR_TRUE);
        mPathBarHighlightColor        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight Color"),PathBar::getDefaultHighlightColor());//GetSysColor(COLOR_ACTIVECAPTION));
        mExplorerGoUpSelSubFolder     = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("GoUpSelSubFolder"));
        mExplorerCustomIcon           = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon"));
        mExplorerFilterColorType      = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Extension Color"),        FILTER_COLOR_TYPE_NONE);
        mExplorerNoSort               = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Sort"));
        mExplorer24HourTime           = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("24 Hour Time"));
        mExplorer2YearDate            = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("2 Year Date"));
        mExplorerShowDrive            = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive"));
        mExplorerShowDriveItem        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Item"));
        mExplorerShowDriveSize        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Size"));
        mExplorerNameCaseType         = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Name Case Type"),         NAME_CASE_TYPE_DEFAULT);
        mExplorerSaveViewStyle        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save View Style"),        XPR_TRUE);
        mExplorerCreateAndEditText    = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Make Text And Edit"));
        mCustomFont                   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font"));
        mExplorerAutoColumnWidth      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Auto Column Width"));
        mExplorerDefaultViewStyle     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default View Style"),     VIEW_STYLE_REPORT);
        mExplorerDefaultSort          = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort"));
        mExplorerDefaultSortOrder     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort Order"),     1);
        mExplorerSaveViewSet          = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save ViewSet"),           SAVE_VIEW_SET_DEFAULT);
        mExplorerExitVerifyViewSet    = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Exit Verify ViewSet"),    XPR_TRUE);
        _tcscpy(mExplorerCustomIconFile[0], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 16"), XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerCustomIconFile[1], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 32"), XPR_STRING_LITERAL("")));
        mExplorerNoNetLastFolder[0]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #1"));
        mExplorerNoNetLastFolder[1]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #2"));
        mExplorerNoNetLastFolder[2]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #3"));
        mExplorerNoNetLastFolder[3]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #4"));
        mExplorerNoNetLastFolder[4]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #5"));
        mExplorerNoNetLastFolder[5]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #6"));
        _tcscpy(mExplorerBkgndFile[0],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #1"),          XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerBkgndFile[1],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #2"),          XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerBkgndFile[2],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #3"),          XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerBkgndFile[3],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #4"),          XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerBkgndFile[4],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #5"),          XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerBkgndFile[5],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #6"),          XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerInitFolder[0], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #1"),    XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerInitFolder[1], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #2"),    XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerInitFolder[2], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #3"),    XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerInitFolder[3], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #4"),    XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerInitFolder[4], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #5"),    XPR_STRING_LITERAL("")));
        _tcscpy(mExplorerInitFolder[5], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #6"),    XPR_STRING_LITERAL("")));
        _tcscpy(mCustomFontText,        sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font Name"),       XPR_STRING_LITERAL("")));

        if (mExplorerCustomIconFile[0][0] == 0)
            _tcscpy(mExplorerCustomIconFile[0], XPR_STRING_LITERAL("%flyExplorer%\\custom_image_list_16.bmp"));

        if (mExplorerCustomIconFile[1][0] == 0)
            _tcscpy(mExplorerCustomIconFile[1], XPR_STRING_LITERAL("%flyExplorer%\\custom_image_list_32.bmp"));

        fxb::SizeFormat::validSizeUnit(mExplorerSizeUnit);

        for (i = 0; i < MAX_VIEW_SPLIT; ++i)
        {
            if (_tcslen(mExplorerInitFolder[i]) == 2 && mExplorerInitFolder[i][1] == XPR_STRING_LITERAL(':'))
                _tcscat(mExplorerInitFolder[i], XPR_STRING_LITERAL("\\"));
        }

        //
        // [Thumbnail]
        //
        mThumbnailWidth               = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Width"),                 DEF_THUMB_SIZE);
        mThumbnailHeight              = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Height"),                DEF_THUMB_SIZE);
        mThumbnailSaveCache           = sIniFile.getValueB(CFG_THUMBNAIL, XPR_STRING_LITERAL("Save Cache"));
        mThumbnailPriority            = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Priority"),              fxb::THUMBNAIL_PRIORITY_BELOW_NORMAL);
        mThumbnailLoadByExt           = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Load By Ext"),           XPR_TRUE);

        //
        // Advanced
        //
        mTray                         = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray"));
        mTrayOnClose                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Close"));
        mFileScrapContextMenu         = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File Scrap"),             XPR_TRUE);
        mFileScrapSave                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save File Scrap"));
        mTabSave                      = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tab Save"),               XPR_TRUE);
        mViewSplitByRatio             = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Split View by Ratio"),    XPR_TRUE);
        mSingleViewActivatePath       = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single Activate Path"));
        mDriveLastFolder              = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Save Path"));
        mDriveShiftKey                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Shift Key"));
        mShellNewMenu                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Shell New"));
        mFileListCurDir               = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File List Cur Dir"));
        mExternalCopyFileOp           = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Copy File Operation"));
        mExternalMoveFileOp           = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Move File Operation"));
        mExternalDeleteFileOp         = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Delete File Operation"));
        mWorkingFolderRealPath        = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Working Folder Real Path"));
        mSingleSelFileSizeUnit        = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Single File Sel Size Unit"), SIZE_UNIT_DEFAULT);
        mMultiSelFileSizeUnit         = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Multi File Sel Size Unit"),  SIZE_UNIT_DEFAULT);
        mNoRefresh                    = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("No Refresh"));
        mSingleInstance               = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single Instance"));
        mConfirmExit                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Confirm Exit"));
        mAnimationMenu                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Animation Menu"));
        mSplitViewTabKey              = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("SplitWnd TabKey"));
        mSplitLastFolder              = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Split Last Folder"));
        mRecentFile                   = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Recent File"),            XPR_TRUE);
        mStandardMenu                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Standard Menu"));
        mRefreshSort                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Refresh Sort"));
        mAddressBarUrl                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("AddressBar URL"));
        mSearchResultClearOnClose     = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Clear Search Result"));
        mFileOpCompleteFlash          = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Flash File Operation"));
        mSaveHistory                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save History"));
        mHistoryCount                 = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Count"),          DEF_HISTORY);
        mBackwardCount                = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Count"),         DEF_BACKWARD);
        mHistoryMenuCount             = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Menu Count"),     DEF_HISTORY_MENU);
        mBackwardMenuCount            = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Menu Count"),    DEF_BACKWARD_MENU);
        mDragType                     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Type"),              DRAG_START_DEFAULT);
        mDragDist                     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Distance"),          DEF_DIST_DRAG);
        mDropType                     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drop Type"),              DROP_DEFAULT);
        mDragFolderTreeExpandTime     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Expand Folder"),     1);
        mDragScrollTime               = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Scroll Time"),       1);
        mDragDefaultFileOp            = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Default Operation"), DRAG_FILE_OP_DEFAULT);
        mDragNoContents               = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drag No Contents"));
        mRegShellContextMenu          = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Reg Shell Context Menu"));
        mTrayRestoreInitFolder        = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Init Folder"));
        mTrayOnMinmize                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Minimize"));
        mTrayOneClick                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray One Click"));
        mLauncher                     = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher"));
        mLauncherTray                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Tray"));
        mLauncherWinStartup           = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Windows Startup"));
        mLauncherGlobalHotKey         = sIniFile.getValueU(CFG_ADVANCED, XPR_STRING_LITERAL("Global Hot Key"));

        if (mDragDist < MIN_DIST_DRAG) mDragDist = DEF_DIST_DRAG;
        if (mDragDist > MAX_DIST_DRAG) mDragDist = DEF_DIST_DRAG;

        if (mDragFolderTreeExpandTime < MIN_EXPAND_FOLDER_MSEC) mDragFolderTreeExpandTime = DEF_EXPAND_FOLDER_MSEC;
        if (mDragFolderTreeExpandTime > MAX_EXPAND_FOLDER_MSEC) mDragFolderTreeExpandTime = DEF_EXPAND_FOLDER_MSEC;

        if (mDragScrollTime < MIN_DRAG_SCROLL_MSEC) mDragScrollTime = DEF_DRAG_SCROLL_MSEC;
        if (mDragScrollTime > MAX_DRAG_SCROLL_MSEC) mDragScrollTime = DEF_DRAG_SCROLL_MSEC;

        _tcscpy(mClipboardSeparator, sIniFile.getValueS(CFG_ADVANCED, XPR_STRING_LITERAL("Div Clipboard Text"), XPR_STRING_LITERAL("\\r\\n")));

        if (mHistoryCount < MIN_HISTORY) mHistoryCount = DEF_HISTORY;
        if (mHistoryCount > MAX_HISTORY) mHistoryCount = DEF_HISTORY;

        if (mBackwardCount < MIN_BACKWARD) mBackwardCount = DEF_BACKWARD;
        if (mBackwardCount > MAX_BACKWARD) mBackwardCount = DEF_BACKWARD;

        if (mHistoryMenuCount < MIN_HISTORY_MENU) mHistoryMenuCount = DEF_HISTORY_MENU;
        if (mHistoryMenuCount > MAX_HISTORY_MENU) mHistoryMenuCount = DEF_HISTORY_MENU;

        if (mBackwardMenuCount < MIN_BACKWARD_MENU) mBackwardMenuCount = DEF_BACKWARD_MENU;
        if (mBackwardMenuCount > MAX_BACKWARD_MENU) mBackwardMenuCount = DEF_BACKWARD_MENU;

        fxb::SizeFormat::validSizeUnit(mSingleSelFileSizeUnit);
        fxb::SizeFormat::validSizeUnit(mMultiSelFileSizeUnit);

        if (sIniFile.getKeyCount() <= 0)
            sResult = XPR_FALSE;
    }

    return sResult;
}

xpr_bool_t Option::saveToFile(const xpr_tchar_t *aFileName, xpr_bool_t aMainOption)
{
    xpr_tchar_t sAppVer[0xff] = {0};
    getAppVer(sAppVer);

    xpr_sint_t i, j;
    xpr_tchar_t sKey[0xff] = {0};
    xpr_tchar_t sEntry[0xff] = {0};

    if (XPR_IS_TRUE(aMainOption))
    {
        fxb::IniFile sIniFile(aFileName);
        sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer Main File"));

        //
        // [Main]
        //
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("version"),                         sAppVer);
        sIniFile.setValueR(CFG_MAIN, XPR_STRING_LITERAL("Window"),                          mWindow);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Window Status"),                   mCmdShow);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Language"),                        mLanguage);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Mode"),         mSingleFolderPaneMode);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #1"),             mShowEachFolderPane[0]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #2"),             mShowEachFolderPane[1]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #3"),             mShowEachFolderPane[2]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #4"),             mShowEachFolderPane[3]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #5"),             mShowEachFolderPane[4]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #6"),             mShowEachFolderPane[5]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Single Folder Pane"),         mShowSingleFolderPane);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Size"),         mSingleFolderPaneSize);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #1"),             mEachFolderPaneSize[0]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #2"),             mEachFolderPaneSize[1]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #3"),             mEachFolderPaneSize[2]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #4"),             mEachFolderPaneSize[3]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #5"),             mEachFolderPaneSize[4]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #6"),             mEachFolderPaneSize[5]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Single Folder Pane"),         mLeftSingleFolderPane);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #1"),             mLeftEachFolderPane[0]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #2"),             mLeftEachFolderPane[1]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #3"),             mLeftEachFolderPane[2]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #4"),             mLeftEachFolderPane[3]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #5"),             mLeftEachFolderPane[4]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #6"),             mLeftEachFolderPane[5]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #1"),                  mActivateBar[0]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #2"),                  mActivateBar[1]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #3"),                  mActivateBar[2]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #4"),                  mActivateBar[3]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #5"),                  mActivateBar[4]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("ActivateBar #6"),                  mActivateBar[5]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #1"),                   mAddressBar[0]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #2"),                   mAddressBar[1]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #3"),                   mAddressBar[2]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #4"),                   mAddressBar[3]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #5"),                   mAddressBar[4]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("AddressBar #6"),                   mAddressBar[5]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #1"),                      mPathBar[0]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #2"),                      mPathBar[1]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #3"),                      mPathBar[2]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #4"),                      mPathBar[3]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #5"),                      mPathBar[4]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PathBar #6"),                      mPathBar[5]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Text"),                mBookmarkBarText);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Multi Line"),          mBookmarkBarMultiLine);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar"),                        mDriveBar);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Short Text"),             mDriveBarShortText);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar ViewSplit"),              mDriveBarViewSplit);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar ViewSplit Left"),         mLeftDriveBarViewSplit);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Multi Line"),             mDriveBarMultiLine);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Row Count"),             mViewSplitRowCount);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Column Count"),          mViewSplitColumnCount);
        sIniFile.setValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #1"),              mViewSplitRatio[0]);
        sIniFile.setValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #2"),              mViewSplitRatio[1]);
        sIniFile.setValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #3"),              mViewSplitRatio[2]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #1"),               mViewSplitSize[0]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #2"),               mViewSplitSize[1]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #3"),               mViewSplitSize[2]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #1"),                   mViewStyle[0]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #2"),                   mViewStyle[1]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #3"),                   mViewStyle[2]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #4"),                   mViewStyle[3]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #5"),                   mViewStyle[4]);
        sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #6"),                   mViewStyle[5]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #1"),                  mLastFolder[0]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #2"),                  mLastFolder[1]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #3"),                  mLastFolder[2]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #4"),                  mLastFolder[3]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #5"),                  mLastFolder[4]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #6"),                  mLastFolder[5]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #1"),               mWorkingFolder[0]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #2"),               mWorkingFolder[1]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #3"),               mWorkingFolder[2]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #4"),               mWorkingFolder[3]);
        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #5"),               mWorkingFolder[4]);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("FileScrapDrop"),                   mFileScrapDrop);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PicViewer"),                       mPicViewer);
        sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Tip Of The Today"),                mTipOfTheToday);

        for (i = 0; i < MAX_VIEW_SPLIT; ++i)
        {
            _stprintf(sKey, XPR_STRING_LITERAL("TabViewSplit #%d"), i + 1);

            sIniFile.setValueI(sKey, XPR_STRING_LITERAL("Current Tab"), (xpr_sint_t)mViewSplitTab[i].mCurTab);

            TabPathDeque::iterator sIterator = mViewSplitTab[i].mTabPathDeque.begin();
            for (j = 0; sIterator != mViewSplitTab[i].mTabPathDeque.end(); ++sIterator, ++j)
            {
                const std::tstring &sFullPath = *sIterator;

                _stprintf(sEntry, XPR_STRING_LITERAL("Tab #%d"), j + 1);

                sIniFile.setValueS(sKey, sEntry, sFullPath.c_str());
            }
        }

        sIniFile.writeFile(xpr::CharSetUtf16);
        return XPR_TRUE;
    }

    fxb::IniFile sIniFile(aFileName);

    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer Configuration File"));

    sIniFile.setSortKey(CFG_CONFIG,    0);
    sIniFile.setSortKey(CFG_GENERAL,   1);
    sIniFile.setSortKey(CFG_FOLDER,    2);
    sIniFile.setSortKey(CFG_CONTENTS,  3);
    sIniFile.setSortKey(CFG_EXPLORER,  4);
    sIniFile.setSortKey(CFG_THUMBNAIL, 5);
    sIniFile.setSortKey(CFG_ADVANCED,  6);

    // [Config]
    sIniFile.setValueS(CFG_CONFIG, XPR_STRING_LITERAL("version"), sAppVer);

    //
    // [General]
    //
    sIniFile.setValueI(CFG_GENERAL, XPR_STRING_LITERAL("Mouse"),                            mMouseClick);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Hidden Attribute"),                 mShowHiddenAttribute);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("System Attribute"),                 mShowSystemAttribute);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Title Full Path"),                  mTitleFullPath);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Address Full Path"),                mAddressFullPath);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip"),                          mTooltip);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip File Name"),                mTooltipWithFileName);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("GridLines"),                        mGridLines);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("FullRowSelect"),                    mFullRowSelect);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Extension"),                        mFileExtType);
    sIniFile.setValueI(CFG_GENERAL, XPR_STRING_LITERAL("Rename Extension Type"),            mRenameExtType);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Multi-Sel, Batch Rename"),          mBatchRenameMultiSel);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Mouse Rename"),                     mRenameByMouse);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Batch Rename With Folder"),         mBatchRenameWithFolder);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("New Item Dlg"),                     mNewItemDlg);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Single Rename Type"),               mSingleRenameType);

    //
    // [Folder]
    //
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #1"),                  mFolderTreeCustomFolder[0]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #2"),                  mFolderTreeCustomFolder[1]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #3"),                  mFolderTreeCustomFolder[2]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #4"),                  mFolderTreeCustomFolder[3]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #5"),                  mFolderTreeCustomFolder[4]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Folder #6"),                  mFolderTreeCustomFolder[5]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #1"),                     mFolderTreeBkgndColor[0]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #2"),                     mFolderTreeBkgndColor[1]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #3"),                     mFolderTreeBkgndColor[2]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #4"),                     mFolderTreeBkgndColor[3]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #5"),                     mFolderTreeBkgndColor[4]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Back Color #6"),                     mFolderTreeBkgndColor[5]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #1"),                     mFolderTreeTextColor[0]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #2"),                     mFolderTreeTextColor[1]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #3"),                     mFolderTreeTextColor[2]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #4"),                     mFolderTreeTextColor[3]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #5"),                     mFolderTreeTextColor[4]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #6"),                     mFolderTreeTextColor[5]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Item Height"),                       mFolderTreeIsItemHeight);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Item Height Size"),                  mFolderTreeItemHeight);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #1"),               mFolderTreeHighlight[0]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #2"),               mFolderTreeHighlight[1]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #3"),               mFolderTreeHighlight[2]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #4"),               mFolderTreeHighlight[3]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #5"),               mFolderTreeHighlight[4]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #6"),               mFolderTreeHighlight[5]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #1"),         mFolderTreeHighlightColor[0]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #2"),         mFolderTreeHighlightColor[1]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #3"),         mFolderTreeHighlightColor[2]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #4"),         mFolderTreeHighlightColor[3]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #5"),         mFolderTreeHighlightColor[4]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #6"),         mFolderTreeHighlightColor[5]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font"),                       mFolderTreeCustomFont);
    sIniFile.setValueS(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font Name"),                  mFolderTreeCustomFontText);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Init, No Expand"),                   mFolderTreeInitNoExpand);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay"),                         mFolderTreeSelDelay);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay Time"),                    mFolderTreeSelDelayTime);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Single Folder Pane Linkage"),        mSingleFolderTreeLinkage);

    //
    // [Contents]
    //
    sIniFile.setValueI(CFG_CONTENTS, CFG_CONTENTS,                          mContentsStyle);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link"),                            mContentsBookmark);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Popup"),                           mContentsTooltip);
    sIniFile.setValueC(CFG_CONTENTS, XPR_STRING_LITERAL("Link Color"),                      mContentsBookmarkColor);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Expand Folder"),              mContentsBookmarkExpandFolder);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Real Path"),                  mContentsBookmarkRealPath);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Fast Network Icon"),          mContentsBookmarkFastNetIcon);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("No Disp Info"),                    mContentsNoDispInfo);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Attributes ARHS Style"),           mContentsARHSAttribute);


    //
    // [Explorer]
    //
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #1"),                mExplorerCustomFolder[0]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #2"),                mExplorerCustomFolder[1]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #3"),                mExplorerCustomFolder[2]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #4"),                mExplorerCustomFolder[3]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #5"),                mExplorerCustomFolder[4]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Folder #6"),                mExplorerCustomFolder[5]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #1"),                   mExplorerBkgndFile[0]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #2"),                   mExplorerBkgndFile[1]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #3"),                   mExplorerBkgndFile[2]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #4"),                   mExplorerBkgndFile[3]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #5"),                   mExplorerBkgndFile[4]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background #6"),                   mExplorerBkgndFile[5]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #1"),                   mExplorerBkgndColor[0]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #2"),                   mExplorerBkgndColor[1]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #3"),                   mExplorerBkgndColor[2]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #4"),                   mExplorerBkgndColor[3]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #5"),                   mExplorerBkgndColor[4]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Back Color #6"),                   mExplorerBkgndColor[5]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #1"),                   mExplorerTextColor[0]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #2"),                   mExplorerTextColor[1]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #3"),                   mExplorerTextColor[2]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #4"),                   mExplorerTextColor[3]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #5"),                   mExplorerTextColor[4]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #6"),                   mExplorerTextColor[5]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #1"),                  mExplorerInitFolderType[0]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #2"),                  mExplorerInitFolderType[1]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #3"),                  mExplorerInitFolderType[2]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #4"),                  mExplorerInitFolderType[3]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #5"),                  mExplorerInitFolderType[4]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #6"),                  mExplorerInitFolderType[5]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #1"),             mExplorerInitFolder[0]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #2"),             mExplorerInitFolder[1]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #3"),             mExplorerInitFolder[2]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #4"),             mExplorerInitFolder[3]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #5"),             mExplorerInitFolder[4]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #6"),             mExplorerInitFolder[5]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Size Unit"),                       mExplorerSizeUnit);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("List Type"),                       mExplorerListType);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Parent Folder"),                   mExplorerParentFolder);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar RealPath"),                mPathBarRealPath);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Activate Color"),                  mActiveViewColor);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Icon"),                    mPathBarIcon);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight"),               mPathBarHighlight);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight Color"),         mPathBarHighlightColor);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("GoUpSelSubFolder"),                mExplorerGoUpSelSubFolder);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon"),                     mExplorerCustomIcon);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 16"),             mExplorerCustomIconFile[0]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 32"),             mExplorerCustomIconFile[1]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Extension Color"),                 mExplorerFilterColorType);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Sort"),                         mExplorerNoSort);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("24 Hour Time"),                    mExplorer24HourTime);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("2 Year Date"),                     mExplorer2YearDate);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive"),                      mExplorerShowDrive);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Item"),                 mExplorerShowDriveItem);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Size"),                 mExplorerShowDriveSize);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Name Case Type"),                  mExplorerNameCaseType);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save View Style"),                 mExplorerSaveViewStyle);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Make Text And Edit"),              mExplorerCreateAndEditText);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font"),                     mCustomFont);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font Name"),                mCustomFontText);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Auto Column Width"),               mExplorerAutoColumnWidth);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default View Style"),              mExplorerDefaultViewStyle);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort"),                    mExplorerDefaultSort);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort Order"),              mExplorerDefaultSortOrder);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save ViewSet"),                    mExplorerSaveViewSet);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Exit Verify ViewSet"),             mExplorerExitVerifyViewSet);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #1"),       mExplorerNoNetLastFolder[0]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #2"),       mExplorerNoNetLastFolder[1]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #3"),       mExplorerNoNetLastFolder[2]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #4"),       mExplorerNoNetLastFolder[3]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #5"),       mExplorerNoNetLastFolder[4]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #6"),       mExplorerNoNetLastFolder[5]);

    //
    // [Thumbnail]
    //
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Width"),                          mThumbnailWidth);
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Height"),                         mThumbnailHeight);
    sIniFile.setValueB(CFG_THUMBNAIL, XPR_STRING_LITERAL("Save Cache"),                     mThumbnailSaveCache);
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Priority"),                       mThumbnailPriority);
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Load By Ext"),                    mThumbnailLoadByExt);

    //
    // [Advanced]
    //
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray"),                            mTray);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Close"),                           mTrayOnClose);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File Scrap"),                      mFileScrapContextMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save File Scrap"),                 mFileScrapSave);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tab Save"),                        mTabSave);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Split View by Ratio"),             mViewSplitByRatio);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single Activate Path"),            mSingleViewActivatePath);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Save Path"),                 mDriveLastFolder);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Shift Key"),                 mDriveShiftKey);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Shell New"),                       mShellNewMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File List Cur Dir"),               mFileListCurDir);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Copy File Operation"),    mExternalCopyFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Move File Operation"),    mExternalMoveFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Delete File Operation"),  mExternalDeleteFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Working Folder Real Path"),        mWorkingFolderRealPath);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Single File Sel Size Unit"),       mSingleSelFileSizeUnit);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Multi File Sel Size Unit"),        mMultiSelFileSizeUnit);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("No Refresh"),                      mNoRefresh);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single Instance"),                 mSingleInstance);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Confirm Exit"),                    mConfirmExit);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Animation Menu"),                  mAnimationMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("SplitWnd TabKey"),                 mSplitViewTabKey);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Split Last Folder"),               mSplitLastFolder);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Recent File"),                     mRecentFile);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Standard Menu"),                   mStandardMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Refresh Sort"),                    mRefreshSort);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("AddressBar URL"),                  mAddressBarUrl);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Clear Search Result"),             mSearchResultClearOnClose);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Flash File Operation"),            mFileOpCompleteFlash);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save History"),                    mSaveHistory);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Count"),                   mHistoryCount);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Count"),                  mBackwardCount);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Menu Count"),              mHistoryMenuCount);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Menu Count"),             mBackwardMenuCount);
    sIniFile.setValueS(CFG_ADVANCED, XPR_STRING_LITERAL("Div Clipboard Text"),              mClipboardSeparator);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Type"),                       mDragType);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Distance"),                   mDragDist);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drop Type"),                       mDropType);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Expand Folder"),              mDragFolderTreeExpandTime);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Scroll Time"),                mDragScrollTime);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Default Operation"),          mDragDefaultFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drag No Contents"),                mDragNoContents);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Reg Shell Context Menu"),          mRegShellContextMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Init Folder"),                mTrayRestoreInitFolder);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Minimize"),                   mTrayOnMinmize);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray One Click"),                  mTrayOneClick);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher"),                        mLauncher);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Tray"),                   mLauncherTray);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Windows Startup"),        mLauncherWinStartup);
    sIniFile.setValueU(CFG_ADVANCED, XPR_STRING_LITERAL("Global Hot Key"),                  mLauncherGlobalHotKey);

    sIniFile.sortKey();

    sIniFile.writeFile(xpr::CharSetUtf16);

    return XPR_TRUE;
}
