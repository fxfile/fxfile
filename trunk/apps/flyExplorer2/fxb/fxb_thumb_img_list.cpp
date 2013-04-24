//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_thumb_img_list.h"

#include "../ConfDir.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
ThumbImgList::ThumbImgList(void)
{
}

ThumbImgList::~ThumbImgList(void)
{
    removeAll();
}

xpr_bool_t ThumbImgList::create(xpr_sint_t cx, xpr_sint_t cy, xpr_uint_t aFlags)
{
    return mImageList.Create(cx, cy, aFlags, 100, 50);
}

xpr_sint_t ThumbImgList::add(ThumbImage *aThumbImage, xpr_uint_t aThumbImageId)
{
    if (XPR_IS_NULL(aThumbImage))
        return -1;

    if (XPR_IS_NULL(aThumbImage->mImage) || aThumbImage->mPath.empty() == true || aThumbImage->mWidth <= 0 || aThumbImage->mHeight <= 0 || aThumbImage->mDepth <= 0)
        return -1;

    xpr_sint_t sImageIndex = mImageList.Add(aThumbImage->mImage, CLR_NONE);

    ThumbElement sThumbElement;
    sThumbElement.mPath             = aThumbImage->mPath;
    sThumbElement.mImageIndex       = sImageIndex;
    sThumbElement.mThumbImageId     = aThumbImageId;
    sThumbElement.mWidth            = aThumbImage->mWidth;
    sThumbElement.mHeight           = aThumbImage->mHeight;
    sThumbElement.mDepth            = aThumbImage->mDepth;
    sThumbElement.mModifiedFileTime = aThumbImage->mModifiedFileTime;

    mThumbDeque.push_back(sThumbElement);

    return sImageIndex;
}

xpr_sint_t ThumbImgList::getImageCount(void) const
{
    return mImageList.GetImageCount();
}

ThumbImgList::ThumbElement *ThumbImgList::getImage(xpr_sint_t aImageIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aImageIndex, mThumbDeque))
        return XPR_NULL;

    return &mThumbDeque[aImageIndex];
}

ThumbImgList::ThumbElement *ThumbImgList::getImage(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_NULL;

    ThumbDeque::iterator sIterator;

    sIterator = mThumbDeque.begin();
    for (; sIterator != mThumbDeque.end(); ++sIterator)
    {
        ThumbElement &sThumbElement = *sIterator;

        if (_tcsicmp(sThumbElement.mPath.c_str(), aPath) == 0)
            return &sThumbElement;
    }

    return XPR_NULL;
}

// return value
// (-1) Not Exist
// ( 0) Not Modified
// ( 1) Modified
xpr_sint_t ThumbImgList::compareTime(xpr_sint_t nImage, LPFILETIME aModifiedFileTime)
{
    if (XPR_IS_NULL(aModifiedFileTime))
        return -1;

    xpr_sint_t sResult = -1;

    ThumbDeque::iterator sIterator;

    sIterator = mThumbDeque.begin();
    for (; sIterator != mThumbDeque.end(); ++sIterator)
    {
        ThumbElement &sThumbElement = *sIterator;

        if (sThumbElement.mImageIndex == nImage)
        {
            sResult = CompareFileTime(&sThumbElement.mModifiedFileTime, aModifiedFileTime) ? 1 : 0;
            break;
        }
    }

    return sResult;
}

CImageList *ThumbImgList::getImageList(void)
{
    return &mImageList;
}

xpr_bool_t ThumbImgList::draw(CDC *aDC, xpr_sint_t aImageIndex, POINT aPoint, xpr_uint_t aStyle)
{
    return mImageList.Draw(aDC, aImageIndex, aPoint, aStyle);
}

// if image remove in image list, then mImageIndex shoud change.
xpr_bool_t ThumbImgList::remove(xpr_sint_t aImageIndex)
{
    ThumbDeque::iterator sIterator;

    sIterator = mThumbDeque.begin();
    while (sIterator != mThumbDeque.end())
    {
        ThumbElement &sThumbElement = *sIterator;

        if (sThumbElement.mImageIndex == aImageIndex)
        {
            sIterator = mThumbDeque.erase(sIterator);
            break;
        }

        sIterator++;
    }

    sIterator = mThumbDeque.begin();
    for (; sIterator != mThumbDeque.end(); ++sIterator)
    {
        ThumbElement &sThumbElement = *sIterator;

        if (sThumbElement.mImageIndex > aImageIndex)
            sThumbElement.mImageIndex--;
    }

    return mImageList.Remove(aImageIndex);
}

void ThumbImgList::removeAll(void)
{
    if (XPR_IS_NOT_NULL(mImageList.m_hImageList))
    {
        xpr_sint_t i, sCount;

        sCount = mImageList.GetImageCount();
        if (sCount > 0)
        {
            for (i = sCount - 1; i >= 0; --i)
                mImageList.Remove(i);
        }
    }

    mThumbDeque.clear();
}

