//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_WIDE_STRING_H__
#define __XPR_WIDE_STRING_H__
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_wide_string_iterator.h"

#if defined(XPR_CFG_STL_TR1)
#include <functional>
#endif // XPR_CFG_STL_TR1

namespace xpr
{
class XPR_DL_API WideString
{
public:
    enum { npos = -1 };

    typedef WideStringIterator Iterator;
    typedef WideStringIterator iterator;
    typedef WideStringConstIterator ConstIterator;
    typedef WideStringConstIterator const_iterator;
    typedef WideStringReverseIterator ReverseIterator;
    typedef WideStringReverseIterator reverse_iterator;
    typedef WideStringConstReverseIterator ConstReverseIterator;
    typedef WideStringConstReverseIterator const_reverse_iterator;

public:
    WideString(void);
    WideString(const WideString &aString);
    WideString(const WideString &aString, xpr_size_t aPos, xpr_size_t aLength = npos);
    WideString(const xpr_wchar_t *aString, xpr_size_t aLength);
    WideString(const xpr_wchar_t *aString);
    WideString(const xpr_size_t aNumber, xpr_wchar_t aChar);
    WideString(Iterator aFirst, Iterator aLast);
    virtual ~WideString(void);

public:
    // iterators
    Iterator             begin(void);
    ConstIterator        begin(void) const;
    ConstIterator        cbegin(void) const;
    Iterator             end(void);
    ConstIterator        end(void) const;
    ConstIterator        cend(void) const;
    ReverseIterator      rbegin(void);
    ConstReverseIterator rbegin(void) const;
    ConstReverseIterator crbegin(void) const;
    ReverseIterator      rend(void);
    ConstReverseIterator rend(void) const;
    ConstReverseIterator crend(void) const;

public:
    // capacity
    xpr_size_t size(void) const;
    xpr_size_t length(void) const;
    xpr_size_t bytes(void) const;
    xpr_size_t max_size(void) const;
    void       resize(xpr_size_t aNumber, xpr_wchar_t aChar);
    void       resize(xpr_size_t aNumber);
    xpr_size_t capacity(void) const;
    void       reserve(xpr_size_t aCapacity = 0);
    void       clear(void);
    xpr_bool_t empty(void);

public:
    // element access
    const xpr_wchar_t& operator[] (xpr_size_t aPos) const;
    xpr_wchar_t&       operator[] (xpr_size_t aPos);
    const xpr_wchar_t& at(xpr_size_t aPos) const;
    xpr_wchar_t&       at(xpr_size_t aPos);

public:
    // modifier
    WideString& operator= (const WideString &aString);
    WideString& operator= (const xpr_wchar_t *aString);
    WideString& operator= (xpr_wchar_t aChar);

    WideString& operator+= (const WideString &aString);
    WideString& operator+= (const xpr_wchar_t *aString);
    WideString& operator+= (xpr_wchar_t aChar);

    WideString& append(const WideString &aString);
    WideString& append(const WideString &aString, xpr_size_t aPos, xpr_size_t aLength);
    WideString& append(const xpr_wchar_t *aString, xpr_size_t aLength);
    WideString& append(const xpr_wchar_t *aString);
    WideString& append(xpr_size_t aNumber, xpr_wchar_t aChar);
    WideString& append(Iterator aFirst, Iterator aLast);

    xpr_wchar_t       &front(void);
    const xpr_wchar_t &front(void) const;
    xpr_wchar_t       &back(void);
    const xpr_wchar_t &back(void) const;

    void    push_back(xpr_wchar_t aChar);
    void    pop_back(void);

    WideString& assign(const WideString &aString);
    WideString& assign(const WideString &aString, xpr_size_t aPos, xpr_size_t aLength);
    WideString& assign(const xpr_wchar_t *aString, xpr_size_t aLength);
    WideString& assign(const xpr_wchar_t *aString);
    WideString& assign(xpr_size_t aNumber, xpr_wchar_t aChar);
    WideString& assign(Iterator aFirst, Iterator aLast);

    WideString& insert(xpr_size_t aPos, const WideString &aString);
    WideString& insert(xpr_size_t aPos1, const WideString &aString, xpr_size_t aPos2, xpr_size_t aLength);
    WideString& insert(xpr_size_t aPos, const xpr_wchar_t *aString, xpr_size_t aLength);
    WideString& insert(xpr_size_t aPos, const xpr_wchar_t *aString);
    WideString& insert(xpr_size_t aPos, xpr_size_t aNumber, xpr_wchar_t aChar);
    Iterator    insert(Iterator aPos, xpr_wchar_t aChar);
    Iterator    insert(Iterator aPos, xpr_size_t aNumber, xpr_wchar_t aChar);
    Iterator    insert(Iterator aPos, Iterator aFirst, Iterator aLast);

    WideString& erase(xpr_size_t aPos = 0, xpr_size_t aLength = npos);
    Iterator    erase(Iterator aPos);
    Iterator    erase(Iterator aFirst, Iterator aLast);

