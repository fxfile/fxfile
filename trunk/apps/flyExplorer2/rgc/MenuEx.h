//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv2 license that can be
// found in the LICENSE file.

#ifndef __MENU_EX_H__
#define __MENU_EX_H__
#pragma once

namespace rgc
{
class MenuItemInfo : public MENUITEMINFO
{
public:
    MenuItemInfo(void)
    {
        xpr::MemZeroSet(this, sizeof(MENUITEMINFO));
        cbSize = sizeof(MENUITEMINFO);
    }
};

class MenuEx : public CMenu
{
    DECLARE_DYNCREATE(MenuEx)

protected:
    enum { InvalidImageIndex = -1 };

    class MenuData;

public:
    MenuEx(void); 
    virtual ~MenuEx(void);

public:
    BOOL LoadMenu(LPCTSTR lpszResourceName);
    BOOL LoadMenu(UINT nIDResource);

    BOOL LoadToolbar(UINT nToolBar);

	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem, CImageList *aImageList = XPR_NULL, xpr_sint_t aImage = InvalidImageIndex);
	BOOL InsertMenuItem(UINT uItem, LPMENUITEMINFO lpMenuItemInfo, BOOL fByPos = FALSE, CImageList *aImageList = XPR_NULL, xpr_sint_t aImage = InvalidImageIndex);
	BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem, CImageList *aImageList = XPR_NULL, xpr_sint_t aImage = InvalidImageIndex);
	BOOL AppendPopupMenu(LPCTSTR lpszNewItem);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
    BOOL RemoveMenu(UINT nPosition, UINT nFlags);

    BOOL GetMenuText(UINT nPosition, CString &strText, UINT nFlags = MF_BYPOSITION );
    BOOL GetMenuText(UINT nPosition, LPTSTR lpszText, UINT nFlags = MF_BYPOSITION);
    BOOL SetMenuText(UINT nPosition, const CString &strText, UINT nFlags = MF_BYPOSITION);
    BOOL SetMenuText(UINT nPosition, LPCTSTR lpcszText, UINT nFlags = MF_BYPOSITION);
    BOOL SetMenuAccelKey(UINT nPosition, LPCTSTR lpcszAccelKey, UINT nFlags = MF_BYPOSITION);

    BOOL TrackPopupMenu(UINT nFlags, CPoint pt, CWnd* pWnd, LPCRECT lpRect = 0);
    BOOL TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect = 0);

    BOOL DestroyMenu(void);

public:
    static LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu *pMenu);

    static BOOL IsMenu(CMenu *pSubMenu);
    static BOOL IsMenu(HMENU hSubMenu);

protected:
    MenuData *GetMenuData(UINT nID);

    BOOL ImageListDuplicate(CImageList *il,int xoffset,CImageList *newlist);

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

    void DrawCheckMark(CDC *pDC,int x,int y,COLORREF color);
    void DrawRadioDot(CDC *pDC,int x,int y,COLORREF color);
    BOOL Draw3DCheckmark(CDC *dc, const CRect& rc,BOOL bSelected, HBITMAP hbmCheck);
    void DrawItemStandardStyle(LPDRAWITEMSTRUCT lpDIS);
    void DrawItemOffice2003Style(LPDRAWITEMSTRUCT lpDIS);
    void DrawItemOffice2007Style(LPDRAWITEMSTRUCT lpDIS);

protected:
    typedef std::set<HMENU> MenuSet;
    MenuSet mSubMenus;
    static MenuSet mAllSubMenus;

    typedef std::deque<MenuData *> MenuDataDeque;
    MenuDataDeque mMenuDatas;

    xpr_sint_t mIconX;
    xpr_sint_t mIconY;
};
} // namespace rgc

#endif // __MENU_EX_H__
