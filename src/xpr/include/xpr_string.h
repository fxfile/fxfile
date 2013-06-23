//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_STRING_H__
#define __XPR_STRING_H__
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_string_iterator.h"

#if defined(XPR_CFG_STL_TR1)
#include <functional>
#endif // XPR_CFG_STL_TR1

namespace xpr
{
class XPR_DL_API String
{
public:
    enum { npos = -1 };

    typedef StringIterator Iterator;
    typedef StringIterator iterator;
    typedef StringConstIterator ConstIterator;
    typedef StringConstIterator const_iterator;
    typedef StringReverseIterator ReverseIterator;
    typedef StringReverseIterator reverse_iterator;
    typedef StringConstReverseIterator ConstReverseIterator;
    typedef StringConstReverseIterator const_reverse_iterator;

public:
    String(void);
    String(const String &aString);
    String(const String &aString, xpr_size_t aPos, xpr_size_t aLength = npos);
    String(const xpr_char_t *aString, xpr_size_t aLength);
    String(const xpr_char_t *aString);
    String(const xpr_size_t aNumber, xpr_char_t aChar);
    String(Iterator aFirst, Iterator aLast);
    virtual ~String(void);

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
    void       resize(xpr_size_t aNumber, xpr_char_t aChar);
    void       resize(xpr_size_t aNumber);
    xpr_size_t capacity(void) const;
    void       reserve(xpr_size_t aCapacity = 0);
    void       clear(void);
    xpr_bool_t empty(void) const;

public:
    // element access
    const xpr_char_t& operator[] (xpr_size_t aPos) const;
    xpr_char_t&       operator[] (xpr_size_t aPos);
    const xpr_char_t& at(xpr_size_t aPos) const;
    xpr_char_t&       at(xpr_size_t aPos);

public:
    // modifier
    String& operator= (const String &aString);
    String& operator= (const xpr_char_t *aString);
    String& operator= (xpr_char_t aChar);

    String& operator+= (const String &aString);
    String& operator+= (const xpr_char_t *aString);
    String& operator+= (xpr_char_t aChar);

    String& append(const String &aString);
    String& append(const String &aString, xpr_size_t aPos, xpr_size_t aLength);
    String& append(const xpr_char_t *aString, xpr_size_t aLength);
    String& append(const xpr_char_t *aString);
    String& append(xpr_size_t aNumber, xpr_char_t aChar);
    String& append(Iterator aFirst, Iterator aLast);

    xpr_char_t       &front(void);
    const xpr_char_t &front(void) const;
    xpr_char_t       &back(void);
    const xpr_char_t &back(void) const;

    void    push_back(xpr_char_t aChar);
    void    pop_back(void);

    String& assign(const String &aString);
    String& assign(const String &aString, xpr_size_t aPos, xpr_size_t aLength);
    String& assign(const xpr_char_t *aString, xpr_size_t aLength);
    String& assign(const xpr_char_t *aString);
    String& assign(xpr_size_t aNumber, xpr_char_t aChar);
    String& assign(Iterator aFirst, Iterator aLast);

    String&  insert(xpr_size_t aPos, const String &aString);
    String&  insert(xpr_size_t aPos1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength);
    String&  insert(xpr_size_t aPos, const xpr_char_t *aString, xpr_size_t aLength);
    String&  insert(xpr_size_t aPos, const xpr_char_t *aString);
    String&  insert(xpr_size_t aPos, xpr_size_t aNumber, xpr_char_t aChar);
    Iterator insert(Iterator aPos, xpr_char_t aChar);
    Iterator insert(Iterator aPos, xpr_size_t aNumber, xpr_char_t aChar);
    Iterator insert(Iterator aPos, Iterator aFirst, Iterator aLast);

    String&  erase(xpr_size_t aPos = 0, xpr_size_t aLength = npos);
    Iterator erase(Iterator aPos);
    Iterator erase(Iterator aFirst, Iterator aLast);

