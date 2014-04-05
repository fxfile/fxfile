//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format.h"
#include "format_factory.h"
#include "singleton_manager.h"
#include "xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
namespace
{
const xpr_tchar_t kXmlFileNameFormatElement[] = XPR_STRING_LITERAL("FileNameFormat");
const xpr_tchar_t kXmlFormatSequenceElement[] = XPR_STRING_LITERAL("FormatSequence");
} // namespace anonymous

FormatSequence::FormatSequence(void)
{
}

FormatSequence::~FormatSequence(void)
{
    clear();
}

void FormatSequence::add(Format *aFormat)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    mFormatDeque.push_back(aFormat);
}

void FormatSequence::addFront(Format *aFormat)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    mFormatDeque.push_front(aFormat);
}

const Format *FormatSequence::get(xpr_size_t aIndex) const
{
    if (FXFILE_STL_IS_INDEXABLE(aIndex, mFormatDeque) == XPR_FALSE)
    {
        return XPR_NULL;
    }

    return mFormatDeque[aIndex];
}

Format *FormatSequence::get(xpr_size_t aIndex)
{
    if (FXFILE_STL_IS_INDEXABLE(aIndex, mFormatDeque) == XPR_FALSE)
    {
        return XPR_NULL;
    }

    return mFormatDeque[aIndex];
}

xpr_bool_t FormatSequence::isEmpty(void) const
{
    return (mFormatDeque.empty() == true) ? XPR_TRUE : XPR_FALSE;
}

xpr_size_t FormatSequence::getCount(void) const
{
    return mFormatDeque.size();
}

void FormatSequence::clear(void)
{
    Format *sFormat;
    FormatDeque::iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mFormatDeque)
    {
        sFormat = *sIterator;

        XPR_ASSERT(sFormat != XPR_NULL);

        XPR_SAFE_DELETE(sFormat);
    }

    mFormatDeque.clear();
}

void FormatSequence::rename(RenameContext &aContext) const
{
    const Format *sFormat;
    FormatDeque::const_iterator sIterator;

    aContext.mNewFileName = aContext.mOldFileName;

    FXFILE_STL_FOR_EACH(sIterator, mFormatDeque)
    {
        sFormat = *sIterator;

        XPR_ASSERT(sFormat != XPR_NULL);

        sFormat->rename(aContext);
    }
}

void FormatSequence::clone(FormatSequence &aFormatSequence) const
{
    aFormatSequence.clear();

    Format *sFormat;
    Format *sNewFormat;
    FormatDeque::const_iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mFormatDeque)
    {
        sFormat = *sIterator;

        XPR_ASSERT(sFormat != XPR_NULL);

        sNewFormat = sFormat->newClone();

        aFormatSequence.add(sNewFormat);
    }
}

FormatSequence *FormatSequence::newClone(void) const
{
    FormatSequence *sNewFormatSequence = new FormatSequence;

    clone(*sNewFormatSequence);

    return sNewFormatSequence;
}

xpr_bool_t FormatSequence::read(const base::XmlReader &aXmlReader, base::XmlReader::Element *aElement)
{
    XPR_ASSERT(aElement != XPR_NULL);

    xpr::tstring   sName;
    Format        *sFormat;
    FormatFactory &sFormatFactory = SingletonManager::get<FormatFactory>();

    base::XmlReader::Element *sFormatElement = aXmlReader.childElement(aElement);
    while (XPR_IS_NOT_NULL(sFormatElement))
    {
        if (XPR_IS_FALSE(aXmlReader.getElement(sFormatElement, sName)))
        {
            break;
        }

        if (sFormatFactory.parseFromXml(aXmlReader, sFormatElement, sFormat) == XPR_TRUE)
        {
            add(sFormat);
        }

        sFormatElement = aXmlReader.nextElement(sFormatElement);
    }

    return XPR_TRUE;
}