    WideString& replace(xpr_size_t aPos, xpr_size_t aLength, const WideString &aString);
    WideString& replace(Iterator aIterator1, Iterator aIterator2, const WideString &aString);
    WideString& replace(xpr_size_t aPos1, xpr_size_t aLength1, const WideString &aString, xpr_size_t aPos2, xpr_size_t aLength2);
    WideString& replace(xpr_size_t aPos, xpr_size_t aLength1, const xpr_wchar_t *aString, xpr_size_t aLength2);
    WideString& replace(Iterator aIterator1, Iterator aIterator2, const xpr_wchar_t *aString, xpr_size_t aLength);
    WideString& replace(xpr_size_t aPos, xpr_size_t aLength, const xpr_wchar_t *aString);
    WideString& replace(Iterator aIterator1, Iterator aIterator2, const xpr_wchar_t *aString);
    WideString& replace(xpr_size_t aPos, xpr_size_t aLength, xpr_size_t aNumber, xpr_wchar_t aChar);
    WideString& replace(Iterator aIterator1, Iterator aIterator2, xpr_size_t aNumber, xpr_wchar_t aChar);
    WideString& replace(Iterator aIterator1, Iterator aIterator2, Iterator aInputIterator1, Iterator aInputIterator2);

    void     swap(WideString &aString);

    void     upper_case(void);
    void     lower_case(void);

    void     trim(void);
    void     trim_left(void);
    void     trim_right(void);

    void     format(const xpr_wchar_t *aFormat, ...);
    void     append_format(const xpr_wchar_t *aFormat, ...);

public:
    // operations
    const xpr_wchar_t *c_str(void) const;

    const xpr_wchar_t *data(void) const;

    xpr_size_t copy(xpr_wchar_t *aString, xpr_size_t aLength, xpr_size_t aPos = 0) const;

