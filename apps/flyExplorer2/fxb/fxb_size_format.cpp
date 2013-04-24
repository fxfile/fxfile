//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_size_format.h"

#include "fxb_ini_file_ex.h"
#include <math.h>

#include "../Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
static const xpr_uint64_t SIZE_KB = 1024ui64;
static const xpr_uint64_t SIZE_MB = 1024ui64 * 1024ui64;
static const xpr_uint64_t SIZE_GB = 1024ui64 * 1024ui64 * 1024ui64;
static const xpr_uint64_t SIZE_TB = 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64;
static const xpr_uint64_t SIZE_PB = 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64;
static const xpr_uint64_t SIZE_EB = 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64;

static const xpr_tchar_t kSizeFormatSection[]      = XPR_STRING_LITERAL("size_format");
static const xpr_tchar_t kSizeKey[]                = XPR_STRING_LITERAL("size_format.item%d.size");
static const xpr_tchar_t kUnitKey[]                = XPR_STRING_LITERAL("size_format.item%d.unit");
static const xpr_tchar_t kDisplayKey[]             = XPR_STRING_LITERAL("size_format.item%d.display");
static const xpr_tchar_t kDefaultDecimalPlaceKey[] = XPR_STRING_LITERAL("size_format.item%d.default_decimal_place");
static const xpr_tchar_t kDecimalPlaceKey[]        = XPR_STRING_LITERAL("size_format.item%d.decimal_place");
static const xpr_tchar_t kRoundOffKey[]            = XPR_STRING_LITERAL("size_format.item%d.round_off");

std::tstring SizeFormat::mNoneText;
std::tstring SizeFormat::mAutoText;
std::tstring SizeFormat::mDefaultText;
std::tstring SizeFormat::mCustomText;
std::tstring SizeFormat::mByteUnitText;

SizeFormat::SizeFormat(void)
{
}

SizeFormat::~SizeFormat(void)
{
    clear();
}

void SizeFormat::clear(void)
{
    FormatDeque::iterator sIterator;
    Item *sItem;

    sIterator = mFormatDeque.begin();
    for (; sIterator != mFormatDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        XPR_SAFE_DELETE(sItem);
    }

    mFormatDeque.clear();
}

void SizeFormat::addItem(Item *aItem)
{
    if (aItem != XPR_NULL)
        mFormatDeque.push_back(aItem);
}

xpr_size_t SizeFormat::getCount(void)
{
    return mFormatDeque.size();
}

SizeFormat::Item *SizeFormat::getItem(xpr_size_t nIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(nIndex, mFormatDeque))
        return XPR_NULL;

    return mFormatDeque[nIndex];
}

