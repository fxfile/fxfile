//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_ATTR_TIME_H__
#define __FXB_ATTR_TIME_H__
#pragma once

namespace fxb
{
class AttrTime : public fxb::Thread
{
public:
    enum Status
    {
        StatusNone,
        StatusChanging,
        StatusChangeCompleted,
        StatusStopped,
    };

    enum
    {
        TypeFile   = 0x00000001,
        TypeFolder = 0x00000002,
    };

public:
    AttrTime(void);
    virtual ~AttrTime(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

    void setSubFolder(xpr_bool_t aSubFolder, xpr_sint_t aDepth);
    void setType(xpr_uint_t aType);
    void setAttributes(xpr_bool_t aAttribute, xpr_sint_t aArchive, xpr_sint_t aReadOnly, xpr_sint_t aHidden, xpr_sint_t aSystem);
    void setTime(LPSYSTEMTIME aCreatedSystemTime, LPSYSTEMTIME aModifiedSystemTime, LPSYSTEMTIME aAccessSystemTime);

    void addPath(const std::tstring &aPath);

    xpr_size_t getCount(void);
    const xpr_tchar_t *getPath(xpr_sint_t aIndex);
    Status getStatus(xpr_size_t *aProcessedCount = XPR_NULL, xpr_size_t *aSucceededCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

    xpr_bool_t OnRcsvDirAttrTimeProc(const std::tstring &aDir, DWORD aAttributes, xpr_sint_t aDepth);
    xpr_bool_t OnAttrTimeProc(const std::tstring &aPath, DWORD aAttributes);

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;

    typedef std::deque<std::tstring> PathDeque;
    PathDeque  mPathDeque;

    Status     mStatus;
    xpr_size_t mProcessedCount;
    xpr_size_t mSucceededCount;
    Cs         mCs;

protected:
    // recursive function information
    xpr_uint_t mType;
    xpr_bool_t mSubFolder;
    xpr_sint_t mDepth;
    xpr_bool_t mAttribute;
    xpr_sint_t mArchive;
    xpr_sint_t mReadOnly;
    xpr_sint_t mHidden;
    xpr_sint_t mSystem;
    LPFILETIME mCreatedFileTime;
    LPFILETIME mModifiedFileTime;
    LPFILETIME mAccessFileTime;
};
} // namespace fxb

#endif // __FXB_ATTR_TIME_H__
