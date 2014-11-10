//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_THUMBNAIL_H__
#define __FXFILE_THUMBNAIL_H__ 1
#pragma once

#include "pattern.h"
#include "thread.h"
#include "thumb_img_list.h"

namespace fxfile
{
enum
{
    THUMBNAIL_PRIORITY_BELOW_NORMAL = 0,
    THUMBNAIL_PRIORITY_ABOVE_NORMAL,
    THUMBNAIL_PRIORITY_NORMAL
};

class Thumbnail : public xpr::Thread::Runnable, public fxfile::base::Singleton<Thumbnail>
{
    friend class fxfile::base::Singleton<Thumbnail>;

public:
    enum { InvalidThumbImageId = 0xffffffff };

protected: Thumbnail(void);
public:   ~Thumbnail(void);

public:
    void create(void);
    void destroy(void);

    void start(void);
    void stop(void);

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
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

    inline xpr_sint_t IdToImage(xpr_uint_t aThumbImageId);

protected:
    struct ThumbItem;

    typedef std::deque<ThumbItem *> ThumbDeque;
    ThumbDeque mThumbDeque;

    typedef std::map<xpr_uint_t, xpr_sint_t> IdMap;
    IdMap      mIdMap;
    xpr_uint_t mIdMgr;

    Thread     mThread;
    HANDLE     mEvent;
    xpr::Mutex mMutex;
    static DWORD WINAPI ThumbThreadProc(LPVOID lpParam);

    CSize         mThumbSize;
    ThumbImgList *mThumbImgList;
};
} // namespace fxfile

#endif // __FXFILE_THUMBNAIL_H__