xpr_bool_t SizeFormat::getSizeFormat(xpr_uint64_t aSize, xpr_tchar_t *aText, xpr_size_t aMaxText)
{
    if (aText == XPR_NULL || aMaxText <= 0)
        return XPR_FALSE;

    aText[0] = XPR_STRING_LITERAL('\0');

    xpr_uint64_t sUnit;
    Item *sItem;
    Item *sTargetSizeUnit;
    FormatDeque::iterator sIterator;

    sTargetSizeUnit = XPR_NULL;

    sIterator = mFormatDeque.begin();
    for (; sIterator != mFormatDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (sItem == XPR_NULL)
            continue;

        sUnit = 0;

        switch (sItem->mSizeUnit)
        {
        case SIZE_UNIT_KB: sUnit = SIZE_KB; break;
        case SIZE_UNIT_MB: sUnit = SIZE_MB; break;
        case SIZE_UNIT_GB: sUnit = SIZE_GB; break;
        case SIZE_UNIT_TB: sUnit = SIZE_TB; break;
        case SIZE_UNIT_PB: sUnit = SIZE_PB; break;
        case SIZE_UNIT_EB: sUnit = SIZE_EB; break;
        }

        if ((sUnit * sItem->mSize) > aSize)
            break;

        sTargetSizeUnit = sItem;
    }

    if (sTargetSizeUnit == XPR_NULL)
    {
        GetFormatedNumber(aSize, aText, aMaxText);
        return XPR_TRUE;
    }

    xpr_double_t sUnitSize = (xpr_double_t)aSize;

    switch (sTargetSizeUnit->mSizeUnit)
    {
    case SIZE_UNIT_KB: sUnitSize /= SIZE_KB; break;
    case SIZE_UNIT_MB: sUnitSize /= SIZE_MB; break;
    case SIZE_UNIT_GB: sUnitSize /= SIZE_GB; break;
    case SIZE_UNIT_TB: sUnitSize /= SIZE_TB; break;
    case SIZE_UNIT_PB: sUnitSize /= SIZE_PB; break;
    case SIZE_UNIT_EB: sUnitSize /= SIZE_EB; break;
    }

    NUMBERFMT sNumberFmt = {0};
    sNumberFmt.NumDigits     = 2;
    sNumberFmt.LeadingZero   = 1;
    sNumberFmt.Grouping      = 3;
    sNumberFmt.lpDecimalSep  = XPR_STRING_LITERAL(".");
    sNumberFmt.lpThousandSep = XPR_STRING_LITERAL(",");
    sNumberFmt.NegativeOrder = 0;

    xpr_tchar_t sSizeText[0xff] = {0};

    if (XPR_IS_TRUE(sTargetSizeUnit->mDefaultDecimalPlace))
    {
        if (sTargetSizeUnit->mSizeUnit == SIZE_UNIT_BYTE || sTargetSizeUnit->mSizeUnit == SIZE_UNIT_NONE)
            sNumberFmt.NumDigits = 0;
        else
        {
            if (sUnitSize > 100 || sUnitSize == 0)
                sNumberFmt.NumDigits = 0;
            else if (sUnitSize > 10)
                sNumberFmt.NumDigits = 1;
        }

        _stprintf(sSizeText, XPR_STRING_LITERAL("%.3f"), sUnitSize);

        const xpr_size_t sLen = _tcslen(sSizeText);
        if (sLen > 0)
            sSizeText[sLen-(3-sNumberFmt.NumDigits)] = XPR_STRING_LITERAL('\0');
    }
    else
    {
        sNumberFmt.NumDigits = sTargetSizeUnit->mDecimalPlace;
        xpr_uint_t sDecimalPlace = sTargetSizeUnit->mDecimalPlace;

        if (sItem->mRoundOff == XPR_FALSE)
            sDecimalPlace++;

        xpr_tchar_t sFormat[0xff] = {0};
        _stprintf(sFormat, XPR_STRING_LITERAL("%%.%df"), sDecimalPlace);
        _stprintf(sSizeText, sFormat, sUnitSize);

        if (sItem->mRoundOff == XPR_FALSE)
        {
            if (_tcslen(sSizeText) > 0)
                sSizeText[_tcslen(sSizeText)-1] = XPR_STRING_LITERAL('\0');
        }
    }

    ::GetNumberFormat(XPR_NULL, XPR_NULL, sSizeText, &sNumberFmt, aText, (xpr_sint_t)aMaxText);

    _tcscat(aText, sTargetSizeUnit->mText.c_str());

    return XPR_TRUE;
}

xpr_bool_t SizeFormat::getDefSizeUnit(xpr_uint64_t aSize, xpr_sint_t aUnit, xpr_tchar_t *aText, xpr_size_t aMaxText)
{
    if (aText == XPR_NULL || aMaxText <= 0)
        return XPR_FALSE;

    GetFormatedNumber(aSize, aText, aMaxText);

    if (aUnit != SIZE_UNIT_NONE)
    {
        const xpr_tchar_t *sUnitText = getDefUnitText(aUnit);
        if (sUnitText != XPR_NULL)
        {
            _tcscat(aText, XPR_STRING_LITERAL(" "));
            _tcscat(aText, sUnitText);
        }
    }

    return XPR_TRUE;
}

void SizeFormat::setText(const xpr_tchar_t *aNoneText,
                         const xpr_tchar_t *aAutoText,
                         const xpr_tchar_t *aDefaultText,
                         const xpr_tchar_t *aCustomText,
                         const xpr_tchar_t *aByteUnitText)
{
    if (XPR_IS_NOT_NULL(aNoneText    )) mNoneText     = aNoneText;
    if (XPR_IS_NOT_NULL(aAutoText    )) mAutoText     = aAutoText;
    if (XPR_IS_NOT_NULL(aDefaultText )) mDefaultText  = aDefaultText;
    if (XPR_IS_NOT_NULL(aCustomText  )) mCustomText   = aCustomText;
    if (XPR_IS_NOT_NULL(aByteUnitText)) mByteUnitText = aByteUnitText;
}

