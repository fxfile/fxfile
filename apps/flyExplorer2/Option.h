//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_OPTION_H__
#define __FX_OPTION_H__
#pragma once

#define MIN_HISTORY             10
#define DEF_HISTORY             100
#define MAX_HISTORY             1000
#define MIN_HISTORY_MENU        5
#define DEF_HISTORY_MENU        10
#define MAX_HISTORY_MENU        30

#define MIN_BACKWARD            10
#define DEF_BACKWARD            100
#define MAX_BACKWARD            1000
#define MIN_BACKWARD_MENU       5
#define DEF_BACKWARD_MENU       10
#define MAX_BACKWARD_MENU       30

#define MIN_DIST_DRAG           8
#define DEF_DIST_DRAG           8
#define MAX_DIST_DRAG           100

#define MIN_EXPAND_FOLDER_MSEC  500
#define DEF_EXPAND_FOLDER_MSEC  1000
#define MAX_EXPAND_FOLDER_MSEC  10000

#define MIN_DRAG_SCROLL_MSEC    100
#define DEF_DRAG_SCROLL_MSEC    200
#define MAX_DRAG_SCROLL_MSEC    5000

#define MIN_FLD_SEL_DELAY_MSEC  0
#define DEF_FLD_SEL_DELAY_MSEC  700
#define MAX_FLD_SEL_DELAY_MSEC  5000

#define MAX_WORKING_FOLDER      5

#define MAX_FONT_TEXT           0xff

#define MIN_THUMB_SIZE          50
#define DEF_THUMB_SIZE          100
#define MAX_THUMB_SIZE          300

#define DEF_FOLDER_PANE_SIZE    200

#define MIN_FOLDER_TREE_HEIGHT  10
#define MAX_FOLDER_TREE_HEIGHT  50

#define MAX_CLIP_SEPARATOR      30

#define MAX_ACCEL               200

#define MAX_VIEW_SPLIT_ROW      2
#define MAX_VIEW_SPLIT_COLUMN   3
#define MAX_VIEW_SPLIT          (MAX_VIEW_SPLIT_ROW * MAX_VIEW_SPLIT_COLUMN)
#define DEF_VIEW_SPLIT_ROW      1
#define DEF_VIEW_SPLIT_COLUMN   1
#define SPLITTER_SIZE           2

enum
{
    RENAME_EXT_TYPE_DEFAULT = 0,
    RENAME_EXT_TYPE_KEEP,
    RENAME_EXT_TYPE_SEL_EXCEPT_FOR_EXT,
};

enum
{
    MOUSE_ONE_CLICK = 0,
    MOUSE_DOUBLE_CLICK,
};

enum
{
    EXT_TYPE_HIDE = 0,
    EXT_TYPE_ALWAYS,
    EXT_TYPE_KNOWN,
};

enum
{
    SINGLE_RENAME_TYPE_DEFAULT = 0,
    SINGLE_RENAME_TYPE_BY_POPUP,
    SINGLE_RENAME_TYPE_BATCH_RENAME,
};

enum
{
    FOLDER_TREE_CUSTOM_NONE = 0,
    FOLDER_TREE_CUSTOM_EXPLORER,
    FOLDER_TREE_CUSTOM_USER_DEFINED,
};

enum
{
    LIST_TYPE_ALL = 0,
    LIST_TYPE_FOLDER,
    LIST_TYPE_FILE,
};

enum
{
    CASE_TYPE_ORIGINAL = 0,
    CASE_TYPE_UPPER,
    CASE_TYPE_LOWER,
};

enum
{
    CONTENTS_NONE = 0,
    CONTENTS_EXPLORER,
    CONTENTS_BASIC,
};

enum
{
    INIT_TYPE_NONE = 0,
    INIT_TYPE_INIT_FOLDER,
    INIT_TYPE_LAST_FOLDER,
};

enum
{
    FILTER_COLOR_TYPE_NONE = 0,
    FILTER_COLOR_TYPE_TEXT,
    FILTER_COLOR_TYPE_BKGND,
};

enum
{
    NAME_CASE_TYPE_DEFAULT = 0,
    NAME_CASE_TYPE_UPPER,
    NAME_CASE_TYPE_LOWER,
};

