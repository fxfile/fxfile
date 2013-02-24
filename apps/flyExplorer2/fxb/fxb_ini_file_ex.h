//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_INI_FILE_EX_H__
#define __FXB_INI_FILE_EX_H__
#pragma once

#include "fxb_ini_file.h"

namespace fxb
{
class IniFileEx : public IniFile
{
    typedef IniFile super;

public:
    IniFileEx(const xpr_tchar_t *aIniPath = XPR_NULL)
        : IniFile(aIniPath)
    {
    }

    virtual ~IniFileEx(void)
    {
    }

public:
    inline xpr_sint_t getValueI(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_sint_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _ttoi(sValue);
    }

    inline xpr_sint_t getValueI(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const xpr_sint_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _ttoi(sValue);
    }

    inline xpr_sint64_t getValueI64(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_sint64_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _ttoi64(sValue);
    }

    inline xpr_sint64_t getValueI64(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const xpr_sint64_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _ttoi64(sValue);
    }

    inline xpr_bool_t getValueB(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_bool_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _ttoi(sValue);
    }

    inline xpr_bool_t getValueB(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const xpr_bool_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _ttoi(sValue);
    }

    inline xpr_double_t getValueF(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_double_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _tstof(sValue);
    }

    inline xpr_double_t getValueF(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const xpr_double_t aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        return _tstof(sValue);
    }

    inline const xpr_tchar_t *getValueS(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aDefValue)
    {
        return super::getValue(aSection, aKey, aDefValue);
    }

    inline const xpr_tchar_t *getValueS(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aDefValue)
    {
        return super::getValue(aSection, aKey, aDefValue);
    }

    inline RECT getValueR(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const RECT &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        RECT sRect = aDefValue;

        _stscanf(sValue, XPR_STRING_LITERAL("%d,%d,%d,%d"), &sRect.left, &sRect.top, &sRect.right, &sRect.bottom);

        return sRect;
    }

    inline RECT getValueR(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const RECT &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        RECT sRect = aDefValue;

        _stscanf(sValue, XPR_STRING_LITERAL("%d,%d,%d,%d"), &sRect.left, &sRect.top, &sRect.right, &sRect.bottom);

        return sRect;
    }

    inline SIZE getValueZ(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const SIZE &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        SIZE sSize = aDefValue;

        _stscanf(sValue, XPR_STRING_LITERAL("%d,%d"), &sSize.cx, &sSize.cy);

        return sSize;
    }

    inline SIZE getValueZ(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const SIZE &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        SIZE sSize = aDefValue;

        _stscanf(sValue, XPR_STRING_LITERAL("%d,%d"), &sSize.cx, &sSize.cy);

        return sSize;
    }

    inline POINT getValueP(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const POINT &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        POINT sPoint = aDefValue;

        _stscanf(sValue, XPR_STRING_LITERAL("%d,%d"), &sPoint.x, &sPoint.y);

        return sPoint;
    }

    inline POINT getValueP(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const POINT &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        POINT sPoint = aDefValue;

        _stscanf(sValue, XPR_STRING_LITERAL("%d,%d"), &sPoint.x, &sPoint.y);

        return sPoint;
    }

    inline COLORREF getValueC(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const COLORREF &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        COLORREF sColor = aDefValue;

        xpr_sint_t r = GetRValue(aDefValue);
        xpr_sint_t g = GetGValue(aDefValue);
        xpr_sint_t b = GetBValue(aDefValue);

        if (sValue[0] == '#')
        {
            _stscanf(sValue + 1, XPR_STRING_LITERAL("%02x%02x%02x"), &r, &g, &b);
        }
        else if (_tcschr(sValue, ',') != XPR_NULL)
        {
            _stscanf(sValue, XPR_STRING_LITERAL("%d,%d,%d"), &r, &g, &b);
        }
        else
        {
            _stscanf(sValue, XPR_STRING_LITERAL("%02x%02x%02x"), &r, &g, &b);
        }

        sColor = RGB((xpr_byte_t)r, (xpr_byte_t)g, (xpr_byte_t)b);

        return sColor;
    }

