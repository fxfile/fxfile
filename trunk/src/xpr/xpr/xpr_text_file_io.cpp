//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_text_file_io.h"
#include "xpr_rcode.h"
#include "xpr_bit.h"
#include "xpr_memory.h"
#include "xpr_debug.h"

namespace xpr
{
namespace
{
const xpr_size_t kDefaultBufferSize = 1024; // 1KB

// reference:
// http://www.unicode.org/faq/utf_bom.html

// +-------------+----------------------+
// | BOM         | character set        |
// +-------------+----------------------+
// | 00 00 FE FF | utf-32 big endian    |
// | FF FE 00 00 | utf-32 little endian |
// | FE FF       | utf-16 big endian    |
// | FF FE       | utf-16 little endian |
// | EF BB BF    | utf-8                |
// | 0E FE FF    | SCSU                 |
// | DD 73 66 73 | utf-EBCDIC           |
// | FB EE 28    | BOCU-1               |
// | 84 31 95 33 | GB-18030             |
// +-------------+----------------------+

const struct
{
    CharSet    mCharSet;
    xpr_char_t mBom[4];
    xpr_size_t mBomLen;
} gCharSetBom[] =
    {
        { CharSetUtf8,    { (xpr_char_t)0xef, (xpr_char_t)0xbb, (xpr_char_t)0xbf, (xpr_char_t)0    }, 3 },
        { CharSetUtf16,   { (xpr_char_t)0xff, (xpr_char_t)0xfe, (xpr_char_t)0,    (xpr_char_t)0    }, 2 },
        { CharSetUtf16be, { (xpr_char_t)0xfe, (xpr_char_t)0xff, (xpr_char_t)0,    (xpr_char_t)0    }, 2 },
        { CharSetUtf32,   { (xpr_char_t)0xff, (xpr_char_t)0xfe, (xpr_char_t)0,    (xpr_char_t)0    }, 4 },
        { CharSetUtf32be, { (xpr_char_t)0,    (xpr_char_t)0,    (xpr_char_t)0xfe, (xpr_char_t)0xff }, 4 },
    };
} // namespace anonymous

TextFileIo::TextFileIo(FileIo &aFileIo)
    : mFileIo(aFileIo)
    , mCharSet(CharSetNone)
{
    mEndOfLine[0] = 0;
    mEndOfLineW[0] = 0;
}

TextFileIo::~TextFileIo(void)
{
}

void TextFileIo::setEncoding(CharSet aCharSet)
{
    mCharSet = aCharSet;
}

CharSet TextFileIo::getEncoding(void) const
{
    return mCharSet;
}

xpr_bool_t TextFileIo::setEndOfLine(const xpr_char_t *aEndOfLine)
{
    XPR_ASSERT(aEndOfLine != XPR_NULL);

    xpr_size_t sLen = strlen(aEndOfLine);
    if (sLen == 0 || sLen > XPR_COUNT_OF(mEndOfLine))
        return XPR_FALSE;

    strcpy(mEndOfLine, aEndOfLine);

    return XPR_TRUE;
}

xpr_bool_t TextFileIo::setEndOfLine(xpr_sint_t aEndOfLineStyle)
{
    switch (aEndOfLineStyle)
    {
    case kUnixStyle:   strcpy(mEndOfLine, XPR_MBCS_STRING_LITERAL("\n"));   break;
    case kWinStyle:    strcpy(mEndOfLine, XPR_MBCS_STRING_LITERAL("\r\n")); break;
    case kOldMacStyle: strcpy(mEndOfLine, XPR_MBCS_STRING_LITERAL("\r"));   break;

    default:
        return XPR_FALSE;
    }

    xpr_size_t sInputBytes = strlen(mEndOfLine) + 1;
    xpr_size_t sOutputBytes = (XPR_COUNT_OF(mEndOfLineW) - 1) * sizeof(xpr_wchar_t);

    CharSet sCharSet = CharSetUtf16;
    if (sizeof(xpr_wchar_t) == 4)
        sCharSet = CharSetUtf32;

    convertCharSet(mEndOfLine, &sInputBytes, CharSetMultiBytes, mEndOfLineW, &sOutputBytes, sCharSet);

    return XPR_TRUE;
}

xpr_bool_t TextFileIo::getEndOfLine(xpr_char_t *aEndOfLine, xpr_size_t aMaxLen) const
{
    XPR_ASSERT(aEndOfLine != XPR_NULL);

    if (aMaxLen == 0)
        return XPR_FALSE;

    xpr_size_t sLen = strlen(mEndOfLine);
    if (sLen > aMaxLen)
        return XPR_FALSE;

    strcpy(aEndOfLine, mEndOfLine);

    return XPR_TRUE;
}

TextFileReader::TextFileReader(FileIo &aFileIo)
    : TextFileIo(aFileIo)
    , mBuffer(XPR_NULL), mBufferSize(kDefaultBufferSize), mBufferReadSize(0), mBufferOffset(0)
    , mTextBuffer(XPR_NULL), mTextMaxLen(0), mTextLen(0)
    , mTextBufferW(XPR_NULL), mTextMaxLenW(0), mTextLenW(0)
    , mEndOfLineLen(0)
{
}

TextFileReader::~TextFileReader(void)
{
    XPR_SAFE_DELETE_ARRAY(mBuffer);
    XPR_SAFE_DELETE_ARRAY(mTextBuffer);
    XPR_SAFE_DELETE_ARRAY(mTextBufferW);
}

xpr_bool_t TextFileReader::canRead(void) const
{
    if (mFileIo.isOpened() == XPR_FALSE)
        return XPR_FALSE;

    xpr_sint_t sOpenMode = mFileIo.getOpenMode();

    if (XPR_TEST_BITS(sOpenMode, FileIo::OpenModeWriteOnly) ||
        XPR_TEST_BITS(sOpenMode, FileIo::OpenModeAppend))
        return XPR_FALSE;

    return XPR_TRUE;
}

CharSet TextFileReader::detectEncoding(void)
{
    xpr_sint64_t sOffset = mFileIo.tell();

    xpr_rcode_t sRcode;
    xpr_char_t sBom[4] = {0,};
    xpr_ssize_t sRead;

    sRcode = mFileIo.read(sBom, 4, &sRead);
    if (XPR_RCODE_IS_ERROR(sRcode) || sRead <= 0)
        return CharSetNone;

    CharSet sCharSet = CharSetNone;

    xpr_size_t i, j;
    static const xpr_size_t sCharSetBomCount = XPR_COUNT_OF(gCharSetBom);

    for (i = 0; i < sCharSetBomCount; ++i)
    {
        if ((xpr_size_t)sRead < gCharSetBom[i].mBomLen)
            continue;

        for (j = 0; j < gCharSetBom[i].mBomLen; ++j)
        {
            if (sBom[j] != gCharSetBom[i].mBom[j])
                break;
        }

        if (j == gCharSetBom[i].mBomLen)
        {
            sCharSet = gCharSetBom[i].mCharSet;
            break;
        }
    }

    if (sCharSet == CharSetNone)
    {
        mFileIo.seekToBegin();
    }
    else
    {
        mFileIo.seekFromBegin(gCharSetBom[i].mBomLen);

        mCharSet = sCharSet;
    }

    return sCharSet;
}

xpr_size_t TextFileReader::getBufferSize(void)
{
    return mBufferSize;
}

void TextFileReader::setBufferSize(xpr_size_t aBufferSize)
{
    if (aBufferSize > 0)
        mBufferSize = aBufferSize;
}

xpr_rcode_t TextFileReader::read(void *aText, xpr_size_t aMaxBytes, CharSet aCharSet, xpr_ssize_t *aReadBytes)
{
    XPR_ASSERT(aText != XPR_NULL);
    XPR_ASSERT(aReadBytes != XPR_NULL);

    if (aMaxBytes == 0)
    {
        *aReadBytes = 0;
        return XPR_RCODE_SUCCESS;
    }

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadSize;

    if (XPR_IS_NULL(mBuffer))
    {
        xpr_byte_t *sNewBuffer = new xpr_byte_t[mBufferSize];
        if (XPR_IS_NULL(sNewBuffer))
            return XPR_RCODE_ENOMEM;

        mBuffer = sNewBuffer;
    }

    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;
    xpr_size_t sReadBytes;

    sReadBytes = 0;

    do
    {
        if (mBufferReadSize > 0 && mBufferOffset < mBufferReadSize)
        {
            sInputBytes = mBufferReadSize - mBufferOffset;
            sOutputBytes = aMaxBytes - sReadBytes;

            sRcode = convertCharSet(mBuffer + mBufferOffset,
                                    &sInputBytes,
                                    mCharSet,
                                    (xpr_byte_t *)aText + sReadBytes,
                                    &sOutputBytes,
                                    aCharSet);

            if (XPR_RCODE_IS_ERROR(sRcode))
                break;

            sReadBytes += sOutputBytes;

            mBufferOffset += sInputBytes;

            if (sReadBytes == aMaxBytes)
                break;
        }

        sRcode = mFileIo.read(mBuffer, mBufferSize, &sReadSize);
        if (XPR_RCODE_IS_ERROR(sRcode) || sReadSize == 0)
            break;

        mBufferReadSize = sReadSize;
        mBufferOffset = 0;

    } while (true);

    *aReadBytes = sReadBytes;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileReader::read(xpr_char_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen)
{
    XPR_ASSERT(aText != XPR_NULL);
    XPR_ASSERT(aReadLen != XPR_NULL);

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadBytes;

    sRcode = read(aText, aMaxLen * sizeof(xpr_char_t), CharSetMultiBytes, &sReadBytes);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    *aReadLen = sReadBytes / sizeof(xpr_char_t);

    return sRcode;
}

xpr_rcode_t TextFileReader::read(xpr_wchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen)
{
    XPR_ASSERT(aText != XPR_NULL);
    XPR_ASSERT(aReadLen != XPR_NULL);

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadBytes;
    CharSet sCharSet;

    sCharSet = CharSetUtf16;
    if (sizeof(xpr_wchar_t) == 4)
        sCharSet = CharSetUtf32;

    sRcode = read(aText, aMaxLen * sizeof(xpr_wchar_t), sCharSet, &sReadBytes);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    *aReadLen = sReadBytes / sizeof(xpr_wchar_t);

    return sRcode;
}

xpr_rcode_t TextFileReader::copyToEndOfLine(xpr_char_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen)
{
    XPR_ASSERT(aText != XPR_NULL);
    XPR_ASSERT(aReadLen != XPR_NULL);

    xpr_char_t *sEndOfLine;

    if (mEndOfLineLen == 0)
    {
        sEndOfLine = strpbrk(mTextBuffer, XPR_MBCS_STRING_LITERAL("\r\n"));
        if (XPR_IS_NULL(sEndOfLine))
            return XPR_RCODE_ENOENT;

        mEndOfLine[0] = sEndOfLine[0];
        if (sEndOfLine[0] == '\r' && sEndOfLine[1] == '\n')
        {
            mEndOfLine[1] = '\n';
            mEndOfLine[2] = 0;
        }
        else
        {
            mEndOfLine[1] = 0;
        }

        mEndOfLineLen = strlen(mEndOfLine);

        xpr_size_t i;
        for (i = 0; i < mEndOfLineLen; ++i)
            mEndOfLineW[i] = mEndOfLine[i];
        mEndOfLineW[i] = 0;
    }
    else
    {
        sEndOfLine = strstr(mTextBuffer, mEndOfLine);
        if (XPR_IS_NULL(sEndOfLine))
            return XPR_RCODE_ENOENT;
    }

    *sEndOfLine = 0;

    xpr_size_t sCopyLen = (xpr_size_t)(sEndOfLine - mTextBuffer);
    if (sCopyLen > aMaxLen)
        return XPR_RCODE_E2BIG;

    strcpy(aText, mTextBuffer);
    *aReadLen = sCopyLen;

    // left shift

    xpr_size_t sMoveTextLen = mTextLen - sCopyLen - mEndOfLineLen;
    if (sMoveTextLen > 0)
    {
        memmove(mTextBuffer, sEndOfLine + mEndOfLineLen, sMoveTextLen * sizeof(xpr_char_t));
        mTextBuffer[sMoveTextLen] = 0;
    }

    mTextLen = sMoveTextLen;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileReader::readLine(xpr_char_t  *aText,
                                     xpr_size_t   aMaxLen,
                                     xpr_ssize_t *aReadLen)
{
    XPR_ASSERT(aText != XPR_NULL);
    XPR_ASSERT(aReadLen != XPR_NULL);

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadLen;

    if (XPR_IS_NULL(mTextBuffer))
    {
        const xpr_size_t sTextBufferNeedLen = mBufferSize * 4;

        xpr_char_t *sNewTextBuffer = new xpr_char_t[sTextBufferNeedLen + 1];
        if (XPR_IS_NULL(sNewTextBuffer))
            return XPR_RCODE_ENOMEM;

        mTextBuffer = sNewTextBuffer;
        mTextMaxLen = sTextBufferNeedLen;
    }

    if (mTextLen > 0)
    {
        sRcode = copyToEndOfLine(aText, aMaxLen, aReadLen);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
            return XPR_RCODE_SUCCESS;
        else
        {
            if (XPR_RCODE_IS_NOT_ENOENT(sRcode))
                return sRcode;
        }
    }

    xpr_size_t sMaxReadLen = mTextMaxLen - mTextLen;
    if (sMaxReadLen == 0)
        return XPR_RCODE_E2BIG;

    sRcode = read(mTextBuffer + mTextLen, sMaxReadLen, &sReadLen);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    mTextLen += sReadLen;

    mTextBuffer[mTextLen] = 0;

    // end of file
    if (sReadLen == 0)
    {
        if (mTextLen == 0)
        {
            *aReadLen = -1;

            return XPR_RCODE_SUCCESS;
        }
        else
        {
            if (mTextLen > aMaxLen)
                return XPR_RCODE_E2BIG;
        }
    }

    sRcode = copyToEndOfLine(aText, aMaxLen, aReadLen);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        if (XPR_RCODE_IS_NOT_ENOENT(sRcode))
            return XPR_RCODE_E2BIG;
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileReader::copyToEndOfLine(xpr_wchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen)
{
    XPR_ASSERT(aText != XPR_NULL);
    XPR_ASSERT(aReadLen != XPR_NULL);

    xpr_wchar_t *sEndOfLine;

    if (mEndOfLineLen == 0)
    {
        sEndOfLine = wcspbrk(mTextBufferW, XPR_WIDE_STRING_LITERAL("\r\n"));
        if (XPR_IS_NULL(sEndOfLine))
            return XPR_RCODE_ENOENT;

        mEndOfLine[0] = (xpr_char_t)sEndOfLine[0];
        if (sEndOfLine[0] == '\r' && sEndOfLine[1] == '\n')
        {
            mEndOfLine[1] = '\n';
            mEndOfLine[2] = 0;
        }
        else
        {
            mEndOfLine[1] = 0;
        }

        mEndOfLineLen = strlen(mEndOfLine);

        xpr_size_t i;
        for (i = 0; i < mEndOfLineLen; ++i)
            mEndOfLineW[i] = mEndOfLine[i];
        mEndOfLineW[i] = 0;
    }
    else
    {
        sEndOfLine = wcsstr(mTextBufferW, mEndOfLineW);
        if (XPR_IS_NULL(sEndOfLine))
            return XPR_RCODE_ENOENT;
    }

    *sEndOfLine = 0;

    xpr_size_t sCopyLen = (xpr_size_t)(sEndOfLine - mTextBufferW);
    if (sCopyLen > aMaxLen)
        return XPR_RCODE_E2BIG;

    wcscpy(aText, mTextBufferW);
    *aReadLen = sCopyLen;

    // left shift

    xpr_size_t sMoveTextLen = mTextLenW - sCopyLen - mEndOfLineLen;
    if (sMoveTextLen > 0)
    {
        memmove(mTextBufferW, sEndOfLine + mEndOfLineLen, sMoveTextLen * sizeof(xpr_wchar_t));
        mTextBufferW[sMoveTextLen] = 0;
    }

    mTextLenW = sMoveTextLen;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileReader::readLine(xpr_wchar_t *aText,
                                     xpr_size_t   aMaxLen,
                                     xpr_ssize_t *aReadLen)
{
    XPR_ASSERT(aText != XPR_NULL);
    XPR_ASSERT(aReadLen != XPR_NULL);

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadLen;

    if (XPR_IS_NULL(mTextBufferW))
    {
        const xpr_size_t sTextBufferNeedLen = mBufferSize * 4;

        xpr_wchar_t *sNewTextBuffer = new xpr_wchar_t[sTextBufferNeedLen + 1];
        if (XPR_IS_NULL(sNewTextBuffer))
            return XPR_RCODE_ENOMEM;

        mTextBufferW = sNewTextBuffer;
        mTextMaxLenW = sTextBufferNeedLen;
    }

    if (mTextLenW > 0)
    {
        sRcode = copyToEndOfLine(aText, aMaxLen, aReadLen);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
            return XPR_RCODE_SUCCESS;
        else
        {
            if (XPR_RCODE_IS_NOT_ENOENT(sRcode))
                return sRcode;
        }
    }

    xpr_size_t sMaxReadLen = mTextMaxLenW - mTextLenW;
    if (sMaxReadLen == 0)
        return XPR_RCODE_E2BIG;

    sRcode = read(mTextBufferW + mTextLenW, sMaxReadLen, &sReadLen);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    mTextLenW += sReadLen;

    mTextBufferW[mTextLenW] = 0;

    // end of file
    if (sReadLen == 0)
    {
        if (mTextLenW == 0)
        {
            *aReadLen = -1;

            return XPR_RCODE_SUCCESS;
        }
        else
        {
            if (mTextLenW > aMaxLen)
                return XPR_RCODE_E2BIG;
        }
    }

    sRcode = copyToEndOfLine(aText, aMaxLen, aReadLen);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        if (XPR_RCODE_IS_NOT_ENOENT(sRcode))
            return XPR_RCODE_E2BIG;
    }

    return XPR_RCODE_SUCCESS;
}

TextFileWriter::TextFileWriter(FileIo &aFileIo)
    : TextFileIo(aFileIo)
    , mBuffer(XPR_NULL), mBufferSize(0)
    , mFormattedText(XPR_NULL), mFormattedTextMaxSize(0)
{
}

TextFileWriter::~TextFileWriter(void)
{
    XPR_SAFE_DELETE_ARRAY(mBuffer);
    XPR_SAFE_DELETE_ARRAY(mFormattedText);
}

xpr_bool_t TextFileWriter::canWrite(void) const
{
    if (mFileIo.isOpened() == XPR_FALSE)
        return XPR_FALSE;

    xpr_sint_t sOpenMode = mFileIo.getOpenMode();

    if (!XPR_TEST_BITS(sOpenMode, FileIo::OpenModeWriteOnly) &&
        !XPR_TEST_BITS(sOpenMode, FileIo::OpenModeAppend))
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_rcode_t TextFileWriter::writeBom(void)
{
    xpr_size_t i;
    xpr_size_t sCount = XPR_COUNT_OF(gCharSetBom);
    const xpr_char_t *sBom = XPR_NULL;
    xpr_size_t sBomLen = 0;

    for (i = 0; i < sCount; ++i)
    {
        if (mCharSet == gCharSetBom[i].mCharSet)
        {
            sBom = gCharSetBom[i].mBom;
            sBomLen = gCharSetBom[i].mBomLen;
            break;
        }
    }

    if (XPR_IS_NULL(sBom))
        return XPR_RCODE_ENOENT;

    xpr_ssize_t sWritten = 0;

    return mFileIo.write(sBom, sBomLen, &sWritten);
}

xpr_rcode_t TextFileWriter::write(const void *aText, xpr_size_t aTextBytes, CharSet aCharSet)
{
    XPR_ASSERT(aText != XPR_NULL);

    const xpr_size_t sBufferNeedSize = aTextBytes * 4;
    if (XPR_IS_NULL(mBuffer) || sBufferNeedSize > mBufferSize)
    {
        xpr_byte_t *sNewBuffer = new xpr_byte_t[sBufferNeedSize];
        if (XPR_IS_NULL(sNewBuffer))
            return XPR_RCODE_ENOMEM;

        XPR_SAFE_DELETE_ARRAY(mBuffer);

        mBuffer = sNewBuffer;
        mBufferSize = sBufferNeedSize;
    }

    xpr_rcode_t sRcode;
    xpr_size_t sOutputBytes = mBufferSize;

    sRcode = convertCharSet(aText, &aTextBytes, aCharSet, mBuffer, &sOutputBytes, mCharSet);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    xpr_ssize_t sWrittenSize;

    return mFileIo.write(mBuffer, sOutputBytes, &sWrittenSize);
}

xpr_rcode_t TextFileWriter::write(const xpr_char_t *aText)
{
    XPR_ASSERT(aText != XPR_NULL);

    xpr_size_t sLen = strlen(aText);

    return write(aText, sLen * sizeof(xpr_char_t), CharSetMultiBytes);
}

xpr_rcode_t TextFileWriter::write(const xpr_wchar_t *aText)
{
    XPR_ASSERT(aText != XPR_NULL);

    xpr_size_t sLen = wcslen(aText);

    CharSet sCharSet = CharSetUtf16;
    if (sizeof(xpr_wchar_t) == 4)
        sCharSet = CharSetUtf32;

    return write(aText, sLen * sizeof(xpr_wchar_t), sCharSet);
}

xpr_rcode_t TextFileWriter::ensureBuffer(xpr_size_t aFormattedTextSize)
{
    xpr_size_t sFormattedTextSize = aFormattedTextSize * 3 / 2 + 10; // with end of line

    if (XPR_IS_NULL(mFormattedText) || (xpr_sint_t)sFormattedTextSize > mFormattedTextMaxSize)
    {
        xpr_byte_t *sNewFormatBuffer = new xpr_byte_t[sFormattedTextSize + 1];
        if (XPR_IS_NULL(sNewFormatBuffer))
            return XPR_RCODE_ENOMEM;

        XPR_SAFE_DELETE_ARRAY(mFormattedText);

        mFormattedText = sNewFormatBuffer;
        mFormattedTextMaxSize = sFormattedTextSize;
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileWriter::writeFormat(const xpr_char_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sFormattedTextLen = _vscprintf(aFormat, sArgs);

    xpr_rcode_t sRcode = ensureBuffer(sFormattedTextLen * sizeof(xpr_char_t));
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    vsprintf((xpr_char_t *)mFormattedText, aFormat, sArgs);

    return write((xpr_char_t *)mFormattedText);
}

xpr_rcode_t TextFileWriter::writeFormat(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sFormattedTextLen = _vscwprintf(aFormat, sArgs);

    xpr_rcode_t sRcode = ensureBuffer(sFormattedTextLen * sizeof(xpr_wchar_t));
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    _vswprintf((xpr_wchar_t *)mFormattedText, aFormat, sArgs);

    return write((xpr_wchar_t *)mFormattedText);
}

xpr_rcode_t TextFileWriter::writeLine(const xpr_char_t *aText)
{
    XPR_ASSERT(aText != XPR_NULL);

    xpr_rcode_t sRcode;

    sRcode = write(aText);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sRcode = write(mEndOfLine);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileWriter::writeLine(const xpr_wchar_t *aText)
{
    XPR_ASSERT(aText != XPR_NULL);

    xpr_rcode_t sRcode;

    sRcode = write(aText);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sRcode = write(mEndOfLine);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileWriter::writeFormatLine(const xpr_char_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sFormattedTextLen = _vscprintf(aFormat, sArgs);

    xpr_rcode_t sRcode = ensureBuffer(sFormattedTextLen * sizeof(xpr_char_t));
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    xpr_char_t *sFormattedText = (xpr_char_t *)mFormattedText;

    vsprintf(sFormattedText, aFormat, sArgs);

    strcat(sFormattedText, mEndOfLine);

    return write(sFormattedText);
}

xpr_rcode_t TextFileWriter::writeFormatLine(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sFormattedTextLen = _vscwprintf(aFormat, sArgs);

    xpr_rcode_t sRcode = ensureBuffer(sFormattedTextLen * sizeof(xpr_wchar_t));
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    xpr_wchar_t *sFormattedText = (xpr_wchar_t *)mFormattedText;

    _vswprintf(sFormattedText, aFormat, sArgs);

    wcscat(sFormattedText, mEndOfLineW);

    return write(sFormattedText);
}
} // namespace xpr
