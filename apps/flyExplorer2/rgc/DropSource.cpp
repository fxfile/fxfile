//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DropSource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

STDMETHODIMP DropSource::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
    if (iid == IID_IUnknown || iid == IID_IDropSource)
    {
        *ppv = this;
        ++mRefCount;
        return NOERROR;
    }
    *ppv = XPR_NULL;

    return E_NOINTERFACE;
}

STDMETHODIMP_(xpr_ulong_t) DropSource::AddRef(void)
{
    return ++mRefCount;
}

STDMETHODIMP_(xpr_ulong_t) DropSource::Release(void)
{
    if (--mRefCount == 0)
    {
        delete this;
        return 0;
    }
    return mRefCount;
}  

DropSource::DropSource(void)
{
    mRefCount = 1;
}

STDMETHODIMP DropSource::QueryContinueDrag(xpr_bool_t fEscapePressed, DWORD grfKeyState)
{
    HRESULT hr;
    if (fEscapePressed)// || (grfKeyState & MK_RBUTTON))
        hr = DRAGDROP_S_CANCEL;
    else if (!(grfKeyState & MK_LBUTTON) && !(grfKeyState & MK_RBUTTON))
        hr = DRAGDROP_S_DROP;
    else
        hr = NOERROR;
    return hr;
}

STDMETHODIMP DropSource::GiveFeedback(DWORD dwEffect)
{
    return DRAGDROP_S_USEDEFAULTCURSORS;
}
