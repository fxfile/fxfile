//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_string.h"
#include "xpr_bit.h"
#include "xpr_memory.h"
#include "xpr_debug.h"
#include "xpr_char.h"
#include "xpr_cstr.h"
#include "xpr_char_set.h"

namespace xpr
{
namespace
{
const xpr_size_t kAllocSizeMask =
      sizeof (xpr_tchar_t) <= 1 ? 15
    : sizeof (xpr_tchar_t) <= 2 ? 7
    : sizeof (xpr_tchar_t) <= 4 ? 3
    : sizeof (xpr_tchar_t) <= 8 ? 1 : 0;

XPR_INLINE xpr_wchar_t toWideChar(const xpr_char_t &aChar)
{
    xpr_wchar_t sChar = 0;
    xpr_size_t  sInputBytes  = sizeof(xpr_char_t);
    xpr_size_t  sOutputBytes = sizeof(xpr_wchar_t);

    XPR_MBS_TO_UTF16(&aChar, &sInputBytes, &sChar, &sOutputBytes);

    return sChar;
}

XPR_INLINE xpr_char_t toChar(const xpr_wchar_t &aChar)
{
    xpr_char_t sChar = 0;
    xpr_size_t sInputBytes  = sizeof(xpr_wchar_t);
    xpr_size_t sOutputBytes = sizeof(xpr_char_t);

    XPR_UTF16_TO_MBS(&aChar, &sInputBytes, &sChar, &sOutputBytes);

    return sChar;
}

XPR_INLINE xpr_wchar_t *toWideString(const xpr_char_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_wchar_t *sString      = new xpr_wchar_t[aLength + 1];
    xpr_size_t   sInputBytes  = aLength * sizeof(xpr_char_t);
    xpr_size_t   sOutputBytes = aLength * sizeof(xpr_wchar_t);

    XPR_MBS_TO_UTF16(aString, &sInputBytes, sString, &sOutputBytes);
    sString[aLength] = 0;

    return sString;
}

XPR_INLINE xpr_char_t *toString(const xpr_wchar_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_char_t *sString      = new xpr_char_t[aLength + 1];
    xpr_size_t  sInputBytes  = aLength * sizeof(xpr_wchar_t);
    xpr_size_t  sOutputBytes = aLength * sizeof(xpr_char_t);

    XPR_UTF16_TO_MBS(aString, &sInputBytes, sString, &sOutputBytes);
    sString[aLength] = 0;

    return sString;
}
} // namespace anonymous

String::String(void)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    assign(XPR_MBCS_STRING_LITERAL(""), 0);
}

String::String(const String &aString)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    assign(aString);
}

String::String(const String &aString, xpr_size_t aPos, xpr_size_t aLength)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    assign(aString, aPos, aLength);
}

String::String(const xpr_char_t *aString, xpr_size_t aLength)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    if (XPR_IS_NULL(aString))
    {
        assign(XPR_STRING_LITERAL(""), 0);
    }
    else
    {
        assign(aString, aLength);
    }
}

String::String(const xpr_wchar_t *aString, xpr_size_t aLength)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    if (XPR_IS_NULL(aString))
    {
        assign(XPR_STRING_LITERAL(""), 0);
    }
    else
    {
        assign(aString, aLength);
    }
}

String::String(const xpr_char_t *aString)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    if (XPR_IS_NULL(aString))
    {
        assign(XPR_STRING_LITERAL(""), 0);
    }
    else
    {
        assign(aString);
    }
}

String::String(const xpr_wchar_t *aString)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    if (XPR_IS_NULL(aString))
    {
        assign(XPR_STRING_LITERAL(""), 0);
    }
    else
    {
        assign(aString);
    }
}

String::String(const xpr_size_t aNumber, xpr_char_t aChar)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    assign(aNumber, aChar);
}

String::String(const xpr_size_t aNumber, xpr_wchar_t aChar)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    assign(aNumber, aChar);
}

String::String(Iterator aFirst, Iterator aLast)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    assign(aFirst, aLast);
}

String::~String(void)
{
    reset();
}

String::Iterator String::begin(void)
{
    return Iterator(mString);
}

String::ConstIterator String::begin(void) const
{
    return ConstIterator(mString);
}

String::ConstIterator String::cbegin(void) const
{
    return ConstIterator(mString);
}

String::Iterator String::end(void)
{
    return Iterator(mString + mLength);
}

String::ConstIterator String::end(void) const
{
    return ConstIterator(mString + mLength);
}

String::ConstIterator String::cend(void) const
{
    return ConstIterator(mString + mLength);
}

String::ReverseIterator String::rbegin(void)
{
    return ReverseIterator(mString + mLength - 1);
}

String::ConstReverseIterator String::rbegin(void) const
{
    return ConstReverseIterator(mString + mLength - 1);
}

String::ConstReverseIterator String::crbegin(void) const
{
    return ConstReverseIterator(mString + mLength - 1);
}

String::ReverseIterator String::rend(void)
{
    return ReverseIterator(mString - 1);
}

String::ConstReverseIterator String::rend(void) const
{
    return ConstReverseIterator(mString - 1);
}

String::ConstReverseIterator String::crend(void) const
{
    return ConstReverseIterator(mString - 1);
}

xpr_size_t String::size(void) const
{
    return mLength;
}

xpr_size_t String::length(void) const
{
    return mLength;
}

xpr_size_t String::bytes(void) const
{
    return mLength * sizeof(xpr_tchar_t);
}

xpr_size_t String::max_size(void) const
{
    return (xpr_size_t)-1;
}

void String::resize(xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0)
    {
        return;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t sChar = toWideChar(aChar);

    resize(aNumber, sChar);
#else // not XPR_CFG_UNICODE
    if (mCapacity > aNumber)
    {
        if (aNumber > mLength)
        {
            memset(mString + mLength, aChar, aNumber - mLength);

            if (aChar != 0)
            {
                mString[aNumber] = 0;
            }

            mLength = aNumber;
        }
        else
        {
            mString[aNumber] = 0;

            mLength = aNumber;
        }
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(aNumber + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return;
        }

        strncpy(sNewString, mString, mLength);
        memset(sNewString + mLength, aChar, aNumber - mLength);

        if (aChar != 0)
        {
            sNewString[aNumber] = 0;
        }

        xpr_size_t sNewLength = aNumber;
        if (aChar == 0)
        {
            sNewLength = mLength;
        }

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }
#endif // XPR_CFG_UNICODE
}

