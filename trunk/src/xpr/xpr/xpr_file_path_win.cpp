//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_file_path.h"
#include "xpr_file_sys.h"
#include "xpr_debug.h"
#include "xpr_uri.h"
#include "xpr_memory.h"

/**
 * file path representaion
 * 1. normal path (local file system)
 *    e.g.) test.log
 *    e.g.) windows\system32\test.log
 *    e.g.) c:\windows\system32\test.log
 *    e.g.) c:/windows/system32/test.log
 * 2. UNC path
 *    e.g.) \\ServerName\ShareName\file
 *    e.g.) \\?\UNC\ServerName\ShareName\file (long UNC)
 *    e.g.) \\?\UNC\c:\windows\system32\test.log (long UNC)
 * 3. URI path
 *    e.g.) file:///c:/windows/system32/test.log
 *    e.g.) file:///c|/windows/system32/test.log
 *    e.g.) file:///test.log
 *    e.g.) file:///etc/fstab
 *    e.g.) file://localhost/c:/windows/system32/test.log
 *    e.g.) file://localhost/c|/windows/system32/test.log
 *    e.g.) file://localhost/etc/fstab
 *    e.g.) file://ServerName/ShareName/file
 *    e.g.) http://www.test.com/test.log
 *    e.g.) https://www.test.com/test.log
 *    e.g.) ftp://www.test.com/test.log
 *    e.g.) smb://ServerName/ShareName/file
 */
namespace xpr
{
static const xpr_tchar_t kUncLong           [] = XPR_STRING_LITERAL("\\\\?\\UNC\\");
static const xpr_tchar_t kUriFileScheme     [] = XPR_STRING_LITERAL("file://");
static const xpr_tchar_t kUriFileSchemeLocal[] = XPR_STRING_LITERAL("file:///");
static const xpr_tchar_t kUriSmbScheme      [] = XPR_STRING_LITERAL("smb://");

#define XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR      XPR_DRIVE_SEPARATOR
#define XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR2     XPR_STRING_LITERAL('|')
#define XPR_ALL_OF_FILE_SEPARATORS_STRING        XPR_STRING_LITERAL("/\\")
#define XPR_CUR_DIR_FILENAME                     XPR_STRING_LITERAL(".")
#define XPR_PARENT_DIR_FILENAME                  XPR_STRING_LITERAL("..")

FilePath::FilePath(void)
    : mPathType(PathTypeNone)
{
}

FilePath::FilePath(const xpr_tchar_t *aFilePath)
    : mPathType(PathTypeNone)
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    assign(aFilePath);
}

FilePath::FilePath(const xpr::tstring &aFilePath)
    : mPathType(PathTypeNone)
{
    assign(aFilePath);
}

FilePath::FilePath(const FilePath &aFilePath)
    : mPathType(PathTypeNone)
{
    assign(aFilePath.mFilePath);
}

FilePath::~FilePath(void)
{
    clear();
}

xpr_size_t FilePath::size(void) const
{
    return mFilePath.size();
}

xpr_size_t FilePath::length(void) const
{
    return mFilePath.length();
}

xpr_size_t FilePath::bytes(void) const
{
    return mFilePath.bytes();
}

void FilePath::clear(void)
{
    mPathType = PathTypeNone;
    mFilePath.clear();
}

xpr_bool_t FilePath::empty(void) const
{
    return mFilePath.empty();
}

FilePath& FilePath::operator=(const xpr_tchar_t aFilePath)
{
    assign(aFilePath);

    return *this;
}

FilePath& FilePath::operator=(const xpr_tchar_t *aFilePath)
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    assign(aFilePath);

    return *this;
}

FilePath& FilePath::operator=(const xpr::tstring &aFilePath)
{
    assign(aFilePath);

    return *this;
}

FilePath& FilePath::operator=(const FilePath &aFilePath)
{
    assign(aFilePath);

    return *this;
}

FilePath& FilePath::operator+=(const xpr_tchar_t aFilePath)
{
    append(aFilePath);

    return *this;
}

FilePath& FilePath::operator+=(const xpr_tchar_t *aFilePath)
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    append(aFilePath);

    return *this;
}

FilePath& FilePath::operator+=(const xpr::tstring &aFilePath)
{
    append(aFilePath);

    return *this;
}

FilePath& FilePath::operator+=(const FilePath &aFilePath)
{
    append(aFilePath);

    return *this;
}

void FilePath::assign(const xpr_tchar_t aFilePath)
{
    xpr_tchar_t sFilePath[2] = { aFilePath, 0 };

    assign(sFilePath);
}

void FilePath::assign(const xpr_tchar_t *aFilePath)
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    mFilePath.assign(aFilePath);

    analyze();

    removeLastCharSeparator();
}

void FilePath::assign(const xpr::tstring &aFilePath)
{
    const xpr_tchar_t *sFilePath = aFilePath.c_str();

    mFilePath.assign(sFilePath);

    analyze();

    removeLastCharSeparator();
}

void FilePath::assign(const FilePath &aFilePath)
{
    const xpr_tchar_t *sFilePath = aFilePath.mFilePath.c_str();

    mFilePath.assign(sFilePath);

    analyze();

    removeLastCharSeparator();
}

/**
 * remove last separator ('\\' or '/')
 */
void FilePath::removeLastCharSeparator(void)
{
    if (mFilePath.empty() == XPR_TRUE)
    {
        return;
    }

    xpr_size_t sLength = mFilePath.length();

    switch (mPathType)
    {
    case PathTypeNormal:
    case PathTypeUnc:
        {
            const xpr_tchar_t &sLastChar = *mFilePath.rbegin();
            if (sLastChar == XPR_FILE_SEPARATOR || sLastChar == XPR_URI_SEPARATOR)
            {
                mFilePath.erase(mFilePath.length() - 1);
            }

            break;
        }

    case PathTypeUri:
        {
            xpr_size_t sSeparatorPosition;
            xpr_size_t sUriLocalFileSchemeLength = _tcslen(kUriFileSchemeLocal);
            xpr_size_t sUriSmbSchemeLength       = _tcslen(kUriSmbScheme);

            if (mFilePath.compare_case(0, sUriLocalFileSchemeLength, kUriFileSchemeLocal) == 0)
            {
                // e.g.) file:///c: or file:///c:/
                // e.g.) file:///c| or file:///c|/

                if (sLength > sUriLocalFileSchemeLength)
                {
                    const xpr_tchar_t &sLastChar = *mFilePath.rbegin();
                    if (sLastChar == XPR_URI_SEPARATOR)
                    {
                        mFilePath.erase(sLength - 1);
                    }
                }
            }
            else
            {
                xpr_size_t sUriSchemeLength = _tcslen(XPR_URI_SCHEME_SEPARATOR);

                sSeparatorPosition = mFilePath.find(XPR_URI_SCHEME_SEPARATOR);
                if ((sSeparatorPosition != xpr::tstring::npos) && ((sSeparatorPosition + sUriSchemeLength + 1) < sLength))
                {
                    const xpr_tchar_t &sLastChar = *mFilePath.rbegin();
                    if (sLastChar == XPR_URI_SEPARATOR)
                    {
                        mFilePath.erase(sLength - 1);
                    }
                }
            }

            break;
        }

    default:
        break;
    }
}

