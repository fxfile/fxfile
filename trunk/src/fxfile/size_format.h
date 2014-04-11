//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SIZE_FORMAT_H__
#define __FXFILE_SIZE_FORMAT_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
namespace base
{
class ConfFileEx;
} // namespace base
} // namespace fxfile

namespace fxfile
{
#define MAX_SIZE_FORMAT               10
#define MAX_SIZE_FORMAT_DISP          50
#define MIN_SIZE_FORMAT_DECIAML_DIGIT 0
#define MAX_SIZE_FORMAT_DECIAML_DIGIT 9

class SizeFormat : public fxfile::base::Singleton<SizeFormat>
{
    friend class fxfile::base::Singleton<SizeFormat>;

protected: SizeFormat(void);
public:   ~SizeFormat(void);

public:
    typedef struct Item
    {
        xpr::string  mText;
        xpr_uint64_t mSize;
        xpr_sint_t   mSizeUnit;
        xpr_bool_t   mDefaultDecimalPlace;
        xpr_sint_t   mDecimalPlace;
        xpr_bool_t   mRoundOff;
    } Item;

public:
    static void setText(const xpr_tchar_t *aNoneText, const xpr_tchar_t *aAutoText, const xpr_tchar_t *aDefaultText, const xpr_tchar_t *aCustomText, const xpr_tchar_t *aByteUnitText);

    void initDefault(void);
    xpr_bool_t load(fxfile::base::ConfFileEx &aConfFile);
    void save(fxfile::base::ConfFileEx &aConfFile) const;

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
    static void getFormatedNumber(xpr_uint64_t aNumber, xpr_tchar_t *aFormatedNumber, xpr_size_t aMaxLen);

protected:
    static const xpr_tchar_t *mNoneText;
    static const xpr_tchar_t *mAutoText;
    static const xpr_tchar_t *mDefaultText;
    static const xpr_tchar_t *mCustomText;
    static const xpr_tchar_t *mByteUnitText;

    typedef std::deque<Item *> FormatDeque;
    FormatDeque mFormatDeque;
};
} // namespace fxfile

#endif // __FXFILE_SIZE_FORMAT_H__