xpr_bool_t FormatSequence::write(base::XmlWriter &aXmlWriter) const
{
    xpr_bool_t sResult = XPR_FALSE;

    if (aXmlWriter.beginElement(kXmlFormatSequenceElement) == XPR_TRUE)
    {
        Format *sFormat;
        FormatDeque::const_iterator sIterator;

        FXFILE_STL_FOR_EACH(sIterator, mFormatDeque)
        {
            sFormat = *sIterator;

            XPR_ASSERT(sFormat != XPR_NULL);

            sFormat->write(aXmlWriter);
        }

        aXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    return sResult;
}

const xpr_tchar_t *FileNameFormat::mExcessMaxLengthString = XPR_NULL;
const xpr_tchar_t *FileNameFormat::mMaxPathLengthString   = XPR_NULL;
const xpr_tchar_t *FileNameFormat::mInvalidFileNameString = XPR_NULL;

FileNameFormat::FileNameFormat(void)
{
}

FileNameFormat::~FileNameFormat(void)
{
    clear();
}

void FileNameFormat::add(FormatSequence *aFormatSequence)
{
    XPR_ASSERT(aFormatSequence != XPR_NULL);

    mFormatSequenceDeque.push_back(aFormatSequence);
}

const FormatSequence *FileNameFormat::get(xpr_size_t aIndex) const
{
    if (FXFILE_STL_IS_INDEXABLE(aIndex, mFormatSequenceDeque) == XPR_FALSE)
    {
        return XPR_NULL;
    }

    return mFormatSequenceDeque[aIndex];
}

FormatSequence *FileNameFormat::get(xpr_size_t aIndex)
{
    if (FXFILE_STL_IS_INDEXABLE(aIndex, mFormatSequenceDeque) == XPR_FALSE)
    {
        return XPR_NULL;
    }

    return mFormatSequenceDeque[aIndex];
}

xpr_bool_t FileNameFormat::isEmpty(void) const
{
    return (mFormatSequenceDeque.empty() == true) ? XPR_TRUE : XPR_FALSE;
}

xpr_size_t FileNameFormat::getCount(void) const
{
    return mFormatSequenceDeque.size();
}

FormatSequence *FileNameFormat::remove(xpr_size_t aIndex)
{
    if (FXFILE_STL_IS_INDEXABLE(aIndex, mFormatSequenceDeque) == XPR_FALSE)
    {
        return XPR_NULL;
    }

    FormatSequence *sFormatSequence = mFormatSequenceDeque[aIndex];

    mFormatSequenceDeque.erase(mFormatSequenceDeque.begin() + aIndex);

    return sFormatSequence;
}

void FileNameFormat::clear(void)
{
    FormatSequence *sFormatSequence;
    FormatSequenceDeque::iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mFormatSequenceDeque)
    {
        sFormatSequence = *sIterator;

        XPR_ASSERT(sFormatSequence != XPR_NULL);

        XPR_SAFE_DELETE(sFormatSequence);
    }

    mFormatSequenceDeque.clear();
}

xpr_bool_t FileNameFormat::rename(RenameContext &aContext) const
{
    xpr_bool_t sSuccess = XPR_TRUE;
    FormatSequence *sFormatSequence;
    FormatSequenceDeque::const_iterator sIterator;

    //
    // set default new filename
    //
    aContext.mNewFileName = aContext.mOldFileName;

    //
    // rename
    //
    FXFILE_STL_FOR_EACH(sIterator, mFormatSequenceDeque)
    {
        sFormatSequence = *sIterator;

        XPR_ASSERT(sFormatSequence != XPR_NULL);

        if (aContext.mNewFileName.empty() == XPR_FALSE)
        {
            aContext.mOldFileName = aContext.mNewFileName;
            aContext.mNewFileName.clear();
        }

        sFormatSequence->rename(aContext);
    }

    //
    // verify file name
    //
    if (aContext.mNewFileName.find_first_of(kInvalidCharsInFilePath) != xpr::tstring::npos)
    {
        if (XPR_IS_NOT_NULL(mInvalidFileNameString))
        {
            aContext.mNewFileName.format(XPR_STRING_LITERAL("<error> - %s(\\ / : * ? \" < > |)"), mInvalidFileNameString);
        }
        else
        {
            aContext.mNewFileName.format(XPR_STRING_LITERAL("<error> - (\\ / : * ? \" < > |)"));
        }

        sSuccess = XPR_FALSE;
    }
    else if (aContext.mNewFileName.length() > aContext.mMaxLen)
    {
        if (XPR_IS_NOT_NULL(mExcessMaxLengthString) && XPR_IS_NOT_NULL(mMaxPathLengthString))
        {
            aContext.mNewFileName.format(XPR_STRING_LITERAL("<error> - %s (%s: %d)"), mExcessMaxLengthString, mMaxPathLengthString, aContext.mMaxLen);
        }
        else
        {
            aContext.mNewFileName.format(XPR_STRING_LITERAL("<error> - (length limited: %d)"), mExcessMaxLengthString, mMaxPathLengthString, aContext.mMaxLen);
        }

        sSuccess = XPR_FALSE;
    }

    return sSuccess;
}

