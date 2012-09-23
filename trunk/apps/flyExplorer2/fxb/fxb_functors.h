//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FUNCTORS_H__
#define __FXB_FUNCTORS_H__
#pragma once

namespace fxb
{
struct HeapDeleteFunctor
{
    template <typename T>
    void operator () (const T &aPtr)
    {
        T sPtr = aPtr;
        XPR_SAFE_DELETE(sPtr);
    }
};

struct HeapDeleteArrayFunctor
{
    template <typename T>
    void operator () (const T &aPtr)
    {
        T sPtr = aPtr;
        XPR_SAFE_DELETE_ARRAY(sPtr);
    }
};

struct MapHeapDeleteFunctor
{
    template <typename T>
    void operator () (const T &aIterator)
    {
        typename T::second_type sPtr = aIterator.second;
        XPR_SAFE_DELETE(sPtr);
    }
};

struct MapHeapDeleteArrayFunctor
{
    template <typename T>
    void operator () (const T &aIterator)
    {
        typename T::second_type sPtr = aIterator.second;
        XPR_SAFE_DELETE_ARRAY(sPtr);
    }
};

struct HeapFreeFunctor
{
    template <typename T>
    void operator () (const T &aPtr)
    {
        T sPtr = aPtr;
        XPR_SAFE_FREE(sPtr);
    }
};

struct MapHeapFreeFunctor
{
    template <typename T>
    void operator () (const T &aPtr)
    {
        typename T::second_type sPtr = aPtr.second;
        XPR_SAFE_FREE(sPtr);
    }
};

template <class T>
void clear(T &aContainer)
{
    std::for_each(aContainer.begin(), aContainer.end(), fxb::HeapDeleteFunctor());
    aContainer.clear();
}

template <class T>
void clearMap(T &aContainer)
{
    std::for_each(aContainer.begin(), aContainer.end(), fxb::MapHeapDeleteFunctor());
    aContainer.clear();
}
} // namespace fxb

#endif // __FXB_FUNCTORS_H__
