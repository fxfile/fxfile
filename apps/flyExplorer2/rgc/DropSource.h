//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DROP_SOURCE_H__
#define __FX_DROP_SOURCE_H__
#pragma once

class DropSource : public IDropSource
{
public:
    DropSource(void);

public:
    /* IUnknown methods */
    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppvObj);
    STDMETHOD_(xpr_ulong_t, AddRef)(void);
    STDMETHOD_(xpr_ulong_t, Release)(void);

public:
    /* IDropSource methods */
    STDMETHOD(QueryContinueDrag)(xpr_bool_t fEscapePressed, DWORD grfKeyState);
    STDMETHOD(GiveFeedback)(DWORD dwEffect);

private:
    xpr_ulong_t mRefCount;
};

#endif // __FX_DROP_SOURCE_H__