void FilePath::analyze(void)
{
    xpr_size_t sLength = mFilePath.length();

    // analyze path type
    if (sLength == 0)
    {
        mPathType = PathTypeNone;
    }
    else if (sLength < 2)
    {
        mPathType = PathTypeNormal;
    }
    else
    {
        if (mFilePath[0] == XPR_FILE_SEPARATOR &&
            mFilePath[1] == XPR_FILE_SEPARATOR)
        {
            mPathType = PathTypeUnc;
        }
        else if (mFilePath[1] == XPR_DRIVE_SEPARATOR)
        {
            mPathType = PathTypeNormal;
        }
        else
        {
            xpr_size_t sSeparatorPosition = mFilePath.find(XPR_URI_SCHEME_SEPARATOR);
            if (sSeparatorPosition != xpr::tstring::npos)
            {
                mPathType = PathTypeUri;
            }
            else
            {
                mPathType = PathTypeNormal;
            }
        }
    }

    // convert to file separator of the analyzed path type
    xpr_size_t  sSeparatorPosition = 0;
    xpr_tchar_t sFileSeparator;
    xpr_bool_t  sKnownPathType = XPR_TRUE;

    switch (mPathType)
    {
    case PathTypeNormal: sFileSeparator = XPR_FILE_SEPARATOR; break;
    case PathTypeUnc:    sFileSeparator = XPR_FILE_SEPARATOR; break;
    case PathTypeUri:    sFileSeparator = XPR_URI_SEPARATOR;  break;

    default:
        sKnownPathType = XPR_FALSE;
        break;
    }

    if (XPR_IS_TRUE(sKnownPathType))
    {
        do
        {
            sSeparatorPosition = mFilePath.find_first_of(XPR_ALL_OF_FILE_SEPARATORS_STRING, sSeparatorPosition);
            if (sSeparatorPosition != xpr::tstring::npos)
            {
                mFilePath[sSeparatorPosition] = sFileSeparator;
                ++sSeparatorPosition;
            }
            else
            {
                break;
            }
        } while (true);
    }
}

void FilePath::append(const xpr_tchar_t aFilePath)
{
    xpr_tchar_t sFilePath[2] = { aFilePath, 0 };

    append(sFilePath);
}

void FilePath::append(const xpr_tchar_t *aFilePath)
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    if (aFilePath[0] == 0)
    {
        // aFilePath is empty
        return;
    }

    if (XPR_IS_TRUE(mFilePath.empty()))
    {
        mFilePath += aFilePath;
    }
    else
    {
        if (*mFilePath.rbegin() == XPR_FILE_SEPARATOR)
        {
            if (aFilePath[0] == XPR_FILE_SEPARATOR)
            {
                mFilePath.erase(mFilePath.length() - 1);
            }
        }
        else
        {
            if (aFilePath[0] != XPR_FILE_SEPARATOR)
            {
                mFilePath += XPR_FILE_SEPARATOR;
            }
        }

        mFilePath += aFilePath;
    }

    analyze();

    removeLastCharSeparator();
}

void FilePath::append(const xpr::tstring &aFilePath)
{
    const xpr_tchar_t *sFilePath = aFilePath.c_str();

    append(sFilePath);
}

void FilePath::append(const FilePath &aFilePath)
{
    const xpr_tchar_t *sFilePath = aFilePath.mFilePath.c_str();

    append(sFilePath);
}

void FilePath::getLastName(xpr::tstring &aLastName) const
{
    xpr_size_t sSeparatorPosition;

    switch (mPathType)
    {
    case PathTypeNormal:
    case PathTypeUnc:
        sSeparatorPosition = mFilePath.rfind(XPR_FILE_SEPARATOR);
        if (sSeparatorPosition != xpr::tstring::npos)
        {
            aLastName = mFilePath.c_str() + sSeparatorPosition + 1;
        }
        else
        {
            if (mPathType == PathTypeNormal)
            {
                aLastName = mFilePath;
            }
        }

        break;

    case PathTypeUri:
        sSeparatorPosition = mFilePath.rfind(XPR_URI_SEPARATOR);
        if (sSeparatorPosition != xpr::tstring::npos)
        {
            aLastName = mFilePath.c_str() + sSeparatorPosition + 1;
        }

        break;
    }
}

void FilePath::setDir(const xpr_tchar_t *aDir)
{
    XPR_ASSERT(aDir != XPR_NULL);

    xpr::tstring sLastName;
    getLastName(sLastName);

    if (aDir[0] == 0)
    {
        assign(sLastName);
    }
    else
    {
        assign(aDir);

        if (XPR_IS_FALSE(sLastName.empty()))
        {
            if (mPathType == PathTypeNormal || mPathType == PathTypeUnc)
            {
                if (*mFilePath.rbegin() != XPR_FILE_SEPARATOR)
                {
                    mFilePath += XPR_FILE_SEPARATOR;
                }
            }
            else
            {
                if (*mFilePath.rbegin() != XPR_URI_SEPARATOR)
                {
                    mFilePath += XPR_URI_SEPARATOR;
                }
            }

            mFilePath += sLastName;
        }
    }
}

void FilePath::setDir(const xpr::tstring &aDir)
{
    const xpr_tchar_t *sDir = aDir.c_str();

    setDir(sDir);
}

void FilePath::setRootDir(const xpr_tchar_t *aRootDir)
{
    XPR_ASSERT(aRootDir != XPR_NULL);

    xpr_size_t sInputRootDirLength = _tcslen(aRootDir);
    if (sInputRootDirLength > 0)
    {
        if (aRootDir[sInputRootDirLength - 1] == XPR_FILE_SEPARATOR ||
            aRootDir[sInputRootDirLength - 1] == XPR_URI_SEPARATOR)
        {
            --sInputRootDirLength;
        }
    }

    xpr_size_t sRootDirLength = getRootDirLength();
    if (sRootDirLength > 0)
    {
        mFilePath.replace(0, sRootDirLength, aRootDir, sInputRootDirLength);
    }
    else
    {
        xpr::tstring sFileName;
        getFileName(sFileName);

        assign(aRootDir);

        removeLastCharSeparator();

        setFileName(sFileName);
    }

    analyze();
}

void FilePath::setRootDir(const xpr::tstring &aRootDir)
{
    const xpr_tchar_t *sRootDir = aRootDir.c_str();

    setRootDir(sRootDir);
}

