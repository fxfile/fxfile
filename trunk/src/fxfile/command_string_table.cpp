//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "command_string_table.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
CommandStringTable::CommandStringTable(void)
{
}

CommandStringTable::~CommandStringTable(void)
{
}

void CommandStringTable::load(void)
{
    loadCommands();
    loadCommandButtons();
}

void CommandStringTable::loadCommands(void)
{
    // main menu - file
    mCommandString[ID_FILE_NEW_FOLDER]                = XPR_STRING_LITERAL("cmd.new.folder");
    mCommandString[ID_FILE_NEW_FILE]                  = XPR_STRING_LITERAL("cmd.new.file");
    mCommandString[ID_FILE_NEW_TEXT]                  = XPR_STRING_LITERAL("cmd.new.text_file");
    mCommandString[ID_FILE_NEW_LINK]                  = XPR_STRING_LITERAL("cmd.new.shortcut");
    mCommandString[ID_FILE_NEW_ITEMS]                 = XPR_STRING_LITERAL("cmd.new.items");
    mCommandString[ID_FILE_LINK]                      = XPR_STRING_LITERAL("cmd.create_shortcut");
    mCommandString[ID_FILE_DUPLICATE]                 = XPR_STRING_LITERAL("cmd.duplicate");
    mCommandString[ID_FILE_DELETE]                    = XPR_STRING_LITERAL("cmd.delete");
    mCommandString[ID_FILE_RENAME]                    = XPR_STRING_LITERAL("cmd.rename");
    mCommandString[ID_FILE_BATCH_RENAME_ALL]          = XPR_STRING_LITERAL("cmd.batch_rename");
    mCommandString[ID_FILE_BATCH_RENAME_FILE]         = XPR_STRING_LITERAL("cmd.batch_file_rename");
    mCommandString[ID_FILE_BATCH_RENAME_FOLDER]       = XPR_STRING_LITERAL("cmd.batch_folder_rename");
    mCommandString[ID_FILE_IMAGE_CONV]                = XPR_STRING_LITERAL("cmd.batch_image_conversion");
    mCommandString[ID_FILE_ATTRTIME]                  = XPR_STRING_LITERAL("cmd.change_attr_time");
    mCommandString[ID_FILE_PROPERTY]                  = XPR_STRING_LITERAL("cmd.properties");
    mCommandString[ID_FILE_EXEC]                      = XPR_STRING_LITERAL("cmd.execute");
    mCommandString[ID_FILE_EXEC_PARAM]                = XPR_STRING_LITERAL("cmd.execute_with_param");
    mCommandString[ID_FILE_VIEW]                      = XPR_STRING_LITERAL("cmd.file_view");
    mCommandString[ID_FILE_EDIT]                      = XPR_STRING_LITERAL("cmd.file_edit");
    mCommandString[ID_FILE_VIEWER_ASS]                = XPR_STRING_LITERAL("cmd.file_view_assoicaiton");
    mCommandString[ID_FILE_EDITOR_ASS]                = XPR_STRING_LITERAL("cmd.file_edit_assoicaiton");
    mCommandString[ID_FILE_SHELL_ASS]                 = XPR_STRING_LITERAL("cmd.shell_association");
    mCommandString[ID_FILE_CRC_CREATE]                = XPR_STRING_LITERAL("cmd.create_crc_file");
    mCommandString[ID_FILE_CRC_VERIFY]                = XPR_STRING_LITERAL("cmd.verify_crc_file");
    mCommandString[ID_FILE_SPLIT]                     = XPR_STRING_LITERAL("cmd.file_split");
    mCommandString[ID_FILE_COMBINE]                   = XPR_STRING_LITERAL("cmd.file_combine");
    mCommandString[ID_FILE_MERGE]                     = XPR_STRING_LITERAL("cmd.file_merge");
    mCommandString[ID_FILE_LIST_FILE]                 = XPR_STRING_LITERAL("cmd.create_file_list");
    mCommandString[ID_FILE_TEXT_MERGE]                = XPR_STRING_LITERAL("cmd.text_file_merge");
    mCommandString[ID_FILE_SEND_MAIL]                 = XPR_STRING_LITERAL("cmd.send_mail");
    mCommandString[ID_FILE_PRINT]                     = XPR_STRING_LITERAL("cmd.print");
    mCommandString[ID_FILE_PRINT_PREVIEW]             = XPR_STRING_LITERAL("cmd.print_preview");
    mCommandString[ID_FILE_PRINT_SETUP]               = XPR_STRING_LITERAL("cmd.print_setup");
    mCommandString[ID_APP_MINIMIZE_TRAY]              = XPR_STRING_LITERAL("cmd.minimize_tray");
    mCommandString[ID_APP_EXIT]                       = XPR_STRING_LITERAL("cmd.exit");

    // main menu - edit
    mCommandString[ID_EDIT_UNDO]                      = XPR_STRING_LITERAL("cmd.undo.none");
    mCommandString[ID_EDIT_CUT]                       = XPR_STRING_LITERAL("cmd.cut");
    mCommandString[ID_EDIT_COPY]                      = XPR_STRING_LITERAL("cmd.copy");
    mCommandString[ID_EDIT_NAME_COPY]                 = XPR_STRING_LITERAL("cmd.copy_name");
    mCommandString[ID_EDIT_FILENAME_COPY]             = XPR_STRING_LITERAL("cmd.copy_filename");
    mCommandString[ID_EDIT_PATH_COPY]                 = XPR_STRING_LITERAL("cmd.copy_path");
    mCommandString[ID_EDIT_DEV_PATH_COPY]             = XPR_STRING_LITERAL("cmd.copy_dev_path");
    mCommandString[ID_EDIT_URL_COPY]                  = XPR_STRING_LITERAL("cmd.copy_url");
    mCommandString[ID_EDIT_PASTE]                     = XPR_STRING_LITERAL("cmd.paste");
    mCommandString[ID_EDIT_PASTE_COPY]                = XPR_STRING_LITERAL("cmd.paste_copy");
    mCommandString[ID_EDIT_PASTE_LINK]                = XPR_STRING_LITERAL("cmd.paste_shortcut");
    mCommandString[ID_EDIT_PASTE_SPECIAL]             = XPR_STRING_LITERAL("cmd.paste_special");
    mCommandString[ID_EDIT_FOLDER_MOVE]               = XPR_STRING_LITERAL("cmd.move_to_folder");
    mCommandString[ID_EDIT_FOLDER_COPY]               = XPR_STRING_LITERAL("cmd.copy_to_folder");
    mCommandString[ID_EDIT_FILE_SCRAP_ADD]            = XPR_STRING_LITERAL("cmd.file_scrap.add");
    mCommandString[ID_EDIT_FILE_SCRAP_CUR_COPY]       = XPR_STRING_LITERAL("cmd.file_scrap.copy_to_this");
    mCommandString[ID_EDIT_FILE_SCRAP_CUR_MOVE]       = XPR_STRING_LITERAL("cmd.file_scrap.move_to_this");
    mCommandString[ID_EDIT_FILE_SCRAP_DELETE]         = XPR_STRING_LITERAL("cmd.file_scrap.delete");
    mCommandString[ID_EDIT_FILE_SCRAP_CLIPBOARD_CUT]  = XPR_STRING_LITERAL("cmd.file_scrap.clipboard_cut");
    mCommandString[ID_EDIT_FILE_SCRAP_CLIPBOARD_COPY] = XPR_STRING_LITERAL("cmd.file_scrap.clipboard_copy");
    mCommandString[ID_EDIT_FILE_SCRAP_VIEW]           = XPR_STRING_LITERAL("cmd.file_scrap.view");
    mCommandString[ID_EDIT_FILE_SCRAP_GROUP_ADD]      = XPR_STRING_LITERAL("cmd.file_scrap.add_group");
    mCommandString[ID_EDIT_FILE_SCRAP_GROUP_RENAME]   = XPR_STRING_LITERAL("cmd.file_scrap.rename_group");
    mCommandString[ID_EDIT_FILE_SCRAP_GROUP_DELETE]   = XPR_STRING_LITERAL("cmd.file_scrap.delete_group");
    mCommandString[ID_EDIT_FILE_SCRAP_GROUP_DEFAULT]  = XPR_STRING_LITERAL("cmd.file_scrap.default_group");
    mCommandString[ID_EDIT_FILE_SCRAP_REMOVE_ALL_LIST]= XPR_STRING_LITERAL("cmd.file_scrap.remove_all_list");
    mCommandString[ID_EDIT_FILE_SCRAP_DROP]           = XPR_STRING_LITERAL("cmd.file_scrap.show_floater");
    mCommandString[ID_EDIT_SEL_NAME]                  = XPR_STRING_LITERAL("cmd.select_name");
    mCommandString[ID_EDIT_UNSEL_NAME]                = XPR_STRING_LITERAL("cmd.unselect_name");
    mCommandString[ID_EDIT_SEL_SAME_EXT]              = XPR_STRING_LITERAL("cmd.select_same_ext");
    mCommandString[ID_EDIT_SEL_SAME_FILTER]           = XPR_STRING_LITERAL("cmd.select_same_filter");
    mCommandString[ID_EDIT_SEL_FILE]                  = XPR_STRING_LITERAL("cmd.select_file");
    mCommandString[ID_EDIT_SEL_FOLDER]                = XPR_STRING_LITERAL("cmd.select_folder");
    mCommandString[ID_EDIT_UNSEL_FILE]                = XPR_STRING_LITERAL("cmd.unselect_file");
    mCommandString[ID_EDIT_UNSEL_FOLDER]              = XPR_STRING_LITERAL("cmd.unselect_folder");
    mCommandString[ID_EDIT_SEL_FILTER]                = XPR_STRING_LITERAL("cmd.select_filter");
    mCommandString[ID_EDIT_UNSEL_FILTER]              = XPR_STRING_LITERAL("cmd.unselect_filter");
    mCommandString[ID_EDIT_SEL_ALL]                   = XPR_STRING_LITERAL("cmd.select_all");
    mCommandString[ID_EDIT_UNSEL_ALL]                 = XPR_STRING_LITERAL("cmd.unselect_all");
    mCommandString[ID_EDIT_SEL_REVERSAL]              = XPR_STRING_LITERAL("cmd.select_reversal");

    // main menu - go
    mCommandString[ID_GO_DRIVE]                       = XPR_STRING_LITERAL("cmd.go.drive");
    mCommandString[ID_DRIVE_DYNAMIC_MENU]             = XPR_STRING_LITERAL("cmd.go.drive.dynamic_menu");
    mCommandString[ID_GO_DRIVE_DETAIL]                = XPR_STRING_LITERAL("cmd.drive_detail");
    mCommandString[ID_GO_BOOKMARK]                    = XPR_STRING_LITERAL("cmd.bookmark.popup_menu");
    mCommandString[ID_GO_BOOKMARK_ADD]                = XPR_STRING_LITERAL("cmd.bookmark.add");
    mCommandString[ID_GO_BOOKMARK_MODIFY]             = XPR_STRING_LITERAL("cmd.bookmark.modify");
    mCommandString[ID_GO_WORKING_FOLDER]              = XPR_STRING_LITERAL("cmd.working_folder.popup_menu");
    mCommandString[ID_GO_WORKING_FOLDER_RESET_ALL]    = XPR_STRING_LITERAL("cmd.working_folder.reset_all");
    mCommandString[ID_GO_PATH]                        = XPR_STRING_LITERAL("cmd.go.path");
    mCommandString[ID_GO_INIT_FOLDER_ALL]             = XPR_STRING_LITERAL("cmd.go.init_folder_all");
    mCommandString[ID_GO_INIT_FOLDER_1]               = XPR_STRING_LITERAL("cmd.go.init_folder_1");
    mCommandString[ID_GO_INIT_FOLDER_2]               = XPR_STRING_LITERAL("cmd.go.init_folder_2");
    mCommandString[ID_GO_INIT_FOLDER_3]               = XPR_STRING_LITERAL("cmd.go.init_folder_3");
    mCommandString[ID_GO_INIT_FOLDER_4]               = XPR_STRING_LITERAL("cmd.go.init_folder_4");
    mCommandString[ID_GO_INIT_FOLDER_5]               = XPR_STRING_LITERAL("cmd.go.init_folder_5");
    mCommandString[ID_GO_INIT_FOLDER_6]               = XPR_STRING_LITERAL("cmd.go.init_folder_6");
    mCommandString[ID_GO_SYSTEM_DESKTOP]              = XPR_STRING_LITERAL("cmd.go.system_folder.desktop");
    mCommandString[ID_GO_SYSTEM_LIBRARIES]            = XPR_STRING_LITERAL("cmd.go.system_folder.libraries");
    mCommandString[ID_GO_SYSTEM_DOCUMENTS]            = XPR_STRING_LITERAL("cmd.go.system_folder.documents");
    mCommandString[ID_GO_SYSTEM_PICTURES]             = XPR_STRING_LITERAL("cmd.go.system_folder.pictures");
    mCommandString[ID_GO_SYSTEM_MUSIC]                = XPR_STRING_LITERAL("cmd.go.system_folder.music");
    mCommandString[ID_GO_SYSTEM_VIDEOS]               = XPR_STRING_LITERAL("cmd.go.system_folder.videos");
    mCommandString[ID_GO_SYSTEM_MYDOCUMENT]           = XPR_STRING_LITERAL("cmd.go.system_folder.my_document");
    mCommandString[ID_GO_SYSTEM_MYCOMPUTER]           = XPR_STRING_LITERAL("cmd.go.system_folder.my_computer");
    mCommandString[ID_GO_SYSTEM_NETWORK]              = XPR_STRING_LITERAL("cmd.go.system_folder.network");
    mCommandString[ID_GO_SYSTEM_CONTROLS]             = XPR_STRING_LITERAL("cmd.go.system_folder.controls");
    mCommandString[ID_GO_SYSTEM_RECYCLE]              = XPR_STRING_LITERAL("cmd.go.system_folder.recycle");
    mCommandString[ID_GO_SYSTEM_PROGRAMFILES]         = XPR_STRING_LITERAL("cmd.go.system_folder.program_files");
    mCommandString[ID_GO_SYSTEM_COMMON]               = XPR_STRING_LITERAL("cmd.go.system_folder.common");
    mCommandString[ID_GO_SYSTEM_WINDOWS]              = XPR_STRING_LITERAL("cmd.go.system_folder.windows");
    mCommandString[ID_GO_SYSTEM_WINDOWS_SYSTEM]       = XPR_STRING_LITERAL("cmd.go.system_folder.windows_system");
    mCommandString[ID_GO_SYSTEM_APPDATA]              = XPR_STRING_LITERAL("cmd.go.system_folder.application_data");
    mCommandString[ID_GO_SYSTEM_FONTS]                = XPR_STRING_LITERAL("cmd.go.system_folder.fonts");
    mCommandString[ID_GO_SYSTEM_STARTUP]              = XPR_STRING_LITERAL("cmd.go.system_folder.startup");
    mCommandString[ID_GO_SYSTEM_TEMPORARY]            = XPR_STRING_LITERAL("cmd.go.system_folder.temporary");
    mCommandString[ID_GO_SYSTEM_FAVORITES]            = XPR_STRING_LITERAL("cmd.go.system_folder.favorites");
    mCommandString[ID_GO_SYSTEM_INTERNETCACHE]        = XPR_STRING_LITERAL("cmd.go.system_folder.internet_cache");
    mCommandString[ID_GO_SYSTEM_INTERNETCOOKIE]       = XPR_STRING_LITERAL("cmd.go.system_folder.internet_cookie");
    mCommandString[ID_GO_SYSTEM_SENDTO]               = XPR_STRING_LITERAL("cmd.go.system_folder.send_to");
    mCommandString[ID_GO_SYSTEM_RECENT]               = XPR_STRING_LITERAL("cmd.go.system_folder.recent");
    mCommandString[ID_GO_BACKWARD_DYNAMIC_MENU]       = XPR_STRING_LITERAL("cmd.navigate.back.dynamic_menu");
    mCommandString[ID_GO_BACK]                        = XPR_STRING_LITERAL("cmd.navigate.back");
    mCommandString[ID_GO_FORWARD]                     = XPR_STRING_LITERAL("cmd.navigate.forward");
    mCommandString[ID_GO_FORWARD_DYNAMIC_MENU]        = XPR_STRING_LITERAL("cmd.navigate.forward.dynamic_menu");
    mCommandString[ID_GO_HISTORY]                     = XPR_STRING_LITERAL("cmd.navigate.history");
    mCommandString[ID_GO_BACK_BOX]                    = XPR_STRING_LITERAL("cmd.navigate.back_box");
    mCommandString[ID_GO_FORWARD_BOX]                 = XPR_STRING_LITERAL("cmd.navigate.forward_box");
    mCommandString[ID_GO_HISTORY_BOX]                 = XPR_STRING_LITERAL("cmd.navigate.history_box");
    mCommandString[ID_GO_BACK_CLEAR]                  = XPR_STRING_LITERAL("cmd.navigate.clear_back_history");
    mCommandString[ID_GO_FORWARD_CLEAR]               = XPR_STRING_LITERAL("cmd.navigate.clear_forward_history");
    mCommandString[ID_GO_HISTORY_CLEAR]               = XPR_STRING_LITERAL("cmd.navigate.clear_history");
    mCommandString[ID_GO_HISTORY_ALL_CLEAR]           = XPR_STRING_LITERAL("cmd.navigate.clear_all_history");
    mCommandString[ID_GO_SIBLING_UP]                  = XPR_STRING_LITERAL("cmd.go.sibling_up");
    mCommandString[ID_GO_SIBLING_DOWN]                = XPR_STRING_LITERAL("cmd.go.sibling_down");
    mCommandString[ID_GO_UP]                          = XPR_STRING_LITERAL("cmd.go.up");
    mCommandString[ID_GO_UP_DYNAMIC_MENU]             = XPR_STRING_LITERAL("cmd.go.up.dynamic_menu");
    mCommandString[ID_GO_ROOT]                        = XPR_STRING_LITERAL("cmd.go.root");
    mCommandString[ID_GO_UP_BOX]                      = XPR_STRING_LITERAL("cmd.up_box");
    mCommandString[ID_GO_FXFILE]                      = XPR_STRING_LITERAL("cmd.go.fxfile");

    // main menu - view
    mCommandString[ID_VIEW_TOOLBAR]                   = XPR_STRING_LITERAL("cmd.tool_bar");
    mCommandString[ID_VIEW_DRIVEBAR]                  = XPR_STRING_LITERAL("cmd.drive_bar");
    mCommandString[ID_VIEW_BOOKMARKBAR]               = XPR_STRING_LITERAL("cmd.bookmark_bar");
    mCommandString[ID_VIEW_ADDRESSBAR_SHOW]           = XPR_STRING_LITERAL("cmd.address_bar.drop_down");
    mCommandString[ID_VIEW_DRIVEBAR_SHORTTEXT]        = XPR_STRING_LITERAL("cmd.drive_bar.short_text");
    mCommandString[ID_VIEW_DRIVEBAR_LONGTEXT]         = XPR_STRING_LITERAL("cmd.drive_bar.long_text");
    mCommandString[ID_VIEW_DRIVEBAR_WRAPABLE]         = XPR_STRING_LITERAL("cmd.drive_bar.wrapable");
    mCommandString[ID_VIEW_DRIVEBAR_REFRESH]          = XPR_STRING_LITERAL("cmd.drive_bar.refresh");
    mCommandString[ID_VIEW_BOOKMARKBAR_NONAME]        = XPR_STRING_LITERAL("cmd.bookmark_bar.no_name");
    mCommandString[ID_VIEW_BOOKMARKBAR_NAME]          = XPR_STRING_LITERAL("cmd.bookmark_bar.name");
    mCommandString[ID_VIEW_BOOKMARKBAR_WRAPABLE]      = XPR_STRING_LITERAL("cmd.bookmark_bar.wrapable");
    mCommandString[ID_VIEW_BOOKMARKBAR_REFRESH]       = XPR_STRING_LITERAL("cmd.bookmark_bar.refresh");
    mCommandString[ID_VIEW_TOOL_LOCK]                 = XPR_STRING_LITERAL("cmd.tools.lock");
    mCommandString[ID_VIEW_TOOLBAR_CUSTOMIZE]         = XPR_STRING_LITERAL("cmd.tool_bar.customize");
    mCommandString[ID_VIEW_FOLDER_TREE_NONE]          = XPR_STRING_LITERAL("cmd.folder_tree.none");
    mCommandString[ID_VIEW_FOLDER_TREE_SHOW]          = XPR_STRING_LITERAL("cmd.folder_tree.show");
    mCommandString[ID_VIEW_FOLDER_TREE_SINGLE_PANE]   = XPR_STRING_LITERAL("cmd.folder_tree.single_pane");
    mCommandString[ID_VIEW_FOLDER_TREE_EACH_PANE]     = XPR_STRING_LITERAL("cmd.folder_tree.each_pane");
    mCommandString[ID_VIEW_FOLDER_TREE_LEFT_PANE]     = XPR_STRING_LITERAL("cmd.folder_tree.left_pane");
    mCommandString[ID_VIEW_FOLDER_TREE_RIGHT_PANE]    = XPR_STRING_LITERAL("cmd.folder_tree.right_pane");
    mCommandString[ID_VIEW_PICVIEWER]                 = XPR_STRING_LITERAL("cmd.picture.show");
    mCommandString[ID_VIEW_PIC_ZOOM_IN]               = XPR_STRING_LITERAL("cmd.picture.zoom_in");
    mCommandString[ID_VIEW_PIC_ZOOM_OUT]              = XPR_STRING_LITERAL("cmd.picture.zoom_out");
    mCommandString[ID_VIEW_PIC_ZOOM_100]              = XPR_STRING_LITERAL("cmd.picture.zoom_100");
    mCommandString[ID_VIEW_PIC_RATIO]                 = XPR_STRING_LITERAL("cmd.picture.resize_by_ratio");
    mCommandString[ID_VIEW_PIC_DOCKING]               = XPR_STRING_LITERAL("cmd.picture.docking");
    mCommandString[ID_VIEW_PIC_AUTO_HIDE]             = XPR_STRING_LITERAL("cmd.picture.auto_hide");
    mCommandString[ID_VIEW_PIC_LOCK]                  = XPR_STRING_LITERAL("cmd.picture.lock_picture_size");
    mCommandString[ID_VIEW_PIC_CLIENT]                = XPR_STRING_LITERAL("cmd.picture.lock_window_size");
    mCommandString[ID_VIEW_PIC_LOCK_100]              = XPR_STRING_LITERAL("cmd.picture.lock_100%_size");
    mCommandString[ID_VIEW_PIC_LOCK_THUMBNAIL]        = XPR_STRING_LITERAL("cmd.picture.lock_thumbnail_size");
    mCommandString[ID_VIEW_PIC_SHOW_FILENAME]         = XPR_STRING_LITERAL("cmd.picture.title_filename");
    mCommandString[ID_VIEW_PIC_BACK_CENTER]           = XPR_STRING_LITERAL("cmd.picture.desktop_background.center");
    mCommandString[ID_VIEW_PIC_BACK_TILE]             = XPR_STRING_LITERAL("cmd.picture.desktop_background.tile");
    mCommandString[ID_VIEW_PIC_BACK_STRETCH]          = XPR_STRING_LITERAL("cmd.picture.desktop_background.stretch");
    mCommandString[ID_VIEW_PIC_SHOW_HIDE]             = XPR_STRING_LITERAL("cmd.picture.show_hide");
    mCommandString[ID_VIEW_PIC_HIDE]                  = XPR_STRING_LITERAL("cmd.picture.hide");
    mCommandString[ID_VIEW_PIC_CLOSE]                 = XPR_STRING_LITERAL("cmd.picture.close");
    mCommandString[ID_VIEW_STYLE]                     = XPR_STRING_LITERAL("cmd.view_style");
    mCommandString[ID_VIEW_STYLE_LARGEICONS]          = XPR_STRING_LITERAL("cmd.view_style.large_icons");
    mCommandString[ID_VIEW_STYLE_SMALLICONS]          = XPR_STRING_LITERAL("cmd.view_style.small_icons");
    mCommandString[ID_VIEW_STYLE_LIST]                = XPR_STRING_LITERAL("cmd.view_style.list");
    mCommandString[ID_VIEW_STYLE_DETAILS]             = XPR_STRING_LITERAL("cmd.view_style.details");
    mCommandString[ID_VIEW_STYLE_THUMBNAIL]           = XPR_STRING_LITERAL("cmd.view_style.thumbnail");
    mCommandString[ID_VIEW_ARRAY_NAME]                = XPR_STRING_LITERAL("cmd.sort.name");
    mCommandString[ID_VIEW_ARRAY_EXT]                 = XPR_STRING_LITERAL("cmd.sort.ext");
    mCommandString[ID_VIEW_ARRAY_SIZE]                = XPR_STRING_LITERAL("cmd.sort.size");
    mCommandString[ID_VIEW_ARRAY_TYPE]                = XPR_STRING_LITERAL("cmd.sort.type");
    mCommandString[ID_VIEW_ARRAY_DATE]                = XPR_STRING_LITERAL("cmd.sort.date");
    mCommandString[ID_VIEW_ARRAY_ATTR]                = XPR_STRING_LITERAL("cmd.sort.attr");
    mCommandString[ID_VIEW_ARRAY_ASCENDING]           = XPR_STRING_LITERAL("cmd.sort.ascending");
    mCommandString[ID_VIEW_ARRAY_DECENDING]           = XPR_STRING_LITERAL("cmd.sort.decending");
    mCommandString[ID_VIEW_COLUMN_NAME]               = XPR_STRING_LITERAL("cmd.column.name");
    mCommandString[ID_VIEW_COLUMN_EXT]                = XPR_STRING_LITERAL("cmd.column.ext");
    mCommandString[ID_VIEW_COLUMN_SIZE]               = XPR_STRING_LITERAL("cmd.column.size");
    mCommandString[ID_VIEW_COLUMN_TYPE]               = XPR_STRING_LITERAL("cmd.column.type");
    mCommandString[ID_VIEW_COLUMN_DATE]               = XPR_STRING_LITERAL("cmd.column.date");
    mCommandString[ID_VIEW_COLUMN_ATTR]               = XPR_STRING_LITERAL("cmd.column.attr");
    mCommandString[ID_VIEW_COLUMN_SET]                = XPR_STRING_LITERAL("cmd.column.set");
    mCommandString[ID_VIEW_REFRESH]                   = XPR_STRING_LITERAL("cmd.refresh");

    // main menu - tool
    mCommandString[ID_TOOL_FILE_SEARCH]               = XPR_STRING_LITERAL("cmd.file_search");
    mCommandString[ID_TOOL_EMPTY_RECYCLEBIN]          = XPR_STRING_LITERAL("cmd.recycle_bin.empty");
    mCommandString[ID_TOOL_WINDOWS_EXPLORER]          = XPR_STRING_LITERAL("cmd.windows_explorer");
    mCommandString[ID_TOOL_CMD]                       = XPR_STRING_LITERAL("cmd.command_prompt");
    mCommandString[ID_TOOL_CMD_AS_ADMIN]              = XPR_STRING_LITERAL("cmd.command_prompt_as_admin");
    mCommandString[ID_TOOL_DOS_CMD]                   = XPR_STRING_LITERAL("cmd.dos_command");
    mCommandString[ID_TOOL_FOLDER_SYNC]               = XPR_STRING_LITERAL("cmd.folder_sync");
    mCommandString[ID_TOOL_SHARED_PROC]               = XPR_STRING_LITERAL("cmd.shared_process");
    mCommandString[ID_TOOL_NETWORK_CONNECT]           = XPR_STRING_LITERAL("cmd.network_drive.connect");
    mCommandString[ID_TOOL_NETWORK_DISCONNECT]        = XPR_STRING_LITERAL("cmd.network_drvie.disconnect");
    mCommandString[ID_TOOL_SAVE_OPTION]               = XPR_STRING_LITERAL("cmd.save_option");
    mCommandString[ID_TOOL_ACCELERATOR]               = XPR_STRING_LITERAL("cmd.keyboard.customize");
    mCommandString[ID_TOOL_OPTION]                    = XPR_STRING_LITERAL("cmd.preferences");

    // main menu - window
    mCommandString[ID_WINDOW_TAB_NEW]                 = XPR_STRING_LITERAL("cmd.window.tab.new");
    mCommandString[ID_WINDOW_TAB_DUPLICATE]           = XPR_STRING_LITERAL("cmd.window.tab.duplicate");
    mCommandString[ID_WINDOW_TAB_DUPLICATE_ON_CURSOR] = XPR_STRING_LITERAL("cmd.window.tab.duplicate");
    mCommandString[ID_WINDOW_TAB_FIRST]               = XPR_STRING_LITERAL("cmd.window.tab.first");
    mCommandString[ID_WINDOW_TAB_PREVIOUS]            = XPR_STRING_LITERAL("cmd.window.tab.previous");
    mCommandString[ID_WINDOW_TAB_NEXT]                = XPR_STRING_LITERAL("cmd.window.tab.next");
    mCommandString[ID_WINDOW_TAB_LAST]                = XPR_STRING_LITERAL("cmd.window.tab.last");
    mCommandString[ID_WINDOW_TAB_CLOSE]               = XPR_STRING_LITERAL("cmd.window.tab.close");
    mCommandString[ID_WINDOW_TAB_CLOSE_ON_CURSOR]     = XPR_STRING_LITERAL("cmd.window.tab.close");
    mCommandString[ID_WINDOW_TAB_CLOSE_OTHERS]        = XPR_STRING_LITERAL("cmd.window.tab.close_others");
    mCommandString[ID_WINDOW_TAB_CLOSE_OTHERS_ON_CURSOR]
                                                      = XPR_STRING_LITERAL("cmd.window.tab.close_others");
    mCommandString[ID_WINDOW_TAB_CLOSE_ALL]           = XPR_STRING_LITERAL("cmd.window.tab.close_all");
    mCommandString[ID_WINDOW_NEXT]                    = XPR_STRING_LITERAL("cmd.window.next");
    mCommandString[ID_WINDOW_PREV]                    = XPR_STRING_LITERAL("cmd.window.previous");
    mCommandString[ID_WINDOW_SPLIT]                   = XPR_STRING_LITERAL("cmd.window.split");
    mCommandString[ID_WINDOW_SPLIT_SINGLE]            = XPR_STRING_LITERAL("cmd.window.split.single");
    mCommandString[ID_WINDOW_SPLIT_1x2]               = XPR_STRING_LITERAL("cmd.window.split.1x2");
    mCommandString[ID_WINDOW_SPLIT_2x1]               = XPR_STRING_LITERAL("cmd.window.split.2x1");
    mCommandString[ID_WINDOW_SPLIT_2x2]               = XPR_STRING_LITERAL("cmd.window.split.2x2");
    mCommandString[ID_WINDOW_SPLIT_1x3]               = XPR_STRING_LITERAL("cmd.window.split.1x3");
    mCommandString[ID_WINDOW_SPLIT_2x3]               = XPR_STRING_LITERAL("cmd.window.split.2x3");
    mCommandString[ID_WINDOW_SPLIT_EQUALLY]           = XPR_STRING_LITERAL("cmd.window.split.equally");
    mCommandString[ID_WINDOW_SPLIT_10]                = XPR_STRING_LITERAL("cmd.window.split.10%");
    mCommandString[ID_WINDOW_SPLIT_20]                = XPR_STRING_LITERAL("cmd.window.split.20%");
    mCommandString[ID_WINDOW_SPLIT_30]                = XPR_STRING_LITERAL("cmd.window.split.30%");
    mCommandString[ID_WINDOW_SPLIT_40]                = XPR_STRING_LITERAL("cmd.window.split.40%");
    mCommandString[ID_WINDOW_SPLIT_50]                = XPR_STRING_LITERAL("cmd.window.split.50%");
    mCommandString[ID_WINDOW_SPLIT_60]                = XPR_STRING_LITERAL("cmd.window.split.60%");
    mCommandString[ID_WINDOW_SPLIT_70]                = XPR_STRING_LITERAL("cmd.window.split.70%");
    mCommandString[ID_WINDOW_SPLIT_80]                = XPR_STRING_LITERAL("cmd.window.split.80%");
    mCommandString[ID_WINDOW_SPLIT_90]                = XPR_STRING_LITERAL("cmd.window.split.90%");
    mCommandString[ID_WINDOW_SPLIT_SWAP]              = XPR_STRING_LITERAL("cmd.window.swap");
    mCommandString[ID_WINDOW_SPLIT_EQUAL]             = XPR_STRING_LITERAL("cmd.window.equal");
    mCommandString[ID_WINDOW_SPLIT_OTHER_EQUAL]       = XPR_STRING_LITERAL("cmd.window.equal_in_other_window");
    mCommandString[ID_WINDOW_COPY]                    = XPR_STRING_LITERAL("cmd.window.copy_to_other");
    mCommandString[ID_WINDOW_MOVE]                    = XPR_STRING_LITERAL("cmd.window.move_to_other");
    mCommandString[ID_WINDOW_1_COPY]                  = XPR_STRING_LITERAL("cmd.window.copy_to_window_1");
    mCommandString[ID_WINDOW_1_MOVE]                  = XPR_STRING_LITERAL("cmd.window.move_to_window_1");
    mCommandString[ID_WINDOW_2_COPY]                  = XPR_STRING_LITERAL("cmd.window.copy_to_window_2");
    mCommandString[ID_WINDOW_2_MOVE]                  = XPR_STRING_LITERAL("cmd.window.move_to_window_2");
    mCommandString[ID_WINDOW_DUAL_COPY]               = XPR_STRING_LITERAL("cmd.window.cross_copy");
    mCommandString[ID_WINDOW_DUAL_MOVE]               = XPR_STRING_LITERAL("cmd.window.cross_move");
    mCommandString[ID_WINDOW_DUAL_DELETE]             = XPR_STRING_LITERAL("cmd.window.cross_delete");
    mCommandString[ID_WINDOW_COMPARE]                 = XPR_STRING_LITERAL("cmd.window.compare");

    // main menu - help
    mCommandString[ID_APP_HELP]                       = XPR_STRING_LITERAL("cmd.help");
    mCommandString[ID_APP_HELP_FIND]                  = XPR_STRING_LITERAL("cmd.help.find");
    mCommandString[ID_APP_ACCELERATOR]                = XPR_STRING_LITERAL("cmd.help.keyboard");
    mCommandString[ID_APP_TIP]                        = XPR_STRING_LITERAL("cmd.tips");
    mCommandString[ID_APP_HOMEPAGE]                   = XPR_STRING_LITERAL("cmd.homepage");
    mCommandString[ID_APP_EMAIL]                      = XPR_STRING_LITERAL("cmd.email");
    mCommandString[ID_APP_CMD_LINE_ARGS]              = XPR_STRING_LITERAL("cmd.command_line_arguments");
    mCommandString[ID_APP_PROGRAM_OPTS]               = XPR_STRING_LITERAL("cmd.program_options");
    mCommandString[ID_APP_ENV_VARS]                   = XPR_STRING_LITERAL("cmd.environment_variables");
    mCommandString[ID_APP_ABOUT]                      = XPR_STRING_LITERAL("cmd.about");

    // poup menu - scarp floater
    mCommandString[ID_FCD_COPY]                       = XPR_STRING_LITERAL("cmd.file_scrap.copy");
    mCommandString[ID_FCD_MOVE]                       = XPR_STRING_LITERAL("cmd.file_scrap.move");
    mCommandString[ID_FCD_DELETE]                     = XPR_STRING_LITERAL("cmd.file_scrap.delete");
    mCommandString[ID_FCD_CLIPBOARD_CUT]              = XPR_STRING_LITERAL("cmd.file_scrap.clipboard_cut");
    mCommandString[ID_FCD_CLIPBOARD_COPY]             = XPR_STRING_LITERAL("cmd.file_scrap.clipboard_copy");
    mCommandString[ID_FCD_VIEW]                       = XPR_STRING_LITERAL("cmd.file_scrap.view");
    mCommandString[ID_FCD_GROUP_DEFAULT]              = XPR_STRING_LITERAL("cmd.file_scrap.default_group");
    mCommandString[ID_FCD_REMOVE]                     = XPR_STRING_LITERAL("cmd.file_scrap.remove_all_list");
    mCommandString[ID_FCD_ALWAYS_TOP_MOST]            = XPR_STRING_LITERAL("cmd.file_scrap.floater.always_topmost");
    mCommandString[ID_FCD_TRANSPARENT_20]             = XPR_STRING_LITERAL("cmd.file_scrap.floater.transparency.20%");
    mCommandString[ID_FCD_TRANSPARENT_40]             = XPR_STRING_LITERAL("cmd.file_scrap.floater.transparency.40%");
    mCommandString[ID_FCD_TRANSPARENT_60]             = XPR_STRING_LITERAL("cmd.file_scrap.floater.transparency.60%");
    mCommandString[ID_FCD_TRANSPARENT_80]             = XPR_STRING_LITERAL("cmd.file_scrap.floater.transparency.80%");
    mCommandString[ID_FCD_TRANSPARENT_0]              = XPR_STRING_LITERAL("cmd.file_scrap.floater.transparency.none");
    mCommandString[ID_FCD_CLOSE]                      = XPR_STRING_LITERAL("cmd.file_scrap.floater.close");

    // poup menu - scarp group
    mCommandString[ID_SCRAP_GROUP_NEW]                = XPR_STRING_LITERAL("cmd.file_scrap.group.new");
    mCommandString[ID_SCRAP_GROUP_MODIFY]             = XPR_STRING_LITERAL("cmd.file_scrap.group.modify");
    mCommandString[ID_SCRAP_GROUP_DELETE]             = XPR_STRING_LITERAL("cmd.file_scrap.group.delete");
    mCommandString[ID_SCRAP_GROUP_DEFAULT]            = XPR_STRING_LITERAL("cmd.file_scrap.group.default");

    // poup menu - batch create items format
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_NUMBERING]      = XPR_STRING_LITERAL("cmd.new.items.format.numbering");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_DATE]           = XPR_STRING_LITERAL("cmd.new.items.format.today_date");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_YEAR]           = XPR_STRING_LITERAL("cmd.new.items.format.current_year");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_MONTH]          = XPR_STRING_LITERAL("cmd.new.items.format.current_month");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_DAY]            = XPR_STRING_LITERAL("cmd.new.items.format.current_day");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_TIME]           = XPR_STRING_LITERAL("cmd.new.items.format.current_time");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_HOUR]           = XPR_STRING_LITERAL("cmd.new.items.format.current_hour");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_MINUTE]         = XPR_STRING_LITERAL("cmd.new.items.format.current_minute");
    mCommandString[ID_BATCH_CREATE_FORMAT_MENU_SECOND]         = XPR_STRING_LITERAL("cmd.new.items.format.current_second");

    // poup menu - batch rename format
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_NUMBERING]               = XPR_STRING_LITERAL("cmd.batch_rename.format.numbering");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_FULLNAME]                = XPR_STRING_LITERAL("cmd.batch_rename.format.full_filename");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_FILENAME]                = XPR_STRING_LITERAL("cmd.batch_rename.format.filename");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_ORIGINAL_FILENAME_RANGE] = XPR_STRING_LITERAL("cmd.batch_rename.format.original_filename");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_EXTENSION]               = XPR_STRING_LITERAL("cmd.batch_rename.format.extension");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_DATE]                    = XPR_STRING_LITERAL("cmd.batch_rename.format.today_date");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_YEAR]                    = XPR_STRING_LITERAL("cmd.batch_rename.format.current_year");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_MONTH]                   = XPR_STRING_LITERAL("cmd.batch_rename.format.current_month");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_DAY]                     = XPR_STRING_LITERAL("cmd.batch_rename.format.current_day");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_TIME]                    = XPR_STRING_LITERAL("cmd.batch_rename.format.current_time");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_HOUR]                    = XPR_STRING_LITERAL("cmd.batch_rename.format.current_hour");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_MINUTE]                  = XPR_STRING_LITERAL("cmd.batch_rename.format.current_minute");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_SECOND]                  = XPR_STRING_LITERAL("cmd.batch_rename.format.current_second");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_DATE]           = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.date");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_YEAR]           = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.year");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_MONTH]          = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.month");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_DAY]            = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.day");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_TIME]           = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.time");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_AM_PM_HOUR]     = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.am_pm");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_12_HOUR]        = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.12-hour");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_24_HOUR]        = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.24-hour");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_MINUTE]         = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.minute");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_CREATION_SECOND]         = XPR_STRING_LITERAL("cmd.batch_rename.format.file_created_time.second");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_DATE]         = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.date");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_YEAR]         = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.year");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_MONTH]        = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.month");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_DAY]          = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.day");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_TIME]         = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.time");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_AM_PM_HOUR]   = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.am_pm");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_12_HOUR]      = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.12-hour");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_24_HOUR]      = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.24-hour");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_MINUTE]       = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.minute");
    mCommandString[ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_SECOND]       = XPR_STRING_LITERAL("cmd.batch_rename.format.file_modified_time.second");

    // poup menu - search result
    mCommandString[ID_SR_VIEW_ICON]                   = XPR_STRING_LITERAL("cmd.search_result.view_style.large_icon");
    mCommandString[ID_SR_VIEW_SMALLICON]              = XPR_STRING_LITERAL("cmd.search_result.view_style.small_icon");
    mCommandString[ID_SR_VIEW_LIST]                   = XPR_STRING_LITERAL("cmd.search_result.view_style.list");
    mCommandString[ID_SR_VIEW_DETAIL]                 = XPR_STRING_LITERAL("cmd.search_result.view_style.detail");
    mCommandString[ID_SR_REMOVE_LIST]                 = XPR_STRING_LITERAL("cmd.search_result.remove_list");
    mCommandString[ID_SR_ADD_FILE_SCRAP]              = XPR_STRING_LITERAL("cmd.search_result.add_all_to_scrap");
    mCommandString[ID_SR_CLOSE]                       = XPR_STRING_LITERAL("cmd.search_result.close");

    // poup menu - search's user location
    mCommandString[ID_LOC_NEW]                        = XPR_STRING_LITERAL("cmd.search.user_location.new");
    mCommandString[ID_LOC_MODIFY]                     = XPR_STRING_LITERAL("cmd.search.user_location.modify");
    mCommandString[ID_LOC_DELETE]                     = XPR_STRING_LITERAL("cmd.search.user_location.delete");

    // poup menu - tray
    mCommandString[ID_VIEW_SHOW_MAINFRAME]            = XPR_STRING_LITERAL("cmd.tray.open");
    mCommandString[ID_TRAY_APP_EXIT]                  = XPR_STRING_LITERAL("cmd.tray.exit");

    // popup menu - sync
    mCommandString[ID_SYNC_LEFT]                      = XPR_STRING_LITERAL("cmd.sync.left");
    mCommandString[ID_SYNC_RIGHT]                     = XPR_STRING_LITERAL("cmd.sync.right");
    mCommandString[ID_SYNC_NONE]                      = XPR_STRING_LITERAL("cmd.sync.none");
    mCommandString[ID_SYNC_ORIGINAL]                  = XPR_STRING_LITERAL("cmd.sync.restore");
}

