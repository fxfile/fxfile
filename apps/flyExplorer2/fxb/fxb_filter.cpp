//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_filter.h"

#include "fxb_ini_file_ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
const xpr_tchar_t *Filter::mFolderString          = XPR_NULL;
const xpr_tchar_t *Filter::mGeneralFileString     = XPR_NULL;
const xpr_tchar_t *Filter::mExecutableFileString  = XPR_NULL;
const xpr_tchar_t *Filter::mCompressedFileString  = XPR_NULL;
const xpr_tchar_t *Filter::mDocumentFileString    = XPR_NULL;
const xpr_tchar_t *Filter::mImageFileString       = XPR_NULL;
const xpr_tchar_t *Filter::mSoundFileString       = XPR_NULL;
const xpr_tchar_t *Filter::mMovieFileString       = XPR_NULL;
const xpr_tchar_t *Filter::mWebFileString         = XPR_NULL;
const xpr_tchar_t *Filter::mProgrammingFileString = XPR_NULL;
const xpr_tchar_t *Filter::mTemporaryFileString   = XPR_NULL;

static const xpr_tchar_t kFilterSection[] = XPR_STRING_LITERAL("filter");
static const xpr_tchar_t kNameKey      [] = XPR_STRING_LITERAL("filter.item%d_name");
static const xpr_tchar_t kExtensionKey [] = XPR_STRING_LITERAL("filter.item%d_extension");
static const xpr_tchar_t kColorKey     [] = XPR_STRING_LITERAL("filter.item%d_color");
static const xpr_tchar_t kIconIndexKey [] = XPR_STRING_LITERAL("filter.item%d_icon_index");

Filter::Filter(void)
{
}

Filter::~Filter(void)
{
    clear();
}

void Filter::addFilter(FilterItem *aFilterItem)
{
    if (XPR_IS_NOT_NULL(aFilterItem))
        mFilterDeque.push_back(aFilterItem);
}

xpr_sint_t Filter::getCount(void)
{
    return (xpr_sint_t)mFilterDeque.size();
}

FilterItem *Filter::getFilter(xpr_sint_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mFilterDeque))
        return XPR_NULL;

    return mFilterDeque[aIndex];
}

FilterItem *Filter::getFilterFromName(const xpr_tchar_t *aName)
{
    if (XPR_IS_NULL(aName))
        return XPR_NULL;

    FilterItem *sFilterItem;
    FilterDeque::iterator sIterator;

    sIterator = mFilterDeque.begin();
    for (; sIterator != mFilterDeque.end(); ++sIterator)
    {
        sFilterItem = *sIterator;
        if (XPR_IS_NULL(sFilterItem))
            continue;

        if (!_tcsicmp(sFilterItem->mName.c_str(), aName))
            return sFilterItem;
    }

    return XPR_NULL;
}

FilterItem *Filter::getFilterFromName(const std::tstring &aName)
{
    return getFilterFromName(aName.c_str());
}