void FilePath::setParentDir(const xpr_tchar_t *aParentDir)
{
    XPR_ASSERT(aParentDir != XPR_NULL);

    xpr_size_t sInputParentDirLength = _tcslen(aParentDir);
    if (sInputParentDirLength > 0)
    {
        if (aParentDir[sInputParentDirLength - 1] == XPR_FILE_SEPARATOR ||
            aParentDir[sInputParentDirLength - 1] == XPR_URI_SEPARATOR)
        {
            --sInputParentDirLength;
        }
    }

    xpr_size_t sParentDirLength = getParentDirLength();
    if (sParentDirLength > 0)
    {
        mFilePath.replace(0, sParentDirLength, aParentDir, sInputParentDirLength);
    }
    else
    {
        xpr::tstring sFileName;
        getFileName(sFileName);

        assign(aParentDir);

        removeLastCharSeparator();

        setFileName(sFileName);
    }

    analyze();
}

void FilePath::setParentDir(const xpr::tstring &aParentDir)
{
    const xpr_tchar_t *sParentDir = aParentDir.c_str();

    setParentDir(sParentDir);
}

xpr_bool_t FilePath::validate(const xpr_tchar_t *aFileName) const
{
    XPR_ASSERT(aFileName != XPR_NULL);

    xpr::tstring sInvalidChars;
    FilePath::getInvalidChars(sInvalidChars);

    if (XPR_IS_NOT_NULL(_tcspbrk(aFileName, sInvalidChars.c_str())))
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t FilePath::setFileName(const xpr_tchar_t *aFileName, xpr_bool_t aWithExt)
{
    XPR_ASSERT(aFileName != XPR_NULL);

    // validate file name
    if (XPR_IS_FALSE(validate(aFileName)))
    {
        return XPR_FALSE;
    }

    // set file name with extenstion option
    xpr_size_t sFileNamePosition = getFileNamePosition();

    xpr::tstring sExt;
    if (XPR_IS_FALSE(aWithExt))
    {
        getExt(sExt);
    }

    if (sFileNamePosition == xpr::tstring::npos)
    {
        mFilePath.clear();
    }
    else
    {
        mFilePath.erase(sFileNamePosition);
    }

    mFilePath += aFileName;

    if (XPR_IS_FALSE(aWithExt))
    {
        mFilePath += sExt;
    }

    return XPR_TRUE;
}

xpr_bool_t FilePath::setFileName(const xpr::tstring &aFileName, xpr_bool_t aWithExt)
{
    const xpr_tchar_t *sFileName = aFileName.c_str();

    return setFileName(sFileName, aWithExt);
}

xpr_bool_t FilePath::setExt(const xpr_tchar_t *aExt)
{
    XPR_ASSERT(aExt != XPR_NULL);

    // validate file extension
    if (XPR_IS_FALSE(validate(aExt)))
    {
        return XPR_FALSE;
    }

    // set file extension
    xpr_size_t sSeparator = xpr::tstring::npos;

    switch (mPathType)
    {
    case PathTypeNormal:
    case PathTypeUnc:
        sSeparator = mFilePath.rfind(XPR_FILE_SEPARATOR);
        break;

    case PathTypeUri:
        sSeparator = mFilePath.rfind(XPR_URI_SEPARATOR);
        break;

    default:
        break;
    }

    xpr_size_t sDot = mFilePath.rfind(XPR_FILE_EXT_SEPARATOR);

    if (sDot != xpr::tstring::npos && sSeparator < sDot)
    {
        mFilePath.replace(sDot, xpr::tstring::npos, aExt);
    }
    else
    {
        mFilePath += aExt;
    }

    return XPR_TRUE;
}

xpr_bool_t FilePath::setExt(const xpr::tstring &aExt)
{
    const xpr_tchar_t *sExt = aExt.c_str();

    return setExt(sExt);
}

void FilePath::swap(FilePath &aFilePath)
{
    mFilePath.swap(aFilePath.mFilePath);
}

void FilePath::upperCase(void)
{
    mFilePath.upper_case();
}

void FilePath::lowerCase(void)
{
    mFilePath.lower_case();
}

xpr_size_t FilePath::getSeparatorCount(void) const
{
    xpr_size_t  sSeparatorCount    = 0;
    xpr_size_t  sSeparatorPosition = 0;
    xpr_tchar_t sSeperator         = 0;

    switch (mPathType)
    {
    case PathTypeUnc:
        sSeparatorPosition = 2;

    case PathTypeNormal:
        sSeperator = XPR_FILE_SEPARATOR;
        break;

    case PathTypeUri:
        sSeperator = XPR_URI_SEPARATOR;
        break;

    default:
        break;
    }

    if (sSeperator != 0)
    {
        do
        {
            sSeparatorPosition = mFilePath.find(sSeperator, sSeparatorPosition);
            if (sSeparatorPosition == xpr::tstring::npos)
            {
                break;
            }
            else
            {
                ++sSeparatorPosition;
                ++sSeparatorCount;
            }
        } while (true);
    }

    return sSeparatorCount;
}

// normalize (convert canonical path), if it contains '.' or '..'.
xpr_bool_t FilePath::normalize(void)
{
    xpr_size_t    i;
    xpr_bool_t    sResult                = XPR_FALSE;
    xpr_bool_t    sStopLoop;
    xpr_size_t    sSeparatorCount        = getSeparatorCount();
    xpr::tstring  sNormalizedFilePath    = mFilePath;
    xpr_size_t   *sSeparatorPositions    = XPR_NULL;
    xpr_size_t    sBeginSearchPosition   = 0;
    xpr_size_t    sSeparatorPosition     = 0;
    xpr_size_t    sLastSeparatorPosition = 0;
    xpr_size_t    sNamePosition          = 0;
    xpr_tchar_t   sSeparator             = 0;
    xpr_size_t    sNameLength;

    if (sSeparatorCount == 0)
    {
        if (sNormalizedFilePath.compare(XPR_CUR_DIR_FILENAME) == 0)
        {
            sNormalizedFilePath.clear();

            sResult = XPR_TRUE;
        }
        else if (sNormalizedFilePath.compare(XPR_PARENT_DIR_FILENAME) == 0)
        {
            sResult = XPR_FALSE;
        }
        else
        {
            sResult = XPR_TRUE;
        }
    }
    else
    {
        sSeparatorPositions = new xpr_size_t[sSeparatorCount];

        XPR_ASSERT(sSeparatorPositions != XPR_NULL);

        switch (mPathType)
        {
        case PathTypeUnc:
            sBeginSearchPosition = 2;

        case PathTypeNormal:
            sSeparator = XPR_FILE_SEPARATOR;
            break;

        case PathTypeUri:
            sBeginSearchPosition = sNormalizedFilePath.find(XPR_URI_SCHEME_SEPARATOR);
            XPR_ASSERT(sBeginSearchPosition != xpr::tstring::npos);

            sBeginSearchPosition += _tcslen(XPR_URI_SCHEME_SEPARATOR);

            sSeparator = XPR_URI_SEPARATOR;
            break;

        default:
            break;
        }

        if (sSeparator != 0)
        {
            sResult = XPR_TRUE;

            if (XPR_IS_FALSE(sNormalizedFilePath.empty()))
            {
                i         = 0;
                sStopLoop = XPR_FALSE;

                do
                {
                    sSeparatorPosition = sNormalizedFilePath.find(sSeparator, (i == 0) ? sBeginSearchPosition : (sLastSeparatorPosition + 1));
                    if (sSeparatorPosition == xpr::tstring::npos)
                    {
                        sSeparatorPosition = sNormalizedFilePath.length();

                        sStopLoop = XPR_TRUE;
                    }

                    if (i == 0)
                    {
                        sNamePosition = sBeginSearchPosition;
                        sNameLength   = sSeparatorPosition - sBeginSearchPosition;
                    }
                    else
                    {
                        sNamePosition = sLastSeparatorPosition + 1;
                        sNameLength   = sSeparatorPosition - sLastSeparatorPosition - 1; // without separator
                    }

                    if (sNameLength == 2)
                    {
                        if (sNormalizedFilePath.compare(sNamePosition, 2, XPR_PARENT_DIR_FILENAME) == 0)
                        {
                            if (i == 0)
                            {
                                sResult = XPR_FALSE;
                                break;
                            }
                            else if (i == 1)
                            {
                                sNormalizedFilePath.erase(sBeginSearchPosition, sSeparatorPosition - sBeginSearchPosition + 1); // with separator

                                sLastSeparatorPosition = sBeginSearchPosition;
                            }
                            else
                            {
                                sNormalizedFilePath.erase(sSeparatorPositions[i - 2], sSeparatorPosition - sSeparatorPositions[i - 2]); // with separator

                                sLastSeparatorPosition = sSeparatorPositions[i - 2];
                            }

                            --i;

                            continue;
                        }
                    }
                    else if (sNameLength == 1)
                    {
                        if (sNormalizedFilePath.compare(sNamePosition, 1, XPR_CUR_DIR_FILENAME) == 0)
                        {
                            if (i == 0)
                            {
                                sNormalizedFilePath.erase(sBeginSearchPosition, 2); // with separator

                                sLastSeparatorPosition = sBeginSearchPosition;
                            }
                            else
                            {
                                sNormalizedFilePath.erase(sLastSeparatorPosition, 2); // with separator
                            }

                            continue;
                        }
                    }
                    else
                    {
                        // nothing to do
                    }

                    sSeparatorPositions[i++] = sSeparatorPosition;
                    sLastSeparatorPosition = sSeparatorPosition;

                } while (XPR_IS_FALSE(sStopLoop));
            }
            else
            {
                // empty
            }
        }
        else
        {
            // unknown path type
        }
    }

    XPR_SAFE_DELETE_ARRAY(sSeparatorPositions);

    if (XPR_IS_TRUE(sResult))
    {
        mFilePath = sNormalizedFilePath;
    }

    return sResult;
}

const xpr::tstring &FilePath::getPath(void) const
{
    return mFilePath;
}

xpr_bool_t FilePath::getPath(xpr_tchar_t *aFilePath, xpr_size_t aMaxLen) const
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    if (mFilePath.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aFilePath, mFilePath.c_str());

    return XPR_TRUE;
}