void FileNameFormat::clone(FileNameFormat &aFileNameFormat) const
{
    aFileNameFormat.clear();

    FormatSequence *sFormatSequence;
    FormatSequence *sNewFormatSequence;
    FormatSequenceDeque::const_iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mFormatSequenceDeque)
    {
        sFormatSequence = *sIterator;

        XPR_ASSERT(sFormatSequence != XPR_NULL);

        sNewFormatSequence = sFormatSequence->newClone();

        aFileNameFormat.add(sNewFormatSequence);
    }
}

FileNameFormat *FileNameFormat::newClone(void) const
{
    FileNameFormat *sNewFileNameFormat = new FileNameFormat;

    clone(*sNewFileNameFormat);

    return sNewFileNameFormat;
}

xpr_bool_t FileNameFormat::load(const xpr::tstring &aFilePath)
{
    xpr_bool_t       sResult = XPR_FALSE;
    FormatSequence  *sFormatSequence;
    base::XmlReader  sXmlReader;

    sResult = sXmlReader.load(aFilePath);
    if (XPR_IS_TRUE(sResult))
    {
        base::XmlReader::Element *sFileNameFormatElement = sXmlReader.getRootElement();
        if (XPR_IS_NOT_NULL(sFileNameFormatElement))
        {
            xpr::tstring sName;

            if (XPR_IS_TRUE(sXmlReader.getElement(sFileNameFormatElement, sName)) && sName.compare(kXmlFileNameFormatElement) == 0)
            {
                base::XmlReader::Element *sFormatSequenceElement = sXmlReader.childElement(sFileNameFormatElement);
                while (XPR_IS_NOT_NULL(sFormatSequenceElement))
                {
                    if (XPR_IS_FALSE(sXmlReader.getElement(sFormatSequenceElement, sName)))
                    {
                        break;
                    }

                    if (sName.compare(kXmlFormatSequenceElement) != 0)
                    {
                        break;
                    }

                    sFormatSequence = new FormatSequence;

                    if (sFormatSequence->read(sXmlReader, sFormatSequenceElement) == XPR_TRUE)
                    {
                        add(sFormatSequence);
                    }
                    else
                    {
                        XPR_SAFE_DELETE(sFormatSequence);
                    }

                    sFormatSequenceElement = sXmlReader.nextElement(sFormatSequenceElement);
                }
            }
        }
    }

    return sResult;
}

xpr_bool_t FileNameFormat::save(const xpr::tstring &aFilePath) const
{
    xpr_bool_t      sResult = XPR_FALSE;
    base::XmlWriter sXmlWriter;

    sXmlWriter.beginDocument();

    if (sXmlWriter.beginElement(kXmlFileNameFormatElement) == XPR_TRUE)
    {
        FormatSequence *sFormatSequence;
        FormatSequenceDeque::const_iterator sFormatIterator;

        FXFILE_STL_FOR_EACH(sFormatIterator, mFormatSequenceDeque)
        {
            sFormatSequence = *sFormatIterator;

            XPR_ASSERT(sFormatSequence != XPR_NULL);

            sFormatSequence->write(sXmlWriter);
        }

        sXmlWriter.endElement();

        sResult = XPR_TRUE;
    }

    sXmlWriter.endDocument();

    if (XPR_IS_TRUE(sResult))
    {
        sResult = sXmlWriter.save(aFilePath);
    }

    return sResult;
}

void FileNameFormat::setString(const xpr_tchar_t *aExcessMaxLengthString,
                               const xpr_tchar_t *aMaxPathLengthString,
                               const xpr_tchar_t *aInvalidFileNameString)
{
    mExcessMaxLengthString = aExcessMaxLengthString;
    mMaxPathLengthString   = aMaxPathLengthString;
    mInvalidFileNameString = aInvalidFileNameString;
}
} // namespace cmd
} // namespace fxfile
