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

namespace xpr
{
static const xpr_size_t kAllocSizeMask =
      sizeof (xpr_char_t) <= 1 ? 15
    : sizeof (xpr_char_t) <= 2 ? 7
    : sizeof (xpr_char_t) <= 4 ? 3
    : sizeof (xpr_char_t) <= 8 ? 1 : 0;

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
        assign(XPR_MBCS_STRING_LITERAL(""), 0);
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
        assign(XPR_MBCS_STRING_LITERAL(""), 0);
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

String::String(Iterator aFirst, Iterator aLast)
    : mString(XPR_NULL)
    , mLength(0)
    , mCapacity(0)
{
    assign(aFirst, aLast);
}

String::~String(void)
{
    clear();
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
    return mLength * sizeof(xpr_char_t);
}

xpr_size_t String::max_size(void) const
{
    return (xpr_size_t)-1;
}

void String::resize(xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0)
        return;

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
}

xpr_size_t String::get_capacity(const xpr_size_t &aMinCapacity) const
{
    return aMinCapacity | kAllocSizeMask;
}

xpr_char_t *String::alloc(const xpr_size_t &aMinCapacity, xpr_size_t &aCapacity) const
{
    xpr_size_t sNewCapacity = get_capacity(aMinCapacity);

    xpr_char_t *sNewString = new xpr_char_t[sNewCapacity];
    if (XPR_IS_NULL(sNewString))
        return XPR_NULL;

    aCapacity = sNewCapacity;

    return sNewString;
}

void String::resize(xpr_size_t aNumber)
{
    resize(aNumber, 0);
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
        xpr_char_t *sNewString = alloc(aCapacity, sNewCapacity);

        XPR_ASSERT(sNewString != XPR_NULL);
        if (XPR_IS_NULL(sNewString))
            return;

        strncpy(sNewString, mString, mLength);
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

String& String::assign(const String &aString)
{
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
    if (XPR_IS_NULL(aString))
    {
        return *this;
    }

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
}

String& String::assign(const xpr_char_t *aString)
{
    if (XPR_IS_NULL(aString))
    {
        return *this;
    }

    xpr_size_t sLength = strlen(aString);

    return assign(aString, sLength);
}

String& String::assign(xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0)
    {
        return *this;
    }

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
}

String& String::assign(Iterator aFirst, Iterator aLast)
{
    xpr_char_t *sString = &(*aFirst);
    xpr_size_t  sLength = aLast - aFirst;

    return assign(sString, sLength);
}

const xpr_char_t& String::operator[] (xpr_size_t aPos) const
{
    XPR_ASSERT(aPos < mLength);

    return mString[aPos];
}

xpr_char_t& String::operator[] (xpr_size_t aPos)
{
    XPR_ASSERT(aPos < mLength);

    return mString[aPos];
}

const xpr_char_t& String::at(xpr_size_t aPos) const
{
    XPR_ASSERT(aPos < mLength);

    return mString[aPos];
}

xpr_char_t& String::at(xpr_size_t aPos)
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

String& String::operator= (xpr_char_t aChar)
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

String& String::operator+= (xpr_char_t aChar)
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
    if (XPR_IS_NULL(aString) || aLength == 0)
    {
        return *this;
    }

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
}

String& String::append(const xpr_char_t *aString)
{
    if (XPR_IS_NULL(aString))
    {
        return *this;
    }

    xpr_size_t sLength = strlen(aString);

    return append(aString, sLength);
}

String& String::append(xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0)
    {
        return *this;
    }

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
}

String& String::append(Iterator aFirst, Iterator aLast)
{
    xpr_char_t *sString = &(*aFirst);
    xpr_size_t  sLength = aLast - aFirst;

    return append(sString, sLength);
}

xpr_char_t &String::front(void)
{
    return mString[0];
}

const xpr_char_t &String::front(void) const
{
    return mString[0];
}

xpr_char_t &String::back(void)
{
    if (mLength == 0)
        return mString[0];

    return mString[mLength - 1];
}

const xpr_char_t &String::back(void) const
{
    if (mLength == 0)
        return mString[0];

    return mString[mLength - 1];
}

void String::push_back(xpr_char_t aChar)
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
    if (XPR_IS_NULL(aString) || aLength == 0)
    {
        return *this;
    }

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
}

