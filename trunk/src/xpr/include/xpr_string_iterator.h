//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_STRING_ITERATOR_H__
#define __XPR_STRING_ITERATOR_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
class String;
class StringConstIterator;
class StringConstReverseIterator;

class XPR_DL_API StringIterator
{
    friend class String;
    friend class StringConstIterator;

public:
    StringIterator(void);

protected:
    explicit StringIterator(xpr_char_t *aString);

public:
    xpr_char_t&     operator*  () const;
    xpr_char_t*     operator-> () const;
    StringIterator& operator++ ();
    StringIterator  operator++ (xpr_sint_t);
    StringIterator& operator-- ();
    StringIterator  operator-- (xpr_sint_t);
    StringIterator& operator+= (xpr_sint_t aOffset);
    StringIterator  operator+  (xpr_sint_t aOffset) const;
    StringIterator& operator-= (xpr_sint_t aOffset);
    StringIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t      operator-  (const StringIterator &aIterator) const;
    xpr_char_t&     operator[] (xpr_sint_t aOffset) const;
    bool            operator== (const StringIterator &aIterator) const;
    bool            operator!= (const StringIterator &aIterator) const;
    bool            operator<  (const StringIterator &aIterator) const;
    bool            operator<= (const StringIterator &aIterator) const;
    bool            operator>  (const StringIterator &aIterator) const;
    bool            operator>= (const StringIterator &aIterator) const;

protected:
    xpr_char_t *mString;
};

class XPR_DL_API StringConstIterator
{
    friend class String;

public:
    StringConstIterator(void);
    StringConstIterator(const StringIterator &aIterator);

protected:
    explicit StringConstIterator(const xpr_char_t *aString);

public:
    const xpr_char_t&    operator*  () const;
    const xpr_char_t*    operator-> () const;
    StringConstIterator& operator++ ();
    StringConstIterator  operator++ (xpr_sint_t);
    StringConstIterator& operator-- ();
    StringConstIterator  operator-- (xpr_sint_t);
    StringConstIterator& operator+= (xpr_sint_t aOffset);
    StringConstIterator  operator+  (xpr_sint_t aOffset) const;
    StringConstIterator& operator-= (xpr_sint_t aOffset);
    StringConstIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t           operator-  (const StringConstIterator &aIterator) const;
    const xpr_char_t&    operator[] (xpr_sint_t aOffset) const;
    bool                 operator== (const StringConstIterator &aIterator) const;
    bool                 operator!= (const StringConstIterator &aIterator) const;
    bool                 operator<  (const StringConstIterator &aIterator) const;
    bool                 operator<= (const StringConstIterator &aIterator) const;
    bool                 operator>  (const StringConstIterator &aIterator) const;
    bool                 operator>= (const StringConstIterator &aIterator) const;

protected:
    const xpr_char_t *mString;
};

class XPR_DL_API StringReverseIterator
{
    friend class String;
    friend class StringConstReverseIterator;

public:
    StringReverseIterator(void);

protected:
    explicit StringReverseIterator(xpr_char_t *aString);

public:
    xpr_char_t&            operator*  () const;
    xpr_char_t*            operator-> () const;
    StringReverseIterator& operator++ ();
    StringReverseIterator  operator++ (xpr_sint_t);
    StringReverseIterator& operator-- ();
    StringReverseIterator  operator-- (xpr_sint_t);
    StringReverseIterator& operator+= (xpr_sint_t aOffset);
    StringReverseIterator  operator+  (xpr_sint_t aOffset) const;
    StringReverseIterator& operator-= (xpr_sint_t aOffset);
    StringReverseIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t             operator-  (const StringReverseIterator &aIterator) const;
    xpr_char_t&            operator[] (xpr_sint_t aOffset) const;
    bool                   operator== (const StringReverseIterator &aIterator) const;
    bool                   operator!= (const StringReverseIterator &aIterator) const;
    bool                   operator<  (const StringReverseIterator &aIterator) const;
    bool                   operator<= (const StringReverseIterator &aIterator) const;
    bool                   operator>  (const StringReverseIterator &aIterator) const;
    bool                   operator>= (const StringReverseIterator &aIterator) const;

protected:
    xpr_char_t *mString;
};

class XPR_DL_API StringConstReverseIterator
{
    friend class String;

public:
    StringConstReverseIterator(void);
    StringConstReverseIterator(const StringReverseIterator &aIterator);

protected:
    explicit StringConstReverseIterator(const xpr_char_t *aString);

public:
    const xpr_char_t&           operator*  () const;
    const xpr_char_t*           operator-> () const;
    StringConstReverseIterator& operator++ ();
    StringConstReverseIterator  operator++ (xpr_sint_t);
    StringConstReverseIterator& operator-- ();
    StringConstReverseIterator  operator-- (xpr_sint_t);
    StringConstReverseIterator& operator+= (xpr_sint_t aOffset);
    StringConstReverseIterator  operator+  (xpr_sint_t aOffset) const;
    StringConstReverseIterator& operator-= (xpr_sint_t aOffset);
    StringConstReverseIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t                  operator-  (const StringConstReverseIterator &aIterator) const;
    const xpr_char_t&           operator[] (xpr_sint_t aOffset) const;
    bool                        operator== (const StringConstReverseIterator &aIterator) const;
    bool                        operator!= (const StringConstReverseIterator &aIterator) const;
    bool                        operator<  (const StringConstReverseIterator &aIterator) const;
    bool                        operator<= (const StringConstReverseIterator &aIterator) const;
    bool                        operator>  (const StringConstReverseIterator &aIterator) const;
    bool                        operator>= (const StringConstReverseIterator &aIterator) const;

protected:
    const xpr_char_t *mString;
};
} // namespace xpr

#endif // __XPR_STRING_ITERATOR_H__