void FilePath::getPath(xpr::tstring &aFilePath) const
{
    aFilePath = mFilePath;
}

xpr_size_t FilePath::getPathType(void) const
{
    return mPathType;
}

xpr_bool_t FilePath::isPathType(xpr_size_t aPathType) const
{
    return (mPathType == aPathType) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FilePath::getScheme(xpr_tchar_t *aScheme, xpr_size_t aMaxLen) const
{
    XPR_ASSERT(aScheme != XPR_NULL);

    xpr::tstring sScheme;
    getScheme(sScheme);

    if (sScheme.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aScheme, sScheme.c_str());

    return XPR_TRUE;
}

xpr_bool_t FilePath::getScheme(xpr::tstring &aScheme) const
{
    xpr_size_t sSchemePosition = mFilePath.find(XPR_URI_SCHEME_SEPARATOR);
    if (sSchemePosition == xpr::tstring::npos)
    {
        return XPR_FALSE;
    }

    aScheme = mFilePath.substr(0, sSchemePosition);

    return XPR_TRUE;
}

xpr_bool_t FilePath::hasScheme(void) const
{
    xpr_size_t sSchemePosition = mFilePath.find(XPR_URI_SCHEME_SEPARATOR);
    if (sSchemePosition == xpr::tstring::npos)
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t FilePath::getRootDir(xpr_tchar_t *aRootDir, xpr_size_t aMaxLen) const
{
    XPR_ASSERT(aRootDir != XPR_NULL);

    xpr::tstring sRootDir;
    if (XPR_IS_FALSE(getRootDir(sRootDir)))
    {
        return XPR_FALSE;
    }

    if (sRootDir.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aRootDir, sRootDir.c_str());

    return XPR_TRUE;
}

xpr_bool_t FilePath::getRootDir(xpr::tstring &aRootDir) const
{
    xpr_size_t sRootDirLength = getRootDirLength();
    if (sRootDirLength == 0)
    {
        return XPR_FALSE;
    }

    aRootDir = mFilePath.substr(0, sRootDirLength);

    return XPR_TRUE;
}

xpr_size_t FilePath::getRootDirLength(void) const
{
    xpr_size_t sRootDirLength = 0;
    xpr_size_t sLength = mFilePath.length();

    switch (mPathType)
    {
    case PathTypeNormal:
        {
            if (sLength >= 2)
            {
                if (mFilePath[1] == XPR_DRIVE_SEPARATOR)
                {
                    sRootDirLength = 2;
                }
            }

            break;
        }

    case PathTypeUnc:
        {
            if (mFilePath[0] == XPR_FILE_SEPARATOR &&
                mFilePath[1] == XPR_FILE_SEPARATOR &&
                mFilePath[2] != XPR_STRING_LITERAL('?'))
            {
                // e.g.) \\ServerName

                xpr_size_t sSeparatorPosition = mFilePath.find(XPR_FILE_SEPARATOR, 2);
                if (sSeparatorPosition != xpr::tstring::npos)
                {
                    sSeparatorPosition = mFilePath.find(XPR_FILE_SEPARATOR, sSeparatorPosition + 1);
                    if (sSeparatorPosition == xpr::tstring::npos)
                    {
                        sRootDirLength = mFilePath.length();
                    }
                    else
                    {
                        sRootDirLength = sSeparatorPosition;
                    }
                }
            }
            else
            {
                // e.g.) \\?\UNC\ServerName (long UNC)

                xpr_size_t sUncLongLength = _tcslen(kUncLong);

                if (mFilePath.compare_case(0, sUncLongLength, kUncLong) == 0)
                {
                    if (sLength > sUncLongLength)
                    {
                        xpr_size_t sSeparatorPosition = mFilePath.find(XPR_FILE_SEPARATOR, sUncLongLength);
                        if (sSeparatorPosition != xpr::tstring::npos)
                        {
                            sSeparatorPosition = mFilePath.find(XPR_FILE_SEPARATOR, sSeparatorPosition + 1);
                            if (sSeparatorPosition == xpr::tstring::npos)
                            {
                                sRootDirLength = mFilePath.length();
                            }
                            else
                            {
                                sRootDirLength = sSeparatorPosition;
                            }
                        }
                    }
                }
            }

            break;
        }

    case PathTypeUri:
        {
            xpr_size_t sSeparatorPosition;
            xpr_size_t sUriLocalFileSchemeLength = _tcslen(kUriFileSchemeLocal);
            xpr_size_t sUriSmbSchemeLength       = _tcslen(kUriSmbScheme);

            if (mFilePath.compare_case(0, sUriLocalFileSchemeLength, kUriFileSchemeLocal) == 0)
            {
                // e.g.) file:///c: or file:///c:/
                // e.g.) file:///c| or file:///c|/

                if (sLength >= (sUriLocalFileSchemeLength + 2))
                {
                    if (mFilePath[sUriLocalFileSchemeLength + 1] == XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR ||
                        mFilePath[sUriLocalFileSchemeLength + 1] == XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR2) // local path
                    {
                        sSeparatorPosition = mFilePath.find(XPR_URI_SEPARATOR, sUriLocalFileSchemeLength);
                        if (sSeparatorPosition == xpr::tstring::npos)
                        {
                            sRootDirLength = mFilePath.length();
                        }
                        else
                        {
                            sRootDirLength = sSeparatorPosition;
                        }
                    }
                }
            }
            else if (mFilePath.compare_case(0, sUriSmbSchemeLength, kUriSmbScheme) == 0)
            {
                // e.g.) smb://ServerName/ShareName/file

                sSeparatorPosition = mFilePath.find(XPR_URI_SEPARATOR, sUriSmbSchemeLength);
                if (sSeparatorPosition != xpr::tstring::npos)
                {
                    sSeparatorPosition = mFilePath.find(XPR_URI_SEPARATOR, sSeparatorPosition + 1);
                    if (sSeparatorPosition == xpr::tstring::npos)
                    {
                        sRootDirLength = mFilePath.length();
                    }
                    else
                    {
                        sRootDirLength = sSeparatorPosition;
                    }
                }
            }
            else
            {
                xpr_size_t sUriSchemeLength = _tcslen(XPR_URI_SCHEME_SEPARATOR);

                sSeparatorPosition = mFilePath.find(XPR_URI_SCHEME_SEPARATOR);
                if ((sSeparatorPosition != xpr::tstring::npos) && ((sSeparatorPosition + sUriSchemeLength + 1) < sLength))
                {
                    sSeparatorPosition = mFilePath.find(XPR_URI_SEPARATOR, sSeparatorPosition + sUriSchemeLength);
                    if (sSeparatorPosition == xpr::tstring::npos)
                    {
                        sRootDirLength = mFilePath.length();
                    }
                    else
                    {
                        sRootDirLength = sSeparatorPosition;
                    }
                }
            }

            break;
        }

    default:
        {
            sRootDirLength = 0;
            break;
        }
    }

    return sRootDirLength;
}

xpr_bool_t FilePath::getRootDir(xpr::FilePath &aFilePath) const
{
    xpr::tstring sRootDir;
    if (XPR_IS_FALSE(getRootDir(sRootDir)))
    {
        return XPR_FALSE;
    }

    aFilePath = sRootDir;

    return XPR_TRUE;
}

xpr_bool_t FilePath::isRootDir(void) const
{
    xpr::tstring sRootDir;
    if (XPR_IS_FALSE(getRootDir(sRootDir)))
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t FilePath::getParentDir(xpr_tchar_t *aParentDir, xpr_size_t aMaxLen) const
{
    XPR_ASSERT(aParentDir != XPR_NULL);

    xpr::tstring sParentDir;
    if (XPR_IS_FALSE(getParentDir(sParentDir)))
    {
        return XPR_FALSE;
    }

    if (sParentDir.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aParentDir, sParentDir.c_str());

    return XPR_TRUE;
}

xpr_bool_t FilePath::getParentDir(xpr::tstring &aParentDir) const
{
    xpr_size_t sParentDirLength = getParentDirLength();
    if (sParentDirLength == 0)
    {
        return XPR_FALSE;
    }

    aParentDir = mFilePath.substr(0, sParentDirLength);

    return XPR_TRUE;
}

xpr_size_t FilePath::getParentDirLength(void) const
{
    xpr_size_t sParentDirLength   = 0;
    xpr_size_t sLength            = mFilePath.length();
    xpr_size_t sSeparatorPosition;
    xpr_size_t sLastSeparatorPosition;

    switch (mPathType)
    {
    case PathTypeNormal:
        {
            sSeparatorPosition = sLastSeparatorPosition = mFilePath.rfind(XPR_FILE_SEPARATOR);
            if (sSeparatorPosition != xpr::tstring::npos)
            {
                sParentDirLength = sLastSeparatorPosition;
            }

            break;
        }

    case PathTypeUnc:
        {
            sSeparatorPosition = sLastSeparatorPosition = mFilePath.rfind(XPR_FILE_SEPARATOR);
            if (sSeparatorPosition != xpr::tstring::npos && sSeparatorPosition > 2)
            {
                xpr_size_t sMinimumSeparator = 2;
                if (isLong() == XPR_TRUE)
                {
                    sMinimumSeparator = _tcslen(kUncLong);
                }

                sSeparatorPosition = mFilePath.rfind(XPR_FILE_SEPARATOR, sSeparatorPosition - 1);
                if (sSeparatorPosition != xpr::tstring::npos && sSeparatorPosition > sMinimumSeparator)
                {
                    sParentDirLength = sLastSeparatorPosition;
                }
            }

            break;
        }

    case PathTypeUri:
        {
            sSeparatorPosition = sLastSeparatorPosition = mFilePath.rfind(XPR_URI_SEPARATOR);
            if (sSeparatorPosition != xpr::tstring::npos)
            {
                xpr_size_t sPrefixLength             = 0;
                xpr_size_t sUriLocalFileSchemeLength = _tcslen(kUriFileSchemeLocal);
                xpr_size_t sUriSmbSchemeLength       = _tcslen(kUriSmbScheme);

                if (mFilePath.compare_case(0, sUriSmbSchemeLength, kUriSmbScheme) == 0)
                {
                    // e.g.) smb://localhost/c:/windows/system32
                    // e.g.) smb://localhost/c:/windows/system32/
                    // e.g.) smb://localhost/c|/windows/system32
                    // e.g.) smb://localhost/c|/windows/system32/
                    // e.g.) smb://ServerName/ShareName/file
                    // e.g.) smb://ServerName/ShareName/file/

                    sPrefixLength = sUriSmbSchemeLength;

                    sSeparatorPosition = mFilePath.rfind(XPR_URI_SEPARATOR, sSeparatorPosition - 1);
                    if (sSeparatorPosition != xpr::tstring::npos)
                    {
                        if (sSeparatorPosition > sPrefixLength)
                        {
                            sParentDirLength = sLastSeparatorPosition;
                        }
                    }
                }
                else
                {
                    if (mFilePath.compare_case(0, sUriLocalFileSchemeLength, kUriFileSchemeLocal) == 0)
                    {
                        // e.g.) file:///c:/work or file:///c:/work/
                        // e.g.) file:///c|/work or file:///c|/work/

                        sPrefixLength = sUriLocalFileSchemeLength;
                    }
                    else
                    {
                        sPrefixLength = mFilePath.find(XPR_URI_SCHEME_SEPARATOR) + _tcslen(XPR_URI_SCHEME_SEPARATOR);
                    }

                    if (sSeparatorPosition > sPrefixLength)
                    {
                        sParentDirLength = sLastSeparatorPosition;
                    }
                }
            }

            break;
        }

    default:
        sParentDirLength = 0;
        break;
    }

    return sParentDirLength;
}

xpr_bool_t FilePath::getParentDir(xpr::FilePath &aFilePath) const
{
    xpr::tstring sParentDir;
    if (XPR_IS_FALSE(getParentDir(sParentDir)))
    {
        return XPR_FALSE;
    }

    aFilePath = sParentDir;

    return XPR_TRUE;
}

xpr_bool_t FilePath::getFileName(xpr_tchar_t *aFileName, xpr_size_t aMaxLen, xpr_bool_t aWithExt) const
{
    XPR_ASSERT(aFileName != XPR_NULL);

    xpr::tstring sFileName;
    if (XPR_IS_FALSE(getFileName(sFileName, aWithExt)))
    {
        return XPR_FALSE;
    }

    if (sFileName.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aFileName, sFileName.c_str());

    return XPR_TRUE;
}

xpr_bool_t FilePath::getFileName(xpr::tstring &aFileName, xpr_bool_t aWithExt) const
{
    xpr_size_t sFileNamePosition = getFileNamePosition();
    if (sFileNamePosition == xpr::tstring::npos)
    {
        return XPR_FALSE;
    }

    aFileName = mFilePath.substr(sFileNamePosition);

    if (XPR_IS_FALSE(aWithExt))
    {
        xpr_size_t sFileExtSeparator = aFileName.rfind(XPR_FILE_EXT_SEPARATOR);
        if (sFileExtSeparator != xpr::tstring::npos)
        {
            aFileName.erase(sFileExtSeparator);
        }
    }

    return XPR_TRUE;
}

xpr_size_t FilePath::getFileNamePosition(void) const
{
    xpr_size_t sFileNamePosition = xpr::tstring::npos;
    xpr_size_t sLength = mFilePath.length();
    xpr_size_t sSeparatorPosition;
    xpr_size_t sLastSeparatorPosition;

    switch (mPathType)
    {
    case PathTypeNormal:
        {
            sSeparatorPosition = sLastSeparatorPosition = mFilePath.rfind(XPR_FILE_SEPARATOR);
            if (sSeparatorPosition != xpr::tstring::npos)
            {
                sFileNamePosition = sLastSeparatorPosition + 1;
            }
            else
            {
                if ((sLength < 2) || (sLength >= 2 && mFilePath[1] != XPR_DRIVE_SEPARATOR))
                {
                    sFileNamePosition = 0;
                }
            }

            break;
        }

    case PathTypeUnc:
        {
            xpr_size_t sMinimumSeparator = 2;
            if (isLong() == XPR_TRUE)
            {
                sMinimumSeparator = _tcslen(kUncLong);
            }

            sSeparatorPosition = sLastSeparatorPosition = mFilePath.rfind(XPR_FILE_SEPARATOR);
            if (sSeparatorPosition != xpr::tstring::npos && sSeparatorPosition > sMinimumSeparator)
            {
                sSeparatorPosition = mFilePath.rfind(XPR_FILE_SEPARATOR, sSeparatorPosition - 1);
                if (sSeparatorPosition != xpr::tstring::npos && sSeparatorPosition > sMinimumSeparator)
                {
                    sFileNamePosition = sLastSeparatorPosition + 1;
                }
            }

            break;
        }

    case PathTypeUri:
        {
            sSeparatorPosition = sLastSeparatorPosition = mFilePath.rfind(XPR_URI_SEPARATOR);
            if (sSeparatorPosition != xpr::tstring::npos)
            {
                xpr_size_t sPrefixLength             = 0;
                xpr_size_t sUriLocalFileSchemeLength = _tcslen(kUriFileSchemeLocal);
                xpr_size_t sUriSmbSchemeLength       = _tcslen(kUriSmbScheme);

                if (mFilePath.compare_case(0, sUriSmbSchemeLength, kUriSmbScheme) == 0)
                {
                    // e.g.) smb://localhost/c:/windows/system32
                    // e.g.) smb://localhost/c:/windows/system32/
                    // e.g.) smb://localhost/c|/windows/system32
                    // e.g.) smb://localhost/c|/windows/system32/
                    // e.g.) smb://ServerName/ShareName/file
                    // e.g.) smb://ServerName/ShareName/file/

                    sPrefixLength = sUriSmbSchemeLength;

                    sSeparatorPosition = mFilePath.rfind(XPR_URI_SEPARATOR, sSeparatorPosition - 1);
                    if (sSeparatorPosition != xpr::tstring::npos)
                    {
                        if (sSeparatorPosition > sPrefixLength)
                        {
                            sFileNamePosition = sLastSeparatorPosition + 1;
                        }
                    }
                }
                else
                {
                    if (mFilePath.compare_case(0, sUriLocalFileSchemeLength, kUriFileSchemeLocal) == 0)
                    {
                        // e.g.) file:///c:/work or file:///c:/work/
                        // e.g.) file:///c|/work or file:///c|/work/
                        // e.g.) file:///test.log

                        sPrefixLength = sUriLocalFileSchemeLength;

                        if (sSeparatorPosition > sPrefixLength)
                        {
                            sFileNamePosition = sLastSeparatorPosition + 1;
                        }
                        else if (sLength > sPrefixLength)
                        {
                            if (sLength <= (sPrefixLength + 1))
                            {
                                // e.g.) file:///t

                                sFileNamePosition = sPrefixLength;
                            }
                            else
                            {
                                if (sLength >= (sPrefixLength + 2))
                                {
                                    if (mFilePath[sPrefixLength + 1] != XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR &&
                                        mFilePath[sPrefixLength + 1] != XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR2)
                                    {
                                        sFileNamePosition = sPrefixLength;
                                    }
                                }
                                else
                                {
                                    sFileNamePosition = sPrefixLength;
                                }
                            }
                        }
                    }
                    else
                    {
                        // e.g.) file://localhost/work/docs
                        // e.g.) http://www.test.com/test.log

                        sPrefixLength = mFilePath.find(XPR_URI_SCHEME_SEPARATOR) + _tcslen(XPR_URI_SCHEME_SEPARATOR);

                        if (sSeparatorPosition > sPrefixLength)
                        {
                            sFileNamePosition = sLastSeparatorPosition + 1;
                        }
                    }
                }
            }

            break;
        }

    default:
        {
            sFileNamePosition = xpr::tstring::npos;
            break;
        }
    }

    return sFileNamePosition;
}

xpr_bool_t FilePath::getFileName(xpr::FilePath &aFilePath, xpr_bool_t aWithExt) const
{
    xpr::tstring sFileName;
    if (XPR_IS_FALSE(getFileName(sFileName, aWithExt)))
    {
        return XPR_FALSE;
    }

    aFilePath = sFileName;

    return XPR_TRUE;
}

xpr_bool_t FilePath::getExt(xpr_tchar_t *aExt, xpr_size_t aMaxLen) const
{
    XPR_ASSERT(aExt != XPR_NULL);

    xpr::tstring sExt;
    if (XPR_IS_FALSE(getExt(sExt)))
    {
        return XPR_FALSE;
    }

    if (sExt.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aExt, sExt.c_str());

    return XPR_TRUE;
}

xpr_bool_t FilePath::getExt(xpr::tstring &aExt) const
{
    xpr::tstring sFileName;
    if (XPR_IS_FALSE(getFileName(sFileName)))
    {
        return XPR_FALSE;
    }

    xpr_size_t sDot = sFileName.rfind(XPR_FILE_EXT_SEPARATOR);
    if (sDot != xpr::tstring::npos)
    {
        aExt = sFileName.substr(sDot);
    }
    else
    {
        aExt.clear();
    }

    return XPR_TRUE;
}

xpr_bool_t FilePath::equalExt(const xpr_tchar_t *aExt) const
{
    XPR_ASSERT(aExt != XPR_NULL);

    xpr::tstring sExt;
    if (XPR_IS_FALSE(getExt(sExt)))
    {
        return XPR_FALSE;
    }

    return (mFilePath.compare_case(aExt) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FilePath::equalExt(xpr::tstring &aExt) const
{
    xpr::tstring sExt;
    if (XPR_IS_FALSE(getExt(sExt)))
    {
        return XPR_FALSE;
    }

    return (mFilePath.compare_case(aExt) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FilePath::hasExt(void) const
{
    xpr::tstring sExt;
    if (XPR_IS_FALSE(getExt(sExt)))
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_sint_t FilePath::compareCase(const xpr_tchar_t *aFilePath, xpr_size_t aLength, xpr_bool_t aCaseSensitivity) const
{
    xpr_size_t sLength  = mFilePath.length();
    xpr_size_t sLength2 = aLength;

    if (sLength2 > 0)
    {
        switch (mPathType)
        {
        case PathTypeNormal:
            {
                if (aFilePath[sLength2 - 1] == XPR_FILE_SEPARATOR)
                {
                    --sLength2;
                }

                break;
            }

        case PathTypeUnc:
            {
                if (sLength2 > 2)
                {
                    if (aFilePath[sLength2 - 1] == XPR_FILE_SEPARATOR)
                    {
                        --sLength2;
                    }
                }

                break;
            }

        case PathTypeUri:
            {
                xpr_size_t sPrefixLength             = 0;
                xpr_size_t sUriLocalFileSchemeLength = _tcslen(kUriFileSchemeLocal);

                if (mFilePath.compare_case(0, sUriLocalFileSchemeLength, kUriFileSchemeLocal) == 0)
                {
                    // e.g.) file:///c:/work or file:///c:/work/
                    // e.g.) file:///c|/work or file:///c|/work/
                    // e.g.) file:///test.log

                    sPrefixLength = sUriLocalFileSchemeLength;
                }
                else
                {
                    xpr_size_t sSeparatorPosition = mFilePath.find(XPR_URI_SCHEME_SEPARATOR);
                    if (sSeparatorPosition != xpr::tstring::npos)
                    {
                        sPrefixLength = sSeparatorPosition + _tcslen(XPR_URI_SCHEME_SEPARATOR);
                    }
                }

                if (sLength2 > sPrefixLength)
                {
                    if (aFilePath[sLength2 - 1] == XPR_URI_SEPARATOR)
                    {
                        --sLength2;
                    }
                }

                break;
            }

        default:
            break;
        }
    }

    if (XPR_IS_TRUE(aCaseSensitivity))
    {
        return mFilePath.compare(0, sLength, aFilePath, 0, sLength2);
    }

    return mFilePath.compare_case(0, sLength, aFilePath, 0, sLength2);
}

xpr_sint_t FilePath::compare(const xpr_tchar_t *aFilePath) const
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    xpr_size_t sLength = _tcslen(aFilePath);

    return compareCase(aFilePath, sLength, XPR_FALSE);
}

xpr_sint_t FilePath::compare(const xpr::tstring &aFilePath) const
{
    const xpr_tchar_t *sFilePath = aFilePath.c_str();
    xpr_size_t         sLength   = aFilePath.length();

    return compareCase(sFilePath, sLength, XPR_FALSE);
}

xpr_sint_t FilePath::compare(const FilePath &aFilePath) const
{
    const xpr_tchar_t *sFilePath = aFilePath.mFilePath.c_str();
    xpr_size_t         sLength   = aFilePath.mFilePath.length();

    return compareCase(sFilePath, sLength, XPR_FALSE);
}

xpr_sint_t FilePath::compareCase(const xpr_tchar_t *aFilePath, xpr_bool_t aCaseSensitivity) const
{
    XPR_ASSERT(aFilePath != XPR_NULL);

    xpr_size_t sLength = _tcslen(aFilePath);

    return compareCase(aFilePath, sLength, aCaseSensitivity);
}

xpr_sint_t FilePath::compareCase(const xpr::tstring &aFilePath, xpr_bool_t aCaseSensitivity) const
{
    const xpr_tchar_t *sFilePath = aFilePath.c_str();
    xpr_size_t         sLength   = aFilePath.length();

    return compareCase(sFilePath, sLength, aCaseSensitivity);
}

xpr_sint_t FilePath::compareCase(const FilePath &aFilePath, xpr_bool_t aCaseSensitivity) const
{
    const xpr_tchar_t *sFilePath = aFilePath.mFilePath.c_str();
    xpr_size_t         sLength   = aFilePath.mFilePath.length();

    return compareCase(sFilePath, sLength, aCaseSensitivity);
}

xpr_bool_t FilePath::isAbsolute(void) const
{
    xpr_bool_t sAbsolutePath = XPR_FALSE;
    xpr_size_t sLength       = mFilePath.length();

    switch (mPathType)
    {
    case PathTypeNormal:
        if (sLength >= 2)
        {
            if (mFilePath[1] == XPR_DRIVE_SEPARATOR)
            {
                sAbsolutePath = XPR_TRUE;
                break;
            }
        }

    case PathTypeUnc:
        sAbsolutePath = XPR_TRUE;
        break;

    case PathTypeUri:
        {
            xpr_size_t sPrefixLength             = 0;
            xpr_size_t sUriLocalFileSchemeLength = _tcslen(kUriFileSchemeLocal);

            if (mFilePath.compare_case(0, sUriLocalFileSchemeLength, kUriFileSchemeLocal) == 0)
            {
                // e.g.) file:///c:/work or file:///c:/work/
                // e.g.) file:///c|/work or file:///c|/work/
                // e.g.) file:///test.log
                // e.g.) file:///work/docs/test.log

                sPrefixLength = sUriLocalFileSchemeLength;

                if (sLength >= (sPrefixLength + 2))
                {
                    if (mFilePath[sPrefixLength + 1] == XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR ||
                        mFilePath[sPrefixLength + 1] == XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR2)
                    {
                        sAbsolutePath = XPR_TRUE;
                    }
                }
            }
            else
            {
                sAbsolutePath = XPR_TRUE;
            }

            break;
        }

    default:
        sAbsolutePath = XPR_FALSE;
        break;
    }

    return sAbsolutePath;
}

xpr_bool_t FilePath::isNetwork(void) const
{
    xpr_bool_t sNetworkPath = XPR_FALSE;
    xpr_size_t sLength      = mFilePath.length();

    switch (mPathType)
    {
    case PathTypeUnc:
        // e.g.) \\?\UNC\c:
        sNetworkPath = XPR_TRUE;

        if (sLength >= 10 && mFilePath[9] == XPR_DRIVE_SEPARATOR)
        {
            sNetworkPath = XPR_FALSE;
        }

        break;

    case PathTypeUri:
        {
            xpr_size_t sUriLocalFileSchemeLength = _tcslen(kUriFileSchemeLocal);

            if (mFilePath.compare_case(0, sUriLocalFileSchemeLength, kUriFileSchemeLocal) != 0)
            {
                // e.g.) file://c: or file://c|
                sNetworkPath = XPR_TRUE;

                if (sLength >= 10 &&
                    (mFilePath[9] == XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR ||
                     mFilePath[9] == XPR_URI_FILE_SCHEME_DRIVE_SEPARATOR2))
                {
                    sNetworkPath = XPR_FALSE;
                }
            }
        }

        break;

    case PathTypeNormal:
    default:
        sNetworkPath = XPR_FALSE;
        break;
    }

    return sNetworkPath;
}

xpr_bool_t FilePath::isLong(void) const
{
    xpr_bool_t sLongPath = XPR_FALSE;

    switch (mPathType)
    {
    case PathTypeUnc:
        if (mFilePath.compare_case(0, _tcslen(kUncLong), kUncLong) == 0)
        {
            sLongPath = XPR_TRUE;
        }
        break;

    case PathTypeUri:
        sLongPath = XPR_TRUE;
        break;

    case PathTypeNormal:
    default:
        sLongPath = XPR_FALSE;
        break;
    }

    return sLongPath;
}

xpr_bool_t FilePath::toUri(xpr_tchar_t *aUri, xpr_size_t aMaxLen) const
{
    XPR_ASSERT(aUri != XPR_NULL);

    xpr::tstring sUri;
    toUri(sUri);

    if (sUri.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aUri, sUri.c_str());

    return XPR_TRUE;
}

xpr_bool_t FilePath::toUri(xpr::tstring &aUri) const
{
    switch (mPathType)
    {
    case PathTypeNormal:
        {
            // e.g.) c:\windows\system32\test.log -> file:///c:/windows/system32/test.log
            // e.g.) test.log                     -> file:///test.log

            aUri = kUriFileSchemeLocal;
            aUri += mFilePath;

            // '\\' -> '/'
            xpr_size_t sSeparatorPosition = _tcslen(kUriFileSchemeLocal);

            do
            {
                sSeparatorPosition = aUri.find(XPR_FILE_SEPARATOR, sSeparatorPosition);
                if (sSeparatorPosition != xpr::tstring::npos)
                {
                    aUri[sSeparatorPosition] = XPR_URI_SEPARATOR;
                    ++sSeparatorPosition;
                }
                else
                {
                    break;
                }
            } while (true);

            break;
        }

    case PathTypeUnc:
        {
            // e.g.) \\ServerName\ShareName\file                     -> file://ServerName/ShareName/file
            // e.g.) \\?\UNC\ServerName\ShareName\file (long UNC)    -> file://ServerName/ShareName/file
            // e.g.) \\?\UNC\c:\windows\system32\test.log (long UNC) -> file:///c:/windows/system32/test.log

            xpr_size_t sSubStrPosition = xpr::tstring::npos;
            xpr_size_t sUncLongLength  = _tcslen(kUncLong);

            if (mFilePath.compare_case(0, sUncLongLength, kUncLong) == 0)
            {
                sSubStrPosition = sUncLongLength;
            }
            else
            {
                sSubStrPosition = 2;
            }

            aUri  = kUriFileScheme;
            aUri += mFilePath.substr(sSubStrPosition);

            // '\\' -> '/'
            xpr_size_t sSeparatorPosition = 0;

            do
            {
                sSeparatorPosition = aUri.find(XPR_FILE_SEPARATOR, sSeparatorPosition);
                if (sSeparatorPosition != xpr::tstring::npos)
                {
                    aUri[sSeparatorPosition] = XPR_URI_SEPARATOR;
                    ++sSeparatorPosition;
                }
                else
                {
                    break;
                }
            } while (true);

            break;
        }

    case PathTypeUri:
        {
            aUri = mFilePath;
            break;
        }

    default:
        {
            aUri = kUriFileSchemeLocal;
            break;
        }
    }

    return XPR_TRUE;
}

xpr_bool_t FilePath::toUri(FilePath &aFilePath) const
{
    xpr::tstring sUri;

    if (XPR_IS_FALSE(toUri(sUri)))
    {
        return XPR_FALSE;
    }

    aFilePath = sUri;

    return XPR_TRUE;
}

xpr_bool_t FilePath::getInvalidChars(xpr_tchar_t *aInvalidChars, xpr_size_t aMaxLen)
{
    XPR_ASSERT(aInvalidChars != XPR_NULL);

    xpr::tstring sInvalidChars;
    getInvalidChars(sInvalidChars);

    if (sInvalidChars.length() > aMaxLen)
    {
        return XPR_FALSE;
    }

    _tcscpy(aInvalidChars, sInvalidChars.c_str());

    return XPR_TRUE;
}

void FilePath::getInvalidChars(xpr::tstring &aInvalidChars)
{
    aInvalidChars = XPR_STRING_LITERAL("<>:\"/\\|?*");
}

xpr_tchar_t FilePath::getFileSeparator(void)
{
    return XPR_FILE_SEPARATOR;
}

xpr_tchar_t FilePath::getPathSeparator(void)
{
    return XPR_PATH_SEPARATOR;
}
} // namespace xpr