void CommandStringTable::loadCommandButtons(void)
{
    //
    // main toolbar
    //
    mCommandButtonString[ID_GO_BACK]                  = XPR_STRING_LITERAL("tool_bar.cmd.navigate.back");
    mCommandButtonString[ID_GO_FORWARD]               = XPR_STRING_LITERAL("tool_bar.cmd.navigate.forward");
    mCommandButtonString[ID_GO_UP]                    = XPR_STRING_LITERAL("tool_bar.cmd.go.up");
    mCommandButtonString[ID_GO_HISTORY]               = XPR_STRING_LITERAL("tool_bar.cmd.navigate.history");

    mCommandButtonString[ID_VIEW_FOLDER_TREE_SHOW]    = XPR_STRING_LITERAL("tool_bar.cmd.folder_tree.show");
    mCommandButtonString[ID_TOOL_FILE_SEARCH]         = XPR_STRING_LITERAL("tool_bar.cmd.file_search");

    mCommandButtonString[ID_GO_BOOKMARK]              = XPR_STRING_LITERAL("tool_bar.cmd.bookmark.popup_menu");
    mCommandButtonString[ID_GO_DRIVE]                 = XPR_STRING_LITERAL("tool_bar.cmd.go.drive");
    mCommandButtonString[ID_GO_WORKING_FOLDER]        = XPR_STRING_LITERAL("tool_bar.cmd.working_folder.popup_menu");
    mCommandButtonString[ID_GO_WORKING_FOLDER_0]      = XPR_STRING_LITERAL("tool_bar.cmd.working_folder.go_1");
    mCommandButtonString[ID_GO_WORKING_FOLDER_1]      = XPR_STRING_LITERAL("tool_bar.cmd.working_folder.go_2");
    mCommandButtonString[ID_GO_WORKING_FOLDER_2]      = XPR_STRING_LITERAL("tool_bar.cmd.working_folder.go_3");
    mCommandButtonString[ID_GO_WORKING_FOLDER_3]      = XPR_STRING_LITERAL("tool_bar.cmd.working_folder.go_4");
    mCommandButtonString[ID_GO_WORKING_FOLDER_4]      = XPR_STRING_LITERAL("tool_bar.cmd.working_folder.go_5");

    mCommandButtonString[ID_EDIT_CUT]                 = XPR_STRING_LITERAL("tool_bar.cmd.cut");
    mCommandButtonString[ID_EDIT_COPY]                = XPR_STRING_LITERAL("tool_bar.cmd.copy");
    mCommandButtonString[ID_EDIT_PASTE]               = XPR_STRING_LITERAL("tool_bar.cmd.paste");
    mCommandButtonString[ID_EDIT_FOLDER_MOVE]         = XPR_STRING_LITERAL("tool_bar.cmd.move_to_folder");
    mCommandButtonString[ID_EDIT_FOLDER_COPY]         = XPR_STRING_LITERAL("tool_bar.cmd.copy_to_folder");

    mCommandButtonString[ID_FILE_RENAME]              = XPR_STRING_LITERAL("tool_bar.cmd.rename");
    mCommandButtonString[ID_FILE_DELETE]              = XPR_STRING_LITERAL("tool_bar.cmd.delete");
    mCommandButtonString[ID_EDIT_UNDO]                = XPR_STRING_LITERAL("tool_bar.cmd.undo");
    mCommandButtonString[ID_FILE_ATTRTIME]            = XPR_STRING_LITERAL("tool_bar.cmd.change_attr_time");
    mCommandButtonString[ID_FILE_PROPERTY]            = XPR_STRING_LITERAL("tool_bar.cmd.properties");

    mCommandButtonString[ID_VIEW_STYLE]               = XPR_STRING_LITERAL("tool_bar.cmd.view_style");
    mCommandButtonString[ID_WINDOW_SPLIT]             = XPR_STRING_LITERAL("tool_bar.cmd.window.split");
    mCommandButtonString[ID_WINDOW_SPLIT_SINGLE]      = XPR_STRING_LITERAL("tool_bar.cmd.window.split.single");
    mCommandButtonString[ID_WINDOW_SPLIT_1x2]         = XPR_STRING_LITERAL("tool_bar.cmd.window.split.1x2");
    mCommandButtonString[ID_WINDOW_SPLIT_2x1]         = XPR_STRING_LITERAL("tool_bar.cmd.window.split.2x1");
    mCommandButtonString[ID_WINDOW_SPLIT_EQUAL]       = XPR_STRING_LITERAL("tool_bar.cmd.window.equal");
    mCommandButtonString[ID_WINDOW_SPLIT_SWAP]        = XPR_STRING_LITERAL("tool_bar.cmd.window.swap");

    mCommandButtonString[ID_FILE_NEW_FOLDER]          = XPR_STRING_LITERAL("tool_bar.cmd.new.folder");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_VIEW]     = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.view");
    mCommandButtonString[ID_TOOL_CMD]                 = XPR_STRING_LITERAL("tool_bar.cmd.command_prompt");
    mCommandButtonString[ID_TOOL_EMPTY_RECYCLEBIN]    = XPR_STRING_LITERAL("tool_bar.cmd.recycle_bin.empty");
    mCommandButtonString[ID_FILE_PRINT]               = XPR_STRING_LITERAL("tool_bar.cmd.print");
    mCommandButtonString[ID_VIEW_PICVIEWER]           = XPR_STRING_LITERAL("tool_bar.cmd.picture.show");
    mCommandButtonString[ID_FILE_VIEW]                = XPR_STRING_LITERAL("tool_bar.cmd.file_view");
    mCommandButtonString[ID_FILE_EDIT]                = XPR_STRING_LITERAL("tool_bar.cmd.file_edit");

    mCommandButtonString[ID_TOOL_OPTION]              = XPR_STRING_LITERAL("tool_bar.cmd.preferences");
    mCommandButtonString[ID_APP_ABOUT]                = XPR_STRING_LITERAL("tool_bar.cmd.about");

    //
    // file scrap toolbar
    //
    mCommandButtonString[ID_EDIT_FILE_SCRAP_GROUP_ADD]       = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.add_group");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_COPY_TO]         = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.copy_to");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_MOVE_TO]         = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.move_to");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_DELETE]          = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.delete");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_REFRESH]         = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.refresh");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_VALIDATE]        = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.validate");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_REMOVE_LIST]     = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.remove_list");
    mCommandButtonString[ID_EDIT_FILE_SCRAP_REMOVE_ALL_LIST] = XPR_STRING_LITERAL("tool_bar.cmd.file_scrap.remove_all_list");
}

const xpr_tchar_t *CommandStringTable::loadString(xpr_uint_t aId)
{
    if (aId == 0)
        return XPR_NULL;

    CommandStringMap::iterator sIterator = mCommandString.find(aId);
    if (sIterator == mCommandString.end())
    {
        XPR_TRACE(XPR_STRING_LITERAL("[CommandStringTable] String ID for %d command ID can not load."), aId);
        return XPR_NULL;
    }

    const xpr_tchar_t *sStringId = sIterator->second.c_str();

    return sStringId;
}

const xpr_tchar_t *CommandStringTable::loadButtonString(xpr_uint_t aId)
{
    if (aId == 0)
        return XPR_NULL;

    CommandStringMap::iterator sIterator = mCommandButtonString.find(aId);
    if (sIterator == mCommandButtonString.end())
    {
        XPR_TRACE(XPR_STRING_LITERAL("[CommandStringTable] String ID for button of %d command ID can not load."), aId);
        return XPR_NULL;
    }

    const xpr_tchar_t *sStringId = sIterator->second.c_str();

    return sStringId;
}
} // namespace fxfile
