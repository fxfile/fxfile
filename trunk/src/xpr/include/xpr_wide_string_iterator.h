//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_WIDE_STRING_ITERATOR_H__
#define __XPR_WIDE_STRING_ITERATOR_H__
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
class WideString;
class WideStringConstIterator;
class WideStringConstReverseIterator;

class XPR_DL_API WideStringIterator
{
    friend class WideString;
    friend class WideStringConstIterator;

public:
    WideStringIterator(void);

protected:
    explicit WideStringIterator(xpr_wchar_t *aString);

public:
    xpr_wchar_t&        operator*  () const;
    xpr_wchar_t*        operator-> () const;
    WideStringIterator& operator++ ();
    WideStringIterator  operator++ (xpr_sint_t);
    WideStringIterator& operator-- ();
    WideStringIterator  operator-- (xpr_sint_t);
    WideStringIterator& operator+= (xpr_sint_t aOffset);
    WideStringIterator  operator+  (xpr_sint_t aOffset) const;
    WideStringIterator& operator-= (xpr_sint_t aOffset);
    WideStringIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t          operator-  (const WideStringIterator &aIterator) const;
    xpr_wchar_t&        operator[] (xpr_sint_t aOffset) const;
    bool                operator== (const WideStringIterator &aIterator) const;
    bool                operator!= (const WideStringIterator &aIterator) const;
    bool                operator<  (const WideStringIterator &aIterator) const;
    bool                operator<= (const WideStringIterator &aIterator) const;
    bool                operator>  (const WideStringIterator &aIterator) const;
    bool                operator>= (const WideStringIterator &aIterator) const;

protected:
    xpr_wchar_t *mString;
};

class XPR_DL_API WideStringConstIterator
{
    friend class WideString;

public:
    WideStringConstIterator(void);
    WideStringConstIterator(const WideStringIterator &aIterator);

protected:
    explicit WideStringConstIterator(const xpr_wchar_t *aString);

public:
    const xpr_wchar_t&       operator*  () const;
    const xpr_wchar_t*       operator-> () const;
    WideStringConstIterator& operator++ ();
    WideStringConstIterator  operator++ (xpr_sint_t);
    WideStringConstIterator& operator-- ();
    WideStringConstIterator  operator-- (xpr_sint_t);
    WideStringConstIterator& operator+= (xpr_sint_t aOffset);
    WideStringConstIterator  operator+  (xpr_sint_t aOffset) const;
    WideStringConstIterator& operator-= (xpr_sint_t aOffset);
    WideStringConstIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t               operator-  (const WideStringConstIterator &aIterator) const;
    const xpr_wchar_t&       operator[] (xpr_sint_t aOffset) const;
    bool                     operator== (const WideStringConstIterator &aIterator) const;
    bool                     operator!= (const WideStringConstIterator &aIterator) const;
    bool                     operator<  (const WideStringConstIterator &aIterator) const;
    bool                     operator<= (const WideStringConstIterator &aIterator) const;
    bool                     operator>  (const WideStringConstIterator &aIterator) const;
    bool                     operator>= (const WideStringConstIterator &aIterator) const;

protected:
    const xpr_wchar_t *mString;
};

class XPR_DL_API WideStringReverseIterator
{
    friend class WideString;
    friend class WideStringConstReverseIterator;

public:
    WideStringReverseIterator(void);

protected:
    explicit WideStringReverseIterator(xpr_wchar_t *aString);

public:
    xpr_wchar_t&               operator*  () const;
    xpr_wchar_t*               operator-> () const;
    WideStringReverseIterator& operator++ ();
    WideStringReverseIterator  operator++ (xpr_sint_t);
    WideStringReverseIterator& operator-- ();
    WideStringReverseIterator  operator-- (xpr_sint_t);
    WideStringReverseIterator& operator+= (xpr_sint_t aOffset);
    WideStringReverseIterator  operator+  (xpr_sint_t aOffset) const;
    WideStringReverseIterator& operator-= (xpr_sint_t aOffset);
    WideStringReverseIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t                 operator-  (const WideStringReverseIterator &aIterator) const;
    xpr_wchar_t&               operator[] (xpr_sint_t aOffset) const;
    bool                       operator== (const WideStringReverseIterator &aIterator) const;
    bool                       operator!= (const WideStringReverseIterator &aIterator) const;
    bool                       operator<  (const WideStringReverseIterator &aIterator) const;
    bool                       operator<= (const WideStringReverseIterator &aIterator) const;
    bool                       operator>  (const WideStringReverseIterator &aIterator) const;
    bool                       operator>= (const WideStringReverseIterator &aIterator) const;

protected:
    xpr_wchar_t *mString;
};

class XPR_DL_API WideStringConstReverseIterator
{
    friend class WideString;

public:
    WideStringConstReverseIterator(void);
    WideStringConstReverseIterator(const WideStringReverseIterator &aIterator);

protected:
    explicit WideStringConstReverseIterator(const xpr_wchar_t *aString);

public:
    const xpr_wchar_t&              operator*  () const;
    const xpr_wchar_t*              operator-> () const;
    WideStringConstReverseIterator& operator++ ();
    WideStringConstReverseIterator  operator++ (xpr_sint_t);
    WideStringConstReverseIterator& operator-- ();
    WideStringConstReverseIterator  operator-- (xpr_sint_t);
    WideStringConstReverseIterator& operator+= (xpr_sint_t aOffset);
    WideStringConstReverseIterator  operator+  (xpr_sint_t aOffset) const;
    WideStringConstReverseIterator& operator-= (xpr_sint_t aOffset);
    WideStringConstReverseIterator  operator-  (xpr_sint_t aOffset) const;
    xpr_sint_t                      operator-  (const WideStringConstReverseIterator &aIterator) const;
    const xpr_wchar_t&              operator[] (xpr_sint_t aOffset) const;
    bool                            operator== (const WideStringConstReverseIterator &aIterator) const;
    bool                            operator!= (const WideStringConstReverseIterator &aIterator) const;
    bool                            operator<  (const WideStringConstReverseIterator &aIterator) const;
    bool                            operator<= (const WideStringConstReverseIterator &aIterator) const;
    bool                            operator>  (const WideStringConstReverseIterator &aIterator) const;
    bool                            operator>= (const WideStringConstReverseIterator &aIterator) const;

protected:
    const xpr_wchar_t *mString;
};
} // namespace xpr

#endif // __XPR_WIDE_STRING_ITERATOR_H__
