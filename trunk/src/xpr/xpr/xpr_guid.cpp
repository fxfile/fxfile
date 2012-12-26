//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_guid.h"
#include "xpr_config.h"
#include "xpr_memory.h"
#include "xpr_rcode.h"
#include "xpr_char_set.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
Guid::Guid(void)
{
    mData1 = 0;
    mData2 = 0;
    mData3 = 0;
    xpr_memset_zero(mData4, sizeof(mData4));
}

Guid::Guid(const Guid &aGuid)
{
    mData1 = aGuid.mData1;
    mData2 = aGuid.mData2;
    mData3 = aGuid.mData3;
    xpr_memcpy(mData4, aGuid.mData4, sizeof(mData4));
}

Guid::~Guid(void)
{
}

xpr_rcode_t Guid::generate(void)
{
#if defined(XPR_CFG_OS_WINDOWS)
    HRESULT hr = ::CoCreateGuid((GUID *)&mData1);
    return SUCCEEDED(hr) ? XPR_RCODE_SUCCESS : XPR_RCODE_EFAULT;
#else
#error Please, support for your OS. <xpr_guid.cpp>
#endif
}

void Guid::clear(void)
{
    xpr_memset_zero(this, sizeof(Guid));
}

xpr_sint_t Guid::compare(const Guid &aGuid2) const
{
    xpr_sint64_t sDiff;

    sDiff = (xpr_sint64_t)mData1 - (xpr_sint64_t)aGuid2.mData1;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    sDiff = (xpr_sint64_t)mData2 - (xpr_sint64_t)aGuid2.mData2;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    sDiff = (xpr_sint64_t)mData3 - (xpr_sint64_t)aGuid2.mData3;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    return xpr_memcmp(mData4, aGuid2.mData4, 8);
}

