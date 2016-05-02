//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_FILE_PATH_H__
#define __XPR_FILE_PATH_H__ 1
#pragma once

#include "xpr_config.h"
#include "xpr_string.h"

#if defined(XPR_CFG_STL_TR1)
#include <functional>
#endif // XPR_CFG_STL_TR1

namespace xpr
{
class XPR_DL_API FilePath
{
public:
    enum
    {
        PathTypeNone = 0,
        PathTypeNormal,
        PathTypeUnc,
        PathTypeUri
    };

public:
    FilePath(void);
    FilePath(const xpr_tchar_t *aFilePath);
    FilePath(const xpr::string &aFilePath);
    FilePath(const FilePath &aFilePath);
    virtual ~FilePath(void);

public:
    // capacity
    xpr_size_t size(void) const;
    xpr_size_t length(void) const;
    xpr_size_t bytes(void) const;
    void       clear(void);
    xpr_bool_t empty(void) const;

public:
    // modifier
    FilePath& operator= (const xpr_tchar_t aFilePath);
    FilePath& operator= (const xpr_tchar_t *aFilePath);
    FilePath& operator= (const xpr::string &aFilePath);
    FilePath& operator= (const FilePath &aFilePath);

    FilePath& operator+=(const xpr_tchar_t aFilePath);
    FilePath& operator+=(const xpr_tchar_t *aFilePath);
    FilePath& operator+=(const xpr::string &aFilePath);
    FilePath& operator+=(const FilePath &aFilePath);

    void assign(const xpr_tchar_t aFilePath);
    void assign(const xpr_tchar_t *aFilePath);
    void assign(const xpr::string &aFilePath);
    void assign(const FilePath &aFilePath);

    void append(const xpr_tchar_t aFilePath);
    void append(const xpr_tchar_t *aFilePath);
    void append(const xpr::string &aFilePath);
    void append(const FilePath &aFilePath);

    void setDir(const xpr_tchar_t *aDir);
    void setDir(const xpr::string &aDir);

    void setRootDir(const xpr_tchar_t *aRootDir);
    void setRootDir(const xpr::string &aRootDir);

    void setParentDir(const xpr_tchar_t *aParentDir);
    void setParentDir(const xpr::string &aParentDir);

    xpr_bool_t setFileName(const xpr_tchar_t *aFileName, xpr_bool_t aWithExt = XPR_TRUE);
    xpr_bool_t setFileName(const xpr::string &aFileName, xpr_bool_t aWithExt = XPR_TRUE);

    xpr_bool_t setExt(const xpr_tchar_t *aExt);
    xpr_bool_t setExt(const xpr::string &aExt);

    void swap(FilePath &aFilePath);

    void upperCase(void);
    void lowerCase(void);

    xpr_bool_t normalize(void);

public:
    // operations
    const xpr::string &getPath(void) const;
    xpr_bool_t getPath(xpr_tchar_t *aFilePath, xpr_size_t aMaxLen) const;
    void getPath(xpr::string &aFilePath) const;

    xpr_size_t getPathType(void) const;
    xpr_bool_t isPathType(xpr_size_t aPathType) const;

    xpr_bool_t getScheme(xpr_tchar_t *aScheme, xpr_size_t aMaxLen) const;
    xpr_bool_t getScheme(xpr::string &aScheme) const;
    xpr_bool_t hasScheme(void) const;

    xpr_bool_t getRootDir(xpr_tchar_t *aRootDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getRootDir(xpr::string &aRootDir) const;
    xpr_bool_t getRootDir(xpr::FilePath &aFilePath) const;
    xpr_bool_t isRootDir(void) const;

    xpr_bool_t getParentDir(xpr_tchar_t *aParentDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getParentDir(xpr::string &aParentDir) const;
    xpr_bool_t getParentDir(xpr::FilePath &aFilePath) const;

    xpr_bool_t getFileName(xpr_tchar_t *aFileName, xpr_size_t aMaxLen, xpr_bool_t aWithExt = XPR_TRUE) const;
    xpr_bool_t getFileName(xpr::string &aFileName, xpr_bool_t aWithExt = XPR_TRUE) const;
    xpr_bool_t getFileName(xpr::FilePath &aFilePath, xpr_bool_t aWithExt = XPR_TRUE) const;

    xpr_bool_t getExt(xpr_tchar_t *aExt, xpr_size_t aMaxLen) const;
    xpr_bool_t getExt(xpr::string &aExt) const;
    xpr_bool_t equalExt(const xpr_tchar_t *aExt) const;
    xpr_bool_t equalExt(xpr::string &aExt) const;
    xpr_bool_t hasExt(void) const;

    xpr_sint_t compare(const xpr_tchar_t *aFilePath) const;
    xpr_sint_t compare(const xpr::string &aFilePath) const;
    xpr_sint_t compare(const FilePath &aFilePath) const;

    xpr_sint_t compareCase(const xpr_tchar_t *aFilePath, xpr_bool_t aCaseSensitivity = XPR_TRUE) const;
    xpr_sint_t compareCase(const xpr::string &aFilePath, xpr_bool_t aCaseSensitivity = XPR_TRUE) const;
    xpr_sint_t compareCase(const FilePath &aFilePath, xpr_bool_t aCaseSensitivity = XPR_TRUE) const;

    xpr_bool_t isAbsolute(void) const;
    xpr_bool_t isNetwork(void) const;
    xpr_bool_t isLong(void) const;

    xpr_bool_t toUri(xpr_tchar_t *aUri, xpr_size_t aMaxLen) const;
    xpr_bool_t toUri(xpr::string &aUri) const;
    xpr_bool_t toUri(FilePath &aFilePath) const;

    static xpr_bool_t getInvalidChars(xpr_tchar_t *aInvalidChars, xpr_size_t aMaxLen);
    static void getInvalidChars(xpr::string &aInvalidChars);

    static xpr_tchar_t getFileSeparator(void);
    static xpr_tchar_t getPathSeparator(void);

protected:
    void analyze(void);

    xpr_size_t getRootDirLength(void) const;
    xpr_size_t getParentDirLength(void) const;
    xpr_size_t getFileNamePosition(void) const;

    void removeLastCharSeparator(void);

    void getLastName(xpr::string &aLastName) const;

    xpr_sint_t compareCase(const xpr_tchar_t *aFilePath, xpr_size_t aLength, xpr_bool_t aCaseSensitivity) const;

    xpr_bool_t validate(const xpr_tchar_t *aFileName) const;

    xpr_size_t getSeparatorCount(void) const;

protected:
    xpr::string mFilePath;
    xpr_size_t  mPathType;
};

XPR_INLINE FilePath operator+ (const FilePath &aFilePath1, const xpr::string &aFilePath2)
{
    return FilePath(aFilePath1) += aFilePath2;
}

XPR_INLINE FilePath operator+ (const xpr_tchar_t *aFilePath1, const FilePath &aFilePath2)
{
    return FilePath(aFilePath1) += aFilePath2;
}

XPR_INLINE FilePath operator+ (xpr_tchar_t aChar, const FilePath &aFilePath2)
{
    return FilePath(xpr::string(1, aChar)) += aFilePath2;
}

XPR_INLINE FilePath operator+ (const FilePath &aFilePath1, const xpr_tchar_t *aFilePath2)
{
    return FilePath(aFilePath1) += aFilePath2;
}

XPR_INLINE FilePath operator+ (const FilePath &aFilePath1, xpr_tchar_t aChar)
{
    return FilePath(aFilePath1) += aChar;
}

XPR_INLINE bool operator== (const FilePath &aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) == 0) ? true : false;
}

XPR_INLINE bool operator== (const xpr_tchar_t *aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath2.compare(aFilePath1) == 0) ? true : false;
}

