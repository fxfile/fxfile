//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_TEXT_FILE_IO_H__
#define __XPR_TEXT_FILE_IO_H__ 1
#pragma once

#include "xpr_file_io.h"
#include "xpr_char_set.h"

namespace xpr
{
class XPR_DL_API TextFileIo
{
public:
    enum
    {
        kUnixStyle,   // \n
        kWinStyle,    // \r\n
        kOldMacStyle, // \r
    };

protected:
    explicit TextFileIo(FileIo &aFileIo);
    virtual ~TextFileIo(void);

public:
    virtual void setEncoding(CharSet aCharSet);
    virtual xpr_bool_t setEndOfLine(const xpr_char_t *aEndOfLine);
    virtual xpr_bool_t setEndOfLine(xpr_sint_t aEndOfLineStyle);
    virtual CharSet getEncoding(void) const;
    virtual xpr_bool_t getEndOfLine(xpr_char_t *aEndOfLine, xpr_size_t aMaxLen) const;

protected:
    FileIo      &mFileIo;
    CharSet      mCharSet;
    xpr_char_t   mEndOfLine[10];
    xpr_wchar_t  mEndOfLineW[10];
};

class XPR_DL_API TextFileReader : public TextFileIo
{
public:
    explicit TextFileReader(FileIo &aFileIo);
    virtual ~TextFileReader(void);

public:
    virtual xpr_bool_t canRead(void) const;

public:
    virtual CharSet detectEncoding(void);
    virtual xpr_size_t getBufferSize(void);
    virtual void setBufferSize(xpr_size_t aBufferSize);

public:
    virtual xpr_rcode_t read(void *aText, xpr_size_t aMaxBytes, CharSet aCharSet, xpr_ssize_t *aReadBytes);
    virtual xpr_rcode_t read(xpr_char_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);
    virtual xpr_rcode_t read(xpr_wchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);
    virtual xpr_rcode_t readLine(xpr_char_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);
    virtual xpr_rcode_t readLine(xpr_wchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);

protected:
    xpr_rcode_t copyToEndOfLine(xpr_char_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);
    xpr_rcode_t copyToEndOfLine(xpr_wchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);

protected:
    xpr_byte_t  *mBuffer;
    xpr_size_t   mBufferSize;
    xpr_size_t   mBufferReadSize;
    xpr_size_t   mBufferOffset;

    xpr_char_t  *mTextBuffer;
    xpr_size_t   mTextMaxLen;
    xpr_size_t   mTextLen;
    xpr_wchar_t *mTextBufferW;
    xpr_size_t   mTextMaxLenW;
    xpr_size_t   mTextLenW;
    xpr_size_t   mEndOfLineLen;
};

class XPR_DL_API TextFileWriter : public TextFileIo
{
public:
    explicit TextFileWriter(FileIo &aFileIo);
    virtual ~TextFileWriter(void);

public:
    virtual xpr_bool_t canWrite(void) const;

public:
    virtual xpr_rcode_t writeBom(void);
    virtual xpr_rcode_t write(const void *aText, xpr_size_t aTextBytes, CharSet aCharSet);
    virtual xpr_rcode_t write(const xpr_char_t *aText);
    virtual xpr_rcode_t write(const xpr_wchar_t *aText);
    virtual xpr_rcode_t writeFormat(const xpr_char_t *aFormat, ...);
    virtual xpr_rcode_t writeFormat(const xpr_wchar_t *aFormat, ...);
    virtual xpr_rcode_t writeLine(const xpr_char_t *aLineText);
    virtual xpr_rcode_t writeLine(const xpr_wchar_t *aLineText);
    virtual xpr_rcode_t writeFormatLine(const xpr_char_t *aFormat, ...);
    virtual xpr_rcode_t writeFormatLine(const xpr_wchar_t *aFormat, ...);

protected:
    xpr_rcode_t ensureBuffer(xpr_size_t aFormattedTextSize);

protected:
    xpr_byte_t *mBuffer;
    xpr_size_t  mBufferSize;
    xpr_byte_t *mFormattedText;
    xpr_size_t  mFormattedTextMaxSize;
};
} // namespace xpr

#endif // __XPR_TEXT_FILE_IO_H__