void String::resize(xpr_size_t aNumber, xpr_wchar_t aChar)
{
    if (aNumber == 0)
    {
        return;
    }

#if defined(XPR_CFG_UNICODE)
    if (mCapacity > aNumber)
    {
        if (aNumber > mLength)
        {
            wmemset(mString + mLength, aChar, aNumber - mLength);

            if (aChar != 0)
            {
                mString[aNumber] = 0;
            }

            mLength = aNumber;
        }
        else
        {
            mString[aNumber] = 0;

            mLength = aNumber;
        }
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(aNumber + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return;
        }

        wcsncpy(sNewString, mString, mLength);
        wmemset(sNewString + mLength, aChar, aNumber - mLength);

        if (aChar != 0)
        {
            sNewString[aNumber] = 0;
        }

        xpr_size_t sNewLength = aNumber;
        if (aChar == 0)
        {
            sNewLength = mLength;
        }

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }
#else // not XPR_CFG_UNICODE
    xpr_char_t sChar = toChar(aChar);

    resize(aNumber, sChar);
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::get_capacity(const xpr_size_t &aMinCapacity) const
{
    return aMinCapacity | kAllocSizeMask;
}

xpr_tchar_t *String::alloc(const xpr_size_t &aMinCapacity, xpr_size_t &aCapacity) const
{
    xpr_size_t sNewCapacity = get_capacity(aMinCapacity);

    xpr_tchar_t *sNewString = new xpr_tchar_t[sNewCapacity];

    XPR_ASSERT(sNewString != XPR_NULL);
    if (XPR_IS_NULL(sNewString))
        return XPR_NULL;

    aCapacity = sNewCapacity;

    return sNewString;
}

void String::resize(xpr_size_t aNumber)
{
    resize(aNumber, XPR_STRING_LITERAL('\0'));
}

xpr_size_t String::capacity(void) const
{
    return mCapacity;
}

void String::reserve(xpr_size_t aCapacity)
{
    if (aCapacity > mCapacity)
    {
        xpr_size_t sNewCapacity;
        xpr_tchar_t *sNewString = alloc(aCapacity, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
            return;

#if defined(XPR_CFG_UNICODE)
        wcsncpy(sNewString, mString, mLength);
#else // not XPR_CFG_UNICODE
        strncpy(sNewString, mString, mLength);
#endif // XPR_CFG_UNICODE
        sNewString[mLength] = 0;

        xpr_size_t sLength = mLength;

        reset();

        mString = sNewString;
        mLength = sLength;
        mCapacity = sNewCapacity;
    }
}

void String::reset(void)
{
    XPR_SAFE_DELETE_ARRAY(mString);
    mLength = 0;
    mCapacity = 0;
}

void String::clear(void)
{
    mLength = 0;

    if (XPR_IS_NOT_NULL(mString))
    {
        mString[0] = 0;
    }
}

xpr_bool_t String::empty(void) const
{
    return (XPR_IS_NULL(mString) || mLength == 0) ? XPR_TRUE : XPR_FALSE;
}

CharSet String::get_char_set(void)
{
#if defined(XPR_CFG_OS_WINDOWS)
    CharSet sCharSet = CharSetNone;

    switch (sizeof(xpr_tchar_t))
    {
    case 1: sCharSet = CharSetMultiBytes; break;
    case 2: sCharSet = CharSetUtf16;      break;
    case 4: sCharSet = CharSetUtf32;      break;

    default:
        XPR_ASSERT(0);
        break;
    }

    return sCharSet;
#else // not XPR_CFG_OS_WINDOWS
#error Please, support your OS. <xpr_string.cpp>
#endif // XPR_CFG_OS_WINDOWS
}

String& String::assign(const String &aString)
{
    if (XPR_IS_NULL(aString.mString))
    {
        clear();
        return *this;
    }

    return assign(aString.mString, aString.mLength);
}

// aLength may be npos.
// aLegnth cannot be greater than length of aString.
String& String::assign(const String &aString, xpr_size_t aPos, xpr_size_t aLength)
{
    if (aLength == 0)
    {
        return *this;
    }

    XPR_ASSERT(aPos <= aString.mLength);

    if (aLength == npos)
    {
        aLength = aString.mLength - aPos;
    }
    else if ((aString.mLength - aPos) < aLength)
    {
        aLength = aString.mLength - aPos;
    }

    return assign(aString.mString + aPos, aLength);
}

// If aLength is greater than length of aString, then memory allocate greater but mLength is length of aString.
String& String::assign(const xpr_char_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    assign(sString, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_size_t sLength = strlen(aString);
    xpr_size_t sActualLength = (aLength > sLength) ? sLength : aLength;

    if (mCapacity > aLength)
    {
        strncpy(mString, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(mString + sLength, 0, (aLength - sLength) * sizeof(xpr_char_t));
        }
        else
        {
            mString[aLength] = 0;
        }

        mLength = sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(aLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        strncpy(sNewString, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(sNewString + sLength, 0, (aLength - sLength) * sizeof(xpr_char_t));
        }
        else
        {
            sNewString[aLength] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sActualLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::assign(const xpr_wchar_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sLength = wcslen(aString);
    xpr_size_t sActualLength = (aLength > sLength) ? sLength : aLength;

    if (mCapacity > aLength)
    {
        wcsncpy(mString, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(mString + sLength, 0, (aLength - sLength) * sizeof(xpr_wchar_t));
        }
        else
        {
            mString[aLength] = 0;
        }

        mLength = sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(aLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wcsncpy(sNewString, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(sNewString + sLength, 0, (aLength - sLength) * sizeof(xpr_wchar_t));
        }
        else
        {
            sNewString[aLength] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sActualLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    assign(sString, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::assign(const xpr_char_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return assign(aString, sLength);
}

String& String::assign(const xpr_wchar_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return assign(aString, sLength);
}

String& String::assign(xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t sChar = toWideChar(aChar);

    return assign(aNumber, sChar);
#else // not XPR_CFG_UNICODE
    xpr_size_t sActualLength = (aChar == 0) ? 0 : aNumber;

    if (mCapacity > aNumber)
    {
        memset(mString, aChar, aNumber);

        if (aChar != 0)
        {
            mString[aNumber] = 0;
        }

        mLength = sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(aNumber + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        memset(sNewString, aChar, aNumber);

        if (aChar != 0)
        {
            sNewString[aNumber] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sActualLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::assign(xpr_size_t aNumber, xpr_wchar_t aChar)
{
    if (aNumber == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sActualLength = (aChar == 0) ? 0 : aNumber;

    if (mCapacity > aNumber)
    {
        wmemset(mString, aChar, aNumber);

        if (aChar != 0)
        {
            mString[aNumber] = 0;
        }

        mLength = sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(aNumber + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wmemset(sNewString, aChar, aNumber);

        if (aChar != 0)
        {
            sNewString[aNumber] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sActualLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_char_t sChar = toChar(aChar);

    return assign(aNumber, sChar);
#endif // XPR_CFG_UNICODE
}

String& String::assign(Iterator aFirst, Iterator aLast)
{
    xpr_tchar_t *sString = &(*aFirst);
    xpr_size_t   sLength = aLast.mString - aFirst.mString;

    return assign(sString, sLength);
}

const xpr_tchar_t& String::operator[] (xpr_size_t aPos) const
{
    XPR_ASSERT(aPos < mLength);

    return mString[aPos];
}

xpr_tchar_t& String::operator[] (xpr_size_t aPos)
{
    XPR_ASSERT(aPos < mLength);

    return mString[aPos];
}

const xpr_tchar_t& String::at(xpr_size_t aPos) const
{
    XPR_ASSERT(aPos < mLength);

    return mString[aPos];
}

xpr_tchar_t& String::at(xpr_size_t aPos)
{
    XPR_ASSERT(aPos < mLength);

    return mString[aPos];
}

String& String::operator= (const String &aString)
{
    return assign(aString);
}

String& String::operator= (const xpr_char_t *aString)
{
    return assign(aString);
}

String& String::operator= (const xpr_wchar_t *aString)
{
    return assign(aString);
}

String& String::operator= (xpr_char_t aChar)
{
    return assign(1, aChar);
}

String& String::operator= (xpr_wchar_t aChar)
{
    return assign(1, aChar);
}

String& String::operator+= (const String &aString)
{
    return append(aString);
}

String& String::operator+= (const xpr_char_t *aString)
{
    return append(aString);
}

String& String::operator+= (const xpr_wchar_t *aString)
{
    return append(aString);
}

String& String::operator+= (xpr_char_t aChar)
{
    return append(1, aChar);
}

String& String::operator+= (xpr_wchar_t aChar)
{
    return append(1, aChar);
}

String& String::append(const String &aString)
{
    return append(aString.mString, aString.mLength);
}

// aLength may be npos.
// aLegnth cannot be greater than length of aString.
String& String::append(const String &aString, xpr_size_t aPos, xpr_size_t aLength)
{
    if (aLength == 0)
    {
        return *this;
    }

    XPR_ASSERT(aPos <= aString.mLength);

    if (aLength == npos)
    {
        aLength = aString.mLength - aPos;
    }
    else if ((aString.mLength - aPos) < aLength)
    {
        aLength = aString.mLength - aPos;
    }

    return append(aString.mString + aPos, aLength);
}

// If aLength is greater than length of aString, then memory allocate greater but mLength is length of aString.
String& String::append(const xpr_char_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aLength == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    append(sString, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_size_t sLength = strlen(aString);
    xpr_size_t sActualLength = (aLength > sLength) ? sLength : aLength;

    if (mCapacity > (mLength + aLength))
    {
        strncpy(mString + mLength, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(mString + mLength + sLength, 0, (aLength - sLength) * sizeof(xpr_char_t));
        }
        else
        {
            mString[mLength + aLength] = 0;
        }

        mLength += sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(mLength + aLength + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + sActualLength;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        strncpy(sNewString, mString, mLength);
        strncpy(sNewString + mLength, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(sNewString + mLength + sLength, 0, (aLength - sLength) * sizeof(xpr_char_t));
        }
        else
        {
            sNewString[mLength + aLength] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::append(const xpr_wchar_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aLength == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sLength = wcslen(aString);
    xpr_size_t sActualLength = (aLength > sLength) ? sLength : aLength;

    if (mCapacity > (mLength + aLength))
    {
        wcsncpy(mString + mLength, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(mString + mLength + sLength, 0, (aLength - sLength) * sizeof(xpr_wchar_t));
        }
        else
        {
            mString[mLength + aLength] = 0;
        }

        mLength += sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(mLength + aLength + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + sActualLength;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wcsncpy(sNewString, mString, mLength);
        wcsncpy(sNewString + mLength, aString, sActualLength);

        if (aLength > sLength)
        {
            memset(sNewString + mLength + sLength, 0, (aLength - sLength) * sizeof(xpr_wchar_t));
        }
        else
        {
            sNewString[mLength + aLength] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    append(sString, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::append(const xpr_char_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return append(aString, sLength);
}

String& String::append(const xpr_wchar_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return append(aString, sLength);
}

String& String::append(xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t sChar = toWideChar(aChar);

    return append(aNumber, sChar);
#else // not XPR_CFG_UNICODE
    xpr_size_t sActualLength = (aChar == 0) ? 0 : aNumber;

    if (mCapacity > (mLength + aNumber))
    {
        memset(mString + mLength, aChar, aNumber);

        if (aChar != 0)
        {
            mString[mLength + aNumber] = 0;
        }

        mLength += sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(mLength + aNumber + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + sActualLength;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        strncpy(sNewString, mString, mLength);
        memset(sNewString + mLength, aChar, aNumber);

        if (aChar != 0)
        {
            sNewString[mLength + aNumber] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::append(xpr_size_t aNumber, xpr_wchar_t aChar)
{
    if (aNumber == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sActualLength = (aChar == 0) ? 0 : aNumber;

    if (mCapacity > (mLength + aNumber))
    {
        wmemset(mString + mLength, aChar, aNumber);

        if (aChar != 0)
        {
            mString[mLength + aNumber] = 0;
        }

        mLength += sActualLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(mLength + aNumber + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + sActualLength;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wcsncpy(sNewString, mString, mLength);
        wmemset(sNewString + mLength, aChar, aNumber);

        if (aChar != 0)
        {
            sNewString[mLength + aNumber] = 0;
        }

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_char_t sChar = toChar(aChar);

    return append(aNumber, sChar);
#endif // XPR_CFG_UNICODE
}

String& String::append(Iterator aFirst, Iterator aLast)
{
    xpr_tchar_t *sString = &(*aFirst);
    xpr_size_t   sLength = aLast - aFirst;

    return append(sString, sLength);
}

xpr_tchar_t &String::front(void)
{
    return mString[0];
}

const xpr_tchar_t &String::front(void) const
{
    return mString[0];
}

xpr_tchar_t &String::back(void)
{
    if (mLength == 0)
        return mString[0];

    return mString[mLength - 1];
}

const xpr_tchar_t &String::back(void) const
{
    if (mLength == 0)
        return mString[0];

    return mString[mLength - 1];
}

void String::push_back(xpr_tchar_t aChar)
{
    append(1, aChar);
}

void String::pop_back(void)
{
    if (mLength > 0)
    {
        erase(mLength - 1);
    }
}

String& String::insert(xpr_size_t aPos, const String &aString)
{
    return insert(aPos, aString.mString, aString.mLength);
}

String& String::insert(xpr_size_t aPos1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength)
{
    XPR_ASSERT(aPos2 <= aString.mLength);

    if (aLength == npos)
    {
        aLength = aString.mLength - aPos2;
    }
    else if ((aString.mLength - aPos2) < aLength)
    {
        aLength = aString.mLength - aPos2;
    }

    return insert(aPos1, aString.mString + aPos2, aLength);
}

String& String::insert(xpr_size_t aPos, const xpr_char_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aLength == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    insert(aPos, sString, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#else // XPR_CFG_UNICODE
    if (aPos == npos || aPos >= mLength)
    {
        return append(aString, aLength);
    }

    xpr_size_t sLength = strlen(aString);
    if (sLength > aLength)
        sLength = aLength;

    if (mCapacity > (mLength + sLength))
    {
        memmove(mString + aPos + sLength, mString + aPos, (mLength - aPos) * sizeof(xpr_char_t));
        strncpy(mString + aPos, aString, sLength);
        mString[mLength + sLength] = 0;

        mLength += sLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(mLength + sLength + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + sLength;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        strncpy(sNewString, mString, aPos);
        strncpy(sNewString + aPos, aString, sLength);
        strncpy(sNewString + aPos + sLength, mString + aPos, mLength - aPos);
        sNewString[mLength + sLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::insert(xpr_size_t aPos, const xpr_wchar_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aLength == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    if (aPos == npos || aPos >= mLength)
    {
        return append(aString, aLength);
    }

    xpr_size_t sLength = wcslen(aString);
    if (sLength > aLength)
        sLength = aLength;

    if (mCapacity > (mLength + sLength))
    {
        memmove(mString + aPos + sLength, mString + aPos, (mLength - aPos) * sizeof(xpr_wchar_t));
        wcsncpy(mString + aPos, aString, sLength);
        mString[mLength + sLength] = 0;

        mLength += sLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(mLength + sLength + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + sLength;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wcsncpy(sNewString, mString, aPos);
        wcsncpy(sNewString + aPos, aString, sLength);
        wcsncpy(sNewString + aPos + sLength, mString + aPos, mLength - aPos);
        sNewString[mLength + sLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    insert(aPos, sString, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::insert(xpr_size_t aPos, const xpr_char_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return insert(aPos, aString, sLength);
}

String& String::insert(xpr_size_t aPos, const xpr_wchar_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return insert(aPos, aString, sLength);
}

String& String::insert(xpr_size_t aPos, xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0 || aChar == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t sChar = toWideChar(aChar);

    insert(aPos, aNumber, sChar);

    return *this;
#else // not XPR_CFG_UNICODE
    if (aPos == npos || aPos >= mLength)
    {
        return append(aNumber, aChar);
    }

    if (mCapacity > (mLength + aNumber))
    {
        memmove(mString + aPos + aNumber, mString + aPos, (mLength - aPos) * sizeof(xpr_char_t));
        memset(mString + aPos, aChar, aNumber);
        mString[mLength + aNumber] = 0;

        mLength += aNumber;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(mLength + aNumber + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + aNumber;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        strncpy(sNewString, mString, aPos);
        memset(sNewString + aPos, aChar, aNumber);
        strncpy(sNewString + aPos + aNumber, mString + aPos, mLength - aPos);
        sNewString[mLength + aNumber] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::insert(xpr_size_t aPos, xpr_size_t aNumber, xpr_wchar_t aChar)
{
    if (aNumber == 0 || aChar == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    if (aPos == npos || aPos >= mLength)
    {
        return append(aNumber, aChar);
    }

    if (mCapacity > (mLength + aNumber))
    {
        memmove(mString + aPos + aNumber, mString + aPos, (mLength - aPos) * sizeof(xpr_wchar_t));
        wmemset(mString + aPos, aChar, aNumber);
        mString[mLength + aNumber] = 0;

        mLength += aNumber;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(mLength + aNumber + 1, sNewCapacity);
        xpr_size_t sNewLength = mLength + aNumber;

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wcsncpy(sNewString, mString, aPos);
        wmemset(sNewString + aPos, aChar, aNumber);
        wcsncpy(sNewString + aPos + aNumber, mString + aPos, mLength - aPos);
        sNewString[mLength + aNumber] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_char_t sChar = toChar(aChar);

    insert(aPos, aNumber, sChar);

    return *this;
#endif // XPR_CFG_UNICODE
}

String::Iterator String::insert(Iterator aPos, xpr_char_t aChar)
{
    return insert(aPos, 1, aChar);
}

String::Iterator String::insert(Iterator aPos, xpr_wchar_t aChar)
{
    return insert(aPos, 1, aChar);
}

String::Iterator String::insert(Iterator aPos, xpr_size_t aNumber, xpr_char_t aChar)
{
    xpr_size_t sPos = aPos.mString - mString;

    insert(sPos, aNumber, aChar);

    return Iterator(mString + sPos);
}

String::Iterator String::insert(Iterator aPos, xpr_size_t aNumber, xpr_wchar_t aChar)
{
    xpr_size_t sPos = aPos.mString - mString;

    insert(sPos, aNumber, aChar);

    return Iterator(mString + sPos);
}

String::Iterator String::insert(Iterator aPos, Iterator aFirst, Iterator aLast)
{
    xpr_size_t   sPos    = aPos.mString - mString;
    xpr_tchar_t *sString = &(*aFirst);
    xpr_size_t   sLength = aLast - aFirst;

    insert(sPos, sString, sLength);

    return Iterator(mString + sPos);
}

String& String::erase(xpr_size_t aPos, xpr_size_t aLength)
{
    xpr_size_t sLength = aLength;

    if (aPos == 0)
    {
        if (sLength == npos || sLength >= mLength)
        {
            clear();
            return *this;
        }
    }

    if (sLength == npos)
    {
        sLength = mLength - aPos;
    }
    else
    {
        sLength = min(sLength, mLength - aPos);
    }

    memmove(mString + aPos, mString + aPos + sLength, (mLength - (aPos + sLength)) * sizeof(xpr_tchar_t));
    mString[mLength - sLength] = 0;

    mLength -= sLength;

    return *this;
}

String::Iterator String::erase(Iterator aPos)
{
    xpr_size_t sPos = aPos.mString - mString;

    erase(sPos, 1);

    return Iterator(mString + sPos);
}

String::Iterator String::erase(Iterator aFirst, Iterator aLast)
{
    xpr_size_t sPos    = aFirst.mString - mString;
    xpr_size_t sLength = aLast - aFirst;

    erase(sPos, sLength);

    return Iterator(mString + sPos);
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength, const String &aString)
{
    return replace(aPos, aLength, aString.mString, aString.mLength);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, const String &aString)
{
    xpr_size_t sPos    = aIterator1.mString - mString;
    xpr_size_t sLength = aIterator2 - aIterator1;

    return replace(sPos, sLength, aString.mString, aString.mLength);
}

String& String::replace(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2)
{
    XPR_ASSERT(aPos2 <= aString.mLength);

    if (aLength2 == npos)
    {
        aLength2 = aString.mLength - aPos2;
    }
    else if ((aString.mLength - aPos2) < aLength2)
    {
        aLength2 = aString.mLength - aPos2;
    }

    return replace(aPos1, aLength1, aString.mString + aPos2, aLength2);
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2)
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos == npos || aPos >= mLength || aLength2 == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength2);

    replace(aPos, aLength1, sString, aLength2);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_size_t sLength1 = aLength1;
    if (sLength1 == npos)
        sLength1 = strlen(mString + aPos);

    xpr_size_t sLength2 = strlen(aString);
    if (sLength2 > aLength2)
        sLength2 = aLength2;

    xpr_size_t sNewLength = mLength - sLength1 + sLength2;

    if (mCapacity > sNewLength)
    {
        memmove(mString + aPos + sLength2, mString + aPos + sLength1, (mLength - aPos - sLength1) * sizeof(xpr_char_t));
        strncpy(mString + aPos, aString, sLength2);
        mString[sNewLength] = 0;

        mLength = sNewLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(sNewLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        strncpy(sNewString, mString, aPos);
        strncpy(sNewString + aPos, aString, sLength2);
        strncpy(sNewString + aPos + sLength2, mString + aPos + sLength1, mLength - aPos - sLength1);
        sNewString[sNewLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength1, const xpr_wchar_t *aString, xpr_size_t aLength2)
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos == npos || aPos >= mLength || aLength2 == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sLength1 = aLength1;
    if (sLength1 == npos)
        sLength1 = wcslen(mString + aPos);

    xpr_size_t sLength2 = wcslen(aString);
    if (sLength2 > aLength2)
        sLength2 = aLength2;

    xpr_size_t sNewLength = mLength - sLength1 + sLength2;

    if (mCapacity > sNewLength)
    {
        memmove(mString + aPos + sLength2, mString + aPos + sLength1, (mLength - aPos - sLength1) * sizeof(xpr_wchar_t));
        wcsncpy(mString + aPos, aString, sLength2);
        mString[sNewLength] = 0;

        mLength = sNewLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(sNewLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wcsncpy(sNewString, mString, aPos);
        wcsncpy(sNewString + aPos, aString, sLength2);
        wcsncpy(sNewString + aPos + sLength2, mString + aPos + sLength1, mLength - aPos - sLength1);
        sNewString[sNewLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength2);

    replace(aPos, aLength1, sString, aLength2);

    XPR_SAFE_DELETE_ARRAY(sString);

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sPos    = aIterator1.mString - mString;
    xpr_size_t sLength = aIterator2 - aIterator1;

    return replace(sPos, sLength, aString, aLength);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, const xpr_wchar_t *aString, xpr_size_t aLength)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sPos    = aIterator1.mString - mString;
    xpr_size_t sLength = aIterator2 - aIterator1;

    return replace(sPos, sLength, aString, aLength);
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return replace(aPos, aLength, aString, sLength);
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength, const xpr_wchar_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return replace(aPos, aLength, aString, sLength);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sPos     = aIterator1.mString - mString;
    xpr_size_t sLength1 = aIterator2 - aIterator1;

    xpr_size_t sLength2 = strlen(aString);

    return replace(sPos, sLength1, aString, sLength2);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, const xpr_wchar_t *aString)
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sPos     = aIterator1.mString - mString;
    xpr_size_t sLength1 = aIterator2 - aIterator1;

    xpr_size_t sLength2 = wcslen(aString);

    return replace(sPos, sLength1, aString, sLength2);
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength, xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aPos == npos || aPos >= mLength || aNumber == 0 || aChar == 0)
    {
        return *this;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t sChar = toWideChar(aChar);

    replace(aPos, aLength, aNumber, sChar);

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_size_t sNewLength = mLength - aLength + aNumber;

    if (mCapacity > sNewLength)
    {
        memmove(mString + aPos + aNumber, mString + aPos + aLength, (mLength - aPos - aLength) * sizeof(xpr_char_t));
        memset(mString + aPos, 0, aNumber);
        memset(mString + aPos, aChar, aNumber);
        mString[sNewLength] = 0;

        mLength = sNewLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(sNewLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        strncpy(sNewString, mString, aPos);
        memset(sNewString + aPos, aChar, aNumber);
        strncpy(sNewString + aPos + aNumber, mString + aPos + aLength, mLength - aPos - aLength);
        sNewString[sNewLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength, xpr_size_t aNumber, xpr_wchar_t aChar)
{
#if defined(XPR_CFG_UNICODE)
    if (aPos == npos || aPos >= mLength || aNumber == 0 || aChar == 0)
    {
        return *this;
    }

    xpr_size_t sNewLength = mLength - aLength + aNumber;

    if (mCapacity > sNewLength)
    {
        memmove(mString + aPos + aNumber, mString + aPos + aLength, (mLength - aPos - aLength) * sizeof(xpr_wchar_t));
        wmemset(mString + aPos, 0, aNumber);
        wmemset(mString + aPos, aChar, aNumber);
        mString[sNewLength] = 0;

        mLength = sNewLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(sNewLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return *this;
        }

        wcsncpy(sNewString, mString, aPos);
        wmemset(sNewString + aPos, aChar, aNumber);
        wcsncpy(sNewString + aPos + aNumber, mString + aPos + aLength, mLength - aPos - aLength);
        sNewString[sNewLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }

    return *this;
#else // not XPR_CFG_UNICODE
    xpr_char_t sChar = toChar(aChar);

    replace(aPos, aLength, aNumber, sChar);

    return *this;
#endif // XPR_CFG_UNICODE
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, xpr_size_t aNumber, xpr_char_t aChar)
{
    xpr_size_t sPos    = aIterator1.mString - mString;
    xpr_size_t sLength = aIterator2 - aIterator1;

    return replace(sPos, sLength, aNumber, aChar);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, xpr_size_t aNumber, xpr_wchar_t aChar)
{
    xpr_size_t sPos    = aIterator1.mString - mString;
    xpr_size_t sLength = aIterator2 - aIterator1;

    return replace(sPos, sLength, aNumber, aChar);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, Iterator aInputIterator1, Iterator aInputIterator2)
{
    xpr_size_t sPos     = aIterator1.mString - mString;
    xpr_size_t sLength1 = aIterator2 - aIterator1;

    xpr_tchar_t *sString  = aInputIterator1.mString;
    xpr_size_t   sLength2 = aInputIterator2 - aInputIterator1;

    return replace(sPos, sLength1, sString, sLength2);
}

void String::swap(String &aString)
{
    xpr_tchar_t *sTempString   = mString;
    xpr_size_t   sTempLength   = mLength;
    xpr_size_t   sTempCapacity = mCapacity;

    mString   = aString.mString;
    mLength   = aString.mLength;
    mCapacity = aString.mCapacity;

    aString.mString   = sTempString;
    aString.mLength   = sTempLength;
    aString.mCapacity = sTempCapacity;
}

void String::upper_case(void)
{
#if defined(XPR_CFG_UNICODE)
    _wcsupr(mString);
#else // not XPR_CFG_UNICODE
    _strupr(mString);
#endif // XPR_CFG_UNICODE
}

void String::lower_case(void)
{
#if defined(XPR_CFG_UNICODE)
    _wcslwr(mString);
#else // not XPR_CFG_UNICODE
    _strlwr(mString);
#endif // XPR_CFG_UNICODE
}

void String::trim(void)
{
    trim_right();
    trim_left();
}

void String::trim_left(void)
{
    xpr_size_t i;
    for (i = 0; i < mLength; ++i)
    {
        if (xpr::isSpace((xpr_char_t)mString[i]) == XPR_FALSE)
        {
            erase(0, i);
            break;
        }
    }
}

void String::trim_right(void)
{
    xpr_size_t i;
    for (i = mLength - 1; i != -1; --i)
    {
        if (xpr::isSpace((xpr_char_t)mString[i]) == XPR_FALSE)
        {
            erase(i + 1, mLength - i - 1);
            break;
        }
    }
}

void String::format(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    format_args(aFormat, sArgs);

    va_end(sArgs);
}

void String::format(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    format_args(aFormat, sArgs);

    va_end(sArgs);
}

void String::format_args(const xpr_char_t *aFormat, va_list aArgs)
{
#if defined(XPR_CFG_UNICODE)
    xpr_size_t   sLength = strlen(aFormat);
    xpr_wchar_t *sFormat = toWideString(aFormat, sLength);

    format_args(sFormat, aArgs);

    XPR_SAFE_DELETE_ARRAY(sFormat);
#else // not XPR_CFG_UNICODE
    xpr_sint_t sLength = _vscprintf(aFormat, aArgs);
    if (sLength <= 0)
        return;

    if (mCapacity > (xpr_size_t)sLength)
    {
        vsprintf(mString, aFormat, aArgs);
        mString[sLength] = 0;

        mLength = sLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(sLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return;
        }

        vsprintf(sNewString, aFormat, aArgs);
        sNewString[sLength] = 0;

        reset();

        mString = sNewString;
        mLength = sLength;
        mCapacity = sNewCapacity;
    }
#endif // XPR_CFG_UNICODE
}

void String::format_args(const xpr_wchar_t *aFormat, va_list aArgs)
{
#if defined(XPR_CFG_UNICODE)
    xpr_sint_t sLength = _vscwprintf(aFormat, aArgs);
    if (sLength <= 0)
        return;

    if (mCapacity > (xpr_size_t)sLength)
    {
        vswprintf(mString, aFormat, aArgs);
        mString[sLength] = 0;

        mLength = sLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(sLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return;
        }

        vswprintf(sNewString, aFormat, aArgs);
        sNewString[sLength] = 0;

        reset();

        mString = sNewString;
        mLength = sLength;
        mCapacity = sNewCapacity;
    }
#else // not XPR_CFG_UNICODE
    xpr_size_t  sLength = wcslen(aFormat);
    xpr_char_t *sFormat = toString(aFormat, sLength);

    format_args(sFormat, aArgs);

    XPR_SAFE_DELETE_ARRAY(sFormat);
#endif // XPR_CFG_UNICODE
}

void String::append_format(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    append_format_args(aFormat, sArgs);

    va_end(sArgs);
}

void String::append_format(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    append_format_args(aFormat, sArgs);

    va_end(sArgs);
}

void String::append_format_args(const xpr_char_t *aFormat, va_list aArgs)
{
#if defined(XPR_CFG_UNICODE)
    xpr_size_t   sLength = strlen(aFormat);
    xpr_wchar_t *sFormat = toWideString(aFormat, sLength);

    append_format_args(sFormat, aArgs);

    XPR_SAFE_DELETE_ARRAY(sFormat);
#else // not XPR_CFG_UNICODE
    xpr_sint_t sLength = _vscprintf(aFormat, aArgs);
    if (sLength <= 0)
        return;

    xpr_size_t sNewLength = mLength + (xpr_size_t)sLength;

    if (mCapacity > sNewLength)
    {
        vsprintf(mString + mLength, aFormat, aArgs);
        mString[sNewLength] = 0;

        mLength = sNewLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_char_t *sNewString = alloc(sNewLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return;
        }

        strncpy(sNewString, mString, mLength);
        vsprintf(sNewString + mLength, aFormat, aArgs);
        sNewString[sNewLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }
#endif // XPR_CFG_UNICODE
}

void String::append_format_args(const xpr_wchar_t *aFormat, va_list aArgs)
{
#if defined(XPR_CFG_UNICODE)
    xpr_sint_t sLength = _vscwprintf(aFormat, aArgs);
    if (sLength <= 0)
        return;

    xpr_size_t sNewLength = mLength + (xpr_size_t)sLength;

    if (mCapacity > sNewLength)
    {
        vswprintf(mString + mLength, aFormat, aArgs);
        mString[sNewLength] = 0;

        mLength = sNewLength;
    }
    else
    {
        xpr_size_t sNewCapacity;
        xpr_wchar_t *sNewString = alloc(sNewLength + 1, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
        {
            return;
        }

        wcsncpy(sNewString, mString, mLength);
        vswprintf(sNewString + mLength, aFormat, aArgs);
        sNewString[sNewLength] = 0;

        reset();

        mString = sNewString;
        mLength = sNewLength;
        mCapacity = sNewCapacity;
    }
#else // not XPR_CFG_UNICODE
    xpr_size_t  sLength = wcslen(aFormat);
    xpr_char_t *sFormat = toString(aFormat, sLength);

    append_format_args(sFormat, aArgs);

    XPR_SAFE_DELETE_ARRAY(sFormat);
#endif // XPR_CFG_UNICODE
}

void String::update(void)
{
#if defined(XPR_CFG_UNICODE)
    mLength = wcsnlen(mString, mCapacity - 1);
#else // not XPR_CFG_UNICODE
    mLength = strnlen(mString, mCapacity - 1);
#endif // XPR_CFG_UNICODE
}

void String::shrink_to_fit(void)
{
    xpr_size_t sFitCapacity = get_capacity(mLength + 1);

    if (mCapacity > sFitCapacity)
    {
        xpr_size_t   sNewCapacity;
        xpr_size_t   sLength = mLength;
        xpr_tchar_t *sNewString = alloc(sLength + 1, sNewCapacity);

#if defined(XPR_CFG_UNICODE)
        wcsncpy(sNewString, mString, sLength);
#else // not XPR_CFG_UNICODE
        strncpy(sNewString, mString, sLength);
#endif // XPR_CFG_UNICODE
        sNewString[sLength] = 0;

        reset();

        mString = sNewString;
        mLength = sLength;
        mCapacity = sNewCapacity;
    }
}

const xpr_tchar_t *String::c_str(void) const
{
    return mString;
}

const xpr_tchar_t *String::data(void) const
{
    return mString;
}

xpr_size_t String::copy(xpr_char_t *aString, xpr_size_t aLength, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aLength == 0 || aPos == npos || aPos >= mLength)
    {
        return 0;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sLength = aLength;

    if ((aPos + sLength) > mLength)
    {
        sLength = mLength - aPos;
    }

    xpr_tchar_t *sString      = mString + aPos;
    xpr_size_t   sInputBytes  = sLength * sizeof(xpr_tchar_t);
    xpr_size_t   sOutputBytes = sLength * sizeof(xpr_char_t);
    XPR_UTF16_TO_MBS(sString, &sInputBytes, aString, &sOutputBytes);
    aString[sLength] = 0;

    return sLength;
#else // not XPR_CFG_UNICODE
    xpr_size_t sLength = aLength;

    if ((aPos + sLength) > mLength)
    {
        sLength = mLength - aPos;
    }

    strncpy(aString, mString + aPos, sLength);

    return sLength;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::copy(xpr_wchar_t *aString, xpr_size_t aLength, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aLength == 0 || aPos == npos || aPos >= mLength)
    {
        return 0;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sLength = aLength;

    if ((aPos + sLength) > mLength)
    {
        sLength = mLength - aPos;
    }

    xpr_tchar_t *sString      = mString + aPos;
    xpr_size_t   sInputBytes  = sLength * sizeof(xpr_tchar_t);
    xpr_size_t   sOutputBytes = sLength * sizeof(xpr_wchar_t);
    XPR_MBS_TO_UTF16(sString, &sInputBytes, aString, &sOutputBytes);
    aString[sLength] = 0;

    return sLength;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    xpr_size_t sLength = copy(sString, aLength, aPos);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sLength;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find(const String &aString, xpr_size_t aPos) const
{
    return find(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos == npos || (aPos + aLength) > mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    xpr_size_t sFoundIndex = find(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    const xpr_char_t *sString = mString + aPos;
    const xpr_char_t *sSearch = aString;
    xpr_char_t sSubStrChar, sChar;
    xpr_size_t sLength;

    if ((sSubStrChar = *sSearch++) != '\0')
    {
        sLength = aLength - 1;

        do
        {
            do
            {
                if ((sChar = *sString++) == '\0')
                {
                    return npos;
                }

            } while (sChar != sSubStrChar);

            if (sLength > aLength)
            {
                return npos;
            }

        } while (strncmp(sString, sSearch, sLength) != 0);

        sString--;
    }

    xpr_size_t sFoundIndex = (xpr_size_t)(sString - mString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos == npos || (aPos + aLength) > mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    const xpr_wchar_t *sString = mString + aPos;
    const xpr_wchar_t *sSearch = aString;
    xpr_wchar_t sSubStrChar, sChar;
    xpr_size_t sLength = aLength;

    if ((sSubStrChar = *sSearch++) != '\0')
    {
        --sLength;

        do
        {
            do
            {
                if ((sChar = *sString++) == '\0')
                {
                    return npos;
                }

            } while (sChar != sSubStrChar);

            if (sLength > aLength)
            {
                return npos;
            }

        } while (wcsncmp(sString, sSearch, sLength) != 0);

        sString--;
    }

    xpr_size_t sFoundIndex = (xpr_size_t)(sString - mString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    xpr_size_t sFoundIndex = find(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find(const xpr_char_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return find(aString, aPos, sLength);
}

xpr_size_t String::find(const xpr_wchar_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return find(aString, aPos, sLength);
}

xpr_size_t String::find(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find(&aChar, aPos, 1);
}

xpr_size_t String::find(xpr_wchar_t aChar, xpr_size_t aPos) const
{
    return find(&aChar, aPos, 1);
}

xpr_size_t String::rfind(const String &aString, xpr_size_t aPos) const
{
    return rfind(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::rfind(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if ((aPos != npos && aPos >= mLength))
    {
        return npos;
    }

    if (aLength == 0)
    {
        return 0;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    xpr_size_t sFoundIndex = rfind(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    if (aPos == npos)
    {
        aPos = mLength - 1;
    }

    const xpr_char_t *sSearch = aString;
    xpr_size_t sLength = strlen(aString);
    if (sLength > aLength)
        sLength = aLength;

    xpr_sint_t sPos = (xpr_sint_t)aPos;
    const xpr_char_t *sResult = XPR_NULL;

    if (mLength == 0 || mLength < sLength)
    {
        return XPR_NULL;
    }

    while (sPos >= 0)
    {
        if (strncmp(mString + sPos, sSearch, sLength) == 0)
        {
            sResult = (xpr_char_t *)mString + sPos;
            break;
        }

        sPos--;
    }

    if (XPR_IS_NULL(sResult))
    {
        return npos;
    }

    xpr_size_t sFoundIndex = (xpr_size_t)(sResult - mString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::rfind(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if ((aPos != npos && aPos >= mLength))
    {
        return npos;
    }

    if (aLength == 0)
    {
        return 0;
    }
#if defined(XPR_CFG_UNICODE)
    if (aPos == npos)
    {
        aPos = mLength - 1;
    }

    const xpr_wchar_t *sSearch = aString;
    xpr_size_t sLength = wcslen(aString);
    if (sLength > aLength)
        sLength = aLength;

    xpr_sint_t sPos = (xpr_sint_t)aPos;
    const xpr_wchar_t *sResult = XPR_NULL;

    if (mLength == 0 || mLength < sLength)
    {
        return XPR_NULL;
    }

    while (sPos >= 0)
    {
        if (wcsncmp(mString + sPos, sSearch, sLength) == 0)
        {
            sResult = (xpr_wchar_t *)mString + sPos;
            break;
        }

        sPos--;
    }

    if (XPR_IS_NULL(sResult))
    {
        return npos;
    }

    xpr_size_t sFoundIndex = (xpr_size_t)(sResult - mString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    xpr_size_t sFoundIndex = rfind(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::rfind(const xpr_char_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return rfind(aString, aPos, sLength);
}

xpr_size_t String::rfind(const xpr_wchar_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return rfind(aString, aPos, sLength);
}

xpr_size_t String::rfind(xpr_char_t aChar, xpr_size_t aPos) const
{
    return rfind(&aChar, aPos, 1);
}

xpr_size_t String::rfind(xpr_wchar_t aChar, xpr_size_t aPos) const
{
    return rfind(&aChar, aPos, 1);
}

xpr_size_t String::find_first_of(const String &aString, xpr_size_t aPos) const
{
    return find_first_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_first_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    xpr_size_t sFoundIndex = find_first_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_size_t sFoundIndex = npos;
    const xpr_char_t *sString = mString + aPos;
    const xpr_char_t *sEndString = mString + mLength - 1;
    const xpr_char_t *sResult;

    do
    {
        sResult = (const xpr_char_t *)memchr(aString, *sString, aLength);
        if (XPR_IS_NOT_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (++sString <= sEndString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_first_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sFoundIndex = npos;
    const xpr_wchar_t *sString = mString + aPos;
    const xpr_wchar_t *sEndString = mString + mLength - 1;
    const xpr_wchar_t *sResult;

    do
    {
        sResult = (const xpr_wchar_t *)wmemchr(aString, *sString, aLength);
        if (XPR_IS_NOT_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (++sString <= sEndString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    xpr_size_t sFoundIndex = find_first_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_first_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return find_first_of(aString, aPos, sLength);
}

xpr_size_t String::find_first_of(const xpr_wchar_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return find_first_of(aString, aPos, sLength);
}

xpr_size_t String::find_first_of(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find_first_of(&aChar, aPos, 1);
}

xpr_size_t String::find_first_of(xpr_wchar_t aChar, xpr_size_t aPos) const
{
    return find_first_of(&aChar, aPos, 1);
}

xpr_size_t String::find_last_of(const String &aString, xpr_size_t aPos) const
{
    return find_last_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_last_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

    if (aPos == npos)
    {
        aPos = mLength - 1;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    xpr_size_t sFoundIndex = find_last_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_size_t sFoundIndex = npos;
    const xpr_char_t *sString = mString + aPos;
    const xpr_char_t *sResult;

    do
    {
        sResult = (const xpr_char_t *)memchr(aString, *sString, aLength);
        if (XPR_IS_NOT_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (--sString >= mString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_last_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sFoundIndex = npos;
    const xpr_wchar_t *sString = mString + aPos;
    const xpr_wchar_t *sResult;

    do
    {
        sResult = (const xpr_wchar_t *)wmemchr(aString, *sString, aLength);
        if (XPR_IS_NOT_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (--sString >= mString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    xpr_size_t sFoundIndex = find_last_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_last_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return find_last_of(aString, aPos, sLength);
}

xpr_size_t String::find_last_of(const xpr_wchar_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return find_last_of(aString, aPos, sLength);
}

xpr_size_t String::find_last_of(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find_last_of(&aChar, aPos, 1);
}

xpr_size_t String::find_last_of(xpr_wchar_t aChar, xpr_size_t aPos) const
{
    return find_last_of(&aChar, aPos, 1);
}

xpr_size_t String::find_first_not_of(const String &aString, xpr_size_t aPos) const
{
    return find_first_not_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_first_not_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    xpr_size_t sFoundIndex = find_first_not_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_size_t sFoundIndex = npos;
    const xpr_char_t *sString = mString + aPos;
    const xpr_char_t *sEndString = mString + mLength - 1;
    const xpr_char_t *sResult;

    do
    {
        sResult = (const xpr_char_t *)memchr(aString, *sString, aLength);
        if (XPR_IS_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (++sString <= sEndString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_first_not_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_size_t sFoundIndex = npos;
    const xpr_wchar_t *sString = mString + aPos;
    const xpr_wchar_t *sEndString = mString + mLength - 1;
    const xpr_wchar_t *sResult;

    do
    {
        sResult = (const xpr_wchar_t *)wmemchr(aString, *sString, aLength);
        if (XPR_IS_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (++sString <= sEndString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    xpr_size_t sFoundIndex = find_first_not_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_first_not_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return find_first_not_of(aString, aPos, sLength);
}

xpr_size_t String::find_first_not_of(const xpr_wchar_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return find_first_not_of(aString, aPos, sLength);
}

xpr_size_t String::find_first_not_of(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find_first_not_of(&aChar, aPos, 1);
}

xpr_size_t String::find_first_not_of(xpr_wchar_t aChar, xpr_size_t aPos) const
{
    return find_first_not_of(&aChar, aPos, 1);
}

xpr_size_t String::find_last_not_of(const String &aString, xpr_size_t aPos) const
{
    return find_last_not_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_last_not_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength);

    xpr_size_t sFoundIndex = find_last_not_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    if (aPos == npos)
    {
        aPos = mLength - 1;
    }

    xpr_size_t sFoundIndex = npos;
    const xpr_char_t *sString = mString + aPos;
    const xpr_char_t *sResult;

    do
    {
        sResult = (const xpr_char_t *)memchr(aString, *sString, aLength);
        if (XPR_IS_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (--sString >= mString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_last_not_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    XPR_ASSERT(aString != XPR_NULL);

    if (aPos != npos && aPos >= mLength)
    {
        return npos;
    }

#if defined(XPR_CFG_UNICODE)
    if (aPos == npos)
    {
        aPos = mLength - 1;
    }

    xpr_size_t sFoundIndex = npos;
    const xpr_wchar_t *sString = mString + aPos;
    const xpr_wchar_t *sResult;

    do
    {
        sResult = (const xpr_wchar_t *)wmemchr(aString, *sString, aLength);
        if (XPR_IS_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (--sString >= mString);

    return sFoundIndex;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength);

    xpr_size_t sFoundIndex = find_last_not_of(sString, aPos, aLength);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sFoundIndex;
#endif // XPR_CFG_UNICODE
}

xpr_size_t String::find_last_not_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return find_last_not_of(aString, aPos, sLength);
}

xpr_size_t String::find_last_not_of(const xpr_wchar_t *aString, xpr_size_t aPos) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return find_last_not_of(aString, aPos, sLength);
}

xpr_size_t String::find_last_not_of(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find_last_not_of(&aChar, aPos, 1);
}

xpr_size_t String::find_last_not_of(xpr_wchar_t aChar, xpr_size_t aPos) const
{
    return find_last_not_of(&aChar, aPos, 1);
}

String String::substr(xpr_size_t aPos, xpr_size_t aLength) const
{
    if (aPos != npos && aPos >= mLength)
    {
        return String();
    }

    xpr_size_t sLength = aLength;
    if ((aPos + sLength) > mLength)
        sLength = mLength - aPos;

    return String(*this, aPos, sLength);
}

xpr_sint_t String::compare(const String &aString) const
{
    if ((XPR_IS_NULL(aString.mString) && XPR_IS_NULL(mString)) ||
        (XPR_IS_NULL(aString.mString) && mLength == 0) ||
        (aString.mLength == 0 && XPR_IS_NULL(mString)) ||
        (aString.mLength == 0 && mLength == 0))
    {
        return 0;
    }

    if (XPR_IS_NULL(aString.mString))
    {
        return 1;
    }

    return compare(0, mLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare(const xpr_char_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return compare(0, mLength, aString, sLength);
}

xpr_sint_t String::compare(const xpr_wchar_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return compare(0, mLength, aString, sLength);
}

xpr_sint_t String::compare(xpr_size_t aPos, xpr_size_t aLength, const String &aString) const
{
    if ((XPR_IS_NULL(aString.mString) && XPR_IS_NULL(mString)) ||
        (XPR_IS_NULL(aString.mString) && aLength == 0) ||
        (aString.mLength == 0 && XPR_IS_NULL(mString)) ||
        (aString.mLength == 0 && aLength == 0))
    {
        return 0;
    }

    if (XPR_IS_NULL(aString.mString))
    {
        return 1;
    }

    return compare(aPos, aLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return compare(aPos, aLength, aString, sLength);
}

xpr_sint_t String::compare(xpr_size_t aPos, xpr_size_t aLength, const xpr_wchar_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return compare(aPos, aLength, aString, sLength);
}

xpr_sint_t String::compare(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2) const
{
    if ((XPR_IS_NULL(aString.mString) && XPR_IS_NULL(mString)) ||
        (XPR_IS_NULL(aString.mString) && aLength1 == 0) ||
        (aLength2 == 0 && XPR_IS_NULL(mString)) ||
        (aLength2 == 0 && aLength1 == 0))
    {
        return 0;
    }

    if (XPR_IS_NULL(aString.mString))
    {
        return 1;
    }

    return compare(aPos1, aLength1, aString.mString + aPos2, aLength2);
}

xpr_sint_t String::compare(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2) const
{
    XPR_ASSERT(aString != XPR_NULL);
    XPR_ASSERT(aPos1 <= mLength);

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength2);

    xpr_sint_t sCompare = compare(aPos1, aLength1, sString, aLength2);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sCompare;
#else // not XPR_CFG_UNICODE
    if (mLength < (aPos1 + aLength1))
        aLength1 = mLength - aPos1;

    xpr_sint_t sCompare = memcmp(mString + aPos1, aString, (aLength1 < aLength2) ? aLength1 : aLength2);
    if (sCompare != 0)
        return sCompare;

    if (aLength1 == aLength2)
        return 0;

    return (aLength1 < aLength2) ? -1 : 1;
#endif // XPR_CFG_UNICODE
}

xpr_sint_t String::compare(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_wchar_t *aString, xpr_size_t aLength2) const
{
    XPR_ASSERT(aString != XPR_NULL);
    XPR_ASSERT(aPos1 <= mLength);

#if defined(XPR_CFG_UNICODE)
    if (mLength < (aPos1 + aLength1))
        aLength1 = mLength - aPos1;

    xpr_sint_t sCompare = wmemcmp(mString + aPos1, aString, (aLength1 < aLength2) ? aLength1 : aLength2);
    if (sCompare != 0)
        return sCompare;

    if (aLength1 == aLength2)
        return 0;

    return (aLength1 < aLength2) ? -1 : 1;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength2);

    xpr_sint_t sCompare = compare(aPos1, aLength1, sString, aLength2);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sCompare;
#endif // XPR_CFG_UNICODE
}

xpr_sint_t String::compare_case(const String &aString) const
{
    if ((XPR_IS_NULL(aString.mString) && XPR_IS_NULL(mString)) ||
        (XPR_IS_NULL(aString.mString) && mLength == 0) ||
        (aString.mLength == 0 && XPR_IS_NULL(mString)) ||
        (aString.mLength == 0 && mLength == 0))
    {
        return 0;
    }

    if (XPR_IS_NULL(aString.mString))
    {
        return 1;
    }

    return compare_case(0, mLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare_case(const xpr_char_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return compare_case(0, mLength, aString, sLength);
}

xpr_sint_t String::compare_case(const xpr_wchar_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return compare_case(0, mLength, aString, sLength);
}

xpr_sint_t String::compare_case(xpr_size_t aPos, xpr_size_t aLength, const String &aString) const
{
    if ((XPR_IS_NULL(aString.mString) && XPR_IS_NULL(mString)) ||
        (XPR_IS_NULL(aString.mString) && aLength == 0) ||
        (aString.mLength == 0 && XPR_IS_NULL(mString)) ||
        (aString.mLength == 0 && aLength == 0))
    {
        return 0;
    }

    if (XPR_IS_NULL(aString.mString))
    {
        return 1;
    }

    return compare_case(aPos, aLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare_case(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = strlen(aString);

    return compare_case(aPos, aLength, aString, sLength);
}

xpr_sint_t String::compare_case(xpr_size_t aPos, xpr_size_t aLength, const xpr_wchar_t *aString) const
{
    XPR_ASSERT(aString != XPR_NULL);

    xpr_size_t sLength = wcslen(aString);

    return compare_case(aPos, aLength, aString, sLength);
}

xpr_sint_t String::compare_case(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2) const
{
    if ((XPR_IS_NULL(aString.mString) && XPR_IS_NULL(mString)) ||
        (XPR_IS_NULL(aString.mString) && aLength1 == 0) ||
        (aLength2 == 0 && XPR_IS_NULL(mString)) ||
        (aLength2 == 0 && aLength1 == 0))
    {
        return 0;
    }

    if (XPR_IS_NULL(aString.mString))
    {
        return 1;
    }

    return compare_case(aPos1, aLength1, aString.mString + aPos2, aLength2);
}

xpr_sint_t String::compare_case(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2) const
{
    XPR_ASSERT(aString != XPR_NULL);
    XPR_ASSERT(aPos1 <= mLength);

#if defined(XPR_CFG_UNICODE)
    xpr_wchar_t *sString = toWideString(aString, aLength2);

    xpr_sint_t sCompare = compare_case(aPos1, aLength1, sString, aLength2);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sCompare;
#else // not XPR_CFG_UNICODE
    if (mLength < (aPos1 + aLength1))
        aLength1 = mLength - aPos1;

    xpr_sint_t sCompare = _memicmp(mString + aPos1, aString, (aLength1 < aLength2) ? aLength1 : aLength2);
    if (sCompare != 0)
        return sCompare;

    if (aLength1 == aLength2)
        return 0;

    return (aLength1 < aLength2) ? -1 : 1;
#endif // XPR_CFG_UNICODE
}

xpr_sint_t String::compare_case(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_wchar_t *aString, xpr_size_t aLength2) const
{
    XPR_ASSERT(aString != XPR_NULL);
    XPR_ASSERT(aPos1 <= mLength);

#if defined(XPR_CFG_UNICODE)
    if (mLength < (aPos1 + aLength1))
        aLength1 = mLength - aPos1;

    xpr_sint_t sCompare = _wcsnicmp(mString + aPos1, aString, (aLength1 < aLength2) ? aLength1 : aLength2);
    if (sCompare != 0)
        return sCompare;

    if (aLength1 == aLength2)
        return 0;

    return (aLength1 < aLength2) ? -1 : 1;
#else // not XPR_CFG_UNICODE
    xpr_char_t *sString = toString(aString, aLength2);

    xpr_sint_t sCompare = compare_case(aPos1, aLength1, sString, aLength2);

    XPR_SAFE_DELETE_ARRAY(sString);

    return sCompare;
#endif // XPR_CFG_UNICODE
}
} // namespace xpr