const xpr_tchar_t *SizeFormat::getDefUnitText(xpr_sint_t aUnit)
{
    switch (aUnit)
    {
    case SIZE_UNIT_DEFAULT: return (mDefaultText.empty()  == false) ? mDefaultText.c_str()  : XPR_STRING_LITERAL("Default");
    case SIZE_UNIT_AUTO:    return (mAutoText.empty()     == false) ? mAutoText.c_str()     : XPR_STRING_LITERAL("Automatic");
    case SIZE_UNIT_CUSTOM:  return (mCustomText.empty()   == false) ? mCustomText.c_str()   : XPR_STRING_LITERAL("Custom");
    case SIZE_UNIT_NONE:    return (mNoneText.empty()     == false) ? mNoneText.c_str()     : XPR_STRING_LITERAL("None");
    case SIZE_UNIT_BYTE:    return (mByteUnitText.empty() == false) ? mByteUnitText.c_str() : XPR_STRING_LITERAL("Byte(s)");
    case SIZE_UNIT_KB:      return XPR_STRING_LITERAL("KB");
    case SIZE_UNIT_MB:      return XPR_STRING_LITERAL("MB");
    case SIZE_UNIT_GB:      return XPR_STRING_LITERAL("GB");
    case SIZE_UNIT_TB:      return XPR_STRING_LITERAL("TB");
    case SIZE_UNIT_PB:      return XPR_STRING_LITERAL("PB");
    case SIZE_UNIT_EB:      return XPR_STRING_LITERAL("EB");
    case SIZE_UNIT_ZB:      return XPR_STRING_LITERAL("ZB");
    case SIZE_UNIT_YB:      return XPR_STRING_LITERAL("YB");
    }

    return XPR_NULL;
}

void SizeFormat::validSizeUnit(xpr_sint_t &aUnit)
{
    switch (aUnit)
    {
    case SIZE_UNIT_DEFAULT:
    case SIZE_UNIT_AUTO:
    case SIZE_UNIT_CUSTOM:
    case SIZE_UNIT_NONE:
    case SIZE_UNIT_BYTE:
    case SIZE_UNIT_KB:
    case SIZE_UNIT_MB:
    case SIZE_UNIT_GB:
    case SIZE_UNIT_TB:
    case SIZE_UNIT_PB:
    case SIZE_UNIT_EB:
    case SIZE_UNIT_ZB:
    case SIZE_UNIT_YB: break;

    default:
        aUnit = SIZE_UNIT_DEFAULT;
        break;
    }
}

void SizeFormat::getDefSizeFormat(xpr_uint64_t aSize, xpr_tchar_t *aFormatedSize, xpr_size_t aMaxLen)
{
    if (aFormatedSize == XPR_NULL || aMaxLen <= 0)
        return;

    static NUMBERFMT sNumberFmt = { 0, 0, 3, XPR_STRING_LITERAL("."), XPR_STRING_LITERAL(","), 0 };

    const xpr_uint64_t sSizeBy1024 = aSize / 1024;
    if (sSizeBy1024 > 1)
    {
        xpr_tchar_t sSizeText[0xff] = {0};
        _stprintf(sSizeText, XPR_STRING_LITERAL("%I64u"), sSizeBy1024);

        ::GetNumberFormat(XPR_NULL, XPR_NULL, sSizeText, &sNumberFmt, aFormatedSize, 32);
        _tcscat(aFormatedSize, XPR_STRING_LITERAL(" KB"));
    }
    else if (aSize == 0ui64)
        _stprintf(aFormatedSize, XPR_STRING_LITERAL("0 KB"));
    else
        _stprintf(aFormatedSize, XPR_STRING_LITERAL("1 KB"));
}

void SizeFormat::getSizeUnitFormat(xpr_uint64_t aSize, xpr_sint_t aUnit, xpr_tchar_t *aFormatedSize, xpr_size_t aMaxLen)
{
    if (aFormatedSize == XPR_NULL || aMaxLen <= 0)
        return;

    aFormatedSize[0] = XPR_STRING_LITERAL('\0');

    xpr_sint_t i;
    xpr_double_t sUnitSize = (xpr_double_t)aSize;

    for (i = SIZE_UNIT_BYTE; i <= SIZE_UNIT_EB; ++i)
    {
        if ((i == aUnit) || (aUnit == SIZE_UNIT_AUTO && sUnitSize < 1024.f) || (aUnit == SIZE_UNIT_NONE))
        {
            NUMBERFMT sNumberFmt = {0};
            sNumberFmt.NumDigits     = 2;
            sNumberFmt.LeadingZero   = 1;
            sNumberFmt.Grouping      = 3;
            sNumberFmt.lpDecimalSep  = XPR_STRING_LITERAL(".");
            sNumberFmt.lpThousandSep = XPR_STRING_LITERAL(",");
            sNumberFmt.NegativeOrder = 0;

            if (i == SIZE_UNIT_BYTE || i == SIZE_UNIT_NONE)
                sNumberFmt.NumDigits = 0;
            else
            {
                if (sUnitSize > 100 || sUnitSize == 0)
                    sNumberFmt.NumDigits = 0;
                else if (sUnitSize > 10)
                    sNumberFmt.NumDigits = 1;
            }

            xpr_tchar_t sSizeText[0xff] = {0};
            _stprintf(sSizeText, XPR_STRING_LITERAL("%.3f"), sUnitSize);

            const xpr_size_t sLen = _tcslen(sSizeText);
            if (sLen > 0)
                sSizeText[sLen-(3-sNumberFmt.NumDigits)] = XPR_STRING_LITERAL('\0');

            ::GetNumberFormat(XPR_NULL, XPR_NULL, sSizeText, &sNumberFmt, aFormatedSize, (xpr_sint_t)aMaxLen);

            if (aUnit != SIZE_UNIT_NONE)
            {
                const xpr_tchar_t *aText = SizeFormat::getDefUnitText(i);
                if (aText != XPR_NULL)
                {
                    _tcscat(aFormatedSize, XPR_STRING_LITERAL(" "));
                    _tcscat(aFormatedSize, aText);
                }
            }

            break;
        }

        sUnitSize /= 1024.f;
    }
}

