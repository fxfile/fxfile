//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_MEM_DC_H__
#define __FXFILE_MEM_DC_H__
#pragma once

class CMemDC : public CDC
{
public:
    CMemDC(CDC *aDC)
        : CDC()
    {
        XPR_ASSERT(aDC != XPR_NULL);

        mDC         = aDC;
        mOldBitmap  = XPR_NULL;
        mMemDC      = !aDC->IsPrinting();
        mAlphaBlend = XPR_FALSE;
        mAlpha      = 0;

        if (XPR_IS_TRUE(mMemDC))
        {
            aDC->GetClipBox(&mRect);

            CreateCompatibleDC(aDC);
            mBitmap.CreateCompatibleBitmap(aDC, mRect.Width(), mRect.Height());

            mOldBitmap = SelectObject(&mBitmap);

            SetWindowOrg(mRect.left, mRect.top);
        }
        else
        {
            m_bPrinting = aDC->m_bPrinting;
            m_hDC       = aDC->m_hDC;
            m_hAttribDC = aDC->m_hAttribDC;
        }
    }

    virtual ~CMemDC(void)
    {
        if (XPR_IS_TRUE(mAlphaBlend))
        {
            BLENDFUNCTION sBlendFunction = {0};
            sBlendFunction.BlendOp             = AC_SRC_OVER;
            sBlendFunction.BlendFlags          = 0;
            sBlendFunction.SourceConstantAlpha = mAlpha;
            sBlendFunction.AlphaFormat         = 0;

            CDC sTempDC;
            sTempDC.CreateCompatibleDC(mDC);

            CBitmap sBitmap, *sOldBitmap;
            sBitmap.CreateCompatibleBitmap(&sTempDC, mRect.Width(), mRect.Height());

            sOldBitmap = sTempDC.SelectObject(&sBitmap);

            sTempDC.FillSolidRect(0, 0, mRect.Width(), mRect.Height(), RGB(255,255,255));

            ::AlphaBlend(
                m_hDC,
                mRect.left, mRect.top, mRect.Width(), mRect.Height(),
                sTempDC.m_hDC,
                0, 0, mRect.Width(), mRect.Height(),
                sBlendFunction);

            sTempDC.SelectObject(sOldBitmap);
        }

        if (XPR_IS_TRUE(mMemDC))
        {
            mDC->BitBlt(mRect.left, mRect.top, mRect.Width(), mRect.Height(), this, mRect.left, mRect.top, SRCCOPY);
            SelectObject(mOldBitmap);
        }
        else
        {
            m_hDC = m_hAttribDC = XPR_NULL;
        }
    }

public:
    void setAlphaBlend(xpr_bool_t aAlphaBlend, xpr_sint_t aAlpha)
    {
        mAlphaBlend = aAlphaBlend;
        mAlpha      = aAlpha;
    }

    CMemDC *operator->() { return this; }

    operator CMemDC*() { return this; }

protected:
    CDC       *mDC;
    CBitmap    mBitmap;
    CBitmap   *mOldBitmap;
    CRect      mRect;
    xpr_bool_t mMemDC;

    xpr_bool_t mAlphaBlend;
    xpr_sint_t mAlpha;
};

#endif // __FXFILE_MEM_DC_H__
