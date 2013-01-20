//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_TAB_PANE_H__
#define __FX_TAB_PANE_H__
#pragma once

#include "TabType.h"

class TabPaneObserver;
class StatusBar;

class TabPane : public CWnd
{
public:
    TabPane(TabType aTabType);
    virtual ~TabPane(void);

public:
    xpr_sint_t getViewIndex(void) const;
    void       setViewIndex(xpr_sint_t aViewIndex);

    void setObserver(TabPaneObserver *aObserver);

public:
    enum { InvalidId = -1 };

    virtual xpr_bool_t isSharedPane(void) const;
    virtual xpr_bool_t canDuplicatePane(void) const;
    virtual TabType    getTabType(void) const;
    virtual xpr_bool_t isTabType(TabType aTabType) const;

    virtual CWnd *     newSubPane(xpr_uint_t aId) = 0;
    virtual CWnd *     getSubPane(xpr_uint_t aId = InvalidId) const = 0;
    virtual xpr_size_t getSubPaneCount(void) const = 0;
    virtual xpr_uint_t getCurSubPaneId(void) const = 0;
    virtual xpr_uint_t setCurSubPane(xpr_uint_t aId) = 0;
    virtual void       destroySubPane(xpr_uint_t aId) = 0;
    virtual void       destroySubPane(void) = 0;

    virtual StatusBar *getStatusBar(void) const = 0;
    virtual const xpr_tchar_t *getStatusPaneText(xpr_sint_t aIndex) const = 0;

protected:
    xpr_bool_t registerWindowClass(const xpr_tchar_t *aClassName);

protected:
    TabPaneObserver *mObserver;

    xpr_sint_t mViewIndex;
    TabType    mTabType;
};

#endif // __FX_TAB_PANE_H__
