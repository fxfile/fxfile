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
struct XmlReader::Element {};
struct XmlReader::Attribute {};

class XmlReader::PrivateObject
{
public:
    PrivateObject(void)
        : mXmlDoc(XPR_NULL)
    {
    }

    ~PrivateObject(void)
    {
        close();
    }

public:
    xpr_bool_t open(const xpr_char_t *aFromEncoding)
    {
        if (XPR_IS_NULL(aFromEncoding))
            return XPR_FALSE;

        xpr_rcode_t sRcode;
        CharSet sFromCharSet = CharSetNone;

        if (_stricmp(aFromEncoding, "") == 0)
            sFromCharSet = CharSetMultiBytes;
        else if (_stricmp(aFromEncoding, "utf-8") == 0)
            sFromCharSet = CharSetUtf8;
        else if (_stricmp(aFromEncoding, "utf-16") == 0)
            sFromCharSet = CharSetUtf16;
        else if (_stricmp(aFromEncoding, "utf-16be") == 0)
            sFromCharSet = CharSetUtf16be;
        else if (_stricmp(aFromEncoding, "utf-32") == 0)
            sFromCharSet = CharSetUtf32;
        else if (_stricmp(aFromEncoding, "utf-32be") == 0)
            sFromCharSet = CharSetUtf32be;

        if (sFromCharSet == CharSetNone)
            return XPR_FALSE;

        if (mUtf16Decoder.isOpened() == XPR_TRUE)
            mUtf16Decoder.close();

        if (mMultiBytesDecoder.isOpened() == XPR_TRUE)
            mMultiBytesDecoder.close();

        sRcode = mUtf16Decoder.open(sFromCharSet, CharSetUtf16);
        if (XPR_RCODE_IS_ERROR(sRcode))
            return XPR_FALSE;

        sRcode = mMultiBytesDecoder.open(sFromCharSet, CharSetMultiBytes);
        if (XPR_RCODE_IS_ERROR(sRcode))
            return XPR_FALSE;

        return XPR_TRUE;
    }

    void close(void)
    {
        mUtf16Decoder.close();
        mMultiBytesDecoder.close();
    }

    xpr_bool_t convertDecoding(const xmlChar *aXmlInput, CharSet aOutputCharSet, void *aOutput, xpr_size_t aOutputBytes)
    {
        if (aXmlInput == XPR_NULL || aOutput == XPR_NULL || aOutputBytes == 0)
            return XPR_FALSE;

        CharSetConverter *sDecoder = XPR_NULL;
        xpr_size_t sNullCharSize = 0;

        if (aOutputCharSet == CharSetUtf16)
        {
            sDecoder = &mUtf16Decoder;
            sNullCharSize = sizeof(xpr_wchar_t);
        }
        else if (aOutputCharSet == CharSetMultiBytes)
        {
            sDecoder = &mMultiBytesDecoder;
            sNullCharSize = sizeof(xpr_char_t);
        }

        if (sDecoder == XPR_NULL || sNullCharSize == 0)
            return XPR_FALSE;

        const xpr_char_t *sInput = (const xpr_char_t *)(aXmlInput);
        xpr_char_t *sOutput = (xpr_char_t *)aOutput;

        xpr_size_t sInputBytes = ::xmlStrlen(aXmlInput);
        xpr_size_t sOutputBytes = aOutputBytes;

        xpr_rcode_t sRcode = sDecoder->convert(sInput, &sInputBytes, sOutput, &sOutputBytes);
        if (XPR_RCODE_IS_ERROR(sRcode))
            return XPR_FALSE;

        if (sNullCharSize == 1)
        {
            sOutput[sOutputBytes + 0] = 0;
        }
        else if (sNullCharSize == 2)
        {
            sOutput[sOutputBytes + 0] = 0;
            sOutput[sOutputBytes + 1] = 0;
        }
        else if (sNullCharSize == 4)
        {
            sOutput[sOutputBytes + 0] = 0;
            sOutput[sOutputBytes + 1] = 0;
            sOutput[sOutputBytes + 2] = 0;
            sOutput[sOutputBytes + 3] = 0;
        }
        else
        {
            return XPR_FALSE;
        }

        return XPR_TRUE;
    }

    xpr_bool_t convertDecoding(const xmlChar *aXmlInput, xpr::string &aOutput)
    {
        XPR_ASSERT(aXmlInput != XPR_NULL);

        xpr_size_t  sMaxOutputLength = ::xmlStrlen(aXmlInput);
        xpr_char_t *sOutput;

        aOutput.clear();
        aOutput.reserve(sMaxOutputLength + 1);

        sOutput = const_cast<xpr_char_t *>(aOutput.data());

        if (convertDecoding(aXmlInput, CharSetMultiBytes, sOutput, sMaxOutputLength * sizeof(xpr_char_t)) == XPR_FALSE)
            return XPR_FALSE;

        aOutput.update();

        return XPR_TRUE;
    }

