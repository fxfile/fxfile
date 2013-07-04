//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CONTEXT_MENU_H__
#define __FXFILE_CONTEXT_MENU_H__ 1
#pragma once

namespace fxfile
{
#define CMID_DEF_ID_FIRST 0x01

#define CMID_VERB_OPEN       XPR_STRING_LITERAL("open")
#define CMID_VERB_OPENAS     XPR_STRING_LITERAL("openas")
#define CMID_VERB_EXPLORE    XPR_STRING_LITERAL("explore")
#define CMID_VERB_FIND       XPR_STRING_LITERAL("find")
#define CMID_VERB_PREVIEW    XPR_STRING_LITERAL("preview")
#define CMID_VERB_PRINT      XPR_STRING_LITERAL("print")
#define CMID_VERB_CUT        XPR_STRING_LITERAL("cut")
#define CMID_VERB_COPY       XPR_STRING_LITERAL("copy")
#define CMID_VERB_DELETE     XPR_STRING_LITERAL("delete")
#define CMID_VERB_RENAME     XPR_STRING_LITERAL("rename")
#define CMID_VERB_PASTE      XPR_STRING_LITERAL("paste")
#define CMID_VERB_LINK       XPR_STRING_LITERAL("link")
#define CMID_VERB_PROPERTIES XPR_STRING_LITERAL("properties")

#define CMID_FILE_SCRAP         0xFFFE
#define CMID_OPEN_PARENT_FOLDER 0xFFFF

typedef struct tagSubClassData
{
    LPCONTEXTMENU2 mContextMenu2;
    WNDPROC        mOldWndProc;
} SUBCLASSDATA, *LPSUBCLASSDATA;

class ContextMenu
{
public:
    ContextMenu(HWND aHwnd);
    virtual ~ContextMenu(void);

public:
    static void setFileScrapMenu(xpr_bool_t aShowFileScrapMenu);
    static void setAnimationMenu(xpr_bool_t aAnimationMenu);

public:
    xpr_bool_t init(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_uint_t aCount);
    xpr_bool_t init(LPSHELLFOLDER aShellFolder);
    xpr_bool_t getMenu(CMenu *aMenu, xpr_uint_t aFirstId = CMID_DEF_ID_FIRST, xpr_uint_t aQueryFlags = CMF_EXPLORE | CMF_CANRENAME);
    xpr_uint_t getFirstId(void);
    xpr_uint_t trackPopupMenu(xpr_uint_t aFlags, LPPOINT aPoint, xpr_uint_t aQueryFlags = CMF_EXPLORE | CMF_CANRENAME);
    xpr_bool_t invokeCommand(xpr_uint_t aId);
    xpr_bool_t invokeCommand(const xpr_tchar_t *aVerb);
    xpr_bool_t getCommandVerb(xpr_uint_t aId, xpr_tchar_t *aVerb, xpr_sint_t aMaxVerbLength);
    void getInterface(LPCONTEXTMENU *aContextMenu, LPCONTEXTMENU2 *aContextMenu2);
    void destroySubclass(void);
    void release(void);

    static xpr_bool_t trackItemMenu(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_uint_t aCount, LPPOINT aPoint, xpr_uint_t aFlags, HWND aWnd, xpr_uint_t aQueryFlags = CMF_EXPLORE | CMF_CANRENAME);
    static xpr_bool_t trackBackMenu(LPSHELLFOLDER aShellFolder, LPPOINT lppt, xpr_uint_t nFlags, HWND aWnd);
    static xpr_bool_t invokeCommand(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_uint_t aCount, xpr_uint_t aId, HWND aWnd);
    static xpr_bool_t invokeCommand(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_uint_t aCount, const xpr_tchar_t *aVerb, HWND aWnd);

protected:
    static LRESULT CALLBACK subclassWndProc(HWND aWnd, xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam);

protected:
    HWND           mHwnd;
    LPSHELLFOLDER  mShellFolder;
    LPCITEMIDLIST *mPidls;
    xpr_uint_t     mCount;

    HMENU          mMenu;
    LPCONTEXTMENU  mContextMenu;
    LPCONTEXTMENU2 mContextMenu2;
    LPSUBCLASSDATA mSubclassData;
    xpr_uint_t     mFirstId;

    static xpr_bool_t mShowFileScrapMenu;
    static xpr_bool_t mAnimationMenu;
};
} // namespace fxfile

#endif // __FXFILE_CONTEXT_MENU_H__
