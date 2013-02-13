//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
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
#include "CfgPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Option::Option(void)
    : mObserver(XPR_NULL)
{
}

Option::~Option(void)
{
}

void Option::setObserver(OptionObserver *aObserver)
{
    mObserver = aObserver;
}

COLORREF Option::getPathBarDefaultHighlightColor(void)
{
    return ::GetSysColor(COLOR_ACTIVECAPTION);
}

void Option::copyConfig(Config &aConfig) const
{
    aConfig = mConfig;
}

void Option::setConfig(const Config &aConfig)
{
    mConfig = aConfig;
}

void Option::notifyConfig(void)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onChangedConfig(*this);
    }
}

void Option::initDefault(void)
{
    loadConfigOption(XPR_STRING_LITERAL("default"));
}

static const xpr_tchar_t CFG_MAIN[]      = XPR_STRING_LITERAL("Main");
static const xpr_tchar_t CFG_CONFIG[]    = XPR_STRING_LITERAL("Config");
static const xpr_tchar_t CFG_GENERAL[]   = XPR_STRING_LITERAL("General");
static const xpr_tchar_t CFG_FOLDER[]    = XPR_STRING_LITERAL("Folder");
static const xpr_tchar_t CFG_CONTENTS[]  = XPR_STRING_LITERAL("Contents");
static const xpr_tchar_t CFG_EXPLORER[]  = XPR_STRING_LITERAL("Explorer");
static const xpr_tchar_t CFG_THUMBNAIL[] = XPR_STRING_LITERAL("Thumbnail");
static const xpr_tchar_t CFG_ADVANCED[]  = XPR_STRING_LITERAL("Advanced");

xpr_bool_t Option::loadMainOption(const xpr_tchar_t *aPath)
{
    xpr_bool_t sResult = XPR_TRUE;

    xpr_sint_t i, j;
    xpr_tchar_t sKey[0xff] = {0};
    xpr_tchar_t sEntry[0xff] = {0};
    const xpr_tchar_t *sValue;

    fxb::IniFile sIniFile(aPath);
    if (sIniFile.readFile() == XPR_FALSE)
        sResult = XPR_FALSE;

    // [Main]
    mMain.mWindowRect               = sIniFile.getValueR(CFG_MAIN, XPR_STRING_LITERAL("Window"),                     CRect(-1,-1,-1,-1));
    mMain.mWindowStatus             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Window Status"),              -1);
    mMain.mSingleFolderPaneMode     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Mode"),    XPR_FALSE);
    mMain.mShowEachFolderPane[0]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #1"),        XPR_TRUE);
    mMain.mShowEachFolderPane[1]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #2"),        XPR_TRUE);
    mMain.mShowEachFolderPane[2]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #3"),        XPR_TRUE);
    mMain.mShowEachFolderPane[3]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #4"),        XPR_TRUE);
    mMain.mShowEachFolderPane[4]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #5"),        XPR_TRUE);
    mMain.mShowEachFolderPane[5]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #6"),        XPR_TRUE);
    mMain.mShowSingleFolderPane     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Single Folder Pane"),    XPR_TRUE);
    mMain.mSingleFolderPaneSize     = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Size"),    DEF_FOLDER_PANE_SIZE);
    mMain.mEachFolderPaneSize[0]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #1"),        DEF_FOLDER_PANE_SIZE);
    mMain.mEachFolderPaneSize[1]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #2"),        DEF_FOLDER_PANE_SIZE);
    mMain.mEachFolderPaneSize[2]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #3"),        DEF_FOLDER_PANE_SIZE);
    mMain.mEachFolderPaneSize[3]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #4"),        DEF_FOLDER_PANE_SIZE);
    mMain.mEachFolderPaneSize[4]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #5"),        DEF_FOLDER_PANE_SIZE);
    mMain.mEachFolderPaneSize[5]    = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #6"),        DEF_FOLDER_PANE_SIZE);
    mMain.mLeftSingleFolderPane     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Single Folder Pane"),    XPR_TRUE);
    mMain.mLeftEachFolderPane[0]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #1"),        XPR_TRUE);
    mMain.mLeftEachFolderPane[1]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #2"),        XPR_TRUE);
    mMain.mLeftEachFolderPane[2]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #3"),        XPR_TRUE);
    mMain.mLeftEachFolderPane[3]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #4"),        XPR_TRUE);
    mMain.mLeftEachFolderPane[4]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #5"),        XPR_TRUE);
    mMain.mLeftEachFolderPane[5]    = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #6"),        XPR_TRUE);
    mMain.mBookmarkBarText          = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Text"),           XPR_TRUE);
    mMain.mBookmarkBarMultiLine     = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Multi Line"),     XPR_FALSE);
    mMain.mDriveBar                 = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar"),                   XPR_TRUE);
    mMain.mDriveBarShortText        = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Short Text"));
    mMain.mDriveBarMultiLine        = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Multi Line"),        XPR_FALSE);
    mMain.mViewSplitRowCount        = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Row Count"),        DEF_VIEW_SPLIT_ROW);
    mMain.mViewSplitColumnCount     = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Column Count"),     DEF_VIEW_SPLIT_COLUMN);
    mMain.mViewSplitRatio[0]        = sIniFile.getValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #1"),         0.0);
    mMain.mViewSplitRatio[1]        = sIniFile.getValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #2"),         0.0);
    mMain.mViewSplitRatio[2]        = sIniFile.getValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #3"),         0.0);
    mMain.mViewSplitSize[0]         = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #1"),          0);
    mMain.mViewSplitSize[1]         = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #2"),          0);
    mMain.mViewSplitSize[2]         = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #3"),          0);
    mMain.mViewStyle[0]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #1"),              LVS_REPORT);
    mMain.mViewStyle[1]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #2"),              LVS_REPORT);
    mMain.mViewStyle[2]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #3"),              LVS_REPORT);
    mMain.mViewStyle[3]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #4"),              LVS_REPORT);
    mMain.mViewStyle[4]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #5"),              LVS_REPORT);
    mMain.mViewStyle[5]             = sIniFile.getValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #6"),              LVS_REPORT);
    _tcscpy(mMain.mLastFolder[0],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #1"),             XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mLastFolder[1],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #2"),             XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mLastFolder[2],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #3"),             XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mLastFolder[3],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #4"),             XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mLastFolder[4],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #5"),             XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mLastFolder[5],     sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #6"),             XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mWorkingFolder[0],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #1"),          XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mWorkingFolder[1],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #2"),          XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mWorkingFolder[2],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #3"),          XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mWorkingFolder[3],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #4"),          XPR_STRING_LITERAL("")));
    _tcscpy(mMain.mWorkingFolder[4],  sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #5"),          XPR_STRING_LITERAL("")));
    mMain.mFileScrapDrop            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("FileScrapDrop"));
    mMain.mPicViewer                = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("PicViewer"));
    mMain.mTipOfTheToday            = sIniFile.getValueB(CFG_MAIN, XPR_STRING_LITERAL("Tip Of The Today"),           XPR_TRUE);

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        if (_tcslen(mMain.mLastFolder[i]) == 2 && mMain.mLastFolder[i][1] == XPR_STRING_LITERAL(':'))
            _tcscat(mMain.mLastFolder[i], XPR_STRING_LITERAL("\\"));
    }

    for (i = 0; i < MAX_WORKING_FOLDER; ++i)
    {
        if (_tcslen(mMain.mWorkingFolder[i]) == 2 && mMain.mWorkingFolder[i][1] == XPR_STRING_LITERAL(':'))
            _tcscat(mMain.mWorkingFolder[i], XPR_STRING_LITERAL("\\"));
    }

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        _stprintf(sKey, XPR_STRING_LITERAL("TabViewSplit #%d"), i + 1);

        mMain.mViewSplitTab[i].mCurTab = sIniFile.getValueI(sKey, XPR_STRING_LITERAL("Current Tab"));

        for (j = 0; ; ++j)
        {
            _stprintf(sEntry, XPR_STRING_LITERAL("Tab #%d"), j + 1);

            sValue = sIniFile.getValueS(sKey, sEntry);
            if (sValue == XPR_NULL)
            {
                break;
            }

            mMain.mViewSplitTab[i].mTabPathDeque.push_back(sValue);
        }
    }

    if (sIniFile.getKeyCount() <= 0)
        sResult = XPR_FALSE;

    return sResult;
}

