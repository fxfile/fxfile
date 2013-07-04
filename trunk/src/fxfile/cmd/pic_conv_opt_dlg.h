//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PIC_CONV_OPT_DLG_H__
#define __FXFILE_PIC_CONV_OPT_DLG_H__ 1
#pragma once

#include "gfl/libgfl.h"

namespace fxfile
{
namespace cmd
{
enum
{
    JPEG = 0,
    TGA,
    TIFF,
    BMP,
    IFF,
    PNG,
    GIF,
    PCX,
    DCX,
    PBM,
    PGM,
    PPM,
    PNM,
    ICO,
    SOFT,
    SGI,
    MIFF,
    XBM,
    XPM,
    PSION3,
    PSION5,
    PALM,
    EMF,
    PS,
    VISTA,
    ALIAS,
    RLA,
    CIN,
    DPX,
    HRU,
    DKB,
    QRT,
    VIVID,
    MTV,
    RAY,
    JIF,
    GPAT,
    GROB,
    TI,
    NGG,
    FITS,
    DEGAS,
    BIORAD,
    RAD,
    PABX,
    PRC,
    WRL,
    PCL,
    WBMP,
    ARCIB,
    UYVYI,
    UYVY,
    RAW,
};

#define MAX_ID_GROUP_COUNT 20
#define MAX_ID_COUNT       5
#define MAX_FORMAT_COUNT   100
#define MAX_FORMAT_CONTROL 5

class PicConvOptDlg : public CDialog
{
    typedef CDialog super;

public:
    PicConvOptDlg(xpr_sint_t       aFormat,
                  GFL_SAVE_PARAMS *aGflSaveParams,
                  xpr_bool_t      *aLossless,
                  xpr_sint_t      *aPaletteTransparent);

    xpr_bool_t isFormatOption(void) { return (mFormats[mFormat][0]) ? XPR_TRUE : XPR_FALSE; }

protected:
    xpr_uint_t mIds[MAX_ID_GROUP_COUNT][MAX_ID_COUNT];
    xpr_uint_t mFormats[MAX_FORMAT_COUNT][MAX_FORMAT_CONTROL];
    xpr_sint_t mFormat;
    GFL_SAVE_PARAMS *mGflSaveParams;
    xpr_sint_t *mPaletteTransparent;
    xpr_bool_t *mLossless;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnHScroll(xpr_uint_t nSBCode, xpr_uint_t nPos, CScrollBar* pScrollBar);
    afx_msg void OnUpdateQuality(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_PIC_CONV_OPT_DLG_H__
