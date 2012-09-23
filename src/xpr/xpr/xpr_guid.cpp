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
    MemZeroSet(mData4, sizeof(mData4));
}

Guid::Guid(const Guid &aGuid)
{
    *this = aGuid;
}

Guid::~Guid(void)
{
}

xpr_rcode_t Guid::generate(void)
{
#if defined(XPR_CFG_OS_WINDOWS)
    HRESULT hr = ::CoCreateGuid((GUID *)this);
    return SUCCEEDED(hr) ? XPR_RCODE_SUCCESS : XPR_RCODE_EFAULT;
#else
#error Please, support for your OS. <xpr_guid.cpp>
#endif
}

void Guid::clear(void)
{
    MemZeroSet(this, sizeof(Guid));
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

    return MemCmp(mData4, aGuid2.mData4, 8);
}

xpr_bool_t Guid::test(const Guid &aGuid2) const
{
    return (MemCmp(this, &aGuid2, sizeof(Guid)) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t Guid::none(void) const
{
    Guid aNone;
    aNone.clear();

    return test(aNone);
}

xpr_bool_t Guid::toString(xpr_tchar_t *aGuid, xpr_bool_t aWithBrace) const
{
    if (aGuid == XPR_NULL)
        return XPR_FALSE;

#if defined(XPR_CFG_OS_WINDOWS)

    xpr_tchar_t *sGuidString = XPR_NULL;

    HRESULT hr = ::StringFromIID((REFIID)*this, (LPOLESTR *)&sGuidString);
    if (FAILED(hr))
        return XPR_FALSE;

    if (aWithBrace == XPR_TRUE)
        _tcscpy(aGuid, sGuidString);
    else
    {
        _tcscpy(aGuid, sGuidString+1);
        aGuid[_tcslen(aGuid)-1] = 0;
    }

    ::CoTaskMemFree(sGuidString);

#else
#error Please, support for your OS. <xpr_guid.cpp>
#endif

    return XPR_TRUE;
}

xpr_bool_t Guid::fromString(const xpr_tchar_t *aGuid)
{
    if (aGuid == XPR_NULL)
        return XPR_FALSE;

#if defined(XPR_CFG_OS_WINDOWS)

    if (aGuid[0] != XPR_STRING_LITERAL('{'))
    {
        xpr_wchar_t sStringWithBrace[0xff] = {0};
        wcscpy(sStringWithBrace, XPR_WIDE_STRING_LITERAL("{"));

        xpr_size_t sOutputBytes = 0xfe * sizeof(xpr_wchar_t);
        XPR_TCS_TO_UTF16(aGuid, _tcslen(aGuid) * sizeof(xpr_tchar_t), sStringWithBrace + 1, &sOutputBytes);
        wcscat(sStringWithBrace, XPR_WIDE_STRING_LITERAL("}"));

        HRESULT hr = ::IIDFromString((LPOLESTR)sStringWithBrace, (LPIID)this);
        return SUCCEEDED(hr) ? XPR_TRUE : XPR_FALSE;
    }

    HRESULT hr = ::IIDFromString((LPOLESTR)aGuid, (LPIID)this);
    return SUCCEEDED(hr) ? XPR_TRUE : XPR_FALSE;

#else
#error Please, support for your OS. <xpr_guid.cpp>
#endif
}

void Guid::toGuid(Guid &aGuid) const
{
    aGuid.mData1 = mData1;
    aGuid.mData2 = mData2;
    aGuid.mData3 = mData3;
    MemCpy(aGuid.mData4, mData4, sizeof(mData4));
}

void Guid::fromGuid(const Guid &aGuid)
{
    mData1 = aGuid.mData1;
    mData2 = aGuid.mData2;
    mData3 = aGuid.mData3;
    MemCpy(mData4, aGuid.mData4, sizeof(mData4));
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

    MemCpy(aBuffer, &mData1, sizeof(mData1)); aBuffer += sizeof(mData1);
    MemCpy(aBuffer, &mData2, sizeof(mData2)); aBuffer += sizeof(mData2);
    MemCpy(aBuffer, &mData3, sizeof(mData3)); aBuffer += sizeof(mData3);
    MemCpy(aBuffer,  mData4, sizeof(mData4));

    return XPR_TRUE;
}

xpr_bool_t Guid::fromBuffer(const xpr_byte_t *aBuffer)
{
    if (aBuffer == XPR_NULL)
        return XPR_FALSE;

    MemCpy(&mData1, aBuffer, sizeof(mData1)); aBuffer += sizeof(mData1);
    MemCpy(&mData2, aBuffer, sizeof(mData2)); aBuffer += sizeof(mData2);
    MemCpy(&mData3, aBuffer, sizeof(mData3)); aBuffer += sizeof(mData3);
    MemCpy(mData4,  aBuffer, sizeof(mData4));

    return XPR_TRUE;
}

Guid& Guid::operator = (const Guid &aGuid)
{
    mData1 = aGuid.mData1;
    mData2 = aGuid.mData2;
    mData3 = aGuid.mData3;
    MemCpy(mData4, aGuid.mData4, sizeof(mData4));

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