xpr_bool_t Option::loadConfigOption(const xpr_tchar_t *aFileName)
{
    xpr_bool_t sResult = XPR_TRUE;

    xpr_sint_t i;
    xpr_tchar_t sKey[0xff] = {0};
    xpr_tchar_t sEntry[0xff] = {0};

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
    mConfig.mMouseClick                   = sIniFile.getValueI(CFG_GENERAL, XPR_STRING_LITERAL("Mouse"),                   MOUSE_DOUBLE_CLICK);
    mConfig.mShowSystemAttribute          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("System Attribute"));
    mConfig.mTitleFullPath                = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Title Full Path"));
    mConfig.mAddressFullPath              = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Address Full Path"));
    mConfig.mTooltip                      = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip"),                 XPR_TRUE);
    mConfig.mTooltipWithFileName          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip File Name"));
    mConfig.mGridLines                    = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("GridLines"));
    mConfig.mFullRowSelect                = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("FullRowSelect"));
    mConfig.mFileExtType                  = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Extension"));
    mConfig.mBatchRenameMultiSel          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Multi-Sel, Batch Rename"), XPR_TRUE);
    mConfig.mBatchRenameWithFolder        = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Batch Rename With Folder"));
    mConfig.mRenameByMouse                = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Mouse Rename"),            XPR_TRUE);
    mConfig.mNewItemDlg                   = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("New Item Dlg"),            XPR_TRUE);
    mConfig.mSingleRenameType             = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Single Rename Type"),      SINGLE_RENAME_TYPE_DEFAULT);
    mConfig.mRenameExtType                = sIniFile.getValueI(CFG_GENERAL, XPR_STRING_LITERAL("Rename Extension Type"),   RENAME_EXT_TYPE_DEFAULT);
    mConfig.mShowHiddenAttribute          = sIniFile.getValueB(CFG_GENERAL, XPR_STRING_LITERAL("Hidden Attribute"));

    _tcscpy(mConfig.mLanguage,          sIniFile.getValueS(CFG_MAIN, XPR_STRING_LITERAL("Language"),                   XPR_STRING_LITERAL("Korean")));

    // [Folder]
    mConfig.mFolderTreeBkgndColorType[0]  = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #1"), COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeBkgndColorType[1]  = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #2"), COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeBkgndColorType[2]  = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #3"), COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeBkgndColorType[3]  = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #4"), COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeBkgndColorType[4]  = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #5"), COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeBkgndColorType[5]  = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #6"), COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeBkgndColor[0]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #1"),      GetSysColor(COLOR_WINDOW));
    mConfig.mFolderTreeBkgndColor[1]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #2"),      GetSysColor(COLOR_WINDOW));
    mConfig.mFolderTreeBkgndColor[2]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #3"),      GetSysColor(COLOR_WINDOW));
    mConfig.mFolderTreeBkgndColor[3]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #4"),      GetSysColor(COLOR_WINDOW));
    mConfig.mFolderTreeBkgndColor[4]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #5"),      GetSysColor(COLOR_WINDOW));
    mConfig.mFolderTreeBkgndColor[5]      = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #6"),      GetSysColor(COLOR_WINDOW));
    mConfig.mFolderTreeTextColorType[0]   = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #1"),       COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeTextColorType[1]   = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #2"),       COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeTextColorType[2]   = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #3"),       COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeTextColorType[3]   = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #4"),       COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeTextColorType[4]   = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #5"),       COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeTextColorType[5]   = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #6"),       COLOR_TYPE_DEFAULT);
    mConfig.mFolderTreeTextColor[0]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #1"),            GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mFolderTreeTextColor[1]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #2"),            GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mFolderTreeTextColor[2]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #3"),            GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mFolderTreeTextColor[3]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #4"),            GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mFolderTreeTextColor[4]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #5"),            GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mFolderTreeTextColor[5]       = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #6"),            GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mFolderTreeIsItemHeight       = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Item Height"));
    mConfig.mFolderTreeItemHeight         = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Item Height Size"));
    mConfig.mFolderTreeHighlight[0]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #1"));
    mConfig.mFolderTreeHighlight[1]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #2"));
    mConfig.mFolderTreeHighlight[2]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #3"));
    mConfig.mFolderTreeHighlight[3]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #4"));
    mConfig.mFolderTreeHighlight[4]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #5"));
    mConfig.mFolderTreeHighlight[5]       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #6"));
    mConfig.mFolderTreeHighlightColor[0]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #1"), GetSysColor(COLOR_HIGHLIGHT));
    mConfig.mFolderTreeHighlightColor[1]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #2"), GetSysColor(COLOR_HIGHLIGHT));
    mConfig.mFolderTreeHighlightColor[2]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #3"), GetSysColor(COLOR_HIGHLIGHT));
    mConfig.mFolderTreeHighlightColor[3]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #4"), GetSysColor(COLOR_HIGHLIGHT));
    mConfig.mFolderTreeHighlightColor[4]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #5"), GetSysColor(COLOR_HIGHLIGHT));
    mConfig.mFolderTreeHighlightColor[5]  = sIniFile.getValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #6"), GetSysColor(COLOR_HIGHLIGHT));
    mConfig.mSingleFolderTreeLinkage      = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Single Folder Pane Linkage"));
    mConfig.mFolderTreeCustomFont         = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font"));
    _tcscpy(mConfig.mFolderTreeCustomFontText, sIniFile.getValueS(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font Name"),       XPR_STRING_LITERAL("")));
    mConfig.mFolderTreeInitNoExpand       = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Init, No Expand"));
    mConfig.mFolderTreeSelDelay           = sIniFile.getValueB(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay"));
    mConfig.mFolderTreeSelDelayTime       = sIniFile.getValueI(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay Time"),            DEF_FLD_SEL_DELAY_MSEC);

    if (mConfig.mFolderTreeItemHeight < MIN_FOLDER_TREE_HEIGHT) mConfig.mFolderTreeItemHeight = MIN_FOLDER_TREE_HEIGHT;
    if (mConfig.mFolderTreeItemHeight > MAX_FOLDER_TREE_HEIGHT) mConfig.mFolderTreeItemHeight = MAX_FOLDER_TREE_HEIGHT;

    if (mConfig.mFolderTreeSelDelayTime < MIN_FLD_SEL_DELAY_MSEC) mConfig.mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;
    if (mConfig.mFolderTreeSelDelayTime > MAX_FLD_SEL_DELAY_MSEC) mConfig.mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;

    //
    // [Contents]
    //

    mConfig.mShowAddressBar               = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Address Bar"),       XPR_TRUE);
    mConfig.mShowPathBar                  = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Path Bar"),          XPR_FALSE);
    mConfig.mShowEachDriveBar             = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Each Drive Bar"),    XPR_FALSE);
    mConfig.mDriveBarLeftSide             = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Drive Bar Left Side"),    XPR_FALSE);
    mConfig.mDriveBarShortText            = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Drive Bar Short Text"),   XPR_TRUE);
    mConfig.mShowActivateBar              = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Activate Bar"),      XPR_TRUE);
    mConfig.mShowInfoBar                  = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Info Bar"),          XPR_TRUE);
    mConfig.mShowStatusBar                = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Status Bar"),        XPR_TRUE);

    mConfig.mContentsStyle                = sIniFile.getValueI(CFG_CONTENTS, CFG_CONTENTS,                                 CONTENTS_EXPLORER);
    mConfig.mContentsBookmark             = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link"),                   XPR_TRUE);
    mConfig.mContentsBookmarkColor        = sIniFile.getValueC(CFG_CONTENTS, XPR_STRING_LITERAL("Link Color"),             RGB(0,0,255));
    mConfig.mContentsNoDispSelFileInfo    = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("No Disp Info"));
    mConfig.mContentsARHSAttribute        = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Attributes ARHS Style"));

    mConfig.mBookmarkTooltip              = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Popup"),                  XPR_TRUE);
    mConfig.mBookmarkExpandFolder         = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Expand Folder"));
    mConfig.mBookmarkRealPath             = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Real Path"));
    mConfig.mBookmarkFastNetIcon          = sIniFile.getValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Fast Network Icon"), XPR_TRUE);

    //
    // [Explorer]
    //
    mConfig.mExplorerBkgndImage[0]        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #1"), XPR_FALSE);
    mConfig.mExplorerBkgndImage[1]        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #2"), XPR_FALSE);
    mConfig.mExplorerBkgndImage[2]        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #3"), XPR_FALSE);
    mConfig.mExplorerBkgndImage[3]        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #4"), XPR_FALSE);
    mConfig.mExplorerBkgndImage[4]        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #5"), XPR_FALSE);
    mConfig.mExplorerBkgndImage[5]        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #6"), XPR_FALSE);
    _tcscpy(mConfig.mExplorerBkgndImagePath[0],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #1"),     XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerBkgndImagePath[1],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #2"),     XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerBkgndImagePath[2],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #3"),     XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerBkgndImagePath[3],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #4"),     XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerBkgndImagePath[4],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #5"),     XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerBkgndImagePath[5],  sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #6"),     XPR_STRING_LITERAL("")));
    mConfig.mExplorerBkgndColorType[0]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #1"), COLOR_TYPE_DEFAULT);
    mConfig.mExplorerBkgndColorType[1]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #2"), COLOR_TYPE_DEFAULT);
    mConfig.mExplorerBkgndColorType[2]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #3"), COLOR_TYPE_DEFAULT);
    mConfig.mExplorerBkgndColorType[3]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #4"), COLOR_TYPE_DEFAULT);
    mConfig.mExplorerBkgndColorType[4]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #5"), COLOR_TYPE_DEFAULT);
    mConfig.mExplorerBkgndColorType[5]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #6"), COLOR_TYPE_DEFAULT);
    mConfig.mExplorerBkgndColor[0]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #1"),    GetSysColor(COLOR_WINDOW));
    mConfig.mExplorerBkgndColor[1]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #2"),    GetSysColor(COLOR_WINDOW));
    mConfig.mExplorerBkgndColor[2]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #3"),    GetSysColor(COLOR_WINDOW));
    mConfig.mExplorerBkgndColor[3]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #4"),    GetSysColor(COLOR_WINDOW));
    mConfig.mExplorerBkgndColor[4]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #5"),    GetSysColor(COLOR_WINDOW));
    mConfig.mExplorerBkgndColor[5]        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #6"),    GetSysColor(COLOR_WINDOW));
    mConfig.mExplorerTextColorType[0]     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #1"),     COLOR_TYPE_DEFAULT);
    mConfig.mExplorerTextColorType[1]     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #2"),     COLOR_TYPE_DEFAULT);
    mConfig.mExplorerTextColorType[2]     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #3"),     COLOR_TYPE_DEFAULT);
    mConfig.mExplorerTextColorType[3]     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #4"),     COLOR_TYPE_DEFAULT);
    mConfig.mExplorerTextColorType[4]     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #5"),     COLOR_TYPE_DEFAULT);
    mConfig.mExplorerTextColorType[5]     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #6"),     COLOR_TYPE_DEFAULT);
    mConfig.mExplorerTextColor[0]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #1"),          GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mExplorerTextColor[1]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #2"),          GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mExplorerTextColor[2]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #3"),          GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mExplorerTextColor[3]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #4"),          GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mExplorerTextColor[4]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #5"),          GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mExplorerTextColor[5]         = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #6"),          GetSysColor(COLOR_WINDOWTEXT));
    mConfig.mExplorerInitFolderType[0]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #1"),         INIT_TYPE_NONE);
    mConfig.mExplorerInitFolderType[1]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #2"),         INIT_TYPE_NONE);
    mConfig.mExplorerInitFolderType[2]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #3"),         INIT_TYPE_NONE);
    mConfig.mExplorerInitFolderType[3]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #4"),         INIT_TYPE_NONE);
    mConfig.mExplorerInitFolderType[4]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #5"),         INIT_TYPE_NONE);
    mConfig.mExplorerInitFolderType[5]    = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #6"),         INIT_TYPE_NONE);
    mConfig.mExplorerSizeUnit             = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Size Unit"),              SIZE_UNIT_DEFAULT);
    mConfig.mExplorerListType             = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("List Type"),              LIST_TYPE_ALL);
    mConfig.mExplorerParentFolder         = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Parent Folder"));
    mConfig.mPathBarRealPath              = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar RealPath"));
    mConfig.mActiveViewColor              = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Activate Color"),         RGB(48,192,48));//GetSysColor(COLOR_ACTIVECAPTION));
    mConfig.mPathBarIcon                  = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Icon"));
    mConfig.mPathBarHighlight             = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight"),      XPR_TRUE);
    mConfig.mPathBarHighlightColor        = sIniFile.getValueC(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight Color"),Option::getPathBarDefaultHighlightColor());//GetSysColor(COLOR_ACTIVECAPTION));
    mConfig.mExplorerGoUpSelSubFolder     = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("GoUpSelSubFolder"));
    mConfig.mExplorerCustomIcon           = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon"));
    mConfig.mExplorerNoSort               = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Sort"));
    mConfig.mExplorer24HourTime           = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("24 Hour Time"));
    mConfig.mExplorer2YearDate            = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("2 Year Date"));
    mConfig.mExplorerShowDrive            = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive"));
    mConfig.mExplorerShowDriveItem        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Item"));
    mConfig.mExplorerShowDriveSize        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Size"));
    mConfig.mExplorerNameCaseType         = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Name Case Type"),         NAME_CASE_TYPE_DEFAULT);
    mConfig.mExplorerSaveViewStyle        = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save View Style"),        XPR_TRUE);
    mConfig.mExplorerCreateAndEditText    = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Make Text And Edit"));
    mConfig.mCustomFont                   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font"));
    mConfig.mExplorerAutoColumnWidth      = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Auto Column Width"));
    mConfig.mExplorerDefaultViewStyle     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default View Style"),     VIEW_STYLE_REPORT);
    mConfig.mExplorerDefaultSort          = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort"));
    mConfig.mExplorerDefaultSortOrder     = sIniFile.getValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort Order"),     1);
    mConfig.mExplorerSaveViewSet          = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save ViewSet"),           SAVE_VIEW_SET_DEFAULT);
    mConfig.mExplorerExitVerifyViewSet    = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Exit Verify ViewSet"),    XPR_TRUE);
    _tcscpy(mConfig.mExplorerCustomIconFile[0], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 16"), XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerCustomIconFile[1], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 32"), XPR_STRING_LITERAL("")));
    mConfig.mExplorerNoNetLastFolder[0]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #1"));
    mConfig.mExplorerNoNetLastFolder[1]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #2"));
    mConfig.mExplorerNoNetLastFolder[2]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #3"));
    mConfig.mExplorerNoNetLastFolder[3]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #4"));
    mConfig.mExplorerNoNetLastFolder[4]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #5"));
    mConfig.mExplorerNoNetLastFolder[5]   = sIniFile.getValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #6"));
    _tcscpy(mConfig.mExplorerInitFolder[0], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #1"),    XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerInitFolder[1], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #2"),    XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerInitFolder[2], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #3"),    XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerInitFolder[3], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #4"),    XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerInitFolder[4], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #5"),    XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mExplorerInitFolder[5], sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #6"),    XPR_STRING_LITERAL("")));
    _tcscpy(mConfig.mCustomFontText,        sIniFile.getValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font Name"),       XPR_STRING_LITERAL("")));

    if (mConfig.mExplorerCustomIconFile[0][0] == 0)
        _tcscpy(mConfig.mExplorerCustomIconFile[0], XPR_STRING_LITERAL("%flyExplorer%\\custom_image_list_16.bmp"));

    if (mConfig.mExplorerCustomIconFile[1][0] == 0)
        _tcscpy(mConfig.mExplorerCustomIconFile[1], XPR_STRING_LITERAL("%flyExplorer%\\custom_image_list_32.bmp"));

    fxb::SizeFormat::validSizeUnit(mConfig.mExplorerSizeUnit);

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        if (_tcslen(mConfig.mExplorerInitFolder[i]) == 2 && mConfig.mExplorerInitFolder[i][1] == XPR_STRING_LITERAL(':'))
            _tcscat(mConfig.mExplorerInitFolder[i], XPR_STRING_LITERAL("\\"));
    }

    //
    // [Thumbnail]
    //
    mConfig.mThumbnailWidth               = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Width"),                 DEF_THUMB_SIZE);
    mConfig.mThumbnailHeight              = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Height"),                DEF_THUMB_SIZE);
    mConfig.mThumbnailSaveCache           = sIniFile.getValueB(CFG_THUMBNAIL, XPR_STRING_LITERAL("Save Cache"));
    mConfig.mThumbnailPriority            = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Priority"),              fxb::THUMBNAIL_PRIORITY_BELOW_NORMAL);
    mConfig.mThumbnailLoadByExt           = sIniFile.getValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Load By Ext"),           XPR_TRUE);

    //
    // Advanced
    //
    mConfig.mTray                         = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray"));
    mConfig.mTrayOnClose                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Close"));
    mConfig.mFileScrapContextMenu         = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File Scrap"),             XPR_TRUE);
    mConfig.mFileScrapSave                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save File Scrap"));
    mConfig.mViewSplitByRatio             = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Split View by Ratio"),    XPR_TRUE);
    mConfig.mSingleViewSplitAsActivedView = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single View Split as Actived View"));
    mConfig.mDriveLastFolder              = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Save Path"));
    mConfig.mDriveShiftKey                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Shift Key"));
    mConfig.mShellNewMenu                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Shell New"));
    mConfig.mFileListCurDir               = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File List Cur Dir"));
    mConfig.mExternalCopyFileOp           = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Copy File Operation"));
    mConfig.mExternalMoveFileOp           = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Move File Operation"));
    mConfig.mExternalDeleteFileOp         = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Delete File Operation"));
    mConfig.mWorkingFolderRealPath        = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Working Folder Real Path"));
    mConfig.mSingleSelFileSizeUnit        = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Single File Sel Size Unit"), SIZE_UNIT_DEFAULT);
    mConfig.mMultiSelFileSizeUnit         = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Multi File Sel Size Unit"),  SIZE_UNIT_DEFAULT);
    mConfig.mNoRefresh                    = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("No Refresh"));
    mConfig.mSingleInstance               = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single Instance"));
    mConfig.mConfirmExit                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Confirm Exit"));
    mConfig.mAnimationMenu                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Animation Menu"));
    mConfig.mViewSplitReopenLastFolder    = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("View Split Reopen Last Folder"));
    mConfig.mRecentFile                   = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Recent File"),            XPR_TRUE);
    mConfig.mStandardMenu                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Standard Menu"));
    mConfig.mRefreshSort                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Refresh Sort"));
    mConfig.mAddressBarUrl                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("AddressBar URL"));
    mConfig.mFileOpCompleteFlash          = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Flash File Operation"));
    mConfig.mSaveHistory                  = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save History"));
    mConfig.mHistoryCount                 = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Count"),          DEF_HISTORY);
    mConfig.mBackwardCount                = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Count"),         DEF_BACKWARD);
    mConfig.mHistoryMenuCount             = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Menu Count"),     DEF_HISTORY_MENU);
    mConfig.mBackwardMenuCount            = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Menu Count"),    DEF_BACKWARD_MENU);
    mConfig.mDragType                     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Type"),              DRAG_START_DEFAULT);
    mConfig.mDragDist                     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Distance"),          DEF_DIST_DRAG);
    mConfig.mDropType                     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drop Type"),              DROP_DEFAULT);
    mConfig.mDragFolderTreeExpandTime     = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Expand Folder"),     1);
    mConfig.mDragScrollTime               = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Scroll Time"),       1);
    mConfig.mDragDefaultFileOp            = sIniFile.getValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Default Operation"), DRAG_FILE_OP_DEFAULT);
    mConfig.mDragNoContents               = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drag No Contents"));
    mConfig.mRegShellContextMenu          = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Reg Shell Context Menu"));
    mConfig.mTrayRestoreInitFolder        = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Init Folder"));
    mConfig.mTrayOnMinmize                = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Minimize"));
    mConfig.mTrayOneClick                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray One Click"));
    mConfig.mLauncher                     = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher"));
    mConfig.mLauncherTray                 = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Tray"));
    mConfig.mLauncherWinStartup           = sIniFile.getValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Windows Startup"));
    mConfig.mLauncherGlobalHotKey         = sIniFile.getValueU(CFG_ADVANCED, XPR_STRING_LITERAL("Global Hot Key"));

    if (mConfig.mDragDist < MIN_DIST_DRAG) mConfig.mDragDist = DEF_DIST_DRAG;
    if (mConfig.mDragDist > MAX_DIST_DRAG) mConfig.mDragDist = DEF_DIST_DRAG;

    if (mConfig.mDragFolderTreeExpandTime < MIN_EXPAND_FOLDER_MSEC) mConfig.mDragFolderTreeExpandTime = DEF_EXPAND_FOLDER_MSEC;
    if (mConfig.mDragFolderTreeExpandTime > MAX_EXPAND_FOLDER_MSEC) mConfig.mDragFolderTreeExpandTime = DEF_EXPAND_FOLDER_MSEC;

    if (mConfig.mDragScrollTime < MIN_DRAG_SCROLL_MSEC) mConfig.mDragScrollTime = DEF_DRAG_SCROLL_MSEC;
    if (mConfig.mDragScrollTime > MAX_DRAG_SCROLL_MSEC) mConfig.mDragScrollTime = DEF_DRAG_SCROLL_MSEC;

    _tcscpy(mConfig.mClipboardSeparator, sIniFile.getValueS(CFG_ADVANCED, XPR_STRING_LITERAL("Div Clipboard Text"), XPR_STRING_LITERAL("\\r\\n")));

    if (mConfig.mHistoryCount < MIN_HISTORY) mConfig.mHistoryCount = DEF_HISTORY;
    if (mConfig.mHistoryCount > MAX_HISTORY) mConfig.mHistoryCount = DEF_HISTORY;

    if (mConfig.mBackwardCount < MIN_BACKWARD) mConfig.mBackwardCount = DEF_BACKWARD;
    if (mConfig.mBackwardCount > MAX_BACKWARD) mConfig.mBackwardCount = DEF_BACKWARD;

    if (mConfig.mHistoryMenuCount < MIN_HISTORY_MENU) mConfig.mHistoryMenuCount = DEF_HISTORY_MENU;
    if (mConfig.mHistoryMenuCount > MAX_HISTORY_MENU) mConfig.mHistoryMenuCount = DEF_HISTORY_MENU;

    if (mConfig.mBackwardMenuCount < MIN_BACKWARD_MENU) mConfig.mBackwardMenuCount = DEF_BACKWARD_MENU;
    if (mConfig.mBackwardMenuCount > MAX_BACKWARD_MENU) mConfig.mBackwardMenuCount = DEF_BACKWARD_MENU;

    fxb::SizeFormat::validSizeUnit(mConfig.mSingleSelFileSizeUnit);
    fxb::SizeFormat::validSizeUnit(mConfig.mMultiSelFileSizeUnit);

    if (sIniFile.getKeyCount() <= 0)
        sResult = XPR_FALSE;

    return sResult;
}