xpr_sint_t Filter::getIndex(const xpr_tchar_t *aPath, xpr_bool_t aFolder)
{
    if (XPR_IS_NULL(aPath))
        return 0;

    FilterItem *sFilterItem;
    FilterDeque::iterator sIterator;

    if (XPR_IS_TRUE(aFolder))
    {
        sIterator = mFilterDeque.begin();
        for (; sIterator != mFilterDeque.end(); ++sIterator)
        {
            sFilterItem = *sIterator;
            if (XPR_IS_NULL(sFilterItem))
                continue;

            if (_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*")) == 0)
                return (xpr_sint_t)std::distance(mFilterDeque.begin(), sIterator);
        }

        return -1;
    }

    const xpr_tchar_t *sExt = GetFileExt(aPath);
    if (XPR_IS_NOT_NULL(sExt))
        sExt++;

    if (XPR_IS_NOT_NULL(sExt))
    {
        sIterator = mFilterDeque.begin();
        for (; sIterator != mFilterDeque.end(); ++sIterator)
        {
            sFilterItem = *sIterator;
            if (XPR_IS_NULL(sFilterItem))
                continue;

            if (_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*")) == 0 ||
                _tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*.*")) == 0)
                continue;

            if (IsEqualFilter(sFilterItem->mExts.c_str(), sExt))
                return (xpr_sint_t)std::distance(mFilterDeque.begin(), sIterator);
        }
    }

    sIterator = mFilterDeque.begin();
    for (; sIterator != mFilterDeque.end(); ++sIterator)
    {
        sFilterItem = *sIterator;
        if (XPR_IS_NULL(sFilterItem))
            continue;

        if (_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*")) == 0 ||
            _tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*.*")) != 0)
            continue;

        if (IsEqualFilter(sFilterItem->mExts.c_str(), sExt))
            return (xpr_sint_t)std::distance(mFilterDeque.begin(), sIterator);
    }

    return -1;
}

COLORREF Filter::getColor(const xpr_tchar_t *aPath, xpr_bool_t aFolder)
{
    xpr_sint_t sIndex = getIndex(aPath, aFolder);

    FilterItem *sFilterItem = getFilter(sIndex);
    if (XPR_IS_NULL(sFilterItem))
        return RGB(0,0,0);

    return sFilterItem->mColor;
}

xpr_sint_t Filter::getIconIndex(const xpr_tchar_t *aPath, xpr_bool_t aFolder)
{
    xpr_sint_t sIndex = getIndex(aPath, aFolder);

    FilterItem *sFilterItem = getFilter(sIndex);
    if (XPR_IS_NULL(sFilterItem))
        return -1;

    return sFilterItem->mIconIndex;
}

xpr_bool_t Filter::initDefault(void)
{
    // Default Filtering Type based on Extension
    // +------+--------------+---------------+-------+
    // | type | Name         | Color         | note  |
    // +------+--------------+---------------+-------+
    // |   0  | Folder       | (255,  0,  0) | fixed |
    // |   1  | General      | (  0,  0,  0) | fixed |
    // |   2  | Executable   | (127,  0,255) |       |
    // |   3  | Archieve     | (255,  0,255) |       |
    // |   4  | Document     | (  0,128,128) |       |
    // |   5  | Picture      | ( 35,142, 35) |       |
    // |   6  | Music        | (166,128,100) |       |
    // |   7  | Movie        | (228,120, 51) |       |
    // |   8  | Web          | (119,174,113) |       |
    // |   9  | Programming  | ( 77, 77,255) |       |
    // |  10  | Temp         | (102,102,102) |       |
    // +------+--------------+---------------+-------+

    clear();

    FilterItem *sFilterItem;

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mFolderString) ? mFolderString : XPR_STRING_LITERAL("Folder");
    sFilterItem->mExts      = XPR_STRING_LITERAL("*");
    sFilterItem->mIconIndex = 6;
    sFilterItem->mColor     = RGB(255,0,0);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mGeneralFileString) ? mGeneralFileString : XPR_STRING_LITERAL("General File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("*.*");
    sFilterItem->mIconIndex = 7;
    sFilterItem->mColor     = RGB(0,0,0);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mExecutableFileString) ? mExecutableFileString : XPR_STRING_LITERAL("Executable File");;
    sFilterItem->mExts      = XPR_STRING_LITERAL("exe;com;bat");
    sFilterItem->mIconIndex = 8;
    sFilterItem->mColor     = RGB(127,0,255);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mCompressedFileString) ? mCompressedFileString : XPR_STRING_LITERAL("Compressed File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("zip;rar;alz;ace;cab;gz;jar;tgz;z;arj;lzh;7z;tar;bz2;tgz");
    sFilterItem->mIconIndex = 9;
    sFilterItem->mColor     = RGB(255,0,255);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mDocumentFileString) ? mDocumentFileString : XPR_STRING_LITERAL("Document File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("txt;rtf;hwp;doc;docx;pdf;ppt;pptx;xls;xlsx;cap;gal;hlp;chm;ini;xml;xsd;vsd");
    sFilterItem->mIconIndex = 10;
    sFilterItem->mColor     = RGB(0,128,128);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mImageFileString) ? mImageFileString : XPR_STRING_LITERAL("Image File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("bmp;jpg;jpeg;gif;tif;tga;psd;ico;wmf;png;pcx;jp2");
    sFilterItem->mIconIndex = 11;
    sFilterItem->mColor     = RGB(35,142,35);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mSoundFileString) ? mSoundFileString : XPR_STRING_LITERAL("Sound File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("wav;mp3;ogg;mp2;mid;mp4;flac");
    sFilterItem->mIconIndex = 12;
    sFilterItem->mColor     = RGB(166,128,100);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mMovieFileString) ? mMovieFileString : XPR_STRING_LITERAL("Movie File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("avi;mpg;mpeg;mov;qt;asf;asx;rm;wmv;divx");
    sFilterItem->mIconIndex = 13;
    sFilterItem->mColor     = RGB(228,120,51);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mWebFileString) ? mWebFileString : XPR_STRING_LITERAL("Web File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("php;php3;asp;aspx;cgi;htm;html;shtm;shtml;js;jsp");
    sFilterItem->mIconIndex = 14;
    sFilterItem->mColor     = RGB(119,174,113);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mProgrammingFileString) ? mProgrammingFileString : XPR_STRING_LITERAL("Programming File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("c;cpp;cc;cxx;h;hpp;hh;hxx;sln;vcproj;dsw;dsp;pas;bas;bpl;cs;py;java;pl");
    sFilterItem->mIconIndex = 15;
    sFilterItem->mColor     = RGB(77,77,255);
    addFilter(sFilterItem);

    sFilterItem = new FilterItem;
    sFilterItem->mName      = XPR_IS_NOT_NULL(mTemporaryFileString) ? mTemporaryFileString : XPR_STRING_LITERAL("Temporary File");
    sFilterItem->mExts      = XPR_STRING_LITERAL("bak;tmp;~dfm;~pas;~dsk");
    sFilterItem->mIconIndex = 16;
    sFilterItem->mColor     = RGB(102,102,102);
    addFilter(sFilterItem);

    return XPR_TRUE;
}

void Filter::clear(void)
{
    FilterItem *sFilterItem;
    FilterDeque::iterator sIterator;

    sIterator = mFilterDeque.begin();
    for (; sIterator != mFilterDeque.end(); ++sIterator)
    {
        sFilterItem = *sIterator;
        XPR_SAFE_DELETE(sFilterItem);
    }

    mFilterDeque.clear();
}

xpr_bool_t Filter::load(fxb::IniFileEx &aIniFile)
{
    xpr_size_t         i;
    xpr_tchar_t        sKey[0xff];
    const xpr_tchar_t *sValue;
    FilterItem        *sFilterItem;
    IniFile::Section  *sSection;

    sSection = aIniFile.findSection(kFilterSection);
    if (XPR_IS_NULL(sSection))
        return XPR_FALSE;

    for (i = 0; i < MAX_FILTER; ++i)
    {
        _stprintf(sKey, kNameKey, i + 1);

        sValue = aIniFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            break;

        sFilterItem = new FilterItem;
        XPR_ASSERT(sFilterItem != XPR_NULL);

        sFilterItem->mName = sValue;

        _stprintf(sKey, kExtensionKey, i + 1);
        sFilterItem->mExts = aIniFile.getValueS(sSection, sKey, XPR_STRING_LITERAL(""));

        _stprintf(sKey, kColorKey, i + 1);
        sFilterItem->mColor = aIniFile.getValueC(sSection, sKey, RGB(0,0,0));

        _stprintf(sKey, kIconIndexKey, i + 1);
        sFilterItem->mIconIndex = aIniFile.getValueI(sSection, sKey, -1);

        addFilter(sFilterItem);
    }

    return XPR_TRUE;
}

void Filter::save(fxb::IniFileEx &aIniFile) const
{
    xpr_sint_t        i;
    xpr_tchar_t       sKey[0xff];
    IniFile::Section *sSection;
    FilterItem       *sFilterItem;
    FilterDeque::const_iterator sIterator;

    sSection = aIniFile.addSection(kFilterSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIterator = mFilterDeque.begin();
    for (i = 0; sIterator != mFilterDeque.end(); ++i, ++sIterator)
    {
        sFilterItem = *sIterator;
        XPR_ASSERT(sFilterItem);

        _stprintf(sKey, kNameKey, i + 1);
        aIniFile.setValueS(sSection, sKey, sFilterItem->mName);

        _stprintf(sKey, kExtensionKey, i + 1);
        aIniFile.setValueS(sSection, sKey, sFilterItem->mExts);

        _stprintf(sKey, kColorKey, i + 1);
        aIniFile.setValueC(sSection, sKey, sFilterItem->mColor);

        _stprintf(sKey, kIconIndexKey, i + 1);
        aIniFile.setValueI(sSection, sKey, sFilterItem->mIconIndex);
    }
}

void Filter::setString(const xpr_tchar_t *aFolderString,
                       const xpr_tchar_t *aGeneralFileString,
                       const xpr_tchar_t *aExecutableFileString,
                       const xpr_tchar_t *aCompressedFileString,
                       const xpr_tchar_t *aDocumentFileString,
                       const xpr_tchar_t *aImageFileString,
                       const xpr_tchar_t *aSoundFileString,
                       const xpr_tchar_t *aMovieFileString,
                       const xpr_tchar_t *aWebFileString,
                       const xpr_tchar_t *aProgrammingFileString,
                       const xpr_tchar_t *aTemporaryFileString)
{
    mFolderString          = aFolderString;
    mGeneralFileString     = aGeneralFileString;
    mExecutableFileString  = aExecutableFileString;
    mCompressedFileString  = aCompressedFileString;
    mDocumentFileString    = aDocumentFileString;
    mImageFileString       = aImageFileString;
    mSoundFileString       = aSoundFileString;
    mMovieFileString       = aMovieFileString;
    mWebFileString         = aWebFileString;
    mProgrammingFileString = aProgrammingFileString;
    mTemporaryFileString   = aTemporaryFileString;
}

FilterMgr::FilterMgr(void)
{
}

FilterMgr::~FilterMgr(void)
{
}
} // namespace fxb