void SizeFormat::initDefault(void)
{
    clear();
}

xpr_bool_t SizeFormat::load(fxb::IniFileEx &aIniFile)
{
    clear();

    xpr_size_t        i;
    xpr_tchar_t       sKey[0xff];
    IniFile::Section *sSection;
    Item             *sItem;

    sSection = aIniFile.findSection(kSizeFormatSection);
    if (XPR_IS_NULL(sSection))
        return XPR_FALSE;

    for (i = 0; i < MAX_SIZE_FORMAT; ++i)
    {
        _stprintf(sKey, kSizeKey, i + 1);

        if (aIniFile.findKey(sSection, sKey) == XPR_NULL)
            break;

        sItem = new Item;
        if (XPR_IS_NULL(sItem))
            break;

        sItem->mSize = aIniFile.getValueI64(sSection, sKey, 0);

        _stprintf(sKey, kUnitKey, i + 1);
        sItem->mSizeUnit = aIniFile.getValueI(sSection, sKey, SIZE_UNIT_BYTE);

        _stprintf(sKey, kDisplayKey, i + 1);
        sItem->mText = aIniFile.getValueS(sSection, sKey, XPR_STRING_LITERAL(""));

        _stprintf(sKey, kDefaultDecimalPlaceKey, i + 1);
        sItem->mDefaultDecimalPlace = aIniFile.getValueB(sSection, sKey, XPR_TRUE);

        _stprintf(sKey, kDecimalPlaceKey, i + 1);
        sItem->mDecimalPlace = aIniFile.getValueI(sSection, sKey, 0);

        _stprintf(sKey, kRoundOffKey, i + 1);
        sItem->mRoundOff = aIniFile.getValueB(sSection, sKey, XPR_FALSE);

        if (!XPR_IS_RANGE(MIN_SIZE_FORMAT_DECIAML_DIGIT, sItem->mDecimalPlace, MAX_SIZE_FORMAT_DECIAML_DIGIT))
            sItem->mDecimalPlace = 0;

        addItem(sItem);
    }

    return XPR_TRUE;
}

void SizeFormat::save(fxb::IniFileEx &aIniFile) const
{
    xpr_sint_t        i;
    xpr_tchar_t       sKey[0xff];
    IniFile::Section *sSection;
    Item             *sItem;
    FormatDeque::const_iterator sIterator;

    sSection = aIniFile.addSection(kSizeFormatSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIterator = mFormatDeque.begin();
    for (i = 0; sIterator != mFormatDeque.end(); ++sIterator, ++i)
    {
        sItem = *sIterator;
        XPR_ASSERT(sItem != XPR_NULL);

        _stprintf(sKey, kSizeKey, i + 1);
        aIniFile.setValueI64(sSection, sKey, sItem->mSize);

        _stprintf(sKey, kUnitKey, i + 1);
        aIniFile.setValueI(sSection, sKey, sItem->mSizeUnit);

        _stprintf(sKey, kDisplayKey, i + 1);
        aIniFile.setValueS(sSection, sKey, sItem->mText);

        _stprintf(sKey, kDefaultDecimalPlaceKey, i + 1);
        aIniFile.setValueB(sSection, sKey, sItem->mDefaultDecimalPlace);

        _stprintf(sKey, kDecimalPlaceKey, i + 1);
        aIniFile.setValueI(sSection, sKey, sItem->mDecimalPlace);

        _stprintf(sKey, kRoundOffKey, i + 1);
        aIniFile.setValueB(sSection, sKey, sItem->mRoundOff);
    }
}
} // namespace fxb