xpr_bool_t Option::saveMainOption(const xpr_tchar_t *aFileName)
{
    xpr_sint_t i, j;
    xpr_tchar_t sKey[0xff] = {0};
    xpr_tchar_t sEntry[0xff] = {0};

    xpr_tchar_t sAppVer[0xff] = {0};
    getAppVer(sAppVer);

    fxb::IniFile sIniFile(aFileName);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer Main File"));

    //
    // [Main]
    //
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("version"),                         sAppVer);
    sIniFile.setValueR(CFG_MAIN, XPR_STRING_LITERAL("Window"),                          mMain.mWindowRect);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Window Status"),                   mMain.mWindowStatus);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Mode"),         mMain.mSingleFolderPaneMode);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #1"),             mMain.mShowEachFolderPane[0]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #2"),             mMain.mShowEachFolderPane[1]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #3"),             mMain.mShowEachFolderPane[2]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #4"),             mMain.mShowEachFolderPane[3]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #5"),             mMain.mShowEachFolderPane[4]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Folder Pane #6"),             mMain.mShowEachFolderPane[5]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Show Single Folder Pane"),         mMain.mShowSingleFolderPane);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Single Folder Pane Size"),         mMain.mSingleFolderPaneSize);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #1"),             mMain.mEachFolderPaneSize[0]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #2"),             mMain.mEachFolderPaneSize[1]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #3"),             mMain.mEachFolderPaneSize[2]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #4"),             mMain.mEachFolderPaneSize[3]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #5"),             mMain.mEachFolderPaneSize[4]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("Folder Pane Size #6"),             mMain.mEachFolderPaneSize[5]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Single Folder Pane"),         mMain.mLeftSingleFolderPane);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #1"),             mMain.mLeftEachFolderPane[0]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #2"),             mMain.mLeftEachFolderPane[1]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #3"),             mMain.mLeftEachFolderPane[2]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #4"),             mMain.mLeftEachFolderPane[3]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #5"),             mMain.mLeftEachFolderPane[4]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Left Folder Pane #6"),             mMain.mLeftEachFolderPane[5]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Text"),                mMain.mBookmarkBarText);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("BookmarkBar Multi Line"),          mMain.mBookmarkBarMultiLine);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar"),                        mMain.mDriveBar);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Short Text"),             mMain.mDriveBarShortText);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("DriveBar Multi Line"),             mMain.mDriveBarMultiLine);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Row Count"),             mMain.mViewSplitRowCount);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Column Count"),          mMain.mViewSplitColumnCount);
    sIniFile.setValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #1"),              mMain.mViewSplitRatio[0]);
    sIniFile.setValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #2"),              mMain.mViewSplitRatio[1]);
    sIniFile.setValueF(CFG_MAIN, XPR_STRING_LITERAL("SplitView Ratio #3"),              mMain.mViewSplitRatio[2]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #1"),               mMain.mViewSplitSize[0]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #2"),               mMain.mViewSplitSize[1]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("SplitView Size #3"),               mMain.mViewSplitSize[2]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #1"),                   mMain.mViewStyle[0]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #2"),                   mMain.mViewStyle[1]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #3"),                   mMain.mViewStyle[2]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #4"),                   mMain.mViewStyle[3]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #5"),                   mMain.mViewStyle[4]);
    sIniFile.setValueI(CFG_MAIN, XPR_STRING_LITERAL("View Style #6"),                   mMain.mViewStyle[5]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #1"),                  mMain.mLastFolder[0]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #2"),                  mMain.mLastFolder[1]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #3"),                  mMain.mLastFolder[2]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #4"),                  mMain.mLastFolder[3]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #5"),                  mMain.mLastFolder[4]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Last Folder #6"),                  mMain.mLastFolder[5]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #1"),               mMain.mWorkingFolder[0]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #2"),               mMain.mWorkingFolder[1]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #3"),               mMain.mWorkingFolder[2]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #4"),               mMain.mWorkingFolder[3]);
    sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Working Folder #5"),               mMain.mWorkingFolder[4]);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("FileScrapDrop"),                   mMain.mFileScrapDrop);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("PicViewer"),                       mMain.mPicViewer);
    sIniFile.setValueB(CFG_MAIN, XPR_STRING_LITERAL("Tip Of The Today"),                mMain.mTipOfTheToday);

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        _stprintf(sKey, XPR_STRING_LITERAL("TabViewSplit #%d"), i + 1);

        sIniFile.setValueI(sKey, XPR_STRING_LITERAL("Current Tab"), (xpr_sint_t)mMain.mViewSplitTab[i].mCurTab);

        TabPathDeque::iterator sIterator = mMain.mViewSplitTab[i].mTabPathDeque.begin();
        for (j = 0; sIterator != mMain.mViewSplitTab[i].mTabPathDeque.end(); ++sIterator, ++j)
        {
            const std::tstring &sFullPath = *sIterator;

            _stprintf(sEntry, XPR_STRING_LITERAL("Tab #%d"), j + 1);

            sIniFile.setValueS(sKey, sEntry, sFullPath.c_str());
        }
    }

    sIniFile.writeFile(xpr::CharSetUtf16);

    return XPR_TRUE;
}