    xpr_bool_t convertDecoding(const xmlChar *aXmlInput, xpr::wstring &aOutput)
    {
        XPR_ASSERT(aXmlInput != XPR_NULL);

        xpr_size_t   sMaxOutputLength = ::xmlStrlen(aXmlInput);
        xpr_wchar_t *sOutput;

        aOutput.clear();
        aOutput.reserve(sMaxOutputLength + 1);

        sOutput = const_cast<xpr_wchar_t *>(aOutput.data());

        if (convertDecoding(aXmlInput, CharSetUtf16, sOutput, sMaxOutputLength * sizeof(xpr_wchar_t)) == XPR_FALSE)
            return XPR_FALSE;

        aOutput.update();

        return XPR_TRUE;
    }

public:
    xmlDoc           *mXmlDoc;
    CharSetConverter  mUtf16Decoder;
    CharSetConverter  mMultiBytesDecoder;
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

xpr_bool_t XmlReader::load(const xpr_char_t *aPath)
{
    if (aPath == XPR_NULL || aPath[0] == 0)
        return XPR_FALSE;

    if (mObject->mXmlDoc != XPR_NULL)
        return XPR_FALSE;

    LIBXML_TEST_VERSION;

    // parse the file and get the DOM
    mObject->mXmlDoc = ::xmlReadFile(aPath, XPR_NULL, 0);
    if (mObject->mXmlDoc == XPR_NULL)
        return XPR_FALSE;

    if (mObject->mXmlDoc->encoding == XPR_NULL)
    {
        XPR_TRACE(XPR_STRING_LITERAL("[xml] Load from file (%s file is not xml document)\n"), aPath);

        close();
        return XPR_FALSE;
    }

    const xpr_char_t *sFromEncoding = (const xpr_char_t *)mObject->mXmlDoc->encoding;

    xpr_bool_t sSuccess = mObject->open(sFromEncoding);
    if (XPR_IS_FALSE(sSuccess))
    {
        close();
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t XmlReader::load(const xpr_wchar_t *aPath)
{
    if (aPath == XPR_NULL || aPath[0] == 0)
        return XPR_FALSE;

    xpr_char_t sPath[XPR_MAX_PATH + 1] = {0};
    xpr_size_t sInputBytes = wcslen(aPath) * sizeof(xpr_wchar_t);
    xpr_size_t sOutputBytes = sizeof(xpr_char_t) * XPR_MAX_PATH;
    XPR_UTF16_TO_MBS(aPath, &sInputBytes, sPath, &sOutputBytes);
    sPath[sOutputBytes] = '\0';

    return load(sPath);
}

xpr_bool_t XmlReader::load(const xpr::string &aPath)
{
    const xpr_char_t *sPath = aPath.c_str();

    return load(sPath);
}

xpr_bool_t XmlReader::load(const xpr::wstring &aPath)
{
    const xpr_wchar_t *sPath = aPath.c_str();

    return load(sPath);
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

    xpr_bool_t sSuccess = mObject->open(sFromEncoding);
    if (XPR_IS_FALSE(sSuccess))
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

    mObject->close();
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

XmlReader::Element *XmlReader::nextElement(Element *aElement) const
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

            return sElement;
        }
    }

    return XPR_NULL;
}

XmlReader::Element *XmlReader::nextElement(Element *aElement, xpr_char_t *aNextName, xpr_size_t aMaxNextNameLength) const
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

XmlReader::Element *XmlReader::nextElement(Element *aElement, xpr_wchar_t *aNextName, xpr_size_t aMaxNextNameLength) const
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

XmlReader::Element *XmlReader::nextElement(Element *aElement, xpr::string &aNextName) const
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

            if (aNextName != XPR_NULL)
                getElement(sElement, aNextName);

            return sElement;
        }
    }

    return XPR_NULL;
}

XmlReader::Element *XmlReader::nextElement(Element *aElement, xpr::wstring &aNextName) const
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

            if (aNextName != XPR_NULL)
                getElement(sElement, aNextName);

            return sElement;
        }
    }

    return XPR_NULL;
}

XmlReader::Element *XmlReader::childElement(Element *aElement) const
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

            return sElement;
        }

        sXmlNode = sXmlNode->next;
        if (sXmlNode == XPR_NULL)
            return XPR_NULL;
    }

    return XPR_NULL;
}