enum
{
    SAVE_VIEW_SET_NONE = 0,
    SAVE_VIEW_SET_DEFAULT,
    SAVE_VIEW_SET_SAME_BETWEEN_SPLIT,
    SAVE_VIEW_SET_EACH_FOLDER,
};

enum
{
    VIEW_STYLE_ICON = 0,
    VIEW_STYLE_SMALL_ICON,
    VIEW_STYLE_LIST,
    VIEW_STYLE_REPORT,
    VIEW_STYLE_THUMBNAIL,
};

enum
{
    SIZE_UNIT_DEFAULT = 0,
    SIZE_UNIT_AUTO,
    SIZE_UNIT_CUSTOM,
    SIZE_UNIT_NONE,
    SIZE_UNIT_BYTE = 10,
    SIZE_UNIT_KB,
    SIZE_UNIT_MB,
    SIZE_UNIT_GB,
    SIZE_UNIT_TB,
    SIZE_UNIT_PB,
    SIZE_UNIT_EB,
    SIZE_UNIT_ZB,
    SIZE_UNIT_YB,
};

enum
{
    DRAG_START_NONE = 0,
    DRAG_START_DEFAULT,
    DRAG_START_CTRL,
    DRAG_START_DIST,
};

enum
{
    DROP_NONE = 0,
    DROP_DEFAULT,
};

enum
{
    DRAG_FILE_OP_DEFAULT = 0,
    DRAG_FILE_OP_MOVE,
    DRAG_FILE_OP_COPY,
    DRAG_FILE_OP_LINK,
};

typedef std::deque<std::tstring> TabPathDeque;

class Option
{
    friend class OptionMgr;

public:
    Option(void);
    virtual ~Option(void);

public:
    void initDefault(void);

    xpr_bool_t loadFromFile(const xpr_tchar_t *aPath);
    xpr_bool_t saveToFile(const xpr_tchar_t *aPath, xpr_bool_t aMainOption = XPR_FALSE);

public:
    // main
    CRect           mWindow;
    xpr_sint_t      mCmdShow;
    xpr_tchar_t     mLanguage[0xff];
    xpr_bool_t      mSingleFolderPaneMode;
    xpr_bool_t      mShowEachFolderPane[MAX_VIEW_SPLIT];
    xpr_bool_t      mShowSingleFolderPane;
    xpr_sint_t      mSingleFolderPaneSize;
    xpr_sint_t      mEachFolderPaneSize[MAX_VIEW_SPLIT];
    xpr_bool_t      mLeftSingleFolderPane;
    xpr_bool_t      mLeftEachFolderPane[MAX_VIEW_SPLIT];
    xpr_bool_t      mBookmarkBarText;
    xpr_bool_t      mBookmarkBarMultiLine;
    xpr_bool_t      mDriveBarMultiLine;
    xpr_bool_t      mDriveBar;
    xpr_bool_t      mDriveBarShortText;
    xpr_bool_t      mDriveBarViewSplit;
    xpr_bool_t      mLeftDriveBarViewSplit;
    xpr_sint_t      mViewSplitRowCount;
    xpr_sint_t      mViewSplitColumnCount;
    xpr_double_t    mViewSplitRatio[MAX_VIEW_SPLIT_COLUMN + MAX_VIEW_SPLIT_ROW - 2];
    xpr_sint_t      mViewSplitSize[MAX_VIEW_SPLIT_COLUMN + MAX_VIEW_SPLIT_ROW - 2];
    xpr_bool_t      mActivateBar[MAX_VIEW_SPLIT];
    xpr_bool_t      mAddressBar[MAX_VIEW_SPLIT];
    xpr_bool_t      mPathBar[MAX_VIEW_SPLIT];
    xpr_bool_t      mViewStyle[MAX_VIEW_SPLIT];
    xpr_tchar_t     mLastFolder[MAX_VIEW_SPLIT][XPR_MAX_PATH * 2 + 1];
    xpr_tchar_t     mWorkingFolder[MAX_WORKING_FOLDER][XPR_MAX_PATH * 2 + 1];
    xpr_bool_t      mFileScrapDrop;
    xpr_bool_t      mPicViewer;
    xpr_bool_t      mTipOfTheToday;

