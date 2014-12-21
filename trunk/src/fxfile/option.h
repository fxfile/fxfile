//
// Copyright (c) 2001-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_OPTION_H__
#define __FXFILE_OPTION_H__ 1
#pragma once

#include "fxfile_def.h"

class OptionObserver;

namespace fxfile
{
namespace base
{
class ConfFileEx;
} // namespace base
} // namespace fxfile

namespace fxfile
{
class History;
class FolderLayout;

class Option
{
    friend class OptionManager;

public:
    Option(void);
    virtual ~Option(void);

public:
    void setObserver(OptionObserver *aObserver);

public:
    void initDefault(void);
    void initDefaultConfigOption(void);

    void loadMainOption(fxfile::base::ConfFileEx &aConfFile);
    void saveMainOption(fxfile::base::ConfFileEx &aConfFile) const;

    void loadConfigOption(fxfile::base::ConfFileEx &aConfFile);
    void saveConfigOption(fxfile::base::ConfFileEx &aConfFile) const;

public:
    struct Config;

    void copyConfig(Config &aConfig) const;
    void setConfig(const Config &aConfig);
    void notifyConfig(void);

public:
    static struct Main
    {
        CRect           mWindowRect;
        xpr_sint_t      mWindowStatus;
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
        xpr_sint_t      mViewSplitRowCount;
        xpr_sint_t      mViewSplitColumnCount;
        xpr_double_t    mViewSplitRatio[MAX_VIEW_SPLIT_COLUMN + MAX_VIEW_SPLIT_ROW - 2];
        xpr_sint_t      mViewSplitSize[MAX_VIEW_SPLIT_COLUMN + MAX_VIEW_SPLIT_ROW - 2];
        xpr_tchar_t     mWorkingFolder[MAX_WORKING_FOLDER][XPR_MAX_PATH * 2 + 1];
        xpr_bool_t      mFileScrapDrop;
        xpr_bool_t      mPictureViewer;
        xpr_bool_t      mTipOfTheToday;

        void clearView(void);

        struct Tab;
        struct View;
        typedef std::list <xpr::string> HistoryList;
        typedef std::deque<Tab       *> TabDeque;
        typedef std::deque<View      *> ViewDeque;

        struct Tab
        {
            Tab(void);
            ~Tab(void);

            void clearFolderLayout(void);

            xpr::string mPath;

            HistoryList mBackwardList;
            HistoryList mForwardList;
            HistoryList mHistoryList;

            FolderLayout *mDefaultFolderLayout;
            FolderLayout *mComputerFolderLayout;
            FolderLayout *mVirtualFolderLayout;
        };

        struct View
        {
            View(void);
            ~View(void);

            void clear(void);

            TabDeque   mTabDeque;
            xpr_size_t mCurTab;
        };

        View mView[MAX_VIEW_SPLIT];
    } mMain;

    static struct Config
    {
        // general
        xpr_sint_t      mMouseClick;
        xpr_bool_t      mShowHiddenAttribute;
        xpr_bool_t      mShowSystemAttribute;
        xpr_bool_t      mTitleFullPath;
        xpr_bool_t      mAddressFullPath;
        xpr_bool_t      mTooltip;
        xpr_bool_t      mTooltipWithFileName;
        xpr_sint_t      mFileExtType;
        xpr_sint_t      mRenameExtType;
        xpr_sint_t      mSingleRenameType;
        xpr_bool_t      mBatchRenameMultiSel;
        xpr_bool_t      mBatchRenameWithFolder;
        xpr_bool_t      mRenameByMouse;
        xpr_bool_t      mNewItemDlg;

        xpr_tchar_t     mLanguage[0xff]; // new

        // folder tree
        xpr_uint_t      mFolderTreeBkgndColorType[MAX_VIEW_SPLIT];
        xpr_uint_t      mFolderTreeBkgndColor[MAX_VIEW_SPLIT];
        xpr_uint_t      mFolderTreeTextColorType[MAX_VIEW_SPLIT];
        xpr_uint_t      mFolderTreeTextColor[MAX_VIEW_SPLIT];
        xpr_bool_t      mFolderTreeCustomFont;
        xpr_tchar_t     mFolderTreeCustomFontText[MAX_FONT_TEXT + 1];
        xpr_bool_t      mFolderTreeIsItemHeight;
        xpr_sint_t      mFolderTreeItemHeight;
        xpr_bool_t      mFolderTreeHighlight[MAX_VIEW_SPLIT];
        xpr_uint_t      mFolderTreeHighlightColor[MAX_VIEW_SPLIT];
        xpr_bool_t      mFolderTreeInitNoExpand;
        xpr_bool_t      mFolderTreeSelDelay;
        xpr_sint_t      mFolderTreeSelDelayTime;
        xpr_bool_t      mFolderTreeLinkage;
        xpr_bool_t      mFolderTreeLinkageOnSingle;
        xpr_bool_t      mFolderTreeClassicThemeStyle;

        // layout
        xpr_bool_t      mShowAddressBar;
        xpr_bool_t      mShowPathBar;
        xpr_bool_t      mShowEachDriveBar;
        xpr_bool_t      mDriveBarLeftSide;
        xpr_bool_t      mDriveBarShortText;
        xpr_bool_t      mShowActivateBar;
        xpr_bool_t      mShowInfoBar;
        xpr_bool_t      mShowStatusBar;
        xpr_sint_t      mContentsStyle;
        xpr_bool_t      mContentsBookmark;
        xpr_uint_t      mContentsBookmarkColor;
        xpr_bool_t      mContentsNoDispDetailedInfo;
        xpr_bool_t      mContentsARHSAttribute;