String& String::insert(xpr_size_t aPos, const xpr_char_t *aString)
{
    if (XPR_IS_NULL(aString))
    {
        return *this;
    }

    xpr_size_t sLength = strlen(aString);

    return insert(aPos, aString, sLength);
}

String& String::insert(xpr_size_t aPos, xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aNumber == 0 || aChar == 0)
    {
        return *this;
    }

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
}

String::Iterator String::insert(Iterator aPos, xpr_char_t aChar)
{
    return insert(aPos, 1, aChar);
}

String::Iterator String::insert(Iterator aPos, xpr_size_t aNumber, xpr_char_t aChar)
{
    xpr_size_t sPos = aPos.mString - mString;

    insert(sPos, aNumber, aChar);

    return Iterator(mString + sPos);
}

String::Iterator String::insert(Iterator aPos, Iterator aFirst, Iterator aLast)
{
    xpr_size_t  sPos    = aPos.mString - mString;
    xpr_char_t *sString = &(*aFirst);
    xpr_size_t  sLength = aLast - aFirst;

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

    memmove(mString + aPos, mString + aPos + sLength, (mLength - (aPos + sLength)) * sizeof(xpr_char_t));
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
    if (aPos == npos || aPos >= mLength || XPR_IS_NULL(aString) || aLength2 == 0)
    {
        return *this;
    }

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
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString, xpr_size_t aLength)
{
    xpr_size_t sPos    = aIterator1.mString - mString;
    xpr_size_t sLength = aIterator2 - aIterator1;

    return replace(sPos, sLength, aString, aLength);
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString)
{
    if (XPR_IS_NULL(aString))
    {
        return *this;
    }

    xpr_size_t sLength = strlen(aString);

    return replace(aPos, aLength, aString, sLength);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString)
{
    if (XPR_IS_NULL(aString))
    {
        return *this;
    }

    xpr_size_t sPos     = aIterator1.mString - mString;
    xpr_size_t sLength1 = aIterator2 - aIterator1;

    xpr_size_t sLength2 = strlen(aString);

    return replace(sPos, sLength1, aString, sLength2);
}

String& String::replace(xpr_size_t aPos, xpr_size_t aLength, xpr_size_t aNumber, xpr_char_t aChar)
{
    if (aPos == npos || aPos >= mLength || aNumber == 0 || aChar == 0)
    {
        return *this;
    }

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
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, xpr_size_t aNumber, xpr_char_t aChar)
{
    xpr_size_t sPos    = aIterator1.mString - mString;
    xpr_size_t sLength = aIterator2 - aIterator1;

    return replace(sPos, sLength, aNumber, aChar);
}

String& String::replace(Iterator aIterator1, Iterator aIterator2, Iterator aInputIterator1, Iterator aInputIterator2)
{
    xpr_size_t  sPos    = aIterator1.mString - mString;
    xpr_size_t  sLength1 = aIterator2 - aIterator1;

    xpr_char_t *sString  = aInputIterator1.mString;
    xpr_size_t  sLength2 = aInputIterator2 - aInputIterator1;

    return replace(sPos, sLength1, sString, sLength2);
}

void String::swap(String &aString)
{
    xpr_char_t *sTempString   = mString;
    xpr_size_t  sTempLength   = mLength;
    xpr_size_t  sTempCapacity = mCapacity;

    mString   = aString.mString;
    mLength   = aString.mLength;
    mCapacity = aString.mCapacity;

    aString.mString   = sTempString;
    aString.mLength   = sTempLength;
    aString.mCapacity = sTempCapacity;
}

void String::upper_case(void)
{
    _strupr(mString);
}

void String::lower_case(void)
{
    _strlwr(mString);
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
        if (xpr::isSpace(mString[i]) == XPR_FALSE)
        {
            erase(0, i);
            break;
        }
    }
}

