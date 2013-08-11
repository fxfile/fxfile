//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_dlg.h"
#include "cfg_dlg_observer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgDlg::CfgDlg(xpr_uint_t aResourceId, CWnd *aParentWnd)
    : super(aResourceId, aParentWnd)
    , mObserver(XPR_NULL)
    , mResourceId(aResourceId), mCfgIndex(0)
{
}

void CfgDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

CfgDlgObserver *CfgDlg::getObserver(void) const
{
    return mObserver;
}

void CfgDlg::setObserver(CfgDlgObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_bool_t CfgDlg::Create(xpr_size_t aCfgIndex, CWnd *aParentWnd)
{
    mCfgIndex = aCfgIndex;

    return super::Create(mResourceId, aParentWnd);
}

BEGIN_MESSAGE_MAP(CfgDlg, super)
END_MESSAGE_MAP()

void CfgDlg::addIgnoreApply(xpr_uint_t aId)
{
    mIgnoreApplySet.insert(aId);
}

xpr_size_t CfgDlg::getCfgIndex(void) const
{
    return mCfgIndex;
}

xpr_bool_t CfgDlg::isModified(void)
{
    if (XPR_IS_NULL(mObserver))
        return XPR_FALSE;

    return mObserver->onIsModified(*this);
}

void CfgDlg::setModified(xpr_bool_t aModified)
{
    mObserver->onSetModified(*this, aModified);
}

xpr_bool_t CfgDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED ||
        sNotifyMsg == EN_UPDATE  ||
        sNotifyMsg == CBN_SELCHANGE)
    {
        if (mIgnoreApplySet.find(sId) == mIgnoreApplySet.end())
        {
            setModified();
        }
    }

    return super::OnCommand(wParam, lParam);
}

xpr_bool_t CfgDlg::PreTranslateMessage(MSG *aMsg) 
{
    return super::PreTranslateMessage(aMsg);
}
} // namespace cfg
} // namespace fxfile