    inline COLORREF getValueC(const IniFile::Section *aSection, const xpr_tchar_t *aKey, const COLORREF &aDefValue)
    {
        const xpr_tchar_t *sValue = super::getValue(aSection, aKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            return aDefValue;

        COLORREF sColor = aDefValue;

        xpr_sint_t r = GetRValue(aDefValue);
        xpr_sint_t g = GetGValue(aDefValue);
        xpr_sint_t b = GetBValue(aDefValue);

        if (sValue[0] == '#')
        {
            _stscanf(sValue + 1, XPR_STRING_LITERAL("%02x%02x%02x"), &r, &g, &b);
        }
        else if (_tcschr(sValue, ',') != XPR_NULL)
        {
            _stscanf(sValue, XPR_STRING_LITERAL("%d,%d,%d"), &r, &g, &b);
        }
        else
        {
            _stscanf(sValue, XPR_STRING_LITERAL("%02x%02x%02x"), &r, &g, &b);
        }

        sColor = RGB((xpr_byte_t)r, (xpr_byte_t)g, (xpr_byte_t)b);

        return sColor;
    }

    inline xpr_bool_t setValueI(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, xpr_sint_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueI(IniFile::Section *aSection, const xpr_tchar_t *aKey, xpr_sint_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueI64(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, xpr_sint64_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%I64d"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueI64(IniFile::Section *aSection, const xpr_tchar_t *aKey, xpr_sint64_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%I64d"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueB(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, xpr_bool_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueB(IniFile::Section *aSection, const xpr_tchar_t *aKey, xpr_bool_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueF(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, xpr_double_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%f"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueF(IniFile::Section *aSection, const xpr_tchar_t *aKey, xpr_double_t aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%f"), aValue);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueS(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aValue)
    {
        return super::setValue(aSection, aKey, aValue);
    }

    inline xpr_bool_t setValueS(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const std::tstring &aValue)
    {
        const xpr_tchar_t *sValue = aValue.c_str();

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueS(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const xpr::tstring &aValue)
    {
        const xpr_tchar_t *sValue = aValue.c_str();

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueS(IniFile::Section *aSection, const xpr_tchar_t *aKey, const xpr_tchar_t *aValue)
    {
        return super::setValue(aSection, aKey, aValue);
    }

    inline xpr_bool_t setValueS(IniFile::Section *aSection, const xpr_tchar_t *aKey, const std::tstring &aValue)
    {
        const xpr_tchar_t *sValue = aValue.c_str();

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueS(IniFile::Section *aSection, const xpr_tchar_t *aKey, const xpr::tstring &aValue)
    {
        const xpr_tchar_t *sValue = aValue.c_str();

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueR(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const RECT &aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d,%d,%d"), aValue.left, aValue.top, aValue.right, aValue.bottom);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueR(IniFile::Section *aSection, const xpr_tchar_t *aKey, const RECT &aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d,%d,%d"), aValue.left, aValue.top, aValue.right, aValue.bottom);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueZ(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const SIZE &aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d"), aValue.cx, aValue.cy);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueZ(IniFile::Section *aSection, const xpr_tchar_t *aKey, const SIZE &aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d"), aValue.cx, aValue.cy);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueP(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, const POINT &aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d"), aValue.x, aValue.y);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueP(IniFile::Section *aSection, const xpr_tchar_t *aKey, const POINT &aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d"), aValue.x, aValue.y);

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueC(const xpr_tchar_t *aSection, const xpr_tchar_t *aKey, COLORREF aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d,%d"), GetRValue(aValue), GetGValue(aValue), GetBValue(aValue));

        return super::setValue(aSection, aKey, sValue);
    }

    inline xpr_bool_t setValueC(IniFile::Section *aSection, const xpr_tchar_t *aKey, COLORREF aValue)
    {
        xpr_tchar_t sValue[0xff] = {0};

        _stprintf(sValue, XPR_STRING_LITERAL("%d,%d,%d"), GetRValue(aValue), GetGValue(aValue), GetBValue(aValue));

        return super::setValue(aSection, aKey, sValue);
    }
};
} // namespace fxb

#endif // __FXB_INI_FILE_EX_H__
