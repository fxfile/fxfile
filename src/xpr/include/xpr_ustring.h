//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_USTRING_H__
#define __XPR_USTRING_H__
#pragma once

namespace xpr
{
//class ustring
//{
//public:
//    enum { npos = -1 };
//
//    class Iterator
//    {
//        friend class ustring;
//
//    public:
//        Iterator(void);
//        Iterator(Iterator &aIterator);
//        Iterator(xpr_size_t aPos);
//
//    public:
//        xpr_char32_t   operator*  () const;
//        Iterator&      operator++ ();
//        const Iterator operator++ (xpr_sint_t);
//        Iterator&      operator-- ();
//        const Iterator operator-- (xpr_sint_t);
//
//    protected:
//        ustring *mPtr;
//    };
//
//    class ReverseIterator
//    {
//    public:
//    };
//
//public:
//    ustring(void);
//    ustring(const ustring &aString);
//    ustring(const ustring &aString, xpr_size_t aPos, xpr_size_t aLength = npos);
//    ustring(const xpr_char_t *aString, xpr_size_t aLength);
//    ustring(const xpr_char_t *aString);
//    ustring(const xpr_size_t aNumber, xpr_char_t aChar);
//    ustring(Iterator aBegin, Iterator aEnd);
//    virtual ~ustring(void);
//
//public:
//    // iterators
//    Iterator begin(void);
//    Iterator end(void);
//    const Iterator begin(void) const;
//    const Iterator end(void) const;
//    ReverseIterator rbegin(void);
//    ReverseIterator rend(void);
//    const ReverseIterator rbegin(void) const;
//    const ReverseIterator rend(void) const;
//
//public:
//    // capacity
//    xpr_size_t size(void) const;
//    xpr_size_t length(void) const;
//    xpr_size_t bytes(void) const;
//    xpr_size_t max_size(void) const;
//    void       resize(xpr_size_t aNumber, xpr_char_t aChar) const;
//    void       resize(xpr_size_t aNumber) const;
//    xpr_size_t capacity(void) const;
//    void       reserve(xpr_size_t aCapacity = 0);
//    void       clear(void);
//    xpr_bool_t empty(void);
//
//public:
//    // element access
//    const xpr_char32_t operator[] (xpr_size_t aPos) const;
//    xpr_char32_t       operator[] (xpr_size_t aPos);
//    const xpr_char32_t at(xpr_size_t aPos) const;
//    xpr_char32_t       at(xpr_size_t aPos);
//
//public:
//    // modifier
//    ustring& operator+= (const ustring &aString);
//    ustring& operator+= (const xpr_char_t *aString);
//    ustring& operator+= (xpr_char_t aChar);
//
//    ustring& append(const ustring &aString);
//    ustring& append(const ustring &aString, xpr_size_t aPos, xpr_size_t aLength);
//    ustring& append(const xpr_char_t *aString, xpr_size_t aLength);
//    ustring& append(const xpr_char_t *aString);
//    ustring& append(xpr_size_t aNumber, xpr_char_t aChar);
//    ustring& append(Iterator aBegin, Iterator aEnd);
//
//    void     push_back(xpr_char_t aChar);
//
//    ustring& assign(const ustring &aString);
//    ustring& assign(const ustring &aString, xpr_size_t aPos, xpr_size_t aLength);
//    ustring& assign(const xpr_char_t *aString, xpr_size_t aLength);
//    ustring& assign(const xpr_char_t *aString);
//    ustring& assign(xpr_size_t aNumber, xpr_char_t aChar);
//    ustring& assign(Iterator aBegin, Iterator aEnd);
//
//    ustring& insert(xpr_size_t aPos, const ustring &aString);
//    ustring& insert(xpr_size_t aPos1, const ustring &aString, xpr_size_t aPos2, xpr_size_t aLength);
//    ustring& insert(xpr_size_t aPos, const xpr_char_t *aString, xpr_size_t aLength);
//    ustring& insert(xpr_size_t aPos, const xpr_char_t *aString);
//    ustring& insert(xpr_size_t aPos, xpr_size_t aNumber, xpr_char_t aChar);
//    Iterator insert(Iterator aPos, xpr_char_t aChar);
//    Iterator insert(Iterator aPos, xpr_size_t aNumber, xpr_char_t aChar);
//    Iterator insert(Iterator aPos, Iterator aBegin, Iterator aEnd);
//
//    ustring& erase(xpr_size_t aPos = 0, xpr_size_t aLength = npos);
//    Iterator erase(Iterator aPos);
//    Iterator erase(Iterator aFirst, Iterator aLast);
//
//    ustring& replace(xpr_size_t aPos, xpr_size_t aLength, const ustring &aString);
//    ustring& replace(Iterator aIterator1, Iterator aIterator2, const ustring &aString);
//    ustring& replace(xpr_size_t aPos1, xpr_size_t aLength1, const ustring &aString, xpr_size_t aPos2, xpr_size_t aLength2);
//    ustring& replace(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aLength2);
//    ustring& replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString, xpr_size_t aLength);
//    ustring& replace(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString);
//    ustring& replace(Iterator aIterator1, Iterator aIterator2, const xpr_char_t *aString);
//    ustring& replace(xpr_size_t aPos1, xpr_size_t aLength1, xpr_size_t nLength2, xpr_char_t aChar);
//    ustring& replace(Iterator aIterator1, Iterator aIterator2, xpr_size_t nLength, xpr_char_t aChar);
//    ustring& replace(Iterator aIterator1, Iterator aIterator2, Iterator aInputIterator1, Iterator aInputIterator2);
//
//    void     swap(ustring &aString);
//
//    void     upper_case(void);
//    void     lower_case(void);
//
//    void     trim(void);
//    void     trim_left(void);
//    void     trim_right(void);
//
//    void     format(const xpr_char_t *aFormat, ...);
//    void     append_format(const xpr_char_t *aFormat, ...);
//
//public:
//    // operations
//    const xpr_char_t *c_str(void) const;
//
//    const xpr_char_t *data(void) const;
//
//    xpr_size_t copy(xpr_char_t *aString, xpr_size_t aLength, xpr_size_t aPos = 0) const;
//
//    xpr_size_t find(ustring &aString, xpr_size_t aPos = 0) const;
//    xpr_size_t find(xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
//    xpr_size_t find(xpr_char_t *aString, xpr_size_t aPos = 0) const;
//    xpr_size_t find(xpr_char_t aChar, xpr_size_t aPos = 0) const;
//
//    xpr_size_t rfind(ustring &aString, xpr_size_t aPos = npos) const;
//    xpr_size_t rfind(xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
//    xpr_size_t rfind(xpr_char_t *aString, xpr_size_t aPos = npos) const;
//    xpr_size_t rfind(xpr_char_t aChar, xpr_size_t aPos = npos) const;
//
//    xpr_size_t find_first_of(ustring &aString, xpr_size_t aPos = 0) const;
//    xpr_size_t find_first_of(xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
//    xpr_size_t find_first_of(xpr_char_t *aString, xpr_size_t aPos = 0) const;
//    xpr_size_t find_first_of(xpr_char_t aChar, xpr_size_t aPos = 0) const;
//
//    xpr_size_t find_last_of(ustring &aString, xpr_size_t aPos = npos) const;
//    xpr_size_t find_last_of(xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
//    xpr_size_t find_last_of(xpr_char_t *aString, xpr_size_t aPos = npos) const;
//    xpr_size_t find_last_of(xpr_char_t aChar, xpr_size_t aPos = npos) const;
//
//    xpr_size_t find_first_not_of(ustring &aString, xpr_size_t aPos = 0) const;
//    xpr_size_t find_first_not_of(xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
//    xpr_size_t find_first_not_of(xpr_char_t *aString, xpr_size_t aPos = 0) const;
//    xpr_size_t find_first_not_of(xpr_char_t aChar, xpr_size_t aPos = 0) const;
//
//    xpr_size_t find_last_not_of(ustring &aString, xpr_size_t aPos = npos) const;
//    xpr_size_t find_last_not_of(xpr_char_t *aString, xpr_size_t aPos, xpr_size_t aLength) const;
//    xpr_size_t find_last_not_of(xpr_char_t *aString, xpr_size_t aPos = npos) const;
//    xpr_size_t find_last_not_of(xpr_char_t aChar, xpr_size_t aPos = npos) const;
//
//    ustring    substr(xpr_size_t aPos = 0, xpr_size_t aLength = npos) const;
//
//    xpr_sint_t compare(const ustring &aString) const;
//    xpr_sint_t compare(const xpr_char_t *aString) const;
//    xpr_sint_t compare(xpr_size_t aPos, xpr_size_t aLength, const ustring &aString) const;
//    xpr_sint_t compare(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const;
//    xpr_sint_t compare(xpr_size_t aPos1, xpr_size_t aLength1, const ustring &aString, xpr_size_t aPos2, xpr_size_t aLength2) const;
//    xpr_sint_t compare(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aPos2) const;
//
//    xpr_sint_t compare_incase(const ustring &aString) const;
//    xpr_sint_t compare_incase(const xpr_char_t *aString) const;
//    xpr_sint_t compare_incase(xpr_size_t aPos, xpr_size_t aLength, const ustring &aString) const;
//    xpr_sint_t compare_incase(xpr_size_t aPos, xpr_size_t aLength, const xpr_char_t *aString) const;
//    xpr_sint_t compare_incase(xpr_size_t aPos1, xpr_size_t aLength1, const ustring &aString, xpr_size_t aPos2, xpr_size_t aLength2) const;
//    xpr_sint_t compare_incase(xpr_size_t aPos1, xpr_size_t aLength1, const xpr_char_t *aString, xpr_size_t aPos2) const;
//
//protected:
//    xpr_char_t *mString;
//    xpr_size_t  mLength;
//    xpr_size_t  mCapacity;
//};
} // namespace xpr

#endif // __XPR_USTRING_H__