XmlReader::Element *XmlReader::childElement(Element *aElement, xpr_char_t *aChildName, xpr_size_t aMaxChildNameLength) const
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
            if (sElement == XPR_NULL)
                return XPR_NULL;

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

XmlReader::Element *XmlReader::childElement(Element *aElement, xpr_wchar_t *aChildName, xpr_size_t aMaxChildNameLength) const
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
            if (sElement == XPR_NULL)
                return XPR_NULL;

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

XmlReader::Element *XmlReader::childElement(Element *aElement, xpr::string &aChildName) const
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
            if (sElement == XPR_NULL)
                return XPR_NULL;

            if (aChildName != XPR_NULL)
                getElement(sElement, aChildName);

            return sElement;
        }

        sXmlNode = sXmlNode->next;
        if (sXmlNode == XPR_NULL)
            return XPR_NULL;
    }

    return XPR_NULL;
}

XmlReader::Element *XmlReader::childElement(Element *aElement, xpr::wstring &aChildName) const
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
            if (sElement == XPR_NULL)
                return XPR_NULL;

            if (aChildName != XPR_NULL)
                getElement(sElement, aChildName);

            return sElement;
        }

        sXmlNode = sXmlNode->next;
        if (sXmlNode == XPR_NULL)
            return XPR_NULL;
    }

    return XPR_NULL;
}

