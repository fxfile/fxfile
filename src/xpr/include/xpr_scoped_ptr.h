//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_SCOPED_PTR_H__
#define __XPR_SCOPED_PTR_H__
#pragma once

#include "xpr_memory.h"
#include "xpr_define.h"

namespace xpr
{
template <typename C>
class ScopedPtr
{
    DISALLOW_COPY_AND_ASSIGN(ScopedPtr);

public:
    typedef C ElementType;

public:
    explicit ScopedPtr(ElementType *aPtr = XPR_NULL)
        : mPtr(aPtr)
    {
    }

    ~ScopedPtr(void)
    {
        XPR_SAFE_DELETE(mPtr)
    }

public:
    void reset(ElementType *aPtr = XPR_NULL)
    {
        XPR_SAFE_DELETE(mPtr);
        mPtr = aPtr;
    }

    ElementType &operator*(void) const
    {
        XPR_ASSERT(mPtr != XPR_NULL);
        return *mPtr;
    }

    ElementType *operator->(void) const
    {
        XPR_ASSERT(mPtr != XPR_NULL);
        return mPtr;
    }

    ElementType *get(void) const { return mPtr; }

    bool operator==(ElementType *aPtr) const { return mPtr == aPtr; }
    bool operator!=(ElementType *aPtr) const { return mPtr != aPtr; }

    void swap(ScopedPtr &aScopedPtr2)
    {
        ElementType *tmp_ptr = mPtr;
        mPtr = aScopedPtr2.mPtr;
        aScopedPtr2.mPtr = tmp_ptr;
    }

    ElementType *release(void)
    {
        ElementType *ret_ptr = mPtr;
        XPR_SAFE_DELETE(mPtr);
        return ret_ptr;
    }

private:
    template <typename C2> bool operator==(ScopedPtr<C2> const &aScopedPtr2) const;
    template <typename C2> bool operator!=(ScopedPtr<C2> const &aScopedPtr2) const;

    ElementType *mPtr;
};

template <typename C>
void swap(ScopedPtr<C> &aScopedPtr1, ScopedPtr<C> &aScopedPtr2)
{
    aScopedPtr1.swap(aScopedPtr2);
}

template <typename C>
bool operator==(C *aScopedPtr1, const ScopedPtr<C> &aScopedPtr2)
{
    return aScopedPtr1 == aScopedPtr2.get();
}

template <typename C>
bool operator!=(C *aScopedPtr1, const ScopedPtr<C> &aScopedPtr2)
{
    return aScopedPtr1 != aScopedPtr2.get();
}

template <typename C>
class ScopedArray
{
    DISALLOW_COPY_AND_ASSIGN(ScopedArray);

public:
    typedef C ElementType;

public:
    explicit ScopedArray(ElementType *aPtr = XPR_NULL)
        : mPtr(aPtr)
    {
    }

    ~ScopedArray(void)
    {
        XPR_SAFE_DELETE_ARRAY(mPtr)
    }

public:
    void reset(ElementType *aPtr = XPR_NULL)
    {
        XPR_SAFE_DELETE_ARRAY(mPtr);
        mPtr = aPtr;
    }

    ElementType &operator*(void) const
    {
        XPR_ASSERT(mPtr != XPR_NULL);
        return *mPtr;
    }

    ElementType *operator->(void) const
    {
        XPR_ASSERT(mPtr != XPR_NULL);
        return mPtr;
    }

    ElementType *get(void) const { return mPtr; }

    bool operator==(ElementType *aPtr) const { return mPtr == aPtr; }
    bool operator!=(ElementType *aPtr) const { return mPtr != aPtr; }

    void swap(ScopedArray &aScopedArray2)
    {
        ElementType *tmp_ptr = mPtr;
        mPtr = aScopedArray2.mPtr;
        aScopedArray2.mPtr = tmp_ptr;
    }

    ElementType *release(void)
    {
        ElementType *ret_ptr = mPtr;
        XPR_SAFE_DELETE_ARRAY(mPtr);
        return ret_ptr;
    }

private:
    template <typename C2> bool operator==(ScopedArray<C2> const &aScopedArray2) const;
    template <typename C2> bool operator!=(ScopedArray<C2> const &aScopedArray2) const;

    ElementType *mPtr;
};

template <typename C>
void swap(ScopedArray<C> &aScopedArray1, ScopedArray<C> &aScopedArray2)
{
    aScopedArray1.swap(aScopedArray2);
}

template <typename C>
bool operator==(C *aScopedArray1, const ScopedArray<C> &aScopedArray2)
{
    return aScopedArray1 == aScopedArray2.get();
}

template <typename C>
bool operator!=(C *aScopedArray1, const ScopedArray<C> &aScopedArray2)
{
    return aScopedArray1 != aScopedArray2.get();
}

template <typename C>
class ScopedMallocPtr
{
    DISALLOW_COPY_AND_ASSIGN(ScopedMallocPtr);

public:
    typedef C ElementType;

public:
    explicit ScopedMallocPtr(ElementType *aPtr = XPR_NULL)
        : mPtr(aPtr)
    {
    }

    ~ScopedMallocPtr(void)
    {
        XPR_SAFE_FREE(mPtr)
    }

public:
    void reset(ElementType *aPtr = XPR_NULL)
    {
        XPR_SAFE_FREE(mPtr);
        mPtr = aPtr;
    }

    ElementType &operator*(void) const
    {
        XPR_ASSERT(mPtr != XPR_NULL);
        return *mPtr;
    }

    ElementType *operator->(void) const
    {
        XPR_ASSERT(mPtr != XPR_NULL);
        return mPtr;
    }

    ElementType *get(void) const { return mPtr; }

    bool operator==(ElementType *aPtr) const { return mPtr == aPtr; }
    bool operator!=(ElementType *aPtr) const { return mPtr != aPtr; }

    void swap(ScopedMallocPtr &aScopedMallocPtr2)
    {
        ElementType *tmp_ptr = mPtr;
        mPtr = aScopedMallocPtr2.mPtr;
        aScopedMallocPtr2.mPtr = tmp_ptr;
    }

    ElementType *release(void)
    {
        ElementType *ret_ptr = mPtr;
        XPR_SAFE_FREE(mPtr);
        return ret_ptr;
    }

private:
    template <typename C2> bool operator==(ScopedMallocPtr<C2> const &aScopedMallocPtr2) const;
    template <typename C2> bool operator!=(ScopedMallocPtr<C2> const &aScopedMallocPtr2) const;

    ElementType *mPtr;
};

template <typename C>
void swap(ScopedMallocPtr<C> &aScopedMallocPtr1, ScopedMallocPtr<C> &aScopedMallocPtr2)
{
    aScopedMallocPtr1.swap(aScopedMallocPtr2);
}

template <typename C>
bool operator==(C *aScopedMallocPtr1, const ScopedMallocPtr<C> &aScopedMallocPtr2)
{
    return aScopedMallocPtr1 == aScopedMallocPtr2.get();
}

template <typename C>
bool operator!=(C *aScopedMallocPtr1, const ScopedMallocPtr<C> &aScopedMallocPtr2)
{
    return aScopedMallocPtr1 != aScopedMallocPtr2.get();
}
} // namespace xpr

#endif // __XPR_SCOPED_PTR_H__
