//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_TEXT_FILE_IO_H__
#define __XPR_TEXT_FILE_IO_H__
#pragma once

#include "xpr_file_io.h"
#include "xpr_char_set.h"

namespace xpr
{
class TextFileIo
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
    virtual xpr_bool_t setEndOfLine(const xpr_tchar_t *aEndOfLine);
    virtual xpr_bool_t setEndOfLine(xpr_sint_t aEndOfLineStyle);
    virtual CharSet getEncoding(void) const;
    virtual xpr_bool_t getEndOfLine(xpr_tchar_t *aEndOfLine, xpr_size_t aMaxLen) const;

protected:
    FileIo      &mFileIo;
    CharSet      mCharSet;
    xpr_tchar_t  mEndOfLine[10];
};

class TextFileReader : public TextFileIo
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
    virtual xpr_rcode_t read(xpr_tchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);
    virtual xpr_rcode_t readLine(xpr_tchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);

protected:
    xpr_rcode_t copyToEndOfLine(xpr_tchar_t *aText, xpr_size_t aMaxLen, xpr_ssize_t *aReadLen);

protected:
    xpr_byte_t  *mBuffer;
    xpr_size_t   mBufferSize;
    xpr_size_t   mBufferReadSize;
    xpr_size_t   mBufferOffset;
    xpr_tchar_t *mTextBuffer;
    xpr_size_t   mTextMaxLen;
    xpr_size_t   mTextLen;
    xpr_size_t   mEndOfLineLen;
};

class TextFileWriter : public TextFileIo
{
public:
    explicit TextFileWriter(FileIo &aFileIo);
    virtual ~TextFileWriter(void);

public:
    virtual xpr_bool_t canWrite(void) const;

public:

public:
    virtual xpr_rcode_t writeBom(void);
    virtual xpr_rcode_t write(const void *aText, xpr_size_t aTextBytes, CharSet aCharSet);
    virtual xpr_rcode_t write(const xpr_tchar_t *aText);
    virtual xpr_rcode_t writeFormat(const xpr_tchar_t *aFormat, ...);
    virtual xpr_rcode_t writeLine(const xpr_tchar_t *aLineText);
    virtual xpr_rcode_t writeFormatLine(const xpr_tchar_t *aFormat, ...);

protected:
    xpr_rcode_t ensureBuffer(const xpr_tchar_t *aFormat, va_list aArgs);

protected:
    xpr_byte_t  *mBuffer;
    xpr_size_t   mBufferSize;
    xpr_tchar_t *mFormattedText;
    xpr_size_t   mFormattedTextMaxLen;
};
} // namespace xpr

#endif // __XPR_TEXT_FILE_IO_H__
