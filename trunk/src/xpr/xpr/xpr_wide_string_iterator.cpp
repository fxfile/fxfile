//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_wide_string_iterator.h"

namespace xpr
{
WideStringIterator::WideStringIterator(void)
    : mString(XPR_NULL)
{
}

WideStringIterator::WideStringIterator(xpr_wchar_t *aString)
    : mString(aString)
{
}

xpr_wchar_t& WideStringIterator::operator* () const
{
    return *mString;
}

xpr_wchar_t* WideStringIterator::operator-> () const
{
    return mString;
}

WideStringIterator& WideStringIterator::operator++ ()
{
    ++mString;

    return *this;
}

WideStringIterator WideStringIterator::operator++ (xpr_sint_t)
{
    WideStringIterator sIterator = *this;

    ++mString;

    return sIterator;
}

WideStringIterator& WideStringIterator::operator-- ()
{
    --mString;

    return *this;
}

WideStringIterator WideStringIterator::operator-- (xpr_sint_t)
{
    WideStringIterator sIterator = *this;

    --mString;

    return sIterator;
}

WideStringIterator& WideStringIterator::operator+= (xpr_sint_t aOffset)
{
    mString += aOffset;

    return *this;
}

WideStringIterator WideStringIterator::operator+ (xpr_sint_t aOffset) const
{
    WideStringIterator sIterator = *this;

    sIterator += aOffset;

    return sIterator;
}

WideStringIterator& WideStringIterator::operator-= (xpr_sint_t aOffset)
{
    mString -= aOffset;

    return *this;
}

WideStringIterator WideStringIterator::operator- (xpr_sint_t aOffset) const
{
    WideStringIterator sIterator = *this;

    sIterator -= aOffset;

    return sIterator;
}

xpr_sint_t WideStringIterator::operator- (const WideStringIterator &aIterator) const
{
    return (xpr_sint_t)(mString - aIterator.mString);
}

xpr_wchar_t& WideStringIterator::operator[] (xpr_sint_t aOffset) const
{
    return mString[aOffset];
}

bool WideStringIterator::operator== (const WideStringIterator &aIterator) const
{
    return (mString == aIterator.mString) ? true : false;
}

bool WideStringIterator::operator!= (const WideStringIterator &aIterator) const
{
    return (mString != aIterator.mString) ? true : false;
}

bool WideStringIterator::operator< (const WideStringIterator &aIterator) const
{
    return (mString < aIterator.mString) ? true : false;
}

bool WideStringIterator::operator<= (const WideStringIterator &aIterator) const
{
    return (mString <= aIterator.mString) ? true : false;
}

bool WideStringIterator::operator> (const WideStringIterator &aIterator) const
{
    return (mString > aIterator.mString) ? true : false;
}

bool WideStringIterator::operator>= (const WideStringIterator &aIterator) const
{
    return (mString >= aIterator.mString) ? true : false;
}










WideStringConstIterator::WideStringConstIterator(void)
    : mString(XPR_NULL)
{
}

WideStringConstIterator::WideStringConstIterator(const WideStringIterator &aIterator)
    : mString(aIterator.mString)
{
}

WideStringConstIterator::WideStringConstIterator(const xpr_wchar_t *aString)
    : mString(aString)
{
}

const xpr_wchar_t& WideStringConstIterator::operator* () const
{
    return *mString;
}

const xpr_wchar_t* WideStringConstIterator::operator-> () const
{
    return mString;
}

WideStringConstIterator& WideStringConstIterator::operator++ ()
{
    ++mString;

    return *this;
}

WideStringConstIterator WideStringConstIterator::operator++ (xpr_sint_t)
{
    WideStringConstIterator sIterator = *this;

    ++mString;

    return sIterator;
}

WideStringConstIterator& WideStringConstIterator::operator-- ()
{
    --mString;

    return *this;
}

WideStringConstIterator WideStringConstIterator::operator-- (xpr_sint_t)
{
    WideStringConstIterator sIterator = *this;

    --mString;

    return sIterator;
}

WideStringConstIterator& WideStringConstIterator::operator+= (xpr_sint_t aOffset)
{
    mString += aOffset;

    return *this;
}

WideStringConstIterator WideStringConstIterator::operator+ (xpr_sint_t aOffset) const
{
    WideStringConstIterator sIterator = *this;

    sIterator += aOffset;

    return sIterator;
}

WideStringConstIterator& WideStringConstIterator::operator-= (xpr_sint_t aOffset)
{
    mString -= aOffset;

    return *this;
}

WideStringConstIterator WideStringConstIterator::operator- (xpr_sint_t aOffset) const
{
    WideStringConstIterator sIterator = *this;

    sIterator -= aOffset;

    return sIterator;
}

xpr_sint_t WideStringConstIterator::operator- (const WideStringConstIterator &aIterator) const
{
    return (xpr_sint_t)(mString - aIterator.mString);
}

const xpr_wchar_t& WideStringConstIterator::operator[] (xpr_sint_t aOffset) const
{
    return mString[aOffset];
}

bool WideStringConstIterator::operator== (const WideStringConstIterator &aIterator) const
{
    return (mString == aIterator.mString) ? true : false;
}

bool WideStringConstIterator::operator!= (const WideStringConstIterator &aIterator) const
{
    return (mString != aIterator.mString) ? true : false;
}

bool WideStringConstIterator::operator< (const WideStringConstIterator &aIterator) const
{
    return (mString < aIterator.mString) ? true : false;
}

bool WideStringConstIterator::operator<= (const WideStringConstIterator &aIterator) const
{
    return (mString <= aIterator.mString) ? true : false;
}

bool WideStringConstIterator::operator> (const WideStringConstIterator &aIterator) const
{
    return (mString > aIterator.mString) ? true : false;
}

bool WideStringConstIterator::operator>= (const WideStringConstIterator &aIterator) const
{
    return (mString >= aIterator.mString) ? true : false;
}





WideStringReverseIterator::WideStringReverseIterator(void)
    : mString(XPR_NULL)
{
}

WideStringReverseIterator::WideStringReverseIterator(xpr_wchar_t *aString)
    : mString(aString)
{
}

xpr_wchar_t& WideStringReverseIterator::operator* () const
{
    return *mString;
}

xpr_wchar_t* WideStringReverseIterator::operator-> () const
{
    return mString;
}

WideStringReverseIterator& WideStringReverseIterator::operator++ ()
{
    --mString;

    return *this;
}

WideStringReverseIterator WideStringReverseIterator::operator++ (xpr_sint_t)
{
    WideStringReverseIterator sIterator = *this;

    --mString;

    return sIterator;
}

WideStringReverseIterator& WideStringReverseIterator::operator-- ()
{
    ++mString;

    return *this;
}

WideStringReverseIterator WideStringReverseIterator::operator-- (xpr_sint_t)
{
    WideStringReverseIterator sIterator = *this;

    ++mString;

    return sIterator;
}

WideStringReverseIterator& WideStringReverseIterator::operator+= (xpr_sint_t aOffset)
{
    mString += aOffset;

    return *this;
}

WideStringReverseIterator WideStringReverseIterator::operator+ (xpr_sint_t aOffset) const
{
    WideStringReverseIterator sIterator = *this;

    sIterator += aOffset;

    return sIterator;
}

WideStringReverseIterator& WideStringReverseIterator::operator-= (xpr_sint_t aOffset)
{
    mString -= aOffset;

    return *this;
}

WideStringReverseIterator WideStringReverseIterator::operator- (xpr_sint_t aOffset) const
{
    WideStringReverseIterator sIterator = *this;

    sIterator -= aOffset;

    return sIterator;
}

xpr_sint_t WideStringReverseIterator::operator- (const WideStringReverseIterator &aIterator) const
{
    return (xpr_sint_t)(mString - aIterator.mString);
}

xpr_wchar_t& WideStringReverseIterator::operator[] (xpr_sint_t aOffset) const
{
    return mString[-aOffset];
}

bool WideStringReverseIterator::operator== (const WideStringReverseIterator &aIterator) const
{
    return (mString == aIterator.mString) ? true : false;
}

bool WideStringReverseIterator::operator!= (const WideStringReverseIterator &aIterator) const
{
    return (mString != aIterator.mString) ? true : false;
}

bool WideStringReverseIterator::operator< (const WideStringReverseIterator &aIterator) const
{
    return (mString > aIterator.mString) ? true : false;
}

bool WideStringReverseIterator::operator<= (const WideStringReverseIterator &aIterator) const
{
    return (mString >= aIterator.mString) ? true : false;
}

bool WideStringReverseIterator::operator> (const WideStringReverseIterator &aIterator) const
{
    return (mString < aIterator.mString) ? true : false;
}

bool WideStringReverseIterator::operator>= (const WideStringReverseIterator &aIterator) const
{
    return (mString <= aIterator.mString) ? true : false;
}






WideStringConstReverseIterator::WideStringConstReverseIterator(void)
    : mString(XPR_NULL)
{
}

WideStringConstReverseIterator::WideStringConstReverseIterator(const WideStringReverseIterator &aIterator)
    : mString(aIterator.mString)
{
}

WideStringConstReverseIterator::WideStringConstReverseIterator(const xpr_wchar_t *aString)
    : mString(aString)
{
}

const xpr_wchar_t& WideStringConstReverseIterator::operator* () const
{
    return *mString;
}

const xpr_wchar_t* WideStringConstReverseIterator::operator-> () const
{
    return mString;
}

WideStringConstReverseIterator& WideStringConstReverseIterator::operator++ ()
{
    --mString;

    return *this;
}

WideStringConstReverseIterator WideStringConstReverseIterator::operator++ (xpr_sint_t)
{
    WideStringConstReverseIterator sIterator = *this;

    --mString;

    return sIterator;
}

WideStringConstReverseIterator& WideStringConstReverseIterator::operator-- ()
{
    ++mString;

    return *this;
}

WideStringConstReverseIterator WideStringConstReverseIterator::operator-- (xpr_sint_t)
{
    WideStringConstReverseIterator sIterator = *this;

    ++mString;

    return sIterator;
}

WideStringConstReverseIterator& WideStringConstReverseIterator::operator+= (xpr_sint_t aOffset)
{
    mString -= aOffset;

    return *this;
}

WideStringConstReverseIterator WideStringConstReverseIterator::operator+ (xpr_sint_t aOffset) const
{
    WideStringConstReverseIterator sIterator = *this;

    sIterator += aOffset;

    return sIterator;
}

WideStringConstReverseIterator& WideStringConstReverseIterator::operator-= (xpr_sint_t aOffset)
{
    mString += aOffset;

    return *this;
}

WideStringConstReverseIterator WideStringConstReverseIterator::operator- (xpr_sint_t aOffset) const
{
    WideStringConstReverseIterator sIterator = *this;

    sIterator -= aOffset;

    return sIterator;
}

xpr_sint_t WideStringConstReverseIterator::operator- (const WideStringConstReverseIterator &aIterator) const
{
    return (xpr_sint_t)(mString - aIterator.mString);
}

const xpr_wchar_t& WideStringConstReverseIterator::operator[] (xpr_sint_t aOffset) const
{
    return mString[-aOffset];
}

bool WideStringConstReverseIterator::operator== (const WideStringConstReverseIterator &aIterator) const
{
    return (mString == aIterator.mString) ? true : false;
}

bool WideStringConstReverseIterator::operator!= (const WideStringConstReverseIterator &aIterator) const
{
    return (mString != aIterator.mString) ? true : false;
}

bool WideStringConstReverseIterator::operator< (const WideStringConstReverseIterator &aIterator) const
{
    return (mString > aIterator.mString) ? true : false;
}

bool WideStringConstReverseIterator::operator<= (const WideStringConstReverseIterator &aIterator) const
{
    return (mString >= aIterator.mString) ? true : false;
}

bool WideStringConstReverseIterator::operator> (const WideStringConstReverseIterator &aIterator) const
{
    return (mString < aIterator.mString) ? true : false;
}

bool WideStringConstReverseIterator::operator>= (const WideStringConstReverseIterator &aIterator) const
{
    return (mString <= aIterator.mString) ? true : false;
}
} // namespace xpr