xpr_bool_t Option::saveConfigOption(const xpr_tchar_t *aFileName)
{
    xpr_tchar_t sAppVer[0xff] = {0};
    getAppVer(sAppVer);

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
    sIniFile.setValueI(CFG_GENERAL, XPR_STRING_LITERAL("Mouse"),                            mConfig.mMouseClick);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Hidden Attribute"),                 mConfig.mShowHiddenAttribute);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("System Attribute"),                 mConfig.mShowSystemAttribute);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Title Full Path"),                  mConfig.mTitleFullPath);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Address Full Path"),                mConfig.mAddressFullPath);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip"),                          mConfig.mTooltip);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("ToolTip File Name"),                mConfig.mTooltipWithFileName);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("GridLines"),                        mConfig.mGridLines);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("FullRowSelect"),                    mConfig.mFullRowSelect);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Extension"),                        mConfig.mFileExtType);
    sIniFile.setValueI(CFG_GENERAL, XPR_STRING_LITERAL("Rename Extension Type"),            mConfig.mRenameExtType);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Multi-Sel, Batch Rename"),          mConfig.mBatchRenameMultiSel);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Mouse Rename"),                     mConfig.mRenameByMouse);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Batch Rename With Folder"),         mConfig.mBatchRenameWithFolder);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("New Item Dlg"),                     mConfig.mNewItemDlg);
    sIniFile.setValueB(CFG_GENERAL, XPR_STRING_LITERAL("Single Rename Type"),               mConfig.mSingleRenameType);

        sIniFile.setValueS(CFG_MAIN, XPR_STRING_LITERAL("Language"),                        mConfig.mLanguage);

    //
    // [Folder]
    //
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #1"),          mConfig.mFolderTreeBkgndColorType[0]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #2"),          mConfig.mFolderTreeBkgndColorType[1]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #3"),          mConfig.mFolderTreeBkgndColorType[2]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #4"),          mConfig.mFolderTreeBkgndColorType[3]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #5"),          mConfig.mFolderTreeBkgndColorType[4]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Background Color Type #6"),          mConfig.mFolderTreeBkgndColorType[5]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #1"),               mConfig.mFolderTreeBkgndColor[0]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #2"),               mConfig.mFolderTreeBkgndColor[1]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #3"),               mConfig.mFolderTreeBkgndColor[2]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #4"),               mConfig.mFolderTreeBkgndColor[3]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #5"),               mConfig.mFolderTreeBkgndColor[4]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Background Color #6"),               mConfig.mFolderTreeBkgndColor[5]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #1"),                mConfig.mFolderTreeTextColorType[0]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #2"),                mConfig.mFolderTreeTextColorType[1]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #3"),                mConfig.mFolderTreeTextColorType[2]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #4"),                mConfig.mFolderTreeTextColorType[3]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #5"),                mConfig.mFolderTreeTextColorType[4]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Text Color Type #6"),                mConfig.mFolderTreeTextColorType[5]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #1"),                     mConfig.mFolderTreeTextColor[0]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #2"),                     mConfig.mFolderTreeTextColor[1]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #3"),                     mConfig.mFolderTreeTextColor[2]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #4"),                     mConfig.mFolderTreeTextColor[3]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #5"),                     mConfig.mFolderTreeTextColor[4]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Text Color #6"),                     mConfig.mFolderTreeTextColor[5]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Item Height"),                       mConfig.mFolderTreeIsItemHeight);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Item Height Size"),                  mConfig.mFolderTreeItemHeight);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #1"),               mConfig.mFolderTreeHighlight[0]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #2"),               mConfig.mFolderTreeHighlight[1]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #3"),               mConfig.mFolderTreeHighlight[2]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #4"),               mConfig.mFolderTreeHighlight[3]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #5"),               mConfig.mFolderTreeHighlight[4]);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight #6"),               mConfig.mFolderTreeHighlight[5]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #1"),         mConfig.mFolderTreeHighlightColor[0]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #2"),         mConfig.mFolderTreeHighlightColor[1]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #3"),         mConfig.mFolderTreeHighlightColor[2]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #4"),         mConfig.mFolderTreeHighlightColor[3]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #5"),         mConfig.mFolderTreeHighlightColor[4]);
    sIniFile.setValueC(CFG_FOLDER, XPR_STRING_LITERAL("Folder Highlight Color #6"),         mConfig.mFolderTreeHighlightColor[5]);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font"),                       mConfig.mFolderTreeCustomFont);
    sIniFile.setValueS(CFG_FOLDER, XPR_STRING_LITERAL("Custom Font Name"),                  mConfig.mFolderTreeCustomFontText);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Init, No Expand"),                   mConfig.mFolderTreeInitNoExpand);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay"),                         mConfig.mFolderTreeSelDelay);
    sIniFile.setValueI(CFG_FOLDER, XPR_STRING_LITERAL("Sel Delay Time"),                    mConfig.mFolderTreeSelDelayTime);
    sIniFile.setValueB(CFG_FOLDER, XPR_STRING_LITERAL("Single Folder Pane Linkage"),        mConfig.mSingleFolderTreeLinkage);

    //
    // [Contents]
    //
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Address Bar"),                mConfig.mShowAddressBar  );
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Path Bar"),                   mConfig.mShowPathBar     );
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Each Drive Bar"),             mConfig.mShowEachDriveBar);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Drive Bar Left Side"),             mConfig.mDriveBarLeftSide);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Drive Bar Short Text"),            mConfig.mDriveBarShortText);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Activate Bar"),               mConfig.mShowActivateBar );
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Info Bar"),                   mConfig.mShowInfoBar     );
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Status Bar"),                 mConfig.mShowStatusBar   );

    sIniFile.setValueI(CFG_CONTENTS, CFG_CONTENTS,                                          mConfig.mContentsStyle);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link"),                            mConfig.mContentsBookmark);
    sIniFile.setValueC(CFG_CONTENTS, XPR_STRING_LITERAL("Link Color"),                      mConfig.mContentsBookmarkColor);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("No Disp Info"),                    mConfig.mContentsNoDispSelFileInfo);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Attributes ARHS Style"),           mConfig.mContentsARHSAttribute);

    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Popup"),                           mConfig.mBookmarkTooltip);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Expand Folder"),              mConfig.mBookmarkExpandFolder);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Real Path"),                  mConfig.mBookmarkRealPath);
    sIniFile.setValueB(CFG_CONTENTS, XPR_STRING_LITERAL("Link Fast Network Icon"),          mConfig.mBookmarkFastNetIcon);


    //
    // [Explorer]
    //
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #1"),             mConfig.mExplorerBkgndImage[0]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #2"),             mConfig.mExplorerBkgndImage[1]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #3"),             mConfig.mExplorerBkgndImage[2]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #4"),             mConfig.mExplorerBkgndImage[3]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #5"),             mConfig.mExplorerBkgndImage[4]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image #6"),             mConfig.mExplorerBkgndImage[5]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #1"),        mConfig.mExplorerBkgndImagePath[0]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #2"),        mConfig.mExplorerBkgndImagePath[1]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #3"),        mConfig.mExplorerBkgndImagePath[2]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #4"),        mConfig.mExplorerBkgndImagePath[3]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #5"),        mConfig.mExplorerBkgndImagePath[4]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Background Image Path #6"),        mConfig.mExplorerBkgndImagePath[5]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #1"),        mConfig.mExplorerBkgndColorType[0]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #2"),        mConfig.mExplorerBkgndColorType[1]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #3"),        mConfig.mExplorerBkgndColorType[2]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #4"),        mConfig.mExplorerBkgndColorType[3]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #5"),        mConfig.mExplorerBkgndColorType[4]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color Type #6"),        mConfig.mExplorerBkgndColorType[5]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #1"),             mConfig.mExplorerBkgndColor[0]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #2"),             mConfig.mExplorerBkgndColor[1]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #3"),             mConfig.mExplorerBkgndColor[2]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #4"),             mConfig.mExplorerBkgndColor[3]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #5"),             mConfig.mExplorerBkgndColor[4]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Background Color #6"),             mConfig.mExplorerBkgndColor[5]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #1"),              mConfig.mExplorerTextColorType[0]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #2"),              mConfig.mExplorerTextColorType[1]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #3"),              mConfig.mExplorerTextColorType[2]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #4"),              mConfig.mExplorerTextColorType[3]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #5"),              mConfig.mExplorerTextColorType[4]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color Type #6"),              mConfig.mExplorerTextColorType[5]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #1"),                   mConfig.mExplorerTextColor[0]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #2"),                   mConfig.mExplorerTextColor[1]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #3"),                   mConfig.mExplorerTextColor[2]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #4"),                   mConfig.mExplorerTextColor[3]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #5"),                   mConfig.mExplorerTextColor[4]);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Text Color #6"),                   mConfig.mExplorerTextColor[5]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #1"),                  mConfig.mExplorerInitFolderType[0]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #2"),                  mConfig.mExplorerInitFolderType[1]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #3"),                  mConfig.mExplorerInitFolderType[2]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #4"),                  mConfig.mExplorerInitFolderType[3]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #5"),                  mConfig.mExplorerInitFolderType[4]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder #6"),                  mConfig.mExplorerInitFolderType[5]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #1"),             mConfig.mExplorerInitFolder[0]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #2"),             mConfig.mExplorerInitFolder[1]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #3"),             mConfig.mExplorerInitFolder[2]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #4"),             mConfig.mExplorerInitFolder[3]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #5"),             mConfig.mExplorerInitFolder[4]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Init Folder Path #6"),             mConfig.mExplorerInitFolder[5]);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Size Unit"),                       mConfig.mExplorerSizeUnit);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("List Type"),                       mConfig.mExplorerListType);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Parent Folder"),                   mConfig.mExplorerParentFolder);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar RealPath"),                mConfig.mPathBarRealPath);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("Activate Color"),                  mConfig.mActiveViewColor);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Icon"),                    mConfig.mPathBarIcon);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight"),               mConfig.mPathBarHighlight);
    sIniFile.setValueC(CFG_EXPLORER, XPR_STRING_LITERAL("PathBar Highlight Color"),         mConfig.mPathBarHighlightColor);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("GoUpSelSubFolder"),                mConfig.mExplorerGoUpSelSubFolder);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon"),                     mConfig.mExplorerCustomIcon);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 16"),             mConfig.mExplorerCustomIconFile[0]);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Icon Path 32"),             mConfig.mExplorerCustomIconFile[1]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Sort"),                         mConfig.mExplorerNoSort);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("24 Hour Time"),                    mConfig.mExplorer24HourTime);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("2 Year Date"),                     mConfig.mExplorer2YearDate);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive"),                      mConfig.mExplorerShowDrive);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Item"),                 mConfig.mExplorerShowDriveItem);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Show Drive Size"),                 mConfig.mExplorerShowDriveSize);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Name Case Type"),                  mConfig.mExplorerNameCaseType);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save View Style"),                 mConfig.mExplorerSaveViewStyle);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Make Text And Edit"),              mConfig.mExplorerCreateAndEditText);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font"),                     mConfig.mCustomFont);
    sIniFile.setValueS(CFG_EXPLORER, XPR_STRING_LITERAL("Custom Font Name"),                mConfig.mCustomFontText);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Auto Column Width"),               mConfig.mExplorerAutoColumnWidth);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default View Style"),              mConfig.mExplorerDefaultViewStyle);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort"),                    mConfig.mExplorerDefaultSort);
    sIniFile.setValueI(CFG_EXPLORER, XPR_STRING_LITERAL("Default Sort Order"),              mConfig.mExplorerDefaultSortOrder);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Save ViewSet"),                    mConfig.mExplorerSaveViewSet);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("Exit Verify ViewSet"),             mConfig.mExplorerExitVerifyViewSet);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #1"),       mConfig.mExplorerNoNetLastFolder[0]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #2"),       mConfig.mExplorerNoNetLastFolder[1]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #3"),       mConfig.mExplorerNoNetLastFolder[2]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #4"),       mConfig.mExplorerNoNetLastFolder[3]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #5"),       mConfig.mExplorerNoNetLastFolder[4]);
    sIniFile.setValueB(CFG_EXPLORER, XPR_STRING_LITERAL("No Network Last Folder #6"),       mConfig.mExplorerNoNetLastFolder[5]);

    //
    // [Thumbnail]
    //
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Width"),                          mConfig.mThumbnailWidth);
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Height"),                         mConfig.mThumbnailHeight);
    sIniFile.setValueB(CFG_THUMBNAIL, XPR_STRING_LITERAL("Save Cache"),                     mConfig.mThumbnailSaveCache);
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Priority"),                       mConfig.mThumbnailPriority);
    sIniFile.setValueI(CFG_THUMBNAIL, XPR_STRING_LITERAL("Load By Ext"),                    mConfig.mThumbnailLoadByExt);

    //
    // [Advanced]
    //
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray"),                            mConfig.mTray);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Close"),                           mConfig.mTrayOnClose);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File Scrap"),                      mConfig.mFileScrapContextMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save File Scrap"),                 mConfig.mFileScrapSave);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Split View by Ratio"),             mConfig.mViewSplitByRatio);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single View Split as Actived View"),mConfig.mSingleViewSplitAsActivedView);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Save Path"),                 mConfig.mDriveLastFolder);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drive Shift Key"),                 mConfig.mDriveShiftKey);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Shell New"),                       mConfig.mShellNewMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("File List Cur Dir"),               mConfig.mFileListCurDir);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Copy File Operation"),    mConfig.mExternalCopyFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Move File Operation"),    mConfig.mExternalMoveFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("External Delete File Operation"),  mConfig.mExternalDeleteFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Working Folder Real Path"),        mConfig.mWorkingFolderRealPath);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Single File Sel Size Unit"),       mConfig.mSingleSelFileSizeUnit);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Multi File Sel Size Unit"),        mConfig.mMultiSelFileSizeUnit);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("No Refresh"),                      mConfig.mNoRefresh);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Single Instance"),                 mConfig.mSingleInstance);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Confirm Exit"),                    mConfig.mConfirmExit);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Animation Menu"),                  mConfig.mAnimationMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("View Split Reopen Last Folder"),   mConfig.mViewSplitReopenLastFolder);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Recent File"),                     mConfig.mRecentFile);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Standard Menu"),                   mConfig.mStandardMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Refresh Sort"),                    mConfig.mRefreshSort);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("AddressBar URL"),                  mConfig.mAddressBarUrl);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Flash File Operation"),            mConfig.mFileOpCompleteFlash);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Save History"),                    mConfig.mSaveHistory);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Count"),                   mConfig.mHistoryCount);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Count"),                  mConfig.mBackwardCount);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("History Menu Count"),              mConfig.mHistoryMenuCount);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Backward Menu Count"),             mConfig.mBackwardMenuCount);
    sIniFile.setValueS(CFG_ADVANCED, XPR_STRING_LITERAL("Div Clipboard Text"),              mConfig.mClipboardSeparator);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Type"),                       mConfig.mDragType);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Distance"),                   mConfig.mDragDist);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drop Type"),                       mConfig.mDropType);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Expand Folder"),              mConfig.mDragFolderTreeExpandTime);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Scroll Time"),                mConfig.mDragScrollTime);
    sIniFile.setValueI(CFG_ADVANCED, XPR_STRING_LITERAL("Drag Default Operation"),          mConfig.mDragDefaultFileOp);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Drag No Contents"),                mConfig.mDragNoContents);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Reg Shell Context Menu"),          mConfig.mRegShellContextMenu);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Init Folder"),                mConfig.mTrayRestoreInitFolder);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray Minimize"),                   mConfig.mTrayOnMinmize);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Tray One Click"),                  mConfig.mTrayOneClick);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher"),                        mConfig.mLauncher);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Tray"),                   mConfig.mLauncherTray);
    sIniFile.setValueB(CFG_ADVANCED, XPR_STRING_LITERAL("Launcher Windows Startup"),        mConfig.mLauncherWinStartup);
    sIniFile.setValueU(CFG_ADVANCED, XPR_STRING_LITERAL("Global Hot Key"),                  mConfig.mLauncherGlobalHotKey);

    sIniFile.sortKey();

    sIniFile.writeFile(xpr::CharSetUtf16);

    return XPR_TRUE;
}