    struct ViewSplitTab
    {
        TabPathDeque mTabPathDeque;
        xpr_size_t   mCurTab;
    };

    ViewSplitTab mViewSplitTab[MAX_VIEW_SPLIT];

    // general
    xpr_sint_t      mMouseClick;
    xpr_bool_t      mShowHiddenAttribute;
    xpr_bool_t      mShowSystemAttribute;
    xpr_bool_t      mTitleFullPath;
    xpr_bool_t      mAddressFullPath;
    xpr_bool_t      mTooltip;
    xpr_bool_t      mTooltipWithFileName;
    xpr_bool_t      mGridLines;
    xpr_bool_t      mFullRowSelect;
    xpr_sint_t      mFileExtType;
    xpr_sint_t      mRenameExtType;
    xpr_sint_t      mSingleRenameType;
    xpr_bool_t      mBatchRenameMultiSel;
    xpr_bool_t      mBatchRenameWithFolder;
    xpr_bool_t      mRenameByMouse;
    xpr_bool_t      mNewItemDlg;

    // folder tree
    xpr_sint_t      mFolderTreeCustomFolder[MAX_VIEW_SPLIT];
    xpr_uint_t      mFolderTreeBkgndColor[MAX_VIEW_SPLIT];
    xpr_uint_t      mFolderTreeTextColor[MAX_VIEW_SPLIT];
    xpr_bool_t      mFolderTreeCustomFont;
    xpr_tchar_t     mFolderTreeCustomFontText[MAX_FONT_TEXT];
    xpr_bool_t      mFolderTreeIsItemHeight;
    xpr_sint_t      mFolderTreeItemHeight;
    xpr_bool_t      mFolderTreeHighlight[MAX_VIEW_SPLIT];
    xpr_uint_t      mFolderTreeHighlightColor[MAX_VIEW_SPLIT];
    xpr_bool_t      mFolderTreeInitNoExpand;
    xpr_bool_t      mFolderTreeSelDelay;
    xpr_sint_t      mFolderTreeSelDelayTime;
    xpr_bool_t      mSingleFolderTreeLinkage;

    // contents
    xpr_sint_t      mContentsStyle;
    xpr_bool_t      mContentsTooltip;
    xpr_bool_t      mContentsBookmark;
    xpr_uint_t      mContentsBookmarkColor;
    xpr_bool_t      mContentsBookmarkExpandFolder;
    xpr_bool_t      mContentsBookmarkRealPath;
    xpr_bool_t      mContentsBookmarkFastNetIcon;
    xpr_bool_t      mContentsNoDispInfo;
    xpr_bool_t      mContentsARHSAttribute;

    // explorer
    xpr_bool_t      mExplorerCustomFolder[MAX_VIEW_SPLIT];
    xpr_tchar_t     mExplorerBkgndFile[MAX_VIEW_SPLIT][XPR_MAX_PATH + 1];
    xpr_uint_t      mExplorerBkgndColor[MAX_VIEW_SPLIT];
    xpr_uint_t      mExplorerTextColor[MAX_VIEW_SPLIT];
    xpr_sint_t      mExplorerInitFolderType[MAX_VIEW_SPLIT];
    xpr_tchar_t     mExplorerInitFolder[MAX_VIEW_SPLIT][XPR_MAX_PATH + 1];
    xpr_bool_t      mExplorerNoNetLastFolder[MAX_VIEW_SPLIT];
    xpr_sint_t      mExplorerSizeUnit;
    xpr_sint_t      mExplorerListType;
    xpr_bool_t      mExplorerParentFolder;
    xpr_bool_t      mExplorerGoUpSelSubFolder;
    xpr_bool_t      mExplorerCustomIcon;
    xpr_tchar_t     mExplorerCustomIconFile[MAX_VIEW_SPLIT][XPR_MAX_PATH + 1];
    xpr_sint_t      mExplorerFilterColorType;
    xpr_bool_t      mExplorer24HourTime;
    xpr_bool_t      mExplorer2YearDate;
    xpr_bool_t      mExplorerShowDrive;
    xpr_bool_t      mExplorerShowDriveItem;
    xpr_bool_t      mExplorerShowDriveSize;
    xpr_sint_t      mExplorerNameCaseType;
    xpr_bool_t      mExplorerCreateAndEditText;
    xpr_bool_t      mExplorerAutoColumnWidth;
    xpr_sint_t      mExplorerSaveViewSet;
    xpr_bool_t      mExplorerSaveViewStyle;
    xpr_sint_t      mExplorerDefaultViewStyle;
    xpr_sint_t      mExplorerDefaultSort;
    xpr_sint_t      mExplorerDefaultSortOrder;
    xpr_bool_t      mExplorerNoSort;
    xpr_bool_t      mExplorerExitVerifyViewSet;