void String::trim_right(void)
{
    xpr_size_t i;
    for (i = mLength - 1; i >= 0; --i)
    {
        if (xpr::isSpace(mString[i]) == XPR_FALSE)
        {
            erase(i, mLength - i - 1);
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

void String::format_args(const xpr_char_t *aFormat, va_list aArgs)
{
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
}

void String::append_format(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    append_format_args(aFormat, sArgs);

    va_end(sArgs);
}

void String::append_format_args(const xpr_char_t *aFormat, va_list aArgs)
{
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
}

void String::update(void)
{
    mLength = strnlen(mString, mCapacity - 1);
}

void String::shrink_to_fit(void)
{
    xpr_size_t sFitCapacity = get_capacity(mLength + 1);

    if (mCapacity > sFitCapacity)
    {
        xpr_size_t sNewCapacity;
        xpr_size_t sLength = mLength;
        xpr_char_t *sNewString = alloc(sLength + 1, sNewCapacity);

        strncpy(sNewString, mString, sLength);
        sNewString[sLength] = 0;

        reset();

        mString = sNewString;
        mLength = sLength;
        mCapacity = sNewCapacity;
    }
}

const xpr_char_t *String::c_str(void) const
{
    return mString;
}

const xpr_char_t *String::data(void) const
{
    return mString;
}

xpr_size_t String::copy(xpr_char_t *aString, xpr_size_t aLength, xpr_size_t aPos) const
{
    if (XPR_IS_NULL(aString) || aLength == 0 || aPos == npos || aPos >= mLength)
    {
        return 0;
    }

    xpr_size_t sLength = aLength;

    if ((aPos + sLength) > mLength)
    {
        sLength = mLength - aPos;
    }

    strncpy(aString, mString + aPos, sLength);

    return sLength;
}

xpr_size_t String::find(const String &aString, xpr_size_t aPos) const
{
    return find(aString.mString, aPos, mLength);
}

xpr_size_t String::find(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    if (XPR_IS_NULL(aString) || aPos == npos || aPos >= mLength)
    {
        return npos;
    }

    const xpr_char_t *sString = mString + aPos;
    const xpr_char_t *sSearch = aString;
    xpr_char_t sSubStrChar, sChar;
    xpr_size_t sLength;

    if ((sSubStrChar = *sSearch++) != '\0')
    {
        sLength = strlen(sSearch);
        if (sLength > aLength)
            sLength = aLength;

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
}

xpr_size_t String::find(const xpr_char_t *aString, xpr_size_t aPos) const
{
    if (XPR_IS_NULL(aString))
    {
        return npos;
    }

    xpr_size_t sLength = strlen(aString);

    return find(aString, aPos, sLength);
}

xpr_size_t String::find(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find(&aChar, aPos, 1);
}

xpr_size_t String::rfind(const String &aString, xpr_size_t aPos) const
{
    return rfind(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::rfind(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    if (XPR_IS_NULL(aString) || (aPos != npos && aPos >= mLength))
    {
        return npos;
    }

    if (aLength == 0)
    {
        return 0;
    }

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
}

xpr_size_t String::rfind(const xpr_char_t *aString, xpr_size_t aPos) const
{
    if (XPR_IS_NULL(aString))
    {
        return npos;
    }

    xpr_size_t sLength = strlen(aString);

    return rfind(aString, aPos, sLength);
}

xpr_size_t String::rfind(xpr_char_t aChar, xpr_size_t aPos) const
{
    return rfind(&aChar, aPos, 1);
}

xpr_size_t String::find_first_of(const String &aString, xpr_size_t aPos) const
{
    return find_first_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_first_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    if (XPR_IS_NULL(aString) || (aPos != npos && aPos >= mLength))
    {
        return npos;
    }

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
}

xpr_size_t String::find_first_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    if (XPR_IS_NULL(aString))
    {
        return npos;
    }

    xpr_size_t sLength = strlen(aString);

    return find_first_of(aString, aPos, sLength);
}

xpr_size_t String::find_first_of(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find_first_of(&aChar, aPos, 1);
}

xpr_size_t String::find_last_of(const String &aString, xpr_size_t aPos) const
{
    return find_last_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_last_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    if (XPR_IS_NULL(aString) || (aPos != npos && aPos >= mLength))
    {
        return npos;
    }

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
        if (XPR_IS_NOT_NULL(sResult))
        {
            sFoundIndex = (xpr_size_t)(sString - mString);
            break;
        }
    } while (--sString >= mString);

    return sFoundIndex;
}

xpr_size_t String::find_last_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    if (XPR_IS_NULL(aString))
    {
        return npos;
    }

    xpr_size_t sLength = strlen(aString);

    return find_last_of(aString, aPos, sLength);
}

xpr_size_t String::find_last_of(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find_last_of(&aChar, aPos, 1);
}

xpr_size_t String::find_first_not_of(const String &aString, xpr_size_t aPos) const
{
    return find_first_not_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_first_not_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    if (XPR_IS_NULL(aString) || (aPos != npos && aPos >= mLength))
    {
        return npos;
    }

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
}

xpr_size_t String::find_first_not_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    if (XPR_IS_NULL(aString))
    {
        return npos;
    }

    xpr_size_t sLength = strlen(aString);

    return find_first_not_of(aString, aPos, sLength);
}

xpr_size_t String::find_first_not_of(xpr_char_t aChar, xpr_size_t aPos) const
{
    return find_first_not_of(&aChar, aPos, 1);
}

xpr_size_t String::find_last_not_of(const String &aString, xpr_size_t aPos) const
{
    return find_last_not_of(aString.mString, aPos, aString.mLength);
}

xpr_size_t String::find_last_not_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const
{
    if (XPR_IS_NULL(aString) || (aPos != npos && aPos >= mLength))
    {
        return npos;
    }

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
}

xpr_size_t String::find_last_not_of(const xpr_char_t *aString, xpr_size_t aPos) const
{
    if (XPR_IS_NULL(aString))
    {
        return npos;
    }

    xpr_size_t sLength = strlen(aString);

    return find_last_not_of(aString, aPos, sLength);
}

xpr_size_t String::find_last_not_of(xpr_char_t aChar, xpr_size_t aPos) const
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
    return compare(0, mLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare(const xpr_char_t *aString) const
{
    if (XPR_IS_NULL(aString))
        return 1;

    xpr_size_t sLength = strlen(aString);

    return compare(0, mLength, aString, sLength);
}

xpr_sint_t String::compare(xpr_size_t aPos, xpr_size_t aLength, const String &aString) const
{
    return compare(aPos, aLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const
{
    if (XPR_IS_NULL(aString))
        return 1;

    xpr_size_t sLength = strlen(aString);

    return compare(aPos, aLength, aString, sLength);
}

xpr_sint_t String::compare(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2) const
{
    return compare(aPos1, aLength1, aString.mString + aPos2, aLength2);
}

xpr_sint_t String::compare(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2) const
{
    XPR_ASSERT(aPos1 <= mLength);

    if (mLength < (aPos1 + aLength1))
        aLength1 = mLength - aPos1;

    xpr_sint_t sCompare = memcmp(mString + aPos1, aString, (aLength1 < aLength2) ? aLength1 : aLength2);
    if (sCompare != 0)
        return sCompare;

    if (aLength1 == aLength2)
        return 0;

    return (aLength1 < aLength2) ? -1 : 1;
}

xpr_sint_t String::compare_case(const String &aString) const
{
    return compare_case(0, mLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare_case(const xpr_char_t *aString) const
{
    if (XPR_IS_NULL(aString))
        return 1;

    xpr_size_t sLength = strlen(aString);

    return compare_case(0, mLength, aString, sLength);
}

xpr_sint_t String::compare_case(xpr_size_t aPos, xpr_size_t aLength, const String &aString) const
{
    return compare_case(aPos, aLength, aString.mString, aString.mLength);
}

xpr_sint_t String::compare_case(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const
{
    if (XPR_IS_NULL(aString))
        return 1;

    xpr_size_t sLength = strlen(aString);

    return compare_case(aPos, aLength, aString, sLength);
}

xpr_sint_t String::compare_case(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2) const
{
    return compare_case(aPos1, aLength1, aString.mString + aPos2, aLength2);
}

xpr_sint_t String::compare_case(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2) const
{
    XPR_ASSERT(aPos1 <= mLength);

    if (mLength < (aPos1 + aLength1))
        aLength1 = mLength - aPos1;

    xpr_sint_t sCompare = _memicmp(mString + aPos1, aString, (aLength1 < aLength2) ? aLength1 : aLength2);
    if (sCompare != 0)
        return sCompare;

    if (aLength1 == aLength2)
        return 0;

    return (aLength1 < aLength2) ? -1 : 1;
}
} // namespace xpr
