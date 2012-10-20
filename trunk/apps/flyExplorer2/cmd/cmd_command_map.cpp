//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_command_map.h"

#include "cmd_executor.h"

#include "resource.h"
#include "cmd_create_item.h"
#include "cmd_file_oper.h"
#include "cmd_rename.h"
#include "cmd_pic_conv.h"
#include "cmd_attr_time.h"
#include "cmd_file_property.h"
#include "cmd_execute.h"
#include "cmd_file_ass.h"
#include "cmd_checksum.h"
#include "cmd_file_split.h"
#include "cmd_file_list.h"
#include "cmd_send_mail.h"
#include "cmd_print.h"
#include "cmd_recent.h"
#include "cmd_app.h"
#include "cmd_undo.h"
#include "cmd_clipboard.h"
#include "cmd_file_scrap.h"
#include "cmd_select_file.h"
#include "cmd_drive.h"
#include "cmd_bookmark.h"
#include "cmd_go_path.h"
#include "cmd_folder_sync.h"
#include "cmd_working_folder.h"
#include "cmd_history.h"
#include "cmd_main_gui.h"
#include "cmd_pic_viewer.h"
#include "cmd_style.h"
#include "cmd_sort.h"
#include "cmd_column.h"
#include "cmd_refresh.h"
#include "cmd_tools.h"
#include "cmd_network.h"
#include "cmd_cfg.h"
#include "cmd_tab.h"
#include "cmd_window.h"
#include "cmd_help.h"
#include "cmd_tray.h"
#include "cmd_search.h"
#include "cmd_search_result.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
void CommandMap::map(CommandExecutor &aExecutor)
{
    //
    // File Menu
    //
    aExecutor.bindCommand(ID_FILE_NEW_FOLDER,                  new cmd::CreateFolderCommand);
    aExecutor.bindCommand(ID_FILE_NEW_FILE,                    new cmd::CreateGeneralFileCommand);
    aExecutor.bindCommand(ID_FILE_NEW_TEXT,                    new cmd::CreateTextFileCommand);
    aExecutor.bindCommand(ID_FILE_NEW_LINK,                    new cmd::CreateShortcutCommand);
    aExecutor.bindCommand(ID_FILE_NEW_ITEMS,                   new cmd::CreateBatchCommand);
    aExecutor.bindCommand(
        ID_FILE_NEW_FIRST, ID_FILE_NEW_LAST,                   new cmd::FileNewCommand);

    aExecutor.bindCommand(ID_FILE_LINK,                        new cmd::FileLinkCommand);
    aExecutor.bindCommand(ID_FILE_DUPLICATE,                   new cmd::FileDuplicateCommand);
    aExecutor.bindCommand(ID_FILE_DELETE,                      new cmd::FileDeleteCommand);

    aExecutor.bindCommand(ID_FILE_RENAME,                      new cmd::RenameCommand);
    aExecutor.bindCommand(ID_FILE_BATCH_RENAME_ALL,            new cmd::BatchRenameCommand);
    aExecutor.bindCommand(ID_FILE_BATCH_RENAME_FILE,           new cmd::BatchOnlyFileRenameCommand);
    aExecutor.bindCommand(ID_FILE_BATCH_RENAME_FOLDER,         new cmd::BatchOnlyFolderRenameCommand);

    aExecutor.bindCommand(ID_FILE_IMAGE_CONV,                  new cmd::PicConvCommand);
    aExecutor.bindCommand(ID_FILE_ATTRTIME,                    new cmd::AttrTimeCommand);
    aExecutor.bindCommand(ID_FILE_PROPERTY,                    new cmd::FilePropertyCommand);

    aExecutor.bindCommand(ID_FILE_EXEC,                        new cmd::ExecuteCommand);
    aExecutor.bindCommand(ID_FILE_EXEC_PARAM,                  new cmd::ParamExecuteCommand);

    aExecutor.bindCommand(ID_FILE_VIEW,                        new cmd::FileViewCommand);
    aExecutor.bindCommand(ID_FILE_EDIT,                        new cmd::FileEditCommand);
    aExecutor.bindCommand(ID_FILE_VIEWER_ASS,                  new cmd::FileViewerAssCommand);
    aExecutor.bindCommand(ID_FILE_EDITOR_ASS,                  new cmd::FileEditorAssCommand);
    aExecutor.bindCommand(ID_FILE_SHELL_ASS,                   new cmd::FileShellAssCommand);

    aExecutor.bindCommand(ID_FILE_CRC_CREATE,                  new cmd::ChecksumCreateCommand);
    aExecutor.bindCommand(ID_FILE_CRC_VERIFY,                  new cmd::ChecksumVerifyCommand);

    aExecutor.bindCommand(ID_FILE_SPLIT,                       new cmd::FileSplitCommand);
    aExecutor.bindCommand(ID_FILE_COMBINE,                     new cmd::FileCombineCommand);
    aExecutor.bindCommand(ID_FILE_MERGE,                       new cmd::FileMergeCommand);
    aExecutor.bindCommand(ID_FILE_TEXT_MERGE,                  new cmd::TextFileMergeCommand);

    aExecutor.bindCommand(ID_FILE_LIST_FILE,                   new cmd::FileListCommand);

    aExecutor.bindCommand(ID_FILE_SEND_MAIL,                   new cmd::SendMailCommand);

    aExecutor.bindCommand(ID_FILE_PRINT,                       new cmd::PrintCommand);
    aExecutor.bindCommand(ID_FILE_PRINT_PREVIEW,               new cmd::PrintPreviewCommand);
    aExecutor.bindCommand(ID_FILE_PRINT_SETUP,                 new cmd::PrintSetupCommand);

    aExecutor.bindCommand(
        ID_FILE_RECENT_FIRST, ID_FILE_RECENT_LAST,             new cmd::RecentFileCommand);

    aExecutor.bindCommand(ID_APP_MINIMIZE_TRAY,                new cmd::MinimizeTrayCommand);
    aExecutor.bindCommand(ID_APP_EXIT,                         new cmd::AppExitCommand);

    //
    // Edit Menu
    //
    aExecutor.bindCommand(ID_EDIT_UNDO,                        new cmd::UndoCommand);

    aExecutor.bindCommand(ID_EDIT_CUT,                         new cmd::ClipboardFileCutCommand);
    aExecutor.bindCommand(ID_EDIT_COPY,                        new cmd::ClipboardFileCopyCommand);
    aExecutor.bindCommand(ID_EDIT_PASTE,                       new cmd::ClipboardFilePasteCommand);
    aExecutor.bindCommand(ID_EDIT_PASTE_COPY,                  new cmd::ClipboardFilePasteCopyCommand);
    aExecutor.bindCommand(ID_EDIT_PASTE_LINK,                  new cmd::ClipboardFilePasteLinkCommand);
    aExecutor.bindCommand(ID_EDIT_PASTE_SPECIAL,               new cmd::ClipboardFileSpecialPasteCommand);

    aExecutor.bindCommand(ID_EDIT_NAME_COPY,                   new cmd::ClipboardNameCopyCommand);
    aExecutor.bindCommand(ID_EDIT_FILENAME_COPY,               new cmd::ClipboardFileNameCopyCommand);
    aExecutor.bindCommand(ID_EDIT_PATH_COPY,                   new cmd::ClipboardPathCopyCommand);
    aExecutor.bindCommand(ID_EDIT_DEV_PATH_COPY,               new cmd::ClipboardDevPathCopyCommand);
    aExecutor.bindCommand(ID_EDIT_URL_COPY,                    new cmd::ClipboardUrlCopyCommand);

    aExecutor.bindCommand(ID_EDIT_FOLDER_COPY,                 new cmd::FolderCopyCommand);
    aExecutor.bindCommand(ID_EDIT_FOLDER_MOVE,                 new cmd::FolderMoveCommand);

    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_ADD,              new cmd::FileScrapAddCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_CUR_COPY,         new cmd::FileScrapToCopyCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_CUR_MOVE,         new cmd::FileScrapToMoveCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_DELETE,           new cmd::FileScrapDeleteCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_CLIPBOARD_CUT,    new cmd::FileScrapClipboardCutCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_CLIPBOARD_COPY,   new cmd::FileScrapClipboardCopyCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_VIEW,             new cmd::FileScrapViewCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_GROUP_DEFAULT,    new cmd::FileScrapGroupDefaultCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_LIST_REMOVE,      new cmd::FileScrapListRemoveCommand);
    aExecutor.bindCommand(ID_EDIT_FILE_SCRAP_DROP,             new cmd::FileScrapDropCommand);

    aExecutor.bindCommand(ID_EDIT_SEL_NAME,                    new cmd::SelectNameCommand);
    aExecutor.bindCommand(ID_EDIT_UNSEL_NAME,                  new cmd::UnselectNameCommand);
    aExecutor.bindCommand(ID_EDIT_SEL_SAME_FILTER,             new cmd::SelectSameFilterCommand);
    aExecutor.bindCommand(ID_EDIT_SEL_SAME_EXT,                new cmd::SelectSameExtCommand);
    aExecutor.bindCommand(ID_EDIT_SEL_FILE,                    new cmd::SelectFileCommand);
    aExecutor.bindCommand(ID_EDIT_SEL_FOLDER,                  new cmd::SelectFolderCommand);
    aExecutor.bindCommand(ID_EDIT_UNSEL_FILE,                  new cmd::UnselectFileCommand);
    aExecutor.bindCommand(ID_EDIT_UNSEL_FOLDER,                new cmd::UnselectFolderCommand);
    aExecutor.bindCommand(ID_EDIT_SEL_FILTER,                  new cmd::SelectFilterCommand);
    aExecutor.bindCommand(ID_EDIT_UNSEL_FILTER,                new cmd::UnselectFilterCommand);
    aExecutor.bindCommand(ID_EDIT_SEL_ALL,                     new cmd::SelectAllCommand);
    aExecutor.bindCommand(ID_EDIT_UNSEL_ALL,                   new cmd::UnselectAllCommand);
    aExecutor.bindCommand(ID_EDIT_SEL_REVERSAL,                new cmd::SelectReversalCommand);

    aExecutor.bindCommand(ID_EDIT_SEARCH_LOCATION,             new cmd::SearchLocationCommand);

    //
    // Go Menu
    //
    aExecutor.bindCommand(ID_GO_DRIVE,                         new cmd::DrivePopupCommand);
    aExecutor.bindCommand(ID_DRIVE_FIRST, ID_DRIVE_LAST,       new cmd::GoDriveCommand);
    aExecutor.bindCommand(ID_GO_DRIVE_DETAIL,                  new cmd::DriveInfoCommand);

    aExecutor.bindCommand(ID_GO_BOOKMARK,                      new cmd::BookmarkPopupCommand);
    aExecutor.bindCommand(ID_BOOKMARK_FIRST, ID_BOOKMARK_LAST, new cmd::GoBookmarkCommand);
    aExecutor.bindCommand(ID_GO_BOOKMARK_ADD,                  new cmd::BookmarkAddCommand);
    aExecutor.bindCommand(ID_GO_BOOKMARK_MODIFY,               new cmd::BookmarkModifyCommand);
    aExecutor.bindCommand(ID_VIEW_BOOKMARKBAR_REFRESH,         new cmd::BookmarkRefreshCommand);

    aExecutor.bindCommand(ID_GO_WORKING_FOLDER,                new cmd::WorkingFolderPopupCommand);
    aExecutor.bindCommand(
        ID_GO_WORKING_FOLDER_FIRST, ID_GO_WORKING_FOLDER_LAST, new cmd::GoWorkingFolderCommand);
    aExecutor.bindCommand(ID_GO_WORKING_FOLDER_RESET,          new cmd::WorkingFolderResetAllCommand);

    aExecutor.bindCommand(ID_GO_PATH,                          new cmd::GoPathCommand);
    aExecutor.bindCommand(ID_GO_INIT_FOLDER,                   new cmd::GoInitFolderCommand);
    aExecutor.bindCommand(ID_GO_INIT_FOLDER_1,                 new cmd::GoInitFolder1Command);
    aExecutor.bindCommand(ID_GO_INIT_FOLDER_2,                 new cmd::GoInitFolder2Command);
    aExecutor.bindCommand(ID_GO_INIT_FOLDER_3,                 new cmd::GoInitFolder3Command);
    aExecutor.bindCommand(ID_GO_INIT_FOLDER_4,                 new cmd::GoInitFolder4Command);
    aExecutor.bindCommand(ID_GO_INIT_FOLDER_5,                 new cmd::GoInitFolder5Command);
    aExecutor.bindCommand(ID_GO_INIT_FOLDER_6,                 new cmd::GoInitFolder6Command);

    aExecutor.bindCommand(ID_GO_DESKTOP,                       new cmd::GoSystemDesktopCommand);
    aExecutor.bindCommand(ID_GO_MYDOCUMENT,                    new cmd::GoSystemMyDocumentCommand);
    aExecutor.bindCommand(ID_GO_MYCOMPUTER,                    new cmd::GoSystemMyComputerCommand);
    aExecutor.bindCommand(ID_GO_NETWORK,                       new cmd::GoSystemNetworkCommand);
    aExecutor.bindCommand(ID_GO_CONTROLS,                      new cmd::GoSystemControlsCommand);
    aExecutor.bindCommand(ID_GO_RECYCLE,                       new cmd::GoSystemRecycleCommand);
    aExecutor.bindCommand(ID_GO_PROGRAMFILES,                  new cmd::GoSystemProgramFilesCommand);
    aExecutor.bindCommand(ID_GO_COMMON,                        new cmd::GoSystemCommonCommand);
    aExecutor.bindCommand(ID_GO_WINDOWS,                       new cmd::GoSystemWindowsCommand);
    aExecutor.bindCommand(ID_GO_SYSTEM,                        new cmd::GoSystemSystemCommand);
    aExecutor.bindCommand(ID_GO_FONTS,                         new cmd::GoSystemFontsCommand);
    aExecutor.bindCommand(ID_GO_STARTUP,                       new cmd::GoSystemStartupCommand);
    aExecutor.bindCommand(ID_GO_TEMPORARY,                     new cmd::GoSystemTemporaryCommand);
    aExecutor.bindCommand(ID_GO_FAVORITES,                     new cmd::GoSystemFavoritesCommand);
    aExecutor.bindCommand(ID_GO_INTERNETCACHE,                 new cmd::GoSystemInternetCacheCommand);
    aExecutor.bindCommand(ID_GO_INTERNETCOOKIE,                new cmd::GoSystemInternetCookieCommand);
    aExecutor.bindCommand(ID_GO_SENDTO,                        new cmd::GoSystemSendToCommand);
    aExecutor.bindCommand(ID_GO_RECENT,                        new cmd::GoSystemRecentCommand);
    aExecutor.bindCommand(ID_GO_APPDATA,                       new cmd::GoSystemAppDataCommand);

    aExecutor.bindCommand(ID_GO_BACK,                          new cmd::GoDirectBackwardCommand);
    aExecutor.bindCommand(ID_GO_FORWARD,                       new cmd::GoDirectForwardCommand);
    aExecutor.bindCommand(ID_GO_HISTORY,                       new cmd::GoDirectHistoryCommand);
    aExecutor.bindCommand(ID_GO_BACK_BOX,                      new cmd::GoBackwardBoxCommand);
    aExecutor.bindCommand(ID_GO_FORWARD_BOX,                   new cmd::GoForwardBoxCommand);
    aExecutor.bindCommand(ID_GO_HISTORY_BOX,                   new cmd::GoHistoryBoxCommand);
    aExecutor.bindCommand(ID_GO_BACK_CLEAR,                    new cmd::BackwardClearCommand);
    aExecutor.bindCommand(ID_GO_FORWARD_CLEAR,                 new cmd::ForwardClearCommand);
    aExecutor.bindCommand(ID_GO_HISTORY_CLEAR,                 new cmd::HistoryClearCommand);
    aExecutor.bindCommand(ID_GO_HISTORY_ALL_CLEAR,             new cmd::HistoryAllClearCommand);
    aExecutor.bindCommand(ID_GO_SIBLING_UP,                    new cmd::GoSiblingUpCommand);
    aExecutor.bindCommand(ID_GO_SIBLING_DOWN,                  new cmd::GoSiblingDownCommand);
    aExecutor.bindCommand(ID_GO_UP,                            new cmd::GoDirectUpCommand);
    aExecutor.bindCommand(ID_GO_ROOT,                          new cmd::GoRootCommand);
    aExecutor.bindCommand(ID_GO_UP_BOX,                        new cmd::GoUpBoxCommand);
    aExecutor.bindCommand(ID_GO_FLYEXPLORER,                   new cmd::GoFlyExplorerCommand);

    aExecutor.bindCommand(ID_GO_BACKWARD_FIRST, ID_GO_BACKWARD_LAST, new cmd::GoBackwardCommand);
    aExecutor.bindCommand(ID_GO_FORWARD_FIRST,  ID_GO_FORWARD_LAST,  new cmd::GoForwardCommand);
    aExecutor.bindCommand(ID_GO_HISTORY_FIRST,  ID_GO_HISTORY_LAST,  new cmd::GoHistoryCommand);
    aExecutor.bindCommand(ID_GO_UP_FIRST,       ID_GO_UP_LAST,       new cmd::GoUpCommand);

    //
    // View Menu
    //
    aExecutor.bindCommand(ID_VIEW_TOOLBAR,                     new cmd::ShowToolBarCommand);
    aExecutor.bindCommand(ID_VIEW_DRIVEBAR,                    new cmd::ShowDriveBarCommand);
    aExecutor.bindCommand(ID_VIEW_BOOKMARKBAR,                 new cmd::ShowBookmarkBarCommand);
    aExecutor.bindCommand(ID_VIEW_ADDRESSBAR,                  new cmd::ShowAddressBarCommand);
    aExecutor.bindCommand(ID_VIEW_PATHBAR,                     new cmd::ShowPathBarCommand);
    aExecutor.bindCommand(ID_VIEW_ACTIVATEWND,                 new cmd::ShowActivateWndCommand);

    aExecutor.bindCommand(ID_VIEW_ADDRESSBAR_SHOW,             new cmd::AddressBarDropCommand);

    aExecutor.bindCommand(ID_VIEW_DRIVEBAR_SHORTTEXT,          new cmd::DriveBarShortTextCommand);
    aExecutor.bindCommand(ID_VIEW_DRIVEBAR_LONGTEXT,           new cmd::DriveBarLongTextCommand);
    aExecutor.bindCommand(ID_VIEW_DRIVEBAR_WRAPABLE,           new cmd::DriveBarWrapableCommand);
    aExecutor.bindCommand(ID_VIEW_DRIVEBAR_VIEWSPLIT,          new cmd::DriveBarEachAssignCommand);
    aExecutor.bindCommand(ID_VIEW_DRIVEBAR_LEFT,               new cmd::DriveBarLeftPlaceCommand);
    aExecutor.bindCommand(ID_VIEW_DRIVEBAR_RIGHT,              new cmd::DriveBarRightPlaceCommand);
    aExecutor.bindCommand(ID_VIEW_DRIVEBAR_REFRESH,            new cmd::DriveBarRefreshCommand);

    aExecutor.bindCommand(ID_VIEW_BOOKMARKBAR_NONAME,          new cmd::BookmarkBarNoNameCommand);
    aExecutor.bindCommand(ID_VIEW_BOOKMARKBAR_NAME,            new cmd::BookmarkBarNameCommand);
    aExecutor.bindCommand(ID_VIEW_BOOKMARKBAR_WRAPABLE,        new cmd::BookmarkBarWrapableCommand);
    aExecutor.bindCommand(ID_VIEW_BOOKMARKBAR_REFRESH,         new cmd::BookmarkBarRefreshCommand);

    aExecutor.bindCommand(ID_VIEW_TOOL_LOCK,                   new cmd::ToolLockCommand);
    aExecutor.bindCommand(ID_VIEW_TOOLBAR_CUSTOMIZE,           new cmd::ToolBarCustomizeCommand);

    aExecutor.bindCommand(ID_VIEW_STATUS_BAR,                  new cmd::ShowStatusBarCommand);

    aExecutor.bindCommand(ID_VIEW_FOLDER_TREE_NONE,            new cmd::NoneFolderPaneCommand);
    aExecutor.bindCommand(ID_VIEW_FOLDER_TREE_SHOW,            new cmd::ShowFolderPaneCommand);
    aExecutor.bindCommand(ID_VIEW_FOLDER_TREE_SINGLE_PANE,     new cmd::SingleFolderPaneCommand);
    aExecutor.bindCommand(ID_VIEW_FOLDER_TREE_EACH_PANE,       new cmd::EachFolderPaneCommand);
    aExecutor.bindCommand(ID_VIEW_FOLDER_TREE_LEFT_PANE,       new cmd::LeftFolderPaneCommand);
    aExecutor.bindCommand(ID_VIEW_FOLDER_TREE_RIGHT_PANE,      new cmd::RightFolderPaneCommand);
    aExecutor.bindCommand(ID_VIEW_BAR_SEARCH,                  new cmd::ShowSearchBarCommand);

    aExecutor.bindCommand(ID_VIEW_PICVIEWER,                   new cmd::PicViewerCommand);

    aExecutor.bindCommand(ID_VIEW_PIC_ZOOM_IN,                 new cmd::PicViewerZoomInCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_ZOOM_OUT,                new cmd::PicViewerZoomOutCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_ZOOM_100,                new cmd::PicViewerZoom100Command);
    aExecutor.bindCommand(ID_VIEW_PIC_RATIO,                   new cmd::PicViewerRatioResizeCommand);

    aExecutor.bindCommand(ID_VIEW_PIC_DOCKING,                 new cmd::PicViewerDockingCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_AUTO_HIDE,               new cmd::PicViewerAutoHideCommand);

    aExecutor.bindCommand(ID_VIEW_PIC_LOCK,                    new cmd::PicViewerLockImageCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_CLIENT,                  new cmd::PicViewerLockWindowCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_LOCK_100,                new cmd::PicViewerLock100Command);
    aExecutor.bindCommand(ID_VIEW_PIC_LOCK_THUMBNAIL,          new cmd::PicViewerLockThumbnailCommand);

    aExecutor.bindCommand(ID_VIEW_PIC_SHOW_FILENAME,           new cmd::PicViewerShowFileNameCommand);

    aExecutor.bindCommand(ID_VIEW_PIC_BACK_CENTER,             new cmd::PicViewerBackgroundCenterCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_BACK_TILE,               new cmd::PicViewerBackgroundTitleCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_BACK_STRETCH,            new cmd::PicViewerBackgroundStretchCommand);

    aExecutor.bindCommand(ID_VIEW_PIC_SHOW_HIDE,               new cmd::PicViewerToggleCommand);
    aExecutor.bindCommand(ID_VIEW_PIC_HIDE,                    new cmd::PicViewerHideCommand);

    aExecutor.bindCommand(ID_VIEW_STYLE,                       new cmd::ViewStyleToolBarCommand);
    aExecutor.bindCommand(
        ID_VIEW_STYLE_LARGEICON, ID_VIEW_STYLE_THUMBNAIL,      new cmd::ViewStyleCommand);

    aExecutor.bindCommand(
        ID_VIEW_ARRAY_NAME, ID_VIEW_ARRAY_ATTR,                new cmd::SortCommand);
    aExecutor.bindCommand(ID_VIEW_ARRAY_EXT,                   new cmd::SortCommand);
    aExecutor.bindCommand(ID_VIEW_ARRAY_ASCENDING,             new cmd::SortAscendingCommand);
    aExecutor.bindCommand(ID_VIEW_ARRAY_DECENDING,             new cmd::SortDecendingCommand);

    aExecutor.bindCommand(
        ID_VIEW_COLUMN_NAME, ID_VIEW_COLUMN_ATTR,              new cmd::ColumnCommand);
    aExecutor.bindCommand(ID_VIEW_COLUMN_EXT,                  new cmd::ColumnCommand);
    aExecutor.bindCommand(ID_VIEW_COLUMN_SET,                  new cmd::ColumnSetCommand);

    aExecutor.bindCommand(ID_VIEW_REFRESH,                     new cmd::RefreshCommand);

    //
    // Tool Menu
    //
    aExecutor.bindCommand(ID_TOOL_EMPTY_RECYCLEBIN,            new cmd::EmptyRecycleBinCommand);
    aExecutor.bindCommand(ID_TOOL_WINDOWS_EXPLORER,            new cmd::WindowsExplorerCommand);
    aExecutor.bindCommand(ID_TOOL_CMD,                         new cmd::CmdCommand);
    aExecutor.bindCommand(ID_TOOL_DOS_CMD,                     new cmd::DosCmdCommand);
    aExecutor.bindCommand(ID_TOOL_FOLDER_SYNC,                 new cmd::FolderSyncCommand);
    aExecutor.bindCommand(ID_TOOL_SHARED_PROC,                 new cmd::SharedProcCommand);
    aExecutor.bindCommand(ID_TOOL_NETWORK_CONNECT,             new cmd::NetworkDriveConnectCommand);
    aExecutor.bindCommand(ID_TOOL_NETWORK_DISCONNECT,          new cmd::NetworkDriveDisconnectCommand);
    aExecutor.bindCommand(ID_TOOL_SAVE_OPTION,                 new cmd::SaveOptionCommand);
    aExecutor.bindCommand(ID_TOOL_ACCELERATOR,                 new cmd::AcceleratorCommand);
    aExecutor.bindCommand(ID_TOOL_OPTION,                      new cmd::OptionCommand);

    //
    // Window Menu
    //
    aExecutor.bindCommand(ID_WINDOW_TAB_NEW,                   new cmd::NewTabCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_DUPLICATE,             new cmd::DuplicateTabCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_DUPLICATE_ON_CURSOR,   new cmd::DuplicateTabOnCursorCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_FIRST,                 new cmd::FirstTabCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_PREVIOUS,              new cmd::PreviousTabCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_NEXT,                  new cmd::NextTabCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_LAST,                  new cmd::LastTabCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_CLOSE,                 new cmd::CloseTabCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_CLOSE_ON_CURSOR,       new cmd::CloseTabOnCursorCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_CLOSE_ALL_BUT_THIS,    new cmd::CloseAllTabsButThisCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_CLOSE_ALL_BUT_THIS_ON_CURSOR,
                                                               new cmd::CloseAllTabsButThisOnCursorCommand);
    aExecutor.bindCommand(ID_WINDOW_TAB_CLOSE_ALL,             new cmd::CloseAllTabsCommand);
    aExecutor.bindCommand(ID_WINDOW_NEXT,                      new cmd::WindowNextCommand);
    aExecutor.bindCommand(ID_WINDOW_PREV,                      new cmd::WindowPrevCommand);
    aExecutor.bindCommand(ID_WINDOW_SPLIT,                     new cmd::WindowSplitToolBarCommand);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_SINGLE,              new cmd::WindowSplitSingleCommand);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_1x2,                 new cmd::WindowSplit1x2Command);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_2x1,                 new cmd::WindowSplit2x1Command);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_2x2,                 new cmd::WindowSplit2x2Command);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_1x3,                 new cmd::WindowSplit1x3Command);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_2x3,                 new cmd::WindowSplit2x3Command);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_EQUALLY,             new cmd::WindowSplitEquallyCommand);
    aExecutor.bindCommand(
        ID_WINDOW_SPLIT_10, ID_WINDOW_SPLIT_90,                new cmd::WindowSplitRatioCommand);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_SWAP,                new cmd::WindowSwapFolderCommand);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_EQUAL,               new cmd::WindowEqualFolderCommand);
    aExecutor.bindCommand(ID_WINDOW_SPLIT_OTHER_EQUAL,         new cmd::WindowOtherEqualFolderCommand);
    aExecutor.bindCommand(ID_WINDOW_COPY,                      new cmd::WindowCopyCommand);
    aExecutor.bindCommand(ID_WINDOW_MOVE,                      new cmd::WindowMoveCommand);
    aExecutor.bindCommand(ID_WINDOW_1_COPY,                    new cmd::WindowCopy1Command);
    aExecutor.bindCommand(ID_WINDOW_1_MOVE,                    new cmd::WindowMove1Command);
    aExecutor.bindCommand(ID_WINDOW_2_COPY,                    new cmd::WindowCopy2Command);
    aExecutor.bindCommand(ID_WINDOW_2_MOVE,                    new cmd::WindowMove2Command);
    aExecutor.bindCommand(ID_WINDOW_DUAL_COPY,                 new cmd::WindowDualCopyCommand);
    aExecutor.bindCommand(ID_WINDOW_DUAL_MOVE,                 new cmd::WindowDualMoveCommand);
    aExecutor.bindCommand(ID_WINDOW_DUAL_DELETE,               new cmd::WindowDualDeleteCommand);
    aExecutor.bindCommand(ID_WINDOW_COMPARE,                   new cmd::WindowCompareCommand);

    //
    // Help Menu
    //
    aExecutor.bindCommand(ID_APP_HELP,                         new cmd::HelpCommand);
    aExecutor.bindCommand(ID_APP_HELP_FIND,                    new cmd::HelpFindCommand);
    aExecutor.bindCommand(ID_APP_ACCELERATOR,                  new cmd::HelpAcceleratorCommand);
    aExecutor.bindCommand(ID_APP_TIP,                          new cmd::TipOfTodayCommand);
    aExecutor.bindCommand(ID_APP_HOMEPAGE,                     new cmd::HompageCommand);
    aExecutor.bindCommand(ID_APP_EMAIL,                        new cmd::EmailCommand);
    aExecutor.bindCommand(ID_APP_ABOUT,                        new cmd::AboutCommand);

    //
    // Tray Menu
    //
    aExecutor.bindCommand(ID_VIEW_SHOW_MAINFRAME,              new cmd::TrayShowMainFrameCommand);
    aExecutor.bindCommand(ID_TRAY_APP_EXIT,                    new cmd::TrayAppExitCommand);

    //
    // Search Result Menu
    //
    aExecutor.bindCommand(ID_SR_VIEW_ICON,                     new cmd::SearchResultViewIconCommand);
    aExecutor.bindCommand(ID_SR_VIEW_SMALLICON,                new cmd::SearchResultViewSmallIconCommand);
    aExecutor.bindCommand(ID_SR_VIEW_LIST,                     new cmd::SearchResultViewListCommand);
    aExecutor.bindCommand(ID_SR_VIEW_DETAIL,                   new cmd::SearchResultViewDetailCommand);
    aExecutor.bindCommand(ID_SR_REMOVE_LIST,                   new cmd::SearchResultRemoveListCommand);
    aExecutor.bindCommand(ID_SR_ADD_FILE_SCRAP,                new cmd::SearchResultAddFileScrapCommand);
    aExecutor.bindCommand(ID_SR_CLOSE,                         new cmd::SearchResultCloseCommand);
}
} // namespace cmd