    xpr_size_t find(const WideString &aString, xpr_size_t aPos = 0) const;
    xpr_size_t find(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find(const xpr_wchar_t *aString, xpr_size_t aPos = 0) const;
    xpr_size_t find(xpr_wchar_t aChar, xpr_size_t aPos = 0) const;

    xpr_size_t rfind(const WideString &aString, xpr_size_t aPos = npos) const;
    xpr_size_t rfind(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t rfind(const xpr_wchar_t *aString, xpr_size_t aPos = npos) const;
    xpr_size_t rfind(xpr_wchar_t aChar, xpr_size_t aPos = npos) const;

    xpr_size_t find_first_of(const WideString &aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_first_of(const xpr_wchar_t *aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_of(xpr_wchar_t aChar, xpr_size_t aPos = 0) const;

    xpr_size_t find_last_of(const WideString &aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_last_of(const xpr_wchar_t *aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_of(xpr_wchar_t aChar, xpr_size_t aPos = npos) const;

    xpr_size_t find_first_not_of(const WideString &aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_not_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_first_not_of(const xpr_wchar_t *aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_not_of(xpr_wchar_t aChar, xpr_size_t aPos = 0) const;

    xpr_size_t find_last_not_of(const WideString &aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_not_of(const xpr_wchar_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_last_not_of(const xpr_wchar_t *aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_not_of(xpr_wchar_t aChar, xpr_size_t aPos = npos) const;

    WideString substr(xpr_size_t aPos = 0, xpr_size_t aLength = npos) const;

    xpr_sint_t compare(const WideString &aString) const;
    xpr_sint_t compare(const xpr_wchar_t *aString) const;
    xpr_sint_t compare(xpr_size_t aPos, xpr_size_t aLength, const WideString &aString) const;
    xpr_sint_t compare(xpr_size_t aPos, xpr_size_t aLength, const xpr_wchar_t *aString) const;
    xpr_sint_t compare(xpr_size_t aPos1, xpr_size_t aLength1, const WideString &aString, xpr_size_t aPos2, xpr_size_t aLength2) const;
    xpr_sint_t compare(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_wchar_t *aString, xpr_size_t aPos2) const;

    xpr_sint_t compare_incase(const WideString &aString) const;
    xpr_sint_t compare_incase(const xpr_wchar_t *aString) const;
    xpr_sint_t compare_incase(xpr_size_t aPos, xpr_size_t aLength, const WideString &aString) const;
    xpr_sint_t compare_incase(xpr_size_t aPos, xpr_size_t aLength, const xpr_wchar_t *aString) const;
    xpr_sint_t compare_incase(xpr_size_t aPos1, xpr_size_t aLength1, const WideString &aString, xpr_size_t aPos2, xpr_size_t aLength2) const;
    xpr_sint_t compare_incase(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_wchar_t *aString, xpr_size_t aLength2) const;

protected:
    xpr_wchar_t *alloc(const xpr_size_t &sMinCapacity, xpr_size_t &sCapacity) const;
    void reset(void);

protected:
    xpr_wchar_t *mString;
    xpr_size_t   mLength;
    xpr_size_t   mCapacity;
};

typedef WideString wstring;

XPR_INLINE WideString operator+ (const WideString &aString1, const WideString &aString2)
{
    return WideString(aString1) += aString2;
}

XPR_INLINE WideString operator+ (const xpr_wchar_t *aString1, const WideString &aString2)
{
    return WideString(aString1) += aString2;
}

XPR_INLINE WideString operator+ (xpr_wchar_t aChar, const WideString &aString)
{
    return WideString(1, aChar) += aString;
}

XPR_INLINE WideString operator+ (const WideString &aString1, const xpr_wchar_t *aString2)
{
    return WideString(aString1) += aString2;
}

XPR_INLINE WideString operator+ (const WideString &aString, xpr_wchar_t aChar)
{
    return WideString(aString) += aChar;
}

XPR_INLINE bool operator== (const WideString &aString1, const WideString &aString2)
{
    return (aString1.compare(aString2) == 0) ? true : false;
}

XPR_INLINE bool operator== (const xpr_wchar_t *aString1, const WideString &aString2)
{
    return (aString2.compare(aString1) == 0) ? true : false;
}

XPR_INLINE bool operator== (const WideString &aString1, const xpr_wchar_t *aString2)
{
    return (aString1.compare(aString2) == 0) ? true : false;
}

XPR_INLINE bool operator!= (const WideString &aString1, const WideString &aString2)
{
    return (aString1.compare(aString2) != 0) ? true : false;
}

XPR_INLINE bool operator!= (const xpr_wchar_t *aString1, const WideString &aString2)
{
    return (aString2.compare(aString1) != 0) ? true : false;
}

XPR_INLINE bool operator!= (const WideString &aString1, const xpr_wchar_t *aString2)
{
    return (aString1.compare(aString2) != 0) ? true : false;
}

XPR_INLINE bool operator< (const WideString &aString1, const WideString &aString2)
{
    return (aString1.compare(aString2) < 0) ? true : false;
}

XPR_INLINE bool operator< (const xpr_wchar_t *aString1, const WideString &aString2)
{
    return (aString2.compare(aString1) > 0) ? true : false;
}

XPR_INLINE bool operator< (const WideString &aString1, const xpr_wchar_t *aString2)
{
    return (aString1.compare(aString2) < 0) ? true : false;
}

XPR_INLINE bool operator> (const WideString &aString1, const WideString &aString2)
{
    return (aString1.compare(aString2) > 0) ? true : false;
}

XPR_INLINE bool operator> (const xpr_wchar_t *aString1, const WideString &aString2)
{
    return (aString2.compare(aString1) < 0) ? true : false;
}

XPR_INLINE bool operator> (const WideString &aString1, const xpr_wchar_t *aString2)
{
    return (aString1.compare(aString2) > 0) ? true : false;
}

XPR_INLINE bool operator<= (const WideString &aString1, const WideString &aString2)
{
    return (aString1.compare(aString2) <= 0) ? true : false;
}

XPR_INLINE bool operator<= (const xpr_wchar_t *aString1, const WideString &aString2)
{
    return (aString2.compare(aString1) >= 0) ? true : false;
}

XPR_INLINE bool operator<= (const WideString &aString1, const xpr_wchar_t *aString2)
{
    return (aString1.compare(aString2) <= 0) ? true : false;
}

XPR_INLINE bool operator>= (const WideString &aString1, const WideString &aString2)
{
    return (aString1.compare(aString2) >= 0) ? true : false;
}

XPR_INLINE bool operator>= (const xpr_wchar_t *aString1, const WideString &aString2)
{
    return (aString2.compare(aString1) <= 0) ? true : false;
}

XPR_INLINE bool operator>= (const WideString &aString1, const xpr_wchar_t *aString2)
{
    return (aString1.compare(aString2) >= 0) ? true : false;
}
} // namespace xpr

#if defined(XPR_CFG_STL_TR1)
namespace std
{
namespace tr1
{
    template <>
    struct hash<xpr::WideString> : public unary_function<xpr::WideString, xpr_size_t>
    {
        xpr_size_t operator()(const xpr::WideString &aValue) const
        {
            // hash _Keyval to size_t value by pseudorandomizing transform
            xpr_size_t sHashValue = 2166136261U;
            xpr_size_t sFirst     = 0;
            xpr_size_t sLast      = aValue.size();
            xpr_size_t sStride    = 1 + sLast / 10;

            if (sStride < sLast)
            {
                sLast -= sStride;
            }

            for (; sFirst < sLast; sFirst += sStride)
            {
                sHashValue = 16777619U * sHashValue ^ (xpr_size_t)aValue[sFirst];
            }

            return sHashValue;
        }
    };
} // namespace tr1
} // namespace std
#endif // XPR_CFG_STL_TR1

#endif // __XPR_WIDE_STRING_H__