XPR_INLINE bool operator== (const FilePath &aFilePath1, const xpr_tchar_t *aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) == 0) ? true : false;
}

XPR_INLINE bool operator!= (const FilePath &aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) != 0) ? true : false;
}

XPR_INLINE bool operator!= (const xpr_tchar_t *aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath2.compare(aFilePath1) != 0) ? true : false;
}

XPR_INLINE bool operator!= (const FilePath &aFilePath1, const xpr_tchar_t *aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) != 0) ? true : false;
}

XPR_INLINE bool operator< (const FilePath &aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) < 0) ? true : false;
}

XPR_INLINE bool operator< (const xpr_tchar_t *aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath2.compare(aFilePath1) > 0) ? true : false;
}

XPR_INLINE bool operator< (const FilePath &aFilePath1, const xpr_tchar_t *aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) < 0) ? true : false;
}

XPR_INLINE bool operator> (const FilePath &aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) > 0) ? true : false;
}

XPR_INLINE bool operator> (const xpr_tchar_t *aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath2.compare(aFilePath1) < 0) ? true : false;
}

XPR_INLINE bool operator> (const FilePath &aFilePath1, const xpr_tchar_t *aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) > 0) ? true : false;
}

XPR_INLINE bool operator<= (const FilePath &aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) <= 0) ? true : false;
}

XPR_INLINE bool operator<= (const xpr_tchar_t *aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath2.compare(aFilePath1) >= 0) ? true : false;
}

XPR_INLINE bool operator<= (const FilePath &aFilePath1, const xpr_tchar_t *aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) <= 0) ? true : false;
}

XPR_INLINE bool operator>= (const FilePath &aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) >= 0) ? true : false;
}

XPR_INLINE bool operator>= (const xpr_tchar_t *aFilePath1, const FilePath &aFilePath2)
{
    return (aFilePath2.compare(aFilePath1) <= 0) ? true : false;
}

XPR_INLINE bool operator>= (const FilePath &aFilePath1, const xpr_tchar_t *aFilePath2)
{
    return (aFilePath1.compare(aFilePath2) >= 0) ? true : false;
}
} // namespace xpr

#if defined(XPR_CFG_STL_TR1)
XPR_NAMESPACE_STD_TR1_BEGIN

template <>
struct hash<xpr::FilePath> : public unary_function<xpr::FilePath, xpr_size_t>
{
    xpr_size_t operator()(const xpr::FilePath &aValue) const
    {
        // hash _Keyval to size_t value by pseudorandomizing transform
        xpr_size_t sHashValue = 2166136261U;
        xpr_size_t sFirst = 0;
        xpr_size_t sLast = aValue.size();
        xpr_size_t sStride = 1 + sLast / 10;

        const xpr::string &sValue1 = aValue.getPath();
        xpr_size_t         sValue2 = aValue.getPathType();

        if (sStride < sLast)
        {
            sLast -= sStride;
        }

        for (; sFirst < sLast; sFirst += sStride)
        {
            sHashValue = 16777619U * sHashValue ^ (xpr_size_t)sValue1[sFirst];
        }

        sHashValue = 16777619U * sHashValue ^ (xpr_size_t)sValue2;

        return sHashValue;
    }
};

XPR_NAMESPACE_STD_TR1_END

#endif // XPR_CFG_STL_TR

#endif // __XPR_FILE_PATH_H__