xpr_bool_t XmlReader::getElement(Element *aElement, xpr_char_t *aName, xpr_size_t aMaxNameLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    if (mObject->convertDecoding(sXmlNode->name, CharSetMultiBytes, aName, aMaxNameLength * sizeof(xpr_char_t)) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getElement(Element *aElement, xpr_wchar_t *aName, xpr_size_t aMaxNameLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    if (mObject->convertDecoding(sXmlNode->name, CharSetUtf16, aName, aMaxNameLength * sizeof(xpr_wchar_t)) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getElement(Element *aElement, xpr::string &aName) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    if (mObject->convertDecoding(sXmlNode->name, aName) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getElement(Element *aElement, xpr::wstring &aName) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    if (mObject->convertDecoding(sXmlNode->name, aName) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::testElement(Element *aElement, const xpr_char_t *aTestName) const
{
    if (aElement == XPR_NULL || aTestName == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sMaxNameLength = strlen(aTestName);
    xpr_char_t *sName = new xpr_char_t[sMaxNameLength + 1];

    if (getElement(aElement, sName, sMaxNameLength) == XPR_TRUE)
    {
        if (strcmp(sName, aTestName) == 0)
        {
            XPR_SAFE_DELETE_ARRAY(sName);
            return XPR_TRUE;
        }
    }

    XPR_SAFE_DELETE_ARRAY(sName);

    return XPR_FALSE;
}

xpr_bool_t XmlReader::testElement(Element *aElement, const xpr_wchar_t *aTestName) const
{
    if (aElement == XPR_NULL || aTestName == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sMaxNameLength = wcslen(aTestName);
    xpr_wchar_t *sName = new xpr_wchar_t[sMaxNameLength + 1];

    if (getElement(aElement, sName, sMaxNameLength) == XPR_TRUE)
    {
        if (wcscmp(sName, aTestName) == 0)
        {
            XPR_SAFE_DELETE_ARRAY(sName);
            return XPR_TRUE;
        }
    }

    XPR_SAFE_DELETE_ARRAY(sName);

    return XPR_FALSE;
}

xpr_bool_t XmlReader::testElement(Element *aElement, const xpr::string &aTestName) const
{
    const xpr_char_t *sTestName = aTestName.c_str();

    return testElement(aElement, sTestName);
}

xpr_bool_t XmlReader::testElement(Element *aElement, const xpr::wstring &aTestName) const
{
    const xpr_wchar_t *sTestName = aTestName.c_str();

    return testElement(aElement, sTestName);
}

xpr_bool_t XmlReader::getEntity(Element *aElement, xpr_char_t *aEntity, xpr_size_t aMaxEntityLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    sXmlNode = sXmlNode->children;
    if (sXmlNode == XPR_NULL)
        return XPR_FALSE;

    if (sXmlNode->type != XML_TEXT_NODE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlNode->content, CharSetMultiBytes, aEntity, aMaxEntityLength * sizeof(xpr_char_t)) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getEntity(Element *aElement, xpr_wchar_t *aEntity, xpr_size_t aMaxEntityLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    sXmlNode = sXmlNode->children;
    if (sXmlNode == XPR_NULL)
        return XPR_FALSE;

    if (sXmlNode->type != XML_TEXT_NODE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlNode->content, CharSetUtf16, aEntity, aMaxEntityLength * sizeof(xpr_wchar_t)) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getEntity(Element *aElement, xpr::string &aEntity) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    sXmlNode = sXmlNode->children;
    if (sXmlNode == XPR_NULL)
        return XPR_FALSE;

    if (sXmlNode->type != XML_TEXT_NODE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlNode->content, aEntity) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getEntity(Element *aElement, xpr::wstring &aEntity) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode = (xmlNode *)aElement;

    sXmlNode = sXmlNode->children;
    if (sXmlNode == XPR_NULL)
        return XPR_FALSE;

    if (sXmlNode->type != XML_TEXT_NODE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlNode->content, aEntity) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getElementEntity(Element *aElement, xpr_char_t *aName, xpr_size_t aMaxNameLength, xpr_char_t *aEntity, xpr_size_t aMaxEntityLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode      = (xmlNode *)aElement;
    xmlNode *sChildXmlNode = sXmlNode->children;

    if (mObject->convertDecoding(sXmlNode->name, CharSetMultiBytes, aName, aMaxNameLength * sizeof(xpr_char_t)) == XPR_FALSE)
        return XPR_FALSE;

    if (sChildXmlNode != XPR_NULL)
    {
        if (sChildXmlNode->type != XML_TEXT_NODE)
            return XPR_FALSE;

        if (mObject->convertDecoding(sChildXmlNode->content, CharSetMultiBytes, aEntity, aMaxEntityLength * sizeof(xpr_char_t)) == XPR_FALSE)
            return XPR_FALSE;
    }
    else
    {
        aEntity[0] = XPR_STRING_LITERAL('\0');
    }

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getElementEntity(Element *aElement, xpr_wchar_t *aName, xpr_size_t aMaxNameLength, xpr_wchar_t *aEntity, xpr_size_t aMaxEntityLength) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode      = (xmlNode *)aElement;
    xmlNode *sChildXmlNode = sXmlNode->children;

    if (mObject->convertDecoding(sXmlNode->name, CharSetUtf16, aName, aMaxNameLength * sizeof(xpr_wchar_t)) == XPR_FALSE)
        return XPR_FALSE;

    if (sChildXmlNode != XPR_NULL)
    {
        if (sChildXmlNode->type != XML_TEXT_NODE)
            return XPR_FALSE;

        if (mObject->convertDecoding(sChildXmlNode->content, CharSetUtf16, aEntity, aMaxEntityLength * sizeof(xpr_wchar_t)) == XPR_FALSE)
            return XPR_FALSE;
    }
    else
    {
        aEntity[0] = XPR_STRING_LITERAL('\0');
    }

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getElementEntity(Element *aElement, xpr::string &aName, xpr::string &aEntity) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode      = (xmlNode *)aElement;
    xmlNode *sChildXmlNode = sXmlNode->children;

    if (mObject->convertDecoding(sXmlNode->name, aName) == XPR_FALSE)
        return XPR_FALSE;

    if (sChildXmlNode != XPR_NULL)
    {
        if (sChildXmlNode->type != XML_TEXT_NODE)
            return XPR_FALSE;

        if (mObject->convertDecoding(sChildXmlNode->content, aEntity) == XPR_FALSE)
            return XPR_FALSE;
    }
    else
    {
        aEntity.clear();
    }

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getElementEntity(Element *aElement, xpr::wstring &aName, xpr::wstring &aEntity) const
{
    if (aElement == XPR_NULL)
        return XPR_FALSE;

    xmlNode *sXmlNode      = (xmlNode *)aElement;
    xmlNode *sChildXmlNode = sXmlNode->children;

    if (mObject->convertDecoding(sXmlNode->name, aName) == XPR_FALSE)
        return XPR_FALSE;

    if (sChildXmlNode != XPR_NULL)
    {
        if (sChildXmlNode->type != XML_TEXT_NODE)
            return XPR_FALSE;

        if (mObject->convertDecoding(sChildXmlNode->content, aEntity) == XPR_FALSE)
            return XPR_FALSE;
    }
    else
    {
        aEntity.clear();
    }

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

xpr_bool_t XmlReader::getAttribute(Attribute *aAttribute, xpr_char_t *aName, xpr_size_t aMaxNameLength, xpr_char_t *aValue, xpr_size_t aMaxValueLength) const
{
    if (aAttribute == XPR_NULL)
        return XPR_FALSE;

    xmlAttr *sXmlAttribute = (xmlAttr *)aAttribute;

    if (mObject->convertDecoding(sXmlAttribute->name, CharSetMultiBytes, aName, aMaxNameLength * sizeof(xpr_char_t)) == XPR_FALSE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlAttribute->children->content, CharSetMultiBytes, aValue, aMaxValueLength * sizeof(xpr_char_t)) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getAttribute(Attribute *aAttribute, xpr_wchar_t *aName, xpr_size_t aMaxNameLength, xpr_wchar_t *aValue, xpr_size_t aMaxValueLength) const
{
    if (aAttribute == XPR_NULL)
        return XPR_FALSE;

    xmlAttr *sXmlAttribute = (xmlAttr *)aAttribute;

    if (mObject->convertDecoding(sXmlAttribute->name, CharSetUtf16, aName, aMaxNameLength * sizeof(xpr_wchar_t)) == XPR_FALSE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlAttribute->children->content, CharSetUtf16, aValue, aMaxValueLength * sizeof(xpr_wchar_t)) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getAttribute(Attribute *aAttribute, xpr::string &aName, xpr::string &aValue) const
{
    if (aAttribute == XPR_NULL)
        return XPR_FALSE;

    xmlAttr *sXmlAttribute = (xmlAttr *)aAttribute;

    if (mObject->convertDecoding(sXmlAttribute->name, aName) == XPR_FALSE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlAttribute->children->content, aValue) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t XmlReader::getAttribute(Attribute *aAttribute, xpr::wstring &aName, xpr::wstring &aValue) const
{
    if (aAttribute == XPR_NULL)
        return XPR_FALSE;

    xmlAttr *sXmlAttribute = (xmlAttr *)aAttribute;

    if (mObject->convertDecoding(sXmlAttribute->name, aName) == XPR_FALSE)
        return XPR_FALSE;

    if (mObject->convertDecoding(sXmlAttribute->children->content, aValue) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

class XmlWriter::PrivateObject
{
public:
    PrivateObject(void)
        : mXmlBuffer(XPR_NULL)
        , mXmlWriter(XPR_NULL)
        , mXmlChar0(XPR_NULL), mMaxXmlCharBytes0(0)
        , mXmlChar1(XPR_NULL), mMaxXmlCharBytes1(0)
    {
    }

    ~PrivateObject(void)
    {
        close();
    }

public:
    xpr_bool_t open(const xpr_char_t *aToEncoding)
    {
        if (XPR_IS_NULL(aToEncoding))
            return XPR_FALSE;

        xpr_rcode_t sRcode;
        CharSet sToCharSet = CharSetNone;

        if (_stricmp(aToEncoding, "") == 0)
            sToCharSet = CharSetMultiBytes;
        else if (_stricmp(aToEncoding, "utf-8") == 0)
            sToCharSet = CharSetUtf8;
        else if (_stricmp(aToEncoding, "utf-16") == 0)
            sToCharSet = CharSetUtf16;
        else if (_stricmp(aToEncoding, "utf-16be") == 0)
            sToCharSet = CharSetUtf16be;
        else if (_stricmp(aToEncoding, "utf-32") == 0)
            sToCharSet = CharSetUtf32;
        else if (_stricmp(aToEncoding, "utf-32be") == 0)
            sToCharSet = CharSetUtf32be;

        if (sToCharSet == CharSetNone)
            return XPR_FALSE;

        if (mUtf16Encoder.isOpened() == XPR_TRUE)
            mUtf16Encoder.close();

        if (mMultiBytesEncoder.isOpened() == XPR_TRUE)
            mMultiBytesEncoder.close();

        sRcode = mUtf16Encoder.open(CharSetUtf16, sToCharSet);
        if (XPR_RCODE_IS_ERROR(sRcode))
            return XPR_FALSE;

        sRcode = mMultiBytesEncoder.open(CharSetMultiBytes, sToCharSet);
        if (XPR_RCODE_IS_ERROR(sRcode))
            return XPR_FALSE;

        return XPR_TRUE;
    }

    void close(void)
    {
        mUtf16Encoder.close();
        mMultiBytesEncoder.close();

        if (XPR_IS_NOT_NULL(mXmlChar0))
        {
            ::xmlFree(mXmlChar0);
            mXmlChar0 = XPR_NULL;
        }

        if (XPR_IS_NOT_NULL(mXmlChar1))
        {
            ::xmlFree(mXmlChar1);
            mXmlChar1 = XPR_NULL;
        }

        mMaxXmlCharBytes0 = 0;
        mMaxXmlCharBytes1 = 0;
    }

    xmlChar *convertEncoding(xpr_size_t sIndex, CharSet aInputCharSet, const void *aInput, xpr_size_t aInputBytes)
    {
        if (aInput == XPR_NULL || sIndex >= 2)
            return XPR_NULL;

        CharSetConverter *sEncoder = XPR_NULL;

        if (aInputCharSet == CharSetUtf16)
            sEncoder = &mUtf16Encoder;
        else if (aInputCharSet == CharSetMultiBytes)
            sEncoder = &mMultiBytesEncoder;

        if (sEncoder == XPR_NULL)
            return XPR_NULL;

        xpr_size_t sInputBytes = aInputBytes;
        xpr_size_t sOutputBytes = (aInputBytes + 1) * 3;

        xmlChar *sXmlChar = XPR_NULL;

        if (sIndex == 1)
        {
            if (mXmlChar1 == XPR_NULL || sOutputBytes > mMaxXmlCharBytes1)
            {
                if (XPR_IS_NOT_NULL(mXmlChar1))
                    ::xmlFree(mXmlChar1);

                mXmlChar1 = (xpr_uchar_t *)::xmlMalloc(sOutputBytes);
                mMaxXmlCharBytes1 = sOutputBytes;
            }

            sXmlChar = mXmlChar1;
        }
        else
        {
            if (mXmlChar0 == XPR_NULL || sOutputBytes > mMaxXmlCharBytes0)
            {
                if (XPR_IS_NOT_NULL(mXmlChar0))
                    ::xmlFree(mXmlChar0);

                mXmlChar0 = (xpr_uchar_t *)::xmlMalloc(sOutputBytes);
                mMaxXmlCharBytes0 = sOutputBytes;
            }

            sXmlChar = mXmlChar0;
        }

        if (XPR_IS_NULL(sXmlChar))
            return XPR_NULL;

        const xpr_char_t *sInput = (const xpr_char_t *)aInput;
        xpr_char_t *sOutput = (xpr_char_t *)sXmlChar;

        xpr_rcode_t sRcode = sEncoder->convert(sInput, &sInputBytes, sOutput, &sOutputBytes);
        if (XPR_RCODE_IS_ERROR(sRcode))
            return XPR_NULL;

        return sXmlChar;
    }

public:
    xmlBufferPtr      mXmlBuffer;
    xmlTextWriterPtr  mXmlWriter;
    CharSetConverter  mUtf16Encoder;
    CharSetConverter  mMultiBytesEncoder;
    xmlChar          *mXmlChar0;
    xmlChar          *mXmlChar1;
    xpr_size_t        mMaxXmlCharBytes0;
    xpr_size_t        mMaxXmlCharBytes1;
    
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

    mObject->close();
}

xpr_bool_t XmlWriter::beginDocument(const xpr_char_t *aEncoding)
{
    if (aEncoding == XPR_NULL)
        return XPR_FALSE;

    if (mObject->mXmlWriter != XPR_NULL)
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

    const xpr_char_t *sToEncoding = aEncoding;

    xpr_bool_t sSuccess = mObject->open(sToEncoding);
    if (XPR_IS_FALSE(sSuccess))
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

    mObject->close();

    return sResult;
}

xpr_bool_t XmlWriter::beginElement(const xpr_char_t *aName)
{
    if (XPR_IS_NULL(aName))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sLength = strlen(aName);

    xmlChar *sXmlName =
        mObject->convertEncoding(0,
                                 CharSetMultiBytes,
                                 aName,
                                 (sLength + 1) * sizeof(xpr_char_t));

    xpr_sint_t sXmlResult = ::xmlTextWriterStartElement(mObject->mXmlWriter, sXmlName);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::beginElement(const xpr_wchar_t *aName)
{
    if (XPR_IS_NULL(aName))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sLength = wcslen(aName);

    xmlChar *sXmlName =
        mObject->convertEncoding(0,
                                 CharSetUtf16,
                                 aName,
                                 (sLength + 1) * sizeof(xpr_wchar_t));

    xpr_sint_t sXmlResult = ::xmlTextWriterStartElement(mObject->mXmlWriter, sXmlName);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::beginElement(const xpr::string &aName)
{
    const xpr_char_t *sName = aName.c_str();

    return beginElement(sName);
}

xpr_bool_t XmlWriter::beginElement(const xpr::wstring &aName)
{
    const xpr_wchar_t *sName = aName.c_str();

    return beginElement(sName);
}

xpr_bool_t XmlWriter::endElement(void)
{
    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_sint_t sXmlResult = ::xmlTextWriterEndElement(mObject->mXmlWriter);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeElement(const xpr_char_t *aName, const xpr_char_t *aValue)
{
    if (XPR_IS_NULL(aName) || XPR_IS_NULL(aValue))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sNameLength = strlen(aName);
    xpr_size_t sValueLength = strlen(aValue);

    xmlChar *sXmlName =
        mObject->convertEncoding(0,
                                 CharSetMultiBytes,
                                 aName,
                                 (sNameLength + 1) * sizeof(xpr_char_t));

    xmlChar *sXmlValue =
        mObject->convertEncoding(1,
                                 CharSetMultiBytes,
                                 aValue,
                                 (sValueLength + 1) * sizeof(xpr_char_t));

    xpr_sint_t sXmlResult = xmlTextWriterWriteElement(mObject->mXmlWriter, sXmlName, sXmlValue);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeElement(const xpr_wchar_t *aName, const xpr_wchar_t *aValue)
{
    if (XPR_IS_NULL(aName) || XPR_IS_NULL(aValue))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sNameLength = wcslen(aName);
    xpr_size_t sValueLength = wcslen(aValue);

    xmlChar *sXmlName =
        mObject->convertEncoding(0,
                                 CharSetUtf16,
                                 aName,
                                 (sNameLength + 1) * sizeof(xpr_wchar_t));

    xmlChar *sXmlValue =
        mObject->convertEncoding(1,
                                 CharSetUtf16,
                                 aValue,
                                 (sValueLength + 1) * sizeof(xpr_wchar_t));

    xpr_sint_t sXmlResult = xmlTextWriterWriteElement(mObject->mXmlWriter, sXmlName, sXmlValue);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeElement(const xpr::string &aName, const xpr::string &aValue)
{
    const xpr_char_t *sName  = aName.c_str();
    const xpr_char_t *sValue = aValue.c_str();

    return writeElement(sName, sValue);
}

xpr_bool_t XmlWriter::writeElement(const xpr::wstring &aName, const xpr::wstring &aValue)
{
    const xpr_wchar_t *sName  = aName.c_str();
    const xpr_wchar_t *sValue = aValue.c_str();

    return writeElement(sName, sValue);
}

xpr_bool_t XmlWriter::writeElementEntity(const xpr_char_t *aElement, const xpr_char_t *aEntity)
{
    if (XPR_IS_NULL(aElement) || XPR_IS_NULL(aEntity))
        return XPR_FALSE;

    if (beginElement(aElement) == XPR_FALSE)
        return XPR_FALSE;

    xpr_bool_t sResult = writeEntity(aEntity);

    endElement();

    return sResult;
}

xpr_bool_t XmlWriter::writeElementEntity(const xpr_wchar_t *aElement, const xpr_wchar_t *aEntity)
{
    if (XPR_IS_NULL(aElement) || XPR_IS_NULL(aEntity))
        return XPR_FALSE;

    if (beginElement(aElement) == XPR_FALSE)
        return XPR_FALSE;

    xpr_bool_t sResult = writeEntity(aEntity);

    endElement();

    return sResult;
}

xpr_bool_t XmlWriter::writeElementEntity(const xpr::string &aElement, const xpr::string &aEntity)
{
    const xpr_char_t *sElement = aElement.c_str();
    const xpr_char_t *sEntity  = aEntity.c_str();

    return writeElementEntity(sElement, sEntity);
}

xpr_bool_t XmlWriter::writeElementEntity(const xpr::wstring &aElement, const xpr::wstring &aEntity)
{
    const xpr_wchar_t *sElement = aElement.c_str();
    const xpr_wchar_t *sEntity  = aEntity.c_str();

    return writeElementEntity(sElement, sEntity);
}

xpr_bool_t XmlWriter::writeEntity(const xpr_char_t *aEntity)
{
    if (XPR_IS_NULL(aEntity))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sLength = strlen(aEntity);

    xmlChar *sXmlEntity =
        mObject->convertEncoding(0,
                                 CharSetMultiBytes,
                                 aEntity,
                                 (sLength + 1) * sizeof(xpr_char_t));

    xpr_sint_t sXmlResult = ::xmlTextWriterWriteString(mObject->mXmlWriter, sXmlEntity);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeEntity(const xpr_wchar_t *aEntity)
{
    if (XPR_IS_NULL(aEntity))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sLength = wcslen(aEntity);

    xmlChar *sXmlEntity =
        mObject->convertEncoding(0,
                                 CharSetUtf16,
                                 aEntity,
                                 (sLength + 1) * sizeof(xpr_wchar_t));

    xpr_sint_t sXmlResult = ::xmlTextWriterWriteString(mObject->mXmlWriter, sXmlEntity);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeEntity(const xpr::string &aEntity)
{
    const xpr_char_t *sEntity = aEntity.c_str();

    return writeEntity(sEntity);
}

xpr_bool_t XmlWriter::writeEntity(const xpr::wstring &aEntity)
{
    const xpr_wchar_t *sEntity = aEntity.c_str();

    return writeEntity(sEntity);
}

xpr_bool_t XmlWriter::writeAttribute(const xpr_char_t *aName, const xpr_char_t *aValue)
{
    if (XPR_IS_NULL(aName) || XPR_IS_NULL(aValue))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sNameLength = strlen(aName);
    xpr_size_t sValueLength = strlen(aValue);

    xmlChar *sXmlName =
        mObject->convertEncoding(0,
                                 CharSetMultiBytes,
                                 aName,
                                 (sNameLength + 1) * sizeof(xpr_char_t));

    xmlChar *sXmlValue =
        mObject->convertEncoding(1,
                                 CharSetMultiBytes,
                                 aValue,
                                 (sValueLength + 1) * sizeof(xpr_char_t));

    xpr_sint_t sXmlResult = ::xmlTextWriterWriteAttribute(mObject->mXmlWriter, sXmlName, sXmlValue);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeAttribute(const xpr_wchar_t *aName, const xpr_wchar_t *aValue)
{
    if (XPR_IS_NULL(aName) || XPR_IS_NULL(aValue))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sNameLength = wcslen(aName);
    xpr_size_t sValueLength = wcslen(aValue);

    xmlChar *sXmlName =
        mObject->convertEncoding(0,
                                 CharSetUtf16,
                                 aName,
                                 (sNameLength + 1) * sizeof(xpr_wchar_t));

    xmlChar *sXmlValue =
        mObject->convertEncoding(1,
                                 CharSetUtf16,
                                 aValue,
                                 (sValueLength + 1) * sizeof(xpr_wchar_t));

    xpr_sint_t sXmlResult = ::xmlTextWriterWriteAttribute(mObject->mXmlWriter, sXmlName, sXmlValue);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeAttribute(const xpr::string &aName, const xpr::string &aValue)
{
    const xpr_char_t *sName  = aName.c_str();
    const xpr_char_t *sValue = aValue.c_str();

    return writeAttribute(sName, sValue);
}

xpr_bool_t XmlWriter::writeAttribute(const xpr::wstring &aName, const xpr::wstring &aValue)
{
    const xpr_wchar_t *sName  = aName.c_str();
    const xpr_wchar_t *sValue = aValue.c_str();

    return writeAttribute(sName, sValue);
}

xpr_bool_t XmlWriter::writeComment(const xpr_char_t *aComment)
{
    if (XPR_IS_NULL(aComment))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sLength = strlen(aComment);

    xmlChar *sXmlComment =
        mObject->convertEncoding(0,
                                 CharSetMultiBytes,
                                 aComment,
                                 (sLength + 1) * sizeof(xpr_char_t));

    xpr_sint_t sXmlResult = xmlTextWriterWriteComment(mObject->mXmlWriter, sXmlComment);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeComment(const xpr_wchar_t *aComment)
{
    if (XPR_IS_NULL(aComment))
        return XPR_FALSE;

    if (mObject->mXmlWriter == XPR_NULL)
        return XPR_FALSE;

    xpr_size_t sLength = wcslen(aComment);

    xmlChar *sXmlComment =
        mObject->convertEncoding(0,
                                 CharSetUtf16,
                                 aComment,
                                 (sLength + 1) * sizeof(xpr_wchar_t));

    xpr_sint_t sXmlResult = xmlTextWriterWriteComment(mObject->mXmlWriter, sXmlComment);

    return (sXmlResult >= 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t XmlWriter::writeComment(const xpr::string &aComment)
{
    const xpr_char_t *sComment = aComment.c_str();

    return writeComment(sComment);
}

xpr_bool_t XmlWriter::writeComment(const xpr::wstring &aComment)
{
    const xpr_wchar_t *sComment = aComment.c_str();

    return writeComment(sComment);
}

xpr_bool_t XmlWriter::save(const xpr_char_t *aPath) const
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

xpr_bool_t XmlWriter::save(const xpr_wchar_t *aPath) const
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

xpr_bool_t XmlWriter::save(const xpr::string &aPath) const
{
    const xpr_char_t *sPath = aPath.c_str();

    return save(sPath);
}

xpr_bool_t XmlWriter::save(const xpr::wstring &aPath) const
{
    const xpr_wchar_t *sPath = aPath.c_str();

    return save(sPath);
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
