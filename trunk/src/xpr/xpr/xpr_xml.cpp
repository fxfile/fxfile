//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_xml.h"
#include "xpr_char_set.h"
#include "xpr_memory.h"
#include "xpr_math.h"
#include "xpr_debug.h"
#include "xpr_file_io.h"
#include "xpr_file_sys.h"
#include "xpr_rcode.h"

#include <iconv.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

namespace xpr
{
#define XPR_MAX_BUFFER_LENGTH 4096

struct XmlReader::Element {};
struct XmlReader::Attribute {};

class XmlReader::PrivateObject
{
public:
    PrivateObject(void)
        : mXmlDoc(XPR_NULL)
        , mDecoder(XPR_NULL)
        , mBuffer(XPR_NULL), mMaxBufferLength(0)
    {
    }

    ~PrivateObject(void)
    {
        XPR_SAFE_DELETE_ARRAY(mBuffer);
    }

public:
    xpr_bool_t convertDecoding(const xmlChar *aXmlInput, xpr_tchar_t *aOutput, xpr_size_t aMaxOutputLength)
    {
        if (aXmlInput == XPR_NULL || aOutput == XPR_NULL || aMaxOutputLength == 0)
            return XPR_FALSE;

        if (mDecoder == XPR_NULL)
            return XPR_FALSE;

        xpr_size_t sInputLength = ::xmlStrlen(aXmlInput);

        if (mBuffer == XPR_NULL || mMaxBufferLength > sInputLength)
        {
            XPR_SAFE_DELETE_ARRAY(mBuffer);

            mMaxBufferLength = (sInputLength > XPR_MAX_BUFFER_LENGTH) ? (sInputLength * 2) : (XPR_MAX_BUFFER_LENGTH);
            mBuffer = new xpr_tchar_t[mMaxBufferLength+1];
        }

        xpr_size_t sInBytesLeft = sInputLength;
        xpr_size_t sOutBytesLeft = sInputLength * sizeof(xpr_tchar_t);

        xpr_char_t *sOutBuf = (xpr_char_t *)(&mBuffer[0]);
        const xpr_char_t *sInBuf = (const xpr_char_t *)(aXmlInput);

        xpr_size_t sIconvResult = ::iconv(mDecoder, &sInBuf, &sInBytesLeft, &sOutBuf, &sOutBytesLeft);
        if (sIconvResult == (xpr_size_t)-1)
            return XPR_FALSE;

        xpr_size_t sNullPos = sInputLength - (sOutBytesLeft / sizeof(xpr_tchar_t));
        if (XPR_IS_OUT_OF_RANGE(0, sNullPos, mMaxBufferLength))
            return XPR_FALSE;

        mBuffer[sNullPos] = XPR_STRING_LITERAL('\0');

        _tcscpy(aOutput, mBuffer);

        return XPR_TRUE;
    }

public:
    xmlDoc      *mXmlDoc;
    iconv_t      mDecoder;
    xpr_tchar_t *mBuffer;
    xpr_size_t   mMaxBufferLength;
};

XmlReader::XmlReader(void)
    : mObject(new PrivateObject)
{
    XPR_ASSERT(mObject != XPR_NULL);
}

XmlReader::~XmlReader(void)
{
    close();
    XPR_SAFE_DELETE(mObject);
}

xpr_bool_t XmlReader::load(const xpr_tchar_t *aPath)
{
    if (aPath == XPR_NULL || aPath[0] == 0)
        return XPR_FALSE;

    if (mObject->mXmlDoc != XPR_NULL)
        return XPR_FALSE;

    LIBXML_TEST_VERSION;

    xpr_char_t sAnsiPath[XPR_MAX_PATH + 1] = {0};
    xpr_size_t sInputBytes = _tcslen(aPath) * sizeof(xpr_tchar_t);
    xpr_size_t sOutputBytes = sizeof(xpr_char_t) * XPR_MAX_PATH;
    XPR_TCS_TO_MBS(aPath, &sInputBytes, sAnsiPath, &sOutputBytes);
    sAnsiPath[sOutputBytes] = '\0';

    // parse the file and get the DOM
    mObject->mXmlDoc = ::xmlReadFile(sAnsiPath, XPR_NULL, 0);
    if (mObject->mXmlDoc == XPR_NULL)
        return XPR_FALSE;

    if (mObject->mXmlDoc->encoding == XPR_NULL)
    {
        XPR_TRACE(XPR_STRING_LITERAL("[xml] Load from file (%s file is not xml document)\n"), aPath);

        close();
        return XPR_FALSE;
    }

    const xpr_char_t *sFromEncoding = (const xpr_char_t *)mObject->mXmlDoc->encoding;
    const xpr_char_t *sToEncoding = 
#ifdef XPR_CFG_UNICODE
        "utf-16le";
#else
        "char";
#endif

    mObject->mDecoder = ::iconv_open(sToEncoding, sFromEncoding);
    if (mObject->mDecoder == (iconv_t)-1)
    {
        close();
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t XmlReader::load(xpr_byte_t *aBuffer, xpr_size_t aBufferSize)
{
    if (aBuffer == XPR_NULL || aBufferSize == 0)
        return XPR_FALSE;

    if (mObject->mXmlDoc != XPR_NULL)
        return XPR_FALSE;

    LIBXML_TEST_VERSION

    // parse the file and get the DOM
    mObject->mXmlDoc = ::xmlReadMemory((xpr_char_t *)aBuffer, (xpr_sint_t)aBufferSize, "noname.xml", XPR_NULL, 0);
    if (mObject->mXmlDoc == XPR_NULL)
        return XPR_FALSE;

    if (mObject->mXmlDoc->encoding == XPR_NULL)
    {
        XPR_TRACE(XPR_STRING_LITERAL("[xml] Load from memory (not xml document)\n"));

        close();
        return XPR_FALSE;
    }

    const xpr_char_t *sFromEncoding = (const xpr_char_t *)mObject->mXmlDoc->encoding;
    const xpr_char_t *sToEncoding = 
#ifdef XPR_CFG_UNICODE
        "utf-16le";
#else
        "char";
#endif

    mObject->mDecoder = ::iconv_open(sToEncoding, sFromEncoding);
    if (mObject->mDecoder == (iconv_t)-1)
    {
        close();
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

void XmlReader::close(void)
{
    if (mObject->mXmlDoc != XPR_NULL)
    {
        ::xmlFreeDoc(mObject->mXmlDoc);
        mObject->mXmlDoc = XPR_NULL;
    }

    if (mObject->mDecoder != XPR_NULL)
    {
        ::iconv_close(mObject->mDecoder);
        mObject->mDecoder = XPR_NULL;
    }

    XPR_SAFE_DELETE_ARRAY(mObject->mBuffer);
    mObject->mMaxBufferLength = 0;
}

xpr_rcode_t XmlReader::getEncoding(xpr_char_t *aEncoding, xpr_size_t aMaxLength) const
{
    if (aEncoding == XPR_NULL || aMaxLength == 0)
        return XPR_RCODE_EINVAL;

    if (mObject->mXmlDoc == XPR_NULL)
        return XPR_RCODE_EFAULT;

    strcpy(aEncoding, (const xpr_char_t *)mObject->mXmlDoc->encoding);

    return XPR_RCODE_SUCCESS;
}

CharSet XmlReader::getEncoding(void) const
{
    xpr_char_t sEncoding[0xff] = {0};
    xpr_rcode_t sRcode = getEncoding(sEncoding, 0xfe);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        if (_stricmp(sEncoding, "") == 0)
            return CharSetMultiBytes;
        else if (_stricmp(sEncoding, "utf-8") == 0)
            return CharSetUtf8;
        else if (_stricmp(sEncoding, "utf-16") == 0)
            return CharSetUtf16;
        else if (_stricmp(sEncoding, "utf-32") == 0)
            return CharSetUtf32;
        else
            return CharSetNone;
    }

    return CharSetNone;
}

XmlReader::Element *XmlReader::getRootElement(void) const
{
    return (Element *)::xmlDocGetRootElement(mObject->mXmlDoc);
}

XmlReader::Element *XmlReader::nextElement(Element *aElement, xpr_tchar_t *aNextName, xpr_size_t aMaxNextNameLength) const
{
    if (aElement == XPR_NULL)
        return XPR_NULL;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    while (true)
    {
        sXmlNode = sXmlNode->next;
        if (sXmlNode == XPR_NULL)
            return XPR_NULL;

        if (sXmlNode->type == XML_ELEMENT_NODE)
        {
            Element *sElement = (Element *)sXmlNode;
            if (sElement == XPR_NULL)
                return XPR_NULL;

            if (aNextName != XPR_NULL && aMaxNextNameLength > 0)
                getElement(sElement, aNextName, aMaxNextNameLength);

            return sElement;
        }
    }

    return XPR_NULL;
}

XmlReader::Element *XmlReader::childElement(Element *aElement, xpr_tchar_t *aChildName, xpr_size_t aMaxChildNameLength) const
{
    if (aElement == XPR_NULL)
        return XPR_NULL;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    sXmlNode = sXmlNode->children;
    if (sXmlNode == XPR_NULL)
        return XPR_NULL;

    while (true)
    {
        if (sXmlNode->type == XML_ELEMENT_NODE)
        {
            Element *sElement = (Element *)sXmlNode;

            if (aChildName != XPR_NULL && aMaxChildNameLength > 0)
                getElement(sElement, aChildName, aMaxChildNameLength);

            return sElement;
        }

        sXmlNode = sXmlNode->next;
        if (sXmlNode == XPR_NULL)
            return XPR_NULL;
    }

    return XPR_NULL;
}

xpr_bool_t XmlReader::getElement(Element *aElement, xpr_tchar_t *aName, xpr_size_t aMaxNameLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    if (mObject->convertDecoding(sXmlNode->name, aName, aMaxNameLength) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::testElement(Element *aElement, const xpr_tchar_t *aTestName) const
{
    if (aElement == XPR_NULL || aTestName == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sMaxNameLength = _tcslen(aTestName);
    xpr_tchar_t *sName = new xpr_tchar_t[sMaxNameLength + 1];

    if (getElement(aElement, sName, sMaxNameLength) == XPR_TRUE)
    {
        if (_tcscmp(sName, aTestName) == 0)
        {
            XPR_SAFE_DELETE_ARRAY(sName);
            return XPR_TRUE;
        }
    }

    XPR_SAFE_DELETE_ARRAY(sName);

    return XPR_FALSE;
}

xpr_bool_t XmlReader::getEntity(Element *aElement, xpr_tchar_t *aEntity, xpr_size_t aMaxEntityLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    sXmlNode = sXmlNode->children;
    if (sXmlNode == XPR_NULL)
        return XPR_FALSE;

    if (sXmlNode->type != XML_TEXT_NODE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlNode->content, aEntity, aMaxEntityLength) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

XmlReader::Attribute *XmlReader::getFirstAttribute(Element *aElement) const
{
    if (aElement == XPR_NULL)
        return XPR_NULL;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    return (Attribute *)sXmlNode->properties;
}

XmlReader::Attribute *XmlReader::nextAttribute(Attribute *aAttribute) const
{
    if (aAttribute == XPR_NULL)
        return XPR_NULL;

    xmlAttr *sXmlAttribute = (xmlAttr *)aAttribute;

    return (Attribute *)sXmlAttribute->next;
}

xpr_bool_t XmlReader::getAttribute(Attribute *aAttribute, xpr_tchar_t *aName, xpr_size_t aMaxNameLength, xpr_tchar_t *aValue, xpr_size_t aMaxValueLength) const
{
    if (aAttribute == XPR_NULL)
        return XPR_FALSE;

    xmlAttr *sXmlAttribute = (xmlAttr *)aAttribute;

    if (mObject->convertDecoding(sXmlAttribute->name, aName, aMaxNameLength) == XPR_FALSE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlAttribute->children->content, aValue, aMaxValueLength) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

class XmlWriter::PrivateObject
{
public:
    PrivateObject(void)
        : mXmlBuffer(XPR_NULL)
        , mXmlWriter(XPR_NULL)
        , mEncoder(XPR_NULL)
    {
    }

    ~PrivateObject(void)
    {
    }

public:
    xmlChar *convertEncoding(const xpr_tchar_t *aInput)
    {
        if (aInput == XPR_NULL)
            return XPR_NULL;

        if (mEncoder == XPR_NULL)
            return XPR_NULL;

        xpr_size_t sInputLength = _tcslen(aInput);

        xmlChar *sXmlOutput = (xpr_uchar_t *)::xmlMalloc((sInputLength+1)*3);
        if (sXmlOutput == XPR_NULL)
            return XPR_NULL;

        xpr_size_t sInBytesLeft = (sInputLength+1) * 2;
        xpr_size_t sOutBytesLeft = (sInputLength+1) * 3;

        xpr_char_t *sOutBuf = (xpr_char_t *)sXmlOutput;
        const xpr_char_t *sInBuf = (const xpr_char_t *)aInput;

        xpr_size_t sIconvResult = ::iconv(mEncoder, &sInBuf, &sInBytesLeft, &sOutBuf, &sOutBytesLeft);
        if (sIconvResult == (xpr_size_t)-1)
        {
            ::xmlFree(sXmlOutput);
            return XPR_NULL;
        }

        return sXmlOutput;
    }

public:
    xmlBufferPtr mXmlBuffer;
    xmlTextWriterPtr mXmlWriter;
    iconv_t mEncoder;
};

XmlWriter::XmlWriter(void)
    : mObject(new PrivateObject)
{
    XPR_ASSERT(mObject != XPR_NULL);
}

XmlWriter::~XmlWriter(void)
{
    close();
    XPR_SAFE_DELETE(mObject);
}

void XmlWriter::close(void)
{
    if (mObject->mXmlBuffer != XPR_NULL)
    {
        ::xmlBufferFree(mObject->mXmlBuffer);
        mObject->mXmlBuffer = XPR_NULL;
    }

    if (mObject->mEncoder != XPR_NULL)
    {
        ::iconv_close(mObject->mEncoder);
        mObject->mEncoder = XPR_NULL;
    }
}

xpr_bool_t XmlWriter::beginDocument(const xpr_char_t *aEncoding)
{
    if (aEncoding == XPR_NULL)
        return XPR_FALSE;

    if (mObject->mXmlWriter != XPR_NULL || mObject->mEncoder != XPR_NULL)
        return XPR_FALSE;

    if (mObject->mXmlBuffer != XPR_NULL)
    {
        ::xmlBufferFree(mObject->mXmlBuffer);
        mObject->mXmlBuffer = XPR_NULL;
    }

    mObject->mXmlBuffer = ::xmlBufferCreate();
    if (mObject->mXmlBuffer == XPR_NULL)
        return XPR_FALSE;

    mObject->mXmlWriter = ::xmlNewTextWriterMemory(mObject->mXmlBuffer, 0);
    if (mObject->mXmlWriter == XPR_NULL)
    {
        close();
        return XPR_FALSE;
    }

    xpr_sint_t sXmlResult = ::xmlTextWriterStartDocument(mObject->mXmlWriter, XPR_NULL, aEncoding, XPR_NULL);
    if (sXmlResult < 0)
    {
        close();
        return XPR_FALSE;
    }

    const xpr_char_t *sFromEncoding = 
#ifdef XPR_CFG_UNICODE
        "utf-16le";
#else
        "char";
#endif
    const xpr_char_t *sToEncoding = aEncoding;

    mObject->mEncoder = ::iconv_open(sToEncoding, sFromEncoding);
    if (mObject->mEncoder == (iconv_t)-1)
    {
        close();
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t XmlWriter::endDocument(void)
{
    xpr_bool_t sResult = XPR_FALSE;

    if (mObject->mXmlWriter != XPR_NULL)
    {
        xpr_sint_t sXmlResult = ::xmlTextWriterEndDocument(mObject->mXmlWriter);
        if (sXmlResult >= 0)
            sResult = XPR_TRUE;

        ::xmlFreeTextWriter(mObject->mXmlWriter);
        mObject->mXmlWriter = XPR_NULL;
    }

    if (mObject->mEncoder != XPR_NULL)
    {
        ::iconv_close(mObject->mEncoder);
        mObject->mEncoder = XPR_NULL;
    }

    return sResult;
}

xpr_bool_t XmlWriter::beginElement(const xpr_tchar_t *aName)
{
    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xmlChar *sXmlName = mObject->convertEncoding(aName);

    xpr_sint_t sXmlResult = ::xmlTextWriterStartElement(mObject->mXmlWriter, sXmlName);

    if (sXmlName != XPR_NULL) ::xmlFree(sXmlName);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::endElement(void)
{
    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_sint_t sXmlResult = ::xmlTextWriterEndElement(mObject->mXmlWriter);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeElement(const xpr_tchar_t *aName, const xpr_tchar_t *aValue)
{
    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xmlChar *sXmlName = mObject->convertEncoding(aName);
    xmlChar *sXmlValue = mObject->convertEncoding(aValue);

    xpr_sint_t sXmlResult = xmlTextWriterWriteElement(mObject->mXmlWriter, sXmlName, sXmlValue);

    if (sXmlName != XPR_NULL) ::xmlFree(sXmlName);
    if (sXmlValue != XPR_NULL) ::xmlFree(sXmlValue);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeElementEntity(const xpr_tchar_t *aElement, const xpr_tchar_t *aEntity)
{
    if (beginElement(aElement) == XPR_FALSE)
        return XPR_FALSE;

    xpr_bool_t sResult = writeEntity(aEntity);

    endElement();

    return sResult;
}

xpr_bool_t XmlWriter::writeEntity(const xpr_tchar_t *aEntity)
{
    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xmlChar *sXmlEntity = mObject->convertEncoding(aEntity);

    xpr_sint_t sXmlResult = ::xmlTextWriterWriteString(mObject->mXmlWriter, sXmlEntity);

    if (sXmlEntity != XPR_NULL) ::xmlFree(sXmlEntity);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeAttribute(const xpr_tchar_t *aName, const xpr_tchar_t *aValue)
{
    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xmlChar *sXmlName = mObject->convertEncoding(aName);
    xmlChar *sXmlValue = mObject->convertEncoding(aValue);

    xpr_sint_t sXmlResult = ::xmlTextWriterWriteAttribute(mObject->mXmlWriter, sXmlName, sXmlValue);

    if (sXmlName != XPR_NULL) ::xmlFree(sXmlName);
    if (sXmlValue != XPR_NULL) ::xmlFree(sXmlValue);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeComment(const xpr_tchar_t *aComment)
{
    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xmlChar *sXmlComment = mObject->convertEncoding(aComment);

    xpr_sint_t sXmlResult = xmlTextWriterWriteComment(mObject->mXmlWriter, sXmlComment);

    if (sXmlComment != XPR_NULL) ::xmlFree(sXmlComment);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::save(const xpr_tchar_t *aPath) const
{
    if (aPath == XPR_NULL)
        return XPR_FALSE;

    if (mObject->mXmlBuffer == XPR_NULL)
        return XPR_FALSE;

    xpr_rcode_t sRcode;
    xpr_ssize_t sWritten;
    xpr_sint_t sOpenMode;
    xpr::FileIo sFileIo;

    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    sRcode = sFileIo.open(aPath, sOpenMode);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sRcode = sFileIo.write(mObject->mXmlBuffer->content, mObject->mXmlBuffer->use, &sWritten);

    sFileIo.close();

    if (XPR_RCODE_IS_ERROR(sRcode) || mObject->mXmlBuffer->use != sWritten)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_byte_t *XmlWriter::getXmlBuffer(xpr_size_t &aSize) const
{
    aSize = 0;
    if (mObject->mXmlBuffer == XPR_NULL)
        return XPR_NULL;

    aSize = mObject->mXmlBuffer->use;

    return mObject->mXmlBuffer->content;
}
} // namespace xpr