    // thumbnail
    xpr_sint_t      mThumbnailWidth;
    xpr_sint_t      mThumbnailHeight;
    xpr_bool_t      mThumbnailSaveCache;
    xpr_sint_t      mThumbnailPriority;
    xpr_bool_t      mThumbnailLoadByExt;

    // display
    xpr_bool_t      mCustomFont;
    xpr_tchar_t     mCustomFontText[MAX_FONT_TEXT];
    xpr_bool_t      mAnimationMenu;
    xpr_bool_t      mStandardMenu;
    xpr_uint_t      mActiveViewColor;

    // path bar
    xpr_bool_t      mPathBarRealPath;
    xpr_bool_t      mPathBarIcon;
    xpr_bool_t      mPathBarHighlight;
    xpr_uint_t      mPathBarHighlightColor;

    // general functions
    xpr_bool_t      mDriveLastFolder;
    xpr_bool_t      mDriveShiftKey;
    xpr_bool_t      mAddressBarUrl;
    xpr_bool_t      mSearchResultClearOnClose;
    xpr_bool_t      mShellNewMenu;
    xpr_bool_t      mFileListCurDir;
    xpr_bool_t      mWorkingFolderRealPath;
    xpr_tchar_t     mClipboardSeparator[MAX_CLIP_SEPARATOR + 1];

    // file scrap
    xpr_bool_t      mFileScrapContextMenu;
    xpr_bool_t      mFileScrapSave;

    // advanced options
    xpr_bool_t      mSingleInstance;
    xpr_bool_t      mConfirmExit;
    xpr_bool_t      mRecentFile;
    xpr_bool_t      mRegShellContextMenu;

    // tray
    xpr_bool_t      mTray;
    xpr_bool_t      mTrayOnClose;
    xpr_bool_t      mTrayRestoreInitFolder;
    xpr_bool_t      mTrayOnMinmize;
    xpr_bool_t      mTrayOneClick;

    // tab
    xpr_bool_t      mTabSave;

    // view split
    xpr_bool_t      mViewSplitByRatio;
    xpr_bool_t      mSingleViewActivatePath;
    xpr_bool_t      mSplitViewTabKey;
    xpr_bool_t      mSplitLastFolder;

    // file operation
    xpr_bool_t      mExternalCopyFileOp;
    xpr_bool_t      mExternalMoveFileOp;
    xpr_bool_t      mExternalDeleteFileOp;
    xpr_bool_t      mFileOpCompleteFlash;

    // size unit format
    xpr_sint_t      mSingleSelFileSizeUnit;
    xpr_sint_t      mMultiSelFileSizeUnit;

    // automatic refresh
    xpr_bool_t      mNoRefresh;
    xpr_bool_t      mRefreshSort;

    // history
    xpr_bool_t      mSaveHistory;
    xpr_sint_t      mHistoryCount;
    xpr_sint_t      mBackwardCount;
    xpr_sint_t      mHistoryMenuCount;
    xpr_sint_t      mBackwardMenuCount;

    // drag & drop
    xpr_sint_t      mDragType;
    xpr_sint_t      mDragDist;
    xpr_sint_t      mDropType;
    xpr_sint_t      mDragFolderTreeExpandTime;
    xpr_sint_t      mDragScrollTime;
    xpr_sint_t      mDragDefaultFileOp;
    xpr_bool_t      mDragNoContents;

    // global hotkey
    xpr_bool_t      mLauncher;
    xpr_bool_t      mLauncherTray;
    xpr_bool_t      mLauncherWinStartup;
    xpr_ushort_t    mLauncherGlobalHotKey;
};

#endif // __FX_OPTION_H__
