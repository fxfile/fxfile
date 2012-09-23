//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SHELL_NEW_H__
#define __FXB_SHELL_NEW_H__
#pragma once

namespace fxb
{
class ShellNew : public xpr::Singleton<ShellNew>
{
    friend class xpr::Singleton<ShellNew>;

protected: ShellNew(void);
public:   ~ShellNew(void);

public:
    enum Type
    {
        TypeNone = 0,
        TypeNullFile,
        TypeFileName,
        TypeData,
        TypeCommand,
    };

    typedef struct Item
    {
        Type         mType;
        std::tstring mExt;
        xpr_sint_t   mIconIndex;
        std::tstring mFileType;
        std::tstring mRegKey;
        std::tstring mRegValue;
        DWORD        mRegValueType;
    } Item;

public:
    void setBaseCommandId(xpr_uint_t aId);
    void setText(const xpr_tchar_t *aPrefix, const xpr_tchar_t *aSuffix);

    void getRegShellNew(void);
    xpr_size_t getCount(void);
    Item *getItem(xpr_size_t aIndex);
    void clear(void);

    xpr_bool_t doCommand(xpr_uint_t aId, const xpr_tchar_t *aDir, xpr_tchar_t *aPath);

protected:
    xpr_uint_t   mBaseId;
    std::tstring mPrefix;
    std::tstring mSuffix;

    typedef std::map<xpr_uint_t, Item *> ShellNewMap;
    typedef std::deque<Item *> ShellNewDeque;
    ShellNewMap   mShellNewMap;
    ShellNewDeque mShellNewDeque;
};
} // namespace fxb

#endif // __FXB_SHELL_NEW_H__