    String& replace(xpr_size_t aPos, xpr_size_t aLength, const String &aString);
    String& replace(Iterator aIterator1, Iterator aIterator2, const String &aString);
    String& replace(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2);
    String& replace(xpr_size_t aPos, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2);
    String& replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString, xpr_size_t aLength);
    String& replace(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString);
    String& replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString);
    String& replace(xpr_size_t aPos, xpr_size_t aLength, xpr_size_t aNumber, xpr_char_t aChar);
    String& replace(Iterator aIterator1, Iterator aIterator2, xpr_size_t aNumber, xpr_char_t aChar);
    String& replace(Iterator aIterator1, Iterator aIterator2, Iterator aInputIterator1, Iterator aInputIterator2);

    void     swap(String &aString);

    void     upper_case(void);
    void     lower_case(void);

    void     trim(void);
    void     trim_left(void);
    void     trim_right(void);

    void     format(const xpr_char_t *aFormat, ...);
    void     format(const xpr_char_t *aFormat, va_list aArgs);
    void     append_format(const xpr_char_t *aFormat, ...);
    void     append_format(const xpr_char_t *aFormat, va_list aArgs);

    void     update(void);

    void     shrink_to_fit(void);

public:
    // operations
    const xpr_char_t *c_str(void) const;

    const xpr_char_t *data(void) const;

    xpr_size_t copy(xpr_char_t *aString, xpr_size_t aLength, xpr_size_t aPos = 0) const;

    xpr_size_t find(const String &aString, xpr_size_t aPos = 0) const;
    xpr_size_t find(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find(const xpr_char_t *aString, xpr_size_t aPos = 0) const;
    xpr_size_t find(xpr_char_t aChar, xpr_size_t aPos = 0) const;

    xpr_size_t rfind(const String &aString, xpr_size_t aPos = npos) const;
    xpr_size_t rfind(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t rfind(const xpr_char_t *aString, xpr_size_t aPos = npos) const;
    xpr_size_t rfind(xpr_char_t aChar, xpr_size_t aPos = npos) const;

    xpr_size_t find_first_of(const String &aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_first_of(const xpr_char_t *aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_of(xpr_char_t aChar, xpr_size_t aPos = 0) const;

    xpr_size_t find_last_of(const String &aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_last_of(const xpr_char_t *aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_of(xpr_char_t aChar, xpr_size_t aPos = npos) const;

    xpr_size_t find_first_not_of(const String &aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_not_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_first_not_of(const xpr_char_t *aString, xpr_size_t aPos = 0) const;
    xpr_size_t find_first_not_of(xpr_char_t aChar, xpr_size_t aPos = 0) const;

    xpr_size_t find_last_not_of(const String &aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_not_of(const xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
    xpr_size_t find_last_not_of(const xpr_char_t *aString, xpr_size_t aPos = npos) const;
    xpr_size_t find_last_not_of(xpr_char_t aChar, xpr_size_t aPos = npos) const;

    String    substr(xpr_size_t aPos = 0, xpr_size_t aLength = npos) const;

    xpr_sint_t compare(const String &aString) const;
    xpr_sint_t compare(const xpr_char_t *aString) const;
    xpr_sint_t compare(xpr_size_t aPos, xpr_size_t aLength, const String &aString) const;
    xpr_sint_t compare(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const;
    xpr_sint_t compare(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2) const;
    xpr_sint_t compare(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aPos2) const;

    xpr_sint_t compare_case(const String &aString) const;
    xpr_sint_t compare_case(const xpr_char_t *aString) const;
    xpr_sint_t compare_case(xpr_size_t aPos, xpr_size_t aLength, const String &aString) const;
    xpr_sint_t compare_case(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const;
    xpr_sint_t compare_case(xpr_size_t aPos1, xpr_size_t aLength1, const String &aString, xpr_size_t aPos2, xpr_size_t aLength2) const;
    xpr_sint_t compare_case(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2) const;

protected:
    xpr_char_t *alloc(const xpr_size_t &aMinCapacity, xpr_size_t &aCapacity) const;
    xpr_size_t get_capacity(const xpr_size_t &aMinCapacity) const;
    void reset(void);

protected:
    xpr_char_t *mString;
    xpr_size_t  mLength;
    xpr_size_t  mCapacity;
};

typedef String string;

XPR_INLINE String operator+ (const String &aString1, const String &aString2)
{
    return String(aString1) += aString2;
}

XPR_INLINE String operator+ (const xpr_char_t *aString1, const String &aString2)
{
    return String(aString1) += aString2;
}

XPR_INLINE String operator+ (xpr_char_t aChar, const String &aString)
{
    return String(1, aChar) += aString;
}

XPR_INLINE String operator+ (const String &aString1, const xpr_char_t *aString2)
{
    return String(aString1) += aString2;
}

XPR_INLINE String operator+ (const String &aString, xpr_char_t aChar)
{
    return String(aString) += aChar;
}

XPR_INLINE bool operator== (const String &aString1, const String &aString2)
{
    return (aString1.compare(aString2) == 0) ? true : false;
}

XPR_INLINE bool operator== (const xpr_char_t *aString1, const String &aString2)
{
    return (aString2.compare(aString1) == 0) ? true : false;
}

XPR_INLINE bool operator== (const String &aString1, const xpr_char_t *aString2)
{
    return (aString1.compare(aString2) == 0) ? true : false;
}

XPR_INLINE bool operator!= (const String &aString1, const String &aString2)
{
    return (aString1.compare(aString2) != 0) ? true : false;
}

XPR_INLINE bool operator!= (const xpr_char_t *aString1, const String &aString2)
{
    return (aString2.compare(aString1) != 0) ? true : false;
}

XPR_INLINE bool operator!= (const String &aString1, const xpr_char_t *aString2)
{
    return (aString1.compare(aString2) != 0) ? true : false;
}

XPR_INLINE bool operator< (const String &aString1, const String &aString2)
{
    return (aString1.compare(aString2) < 0) ? true : false;
}

XPR_INLINE bool operator< (const xpr_char_t *aString1, const String &aString2)
{
    return (aString2.compare(aString1) > 0) ? true : false;
}

XPR_INLINE bool operator< (const String &aString1, const xpr_char_t *aString2)
{
    return (aString1.compare(aString2) < 0) ? true : false;
}

XPR_INLINE bool operator> (const String &aString1, const String &aString2)
{
    return (aString1.compare(aString2) > 0) ? true : false;
}

XPR_INLINE bool operator> (const xpr_char_t *aString1, const String &aString2)
{
    return (aString2.compare(aString1) < 0) ? true : false;
}

XPR_INLINE bool operator> (const String &aString1, const xpr_char_t *aString2)
{
    return (aString1.compare(aString2) > 0) ? true : false;
}

XPR_INLINE bool operator<= (const String &aString1, const String &aString2)
{
    return (aString1.compare(aString2) <= 0) ? true : false;
}

XPR_INLINE bool operator<= (const xpr_char_t *aString1, const String &aString2)
{
    return (aString2.compare(aString1) >= 0) ? true : false;
}

XPR_INLINE bool operator<= (const String &aString1, const xpr_char_t *aString2)
{
    return (aString1.compare(aString2) <= 0) ? true : false;
}

XPR_INLINE bool operator>= (const String &aString1, const String &aString2)
{
    return (aString1.compare(aString2) >= 0) ? true : false;
}

XPR_INLINE bool operator>= (const xpr_char_t *aString1, const String &aString2)
{
    return (aString2.compare(aString1) <= 0) ? true : false;
}

XPR_INLINE bool operator>= (const String &aString1, const xpr_char_t *aString2)
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
    struct hash<xpr::String> : public unary_function<xpr::String, xpr_size_t>
    {
        xpr_size_t operator()(const xpr::String &aValue) const
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

#endif // __XPR_STRING_H__