xpr_bool_t Guid::test(const Guid &aGuid2) const
{
    return (xpr_memcmp(this, &aGuid2, sizeof(Guid)) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t Guid::none(void) const
{
    Guid aNone;
    aNone.clear();

    return test(aNone);
}

xpr_bool_t Guid::toString(xpr_char_t *aString, xpr_bool_t aWithBrace) const
{
    if (aString == XPR_NULL)
        return XPR_FALSE;

    aString[0] = 0;

    if (XPR_IS_TRUE(aWithBrace))
        strcpy(aString, XPR_MBCS_STRING_LITERAL("{"));

    xpr_sint_t sNumber[11];

    sNumber[0]  = mData1;
    sNumber[1]  = mData2;
    sNumber[2]  = mData3;
    sNumber[3]  = mData4[0];
    sNumber[4]  = mData4[1];
    sNumber[5]  = mData4[2];
    sNumber[6]  = mData4[3];
    sNumber[7]  = mData4[4];
    sNumber[8]  = mData4[5];
    sNumber[9]  = mData4[6];
    sNumber[10] = mData4[7];

    sprintf(aString + strlen(aString),
        XPR_MBCS_STRING_LITERAL("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
        sNumber[0], sNumber[1], sNumber[2],
        sNumber[3], sNumber[4], sNumber[5],
        sNumber[6], sNumber[7], sNumber[8],
        sNumber[9], sNumber[10]);

    if (XPR_IS_TRUE(aWithBrace))
        strcat(aString, XPR_MBCS_STRING_LITERAL("}"));

    return XPR_TRUE;
}

xpr_bool_t Guid::toString(xpr_wchar_t *aString, xpr_bool_t aWithBrace) const
{
    if (aString == XPR_NULL)
        return XPR_FALSE;

    aString[0] = 0;

    if (XPR_IS_TRUE(aWithBrace))
        wcscpy(aString, XPR_WIDE_STRING_LITERAL("{"));

    xpr_sint_t sNumber[11];

    sNumber[0]  = mData1;
    sNumber[1]  = mData2;
    sNumber[2]  = mData3;
    sNumber[3]  = mData4[0];
    sNumber[4]  = mData4[1];
    sNumber[5]  = mData4[2];
    sNumber[6]  = mData4[3];
    sNumber[7]  = mData4[4];
    sNumber[8]  = mData4[5];
    sNumber[9]  = mData4[6];
    sNumber[10] = mData4[7];

    swprintf(aString + wcslen(aString),
        XPR_WIDE_STRING_LITERAL("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
        sNumber[0], sNumber[1], sNumber[2],
        sNumber[3], sNumber[4], sNumber[5],
        sNumber[6], sNumber[7], sNumber[8],
        sNumber[9], sNumber[10]);

    if (XPR_IS_TRUE(aWithBrace))
        wcscat(aString, XPR_WIDE_STRING_LITERAL("}"));

    return XPR_TRUE;
}

xpr_bool_t Guid::fromString(const xpr_char_t *aString)
{
    if (aString == XPR_NULL)
        return XPR_FALSE;

    const xpr_char_t *sString = aString;

    if (sString[0] == XPR_MBCS_STRING_LITERAL('{'))
        ++sString;

    xpr_sint_t sNumber[11];

    sscanf(sString,
        XPR_MBCS_STRING_LITERAL("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
        sNumber + 0, sNumber + 1, sNumber + 2,
        sNumber + 3, sNumber + 4, sNumber + 5,
        sNumber + 6, sNumber + 7, sNumber + 8,
        sNumber + 9, sNumber + 10);

    mData1    = (xpr_uint32_t)sNumber[0];
    mData2    = (xpr_uint16_t)sNumber[1];
    mData3    = (xpr_uint16_t)sNumber[2];
    mData4[0] = (xpr_uint8_t) sNumber[3];
    mData4[1] = (xpr_uint8_t) sNumber[4];
    mData4[2] = (xpr_uint8_t) sNumber[5];
    mData4[3] = (xpr_uint8_t) sNumber[6];
    mData4[4] = (xpr_uint8_t) sNumber[7];
    mData4[5] = (xpr_uint8_t) sNumber[8];
    mData4[6] = (xpr_uint8_t) sNumber[9];
    mData4[7] = (xpr_uint8_t) sNumber[10];

    return XPR_TRUE;
}

xpr_bool_t Guid::fromString(const xpr_wchar_t *aString)
{
    if (aString == XPR_NULL)
        return XPR_FALSE;

    const xpr_wchar_t *sString = aString;

    if (sString[0] == XPR_WIDE_STRING_LITERAL('{'))
        ++sString;

    xpr_sint_t sNumber[11];

    swscanf(sString,
        XPR_WIDE_STRING_LITERAL("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
        sNumber + 0, sNumber + 1, sNumber + 2,
        sNumber + 3, sNumber + 4, sNumber + 5,
        sNumber + 6, sNumber + 7, sNumber + 8,
        sNumber + 9, sNumber + 10);

    mData1    = (xpr_uint32_t)sNumber[0];
    mData2    = (xpr_uint16_t)sNumber[1];
    mData3    = (xpr_uint16_t)sNumber[2];
    mData4[0] = (xpr_uint8_t) sNumber[3];
    mData4[1] = (xpr_uint8_t) sNumber[4];
    mData4[2] = (xpr_uint8_t) sNumber[5];
    mData4[3] = (xpr_uint8_t) sNumber[6];
    mData4[4] = (xpr_uint8_t) sNumber[7];
    mData4[5] = (xpr_uint8_t) sNumber[8];
    mData4[6] = (xpr_uint8_t) sNumber[9];
    mData4[7] = (xpr_uint8_t) sNumber[10];

    return XPR_TRUE;
}

void Guid::toGuid(Guid &aGuid) const
{
    aGuid.mData1 = mData1;
    aGuid.mData2 = mData2;
    aGuid.mData3 = mData3;
    xpr_memcpy(aGuid.mData4, mData4, sizeof(mData4));
}

void Guid::fromGuid(const Guid &aGuid)
{
    mData1 = aGuid.mData1;
    mData2 = aGuid.mData2;
    mData3 = aGuid.mData3;
    xpr_memcpy(mData4, aGuid.mData4, sizeof(mData4));
}

xpr_size_t Guid::getBufferSize(void) const
{
    xpr_size_t sBufferSize = sizeof(mData1) + sizeof(mData2) + sizeof(mData3) + sizeof(mData4);
    return sBufferSize;
}

xpr_bool_t Guid::toBuffer(xpr_byte_t *aBuffer) const
{
    if (aBuffer == XPR_NULL)
        return XPR_FALSE;

    xpr_memcpy(aBuffer, &mData1, sizeof(mData1)); aBuffer += sizeof(mData1);
    xpr_memcpy(aBuffer, &mData2, sizeof(mData2)); aBuffer += sizeof(mData2);
    xpr_memcpy(aBuffer, &mData3, sizeof(mData3)); aBuffer += sizeof(mData3);
    xpr_memcpy(aBuffer,  mData4, sizeof(mData4));

    return XPR_TRUE;
}

xpr_bool_t Guid::fromBuffer(const xpr_byte_t *aBuffer)
{
    if (aBuffer == XPR_NULL)
        return XPR_FALSE;

    xpr_memcpy(&mData1, aBuffer, sizeof(mData1)); aBuffer += sizeof(mData1);
    xpr_memcpy(&mData2, aBuffer, sizeof(mData2)); aBuffer += sizeof(mData2);
    xpr_memcpy(&mData3, aBuffer, sizeof(mData3)); aBuffer += sizeof(mData3);
    xpr_memcpy(mData4,  aBuffer, sizeof(mData4));

    return XPR_TRUE;
}

Guid& Guid::operator = (const Guid &aGuid)
{
    mData1 = aGuid.mData1;
    mData2 = aGuid.mData2;
    mData3 = aGuid.mData3;
    xpr_memcpy(mData4, aGuid.mData4, sizeof(mData4));

    return *this;
}

bool Guid::operator < (const Guid &aGuid) const
{
    return (compare(aGuid) < 0) ? true : false;
}

bool Guid::operator > (const Guid &aGuid) const
{
    return (compare(aGuid) > 0) ? true : false;
}

bool Guid::operator == (const Guid &aGuid) const
{
    return (test(aGuid) == XPR_TRUE) ? true : false;
}

bool Guid::operator != (const Guid &aGuid) const
{
    return (test(aGuid) == XPR_FALSE) ? true : false;
}
} // namespace xpr
