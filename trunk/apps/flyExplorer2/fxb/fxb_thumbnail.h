//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_THUMBNAIL_H__
#define __FX_THUMBNAIL_H__
#pragma once

#include "fxb_thumb_img_list.h"

namespace fxb
{
enum
{
    THUMBNAIL_PRIORITY_BELOW_NORMAL = 0,
    THUMBNAIL_PRIORITY_ABOVE_NORMAL,
    THUMBNAIL_PRIORITY_NORMAL
};

class Thumbnail : public fxb::Thread, public xpr::Singleton<Thumbnail>
{
    friend class xpr::Singleton<Thumbnail>;

public:
    enum { InvalidThumbImageId = 0xffffffff };

protected: Thumbnail(void);
public:   ~Thumbnail(void);

public:
    void create(void);
    void destroy(void);

    xpr_bool_t loadCache(void);
    void saveCache(void);

    xpr_uint_t addImage(ThumbImage *aThumbImage);
    xpr_bool_t getImageId(const xpr_tchar_t *aPath, xpr_uint_t &aThumbImageId);
    xpr_bool_t getAsyncImage(HWND aHwnd, xpr_uint_t aMsg, xpr_uint_t aCode, const xpr_tchar_t *aPath, xpr_bool_t aLoadByExt);
    void drawImageId(CDC *aDC, xpr_uint_t aThumbImageId, POINT aPoint, xpr_uint_t aStyle);
    void removeImageId(xpr_uint_t aThumbImageId);

    CSize getThumbSize(void);
    ThumbImgList *getThumbImgList(void) const;
    void setThumbSize(CSize aThumbSize);
    void setThumbPriority(xpr_sint_t aPriority);

    void verifyThumb(void);
    void initThumb(void);

    void clearAysncImage(void);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

    inline xpr_sint_t IdToImage(xpr_uint_t aThumbImageId);

protected:
    struct ThumbItem;

    typedef std::deque<ThumbItem *> ThumbDeque;
    ThumbDeque mThumbDeque;

    typedef std::map<xpr_uint_t, xpr_sint_t> IdMap;
    IdMap      mIdMap;
    xpr_uint_t mIdMgr;

    HANDLE mEvent;
    Cs     mCs;
    static DWORD WINAPI ThumbThreadProc(LPVOID lpParam);

    CSize         mThumbSize;
    ThumbImgList *mThumbImgList;
};
} // namespace fxb

#endif // __FX_THUMBNAIL_H__