        xpr_bool_t      mBookmarkTooltip;
        xpr_bool_t      mBookmarkExpandFolder;
        xpr_bool_t      mBookmarkRealPath;
        xpr_bool_t      mBookmarkFastNetIcon;

        // file list
        xpr_bool_t      mFileListBkgndImage[MAX_VIEW_SPLIT];
        xpr_tchar_t     mFileListBkgndImagePath[MAX_VIEW_SPLIT][XPR_MAX_PATH + 1];
        xpr_sint_t      mFileListBkgndColorType[MAX_VIEW_SPLIT];
        xpr_uint_t      mFileListBkgndColor[MAX_VIEW_SPLIT];
        xpr_sint_t      mFileListTextColorType[MAX_VIEW_SPLIT];
        xpr_uint_t      mFileListTextColor[MAX_VIEW_SPLIT];
        xpr_sint_t      mFileListInitFolderType[MAX_VIEW_SPLIT];
        xpr_tchar_t     mFileListInitFolder[MAX_VIEW_SPLIT][XPR_MAX_PATH + 1];
        xpr_bool_t      mFileListNoNetLastFolder[MAX_VIEW_SPLIT];
        xpr_sint_t      mFileListSizeUnit;
        xpr_sint_t      mFileListListType;
        xpr_bool_t      mFileListParentFolder;
        xpr_bool_t      mFileListGoUpSelSubFolder;
        xpr_bool_t      mFileListCustomIcon;
        xpr_tchar_t     mFileListCustomIconFile16[XPR_MAX_PATH + 1];
        xpr_tchar_t     mFileListCustomIconFile32[XPR_MAX_PATH + 1];
        xpr_tchar_t     mFileListCustomIconFile48[XPR_MAX_PATH + 1];
        xpr_tchar_t     mFileListCustomIconFile256[XPR_MAX_PATH + 1];
        xpr_bool_t      mFileList24HourTime;
        xpr_bool_t      mFileList2YearDate;
        xpr_bool_t      mFileListShowDrive;
        xpr_bool_t      mFileListShowDriveItem;
        xpr_bool_t      mFileListShowDriveSize;
        xpr_sint_t      mFileListNameCaseType;
        xpr_bool_t      mFileListCreateAndEditText;
        xpr_bool_t      mFileListAutoColumnWidth;
        xpr_sint_t      mFileListSaveFolderLayout;
        xpr_sint_t      mFileListDefaultViewStyle;
        xpr_sint_t      mFileListDefaultSort;
        xpr_sint_t      mFileListDefaultSortOrder;
        xpr_bool_t      mFileListNoSort;
        xpr_bool_t      mFileListExitVerifyFolderLayout;
        xpr_bool_t      mFileListClassicThemeStyle;
        xpr_bool_t      mFileListGridLines;
        xpr_bool_t      mFileListFullRowSelect;

        // thumbnail
        xpr_sint_t      mThumbnailWidth;
        xpr_sint_t      mThumbnailHeight;
        xpr_bool_t      mThumbnailSaveCache;
        xpr_sint_t      mThumbnailPriority;
        xpr_bool_t      mThumbnailLoadByExt;

        // display
        xpr_bool_t      mCustomFont;
        xpr_tchar_t     mCustomFontText[MAX_FONT_TEXT + 1];
        xpr_bool_t      mAnimationMenu;
        xpr_bool_t      mStandardMenu;
        xpr_uint_t      mActiveViewColor;

        // tab
        xpr_bool_t      mTabSwitchToNewTab;
        xpr_bool_t      mTabCloseOnDoubleClick;
        xpr_bool_t      mTabAutoFit;
        xpr_bool_t      mTabShowOneTab;
        xpr_bool_t      mTabConfirmToClose;
        xpr_bool_t      mTabConfirmToCloseAll;
        xpr_bool_t      mTabNearOpenNewTab;
        xpr_bool_t      mTabIcon;
        xpr_bool_t      mTabShowDrive;
        xpr_bool_t      mTabShowNewButton;
        xpr_bool_t      mTabDragMove;

        // path bar
        xpr_bool_t      mPathBarRealPath;
        xpr_bool_t      mPathBarIcon;
        xpr_bool_t      mPathBarHighlight;
        xpr_uint_t      mPathBarHighlightColor;

        // general functions
        xpr_bool_t      mDriveLastFolder;
        xpr_bool_t      mDriveShiftKey;
        xpr_bool_t      mAddressBarUrl;
        xpr_bool_t      mShellNewMenu;
        xpr_bool_t      mFileListCurDir;
        xpr_bool_t      mWorkingFolderRealPath;
        xpr_tchar_t     mClipboardSeparator[MAX_CLIP_SEPARATOR + 1];

        // file scrap
        xpr_bool_t      mFileScrapContextMenu;
        xpr_bool_t      mFileScrapSave;

        // advanced options
        xpr_bool_t      mSingleProcess;
        xpr_bool_t      mConfirmExit;
        xpr_bool_t      mRecentFile;
        xpr_bool_t      mRegShellContextMenu;

        // tray
        xpr_bool_t      mTray;
        xpr_bool_t      mTrayOnClose;
        xpr_bool_t      mTrayRestoreInitFolder;
        xpr_bool_t      mTrayOnMinmize;
        xpr_bool_t      mTrayOneClick;

        // view split
        xpr_bool_t      mViewSplitByRatio;
        xpr_bool_t      mSingleViewSplitAsActivedView;
        xpr_bool_t      mViewSplitReopenLastFolder;

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

        // update check
        xpr_bool_t      mUpdateCheckEnable;
    } mConfig;

protected:
    OptionObserver *mObserver;
};
} // namespace fxfile

#endif // __FXFILE_OPTION_H__
