//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SIZE_FORMAT_H__
#define __FXB_SIZE_FORMAT_H__
#pragma once

namespace fxb
{
#define MAX_SIZE_FORMAT               10
#define MAX_SIZE_FORMAT_DISP          50
#define MIN_SIZE_FORMAT_DECIAML_DIGIT 0
#define MAX_SIZE_FORMAT_DECIAML_DIGIT 9

class IniFileEx;

class SizeFormat : public xpr::Singleton<SizeFormat>
{
    friend class xpr::Singleton<SizeFormat>;

protected: SizeFormat(void);
public:   ~SizeFormat(void);

public:
    typedef struct Item
    {
        std::tstring mText;
        xpr_uint64_t mSize;
        xpr_sint_t   mSizeUnit;
        xpr_bool_t   mDefaultDecimalPlace;
        xpr_sint_t   mDecimalPlace;
        xpr_bool_t   mRoundOff;
    } Item;

public:
    static void setText(const xpr_tchar_t *aNoneText, const xpr_tchar_t *aAutoText, const xpr_tchar_t *aDefaultText, const xpr_tchar_t *aCustomText, const xpr_tchar_t *aByteUnitText);

    void initDefault(void);
    xpr_bool_t load(fxb::IniFileEx &aIniFile);
    void save(fxb::IniFileEx &aIniFile) const;

public:
    void addItem(Item *aItem);
    xpr_size_t getCount(void);
    Item *getItem(xpr_size_t aIndex);
    void clear(void);

    xpr_bool_t getSizeFormat(xpr_uint64_t aSize, xpr_tchar_t *aText, xpr_size_t aMaxText);

    static xpr_bool_t getDefSizeUnit(xpr_uint64_t aSize, xpr_sint_t aUnit, xpr_tchar_t *aText, xpr_size_t aMaxText);
    static const xpr_tchar_t *getDefUnitText(xpr_sint_t aUnit);
    static void getDefSizeFormat(xpr_uint64_t aSize, xpr_tchar_t *aFormatedSize, xpr_size_t aMaxLen);
    static void getSizeUnitFormat(xpr_uint64_t aSize, xpr_sint_t aUnit, xpr_tchar_t *aFormatedSize, xpr_size_t aMaxLen);
    static void validSizeUnit(xpr_sint_t &aUnit);

protected:
    static std::tstring mNoneText;
    static std::tstring mAutoText;
    static std::tstring mDefaultText;
    static std::tstring mCustomText;
    static std::tstring mByteUnitText;

    typedef std::deque<Item *> FormatDeque;
    FormatDeque mFormatDeque;
};
} // namespace fxb

#endif // __FXB_SIZE_FORMAT_H__