void ThumbImgList::save(void)
{
    if (mImageList.GetImageCount() > 0)
    {
        xpr_tchar_t sDataPath[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sIndexPath[XPR_MAX_PATH + 1] = {0};
        ConfDir::instance().getSavePath(ConfDir::TypeThumbnail, sDataPath,  XPR_MAX_PATH, XPR_STRING_LITERAL("dat"));
        ConfDir::instance().getSavePath(ConfDir::TypeThumbnail, sIndexPath, XPR_MAX_PATH, XPR_STRING_LITERAL("idx"));

        // Save Data
        {
            CFile sFile(sDataPath,  CFile::modeCreate | CFile::modeWrite);
            CArchive sArchive(&sFile, CArchive::store);
            mImageList.Write(&sArchive);
        }

        // Save Index
        {
            xpr_rcode_t sRcode;
            xpr_ssize_t sWritten;
            xpr_sint_t sOpenMode;
            xpr::FileIo sFileIo;

            sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
            sRcode = sFileIo.open(sIndexPath, sOpenMode);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                xpr_sint_t sSize;
                ThumbDeque::iterator sIterator;
                xpr_wchar_t sPathW[XPR_MAX_PATH + 1] = {0};
                xpr_size_t sInputBytes;
                xpr_size_t sOutputBytes;

                sIterator = mThumbDeque.begin();
                for (; sIterator != mThumbDeque.end(); ++sIterator)
                {
                    ThumbElement &sThumbElement = *sIterator;

                    sInputBytes = sThumbElement.mPath.length() * sizeof(xpr_tchar_t);
                    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
                    XPR_TCS_TO_UTF16(sThumbElement.mPath.c_str(), &sInputBytes, sPathW, &sOutputBytes);
                    sPathW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

                    sSize = (xpr_sint_t)((sThumbElement.mPath.length() + 1) * sizeof(xpr_wchar_t));

                    sRcode = sFileIo.write(&sSize,                           sizeof(xpr_sint_t), &sWritten);
                    sRcode = sFileIo.write(sPathW,                           sSize,              &sWritten);
                    sRcode = sFileIo.write(&sThumbElement.mImageIndex,       sizeof(xpr_sint_t), &sWritten);
                    sRcode = sFileIo.write(&sThumbElement.mWidth,            sizeof(xpr_sint_t), &sWritten);
                    sRcode = sFileIo.write(&sThumbElement.mHeight,           sizeof(xpr_sint_t), &sWritten);
                    sRcode = sFileIo.write(&sThumbElement.mDepth,            sizeof(xpr_sint_t), &sWritten);
                    sRcode = sFileIo.write(&sThumbElement.mModifiedFileTime, sizeof(FILETIME),   &sWritten);
                }
            }
        }
    }
}

xpr_bool_t ThumbImgList::load(void)
{
    xpr_tchar_t sDataPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sIndexPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getLoadPath(ConfDir::TypeThumbnail, sDataPath,  XPR_MAX_PATH, XPR_STRING_LITERAL("dat"));
    ConfDir::instance().getLoadPath(ConfDir::TypeThumbnail, sIndexPath, XPR_MAX_PATH, XPR_STRING_LITERAL("idx"));

    if (IsExistFile(sDataPath) == XPR_FALSE || IsExistFile(sIndexPath) == XPR_FALSE)
        return XPR_FALSE;

    // Load Data
    {
        CFile sFile(sDataPath,  CFile::modeRead);
        CArchive sArchive(&sFile, CArchive::load);
        mImageList.Read(&sArchive);
    }

    // Load Index
    {
        xpr_sint_t sSize;
        ThumbElement sThumbElement;
        xpr_wchar_t sPathW[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        xpr_size_t sInputBytes;
        xpr_size_t sOutputBytes;

        xpr_rcode_t sRcode;
        xpr_ssize_t sRead;
        xpr::FileIo sFileIo;

        sRcode = sFileIo.open(sIndexPath, xpr::FileIo::OpenModeReadOnly);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            do
            {
                sRcode = sFileIo.read(&sSize, sizeof(xpr_sint_t), &sRead);
                if (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0)
                {
                    sRcode = sFileIo.read(sPathW,                           sSize,              &sRead);
                    sRcode = sFileIo.read(&sThumbElement.mImageIndex,       sizeof(xpr_sint_t), &sRead);
                    sRcode = sFileIo.read(&sThumbElement.mWidth,            sizeof(xpr_sint_t), &sRead);
                    sRcode = sFileIo.read(&sThumbElement.mHeight,           sizeof(xpr_sint_t), &sRead);
                    sRcode = sFileIo.read(&sThumbElement.mDepth,            sizeof(xpr_sint_t), &sRead);
                    sRcode = sFileIo.read(&sThumbElement.mModifiedFileTime, sizeof(FILETIME),   &sRead);

                    sInputBytes = sSize;
                    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                    XPR_UTF16_TO_TCS(sPathW, &sInputBytes, sPath, &sOutputBytes);
                    sPath[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                    sThumbElement.mPath         = sPath;
                    sThumbElement.mThumbImageId = (xpr_uint_t)sThumbElement.mImageIndex;

                    mThumbDeque.push_back(sThumbElement);
                }
            } while (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0);
        }
    }

    return XPR_TRUE;
}

void ThumbImgList::verify(void)
{
    xpr_sint_t i;
    ThumbDeque::iterator sIterator;
    ThumbDeque::iterator sIterator2;

    i = 0;
    sIterator = mThumbDeque.begin();
    while (sIterator != mThumbDeque.end())
    {
        ThumbElement &sThumbElement = *sIterator;

        if (IsExistFile(sThumbElement.mPath.c_str()) == XPR_FALSE)
        {
            sIterator = mThumbDeque.erase(sIterator);

            sIterator2 = mThumbDeque.begin();
            for (; sIterator2 != mThumbDeque.end(); ++sIterator2)
            {
                ThumbElement &sThumbElement2 = *sIterator2;

                if (sThumbElement2.mImageIndex > i)
                    sThumbElement2.mImageIndex--;
            }

            mImageList.Remove(i);

            continue;
        }

        sIterator++;
        i++;
    }
}

xpr_bool_t ThumbImgList::clear(void)
{
    mThumbDeque.clear();

    return mImageList.DeleteImageList();
}
} // namespace fxb
