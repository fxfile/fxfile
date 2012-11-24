//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_text_file_io.h"
#include "xpr_rcode.h"
#include "xpr_bit.h"
#include "xpr_memory.h"

namespace xpr
{
static const xpr_size_t kDefaultBufferSize = 1024; // 1KB

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

static const struct
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

TextFileIo::TextFileIo(FileIo &aFileIo)
    : mFileIo(aFileIo)
    , mCharSet(CharSetNone)
{
    mEndOfLine[0] = 0;
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

xpr_bool_t TextFileIo::setEndOfLine(const xpr_tchar_t *aEndOfLine)
{
    if (XPR_IS_NULL(aEndOfLine))
        return XPR_FALSE;

    xpr_size_t sLen = _tcslen(aEndOfLine);
    if (sLen == 0 || sLen > XPR_COUNT_OF(mEndOfLine))
        return XPR_FALSE;

    _tcscpy(mEndOfLine, aEndOfLine);

    return XPR_TRUE;
}

xpr_bool_t TextFileIo::setEndOfLine(xpr_sint_t aEndOfLineStyle)
{
    switch (aEndOfLineStyle)
    {
    case kUnixStyle:   _tcscpy(mEndOfLine, XPR_STRING_LITERAL("\n"));   break;
    case kWinStyle:    _tcscpy(mEndOfLine, XPR_STRING_LITERAL("\r\n")); break;
    case kOldMacStyle: _tcscpy(mEndOfLine, XPR_STRING_LITERAL("\r"));   break;

    default:
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t TextFileIo::getEndOfLine(xpr_tchar_t *aEndOfLine, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aEndOfLine) || aMaxLen == 0)
        return XPR_FALSE;

    xpr_size_t sLen = _tcslen(mEndOfLine);
    if (sLen > aMaxLen)
        return XPR_FALSE;

    _tcscpy(aEndOfLine, mEndOfLine);

    return XPR_TRUE;
}

TextFileReader::TextFileReader(FileIo &aFileIo)
    : TextFileIo(aFileIo)
    , mBuffer(XPR_NULL), mBufferSize(kDefaultBufferSize), mBufferReadSize(0), mBufferOffset(0)
    , mTextBuffer(XPR_NULL), mTextMaxLen(0), mTextLen(0), mEndOfLineLen(0)
{
}

TextFileReader::~TextFileReader(void)
{
    XPR_SAFE_DELETE_ARRAY(mBuffer);
    XPR_SAFE_DELETE_ARRAY(mTextBuffer);
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
        mFileIo.seekFromBegin(4 - gCharSetBom[i].mBomLen);
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

xpr_rcode_t TextFileReader::read(xpr_tchar_t *aText,
                                 xpr_size_t   aMaxLen,
                                 xpr_ssize_t *aReadLen)
{
    if (XPR_IS_NULL(aText) || XPR_IS_NULL(aReadLen))
        return XPR_RCODE_EINVAL;

    if (aMaxLen == 0)
    {
        *aReadLen = 0;
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
    xpr_size_t sReadLen;

    CharSet sToCharSet = CharSetMultiBytes;
    if (sizeof(xpr_tchar_t) == sizeof(xpr_wchar_t))
        sToCharSet = CharSetUtf16;

    sReadLen = 0;

    do
    {
        if (mBufferReadSize > 0 && mBufferOffset < mBufferReadSize)
        {
            sInputBytes = mBufferReadSize - mBufferOffset;
            sOutputBytes = (aMaxLen - sReadLen) * sizeof(xpr_tchar_t);

            sRcode = convertCharSet(mBuffer + mBufferOffset,
                                    &sInputBytes,
                                    mCharSet,
                                    aText + sReadLen,
                                    &sOutputBytes,
                                    sToCharSet);

            if (XPR_RCODE_IS_ERROR(sRcode))
                break;

            sReadLen += sOutputBytes / sizeof(xpr_tchar_t);

            mBufferOffset += sInputBytes;

            if (sReadLen == aMaxLen)
                break;
        }

        sRcode = mFileIo.read(mBuffer, mBufferSize, &sReadSize);
        if (XPR_RCODE_IS_ERROR(sRcode) || sReadSize == 0)
            break;

        mBufferReadSize = sReadSize;
        mBufferOffset = 0;

    } while (true);

    *aReadLen = sReadLen;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileReader::copyToEndOfLine(xpr_tchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen)
{
    xpr_tchar_t *sEndOfLine;

    if (mEndOfLineLen == 0)
    {
        sEndOfLine = _tcspbrk(mTextBuffer, XPR_STRING_LITERAL("\r\n"));
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

        mEndOfLineLen = _tcslen(mEndOfLine);
    }
    else
    {
        sEndOfLine = _tcsstr(mTextBuffer, mEndOfLine);
        if (XPR_IS_NULL(sEndOfLine))
            return XPR_RCODE_ENOENT;
    }

    *sEndOfLine = 0;

    xpr_size_t sCopyLen = (xpr_size_t)(sEndOfLine - mTextBuffer);
    if (sCopyLen > aMaxLen)
        return XPR_RCODE_E2BIG;

    _tcscpy(aText, mTextBuffer);
    *aReadLen = sCopyLen;

    // left shift

    xpr_size_t sMoveTextLen = mTextLen - sCopyLen - mEndOfLineLen;
    if (sMoveTextLen > 0)
    {
        memmove(mTextBuffer, sEndOfLine + mEndOfLineLen, sMoveTextLen * sizeof(xpr_tchar_t));
        mTextBuffer[sMoveTextLen] = 0;
    }

    mTextLen = sMoveTextLen;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileReader::readLine(xpr_tchar_t *aText,
                                     xpr_size_t   aMaxLen,
                                     xpr_ssize_t *aReadLen)
{
    if (XPR_IS_NULL(aText) || XPR_IS_NULL(aReadLen))
        return XPR_RCODE_EINVAL;

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadLen;

    if (XPR_IS_NULL(mTextBuffer))
    {
        const xpr_size_t sTextBufferNeedLen = mBufferSize * 4;

        xpr_tchar_t *sNewTextBuffer = new xpr_tchar_t[sTextBufferNeedLen + 1];
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

TextFileWriter::TextFileWriter(FileIo &aFileIo)
    : TextFileIo(aFileIo)
    , mBuffer(XPR_NULL), mBufferSize(0)
    , mFormattedText(XPR_NULL), mFormattedTextMaxLen(0)
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
    if (XPR_IS_NULL(aText))
        return XPR_RCODE_EINVAL;

    const xpr_size_t sBufferNeedSize = aTextBytes * 4;
    if (XPR_IS_NULL(mBuffer) || mBufferSize < sBufferNeedSize)
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

xpr_rcode_t TextFileWriter::write(const xpr_tchar_t *aText)
{
    if (XPR_IS_NULL(aText))
        return XPR_RCODE_EINVAL;

    xpr_size_t sLen = _tcslen(aText);

    return write(aText, sLen * sizeof(xpr_tchar_t), CharSetUtf16);
}

xpr_rcode_t TextFileWriter::ensureBuffer(const xpr_tchar_t *aFormat, va_list aArgs)
{
    xpr_sint_t sLen = _vsctprintf(aFormat, aArgs);
    xpr_size_t sFormattedTextNeedLen = sLen * 3 / 2 + 10; // with end of line

    if (XPR_IS_NULL(mFormattedText) || sLen > (xpr_sint_t)sFormattedTextNeedLen)
    {
        xpr_tchar_t *sNewFormatBuffer = new xpr_tchar_t[mFormattedTextMaxLen + 1];
        if (XPR_IS_NULL(sNewFormatBuffer))
            return XPR_RCODE_ENOMEM;

        XPR_SAFE_DELETE_ARRAY(mFormattedText);

        mFormattedText = sNewFormatBuffer;
        mFormattedTextMaxLen = sFormattedTextNeedLen;
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileWriter::writeFormat(const xpr_tchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = ensureBuffer(aFormat, sArgs);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    _vstprintf(mFormattedText, aFormat, sArgs);

    return write(mFormattedText);
}

xpr_rcode_t TextFileWriter::writeLine(const xpr_tchar_t *aText)
{
    xpr_rcode_t sRcode;

    sRcode = write(aText);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sRcode = write(mEndOfLine);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t TextFileWriter::writeFormatLine(const xpr_tchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = ensureBuffer(aFormat, sArgs);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    _vstprintf(mFormattedText, aFormat, sArgs);

    _tcscat(mFormattedText, mEndOfLine);

    return write(mFormattedText);
}
} // namespace xpr
