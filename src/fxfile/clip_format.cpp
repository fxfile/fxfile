//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "clip_format.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
ClipFormat::ClipFormat(void)
{
    mShellIDList       = 0;
    mDropEffect        = 0;
    mShellIDListOffset = 0;
    mFileName          = 0;
    mFileNameW         = 0;
    mFileNameMap       = 0;
    mFileNameMapW      = 0;
    mFileContents      = 0;
    mFileDescriptorA   = 0;
    mFileDescriptorW   = 0;
    mInetUrl           = 0;
}

ClipFormat::~ClipFormat(void)
{
}

void ClipFormat::registerClipFormat(void)
{
    // Register Drag & Drop Format and Clipboard Format
    mShellIDList       = ::RegisterClipboardFormat(CFSTR_SHELLIDLIST);
    mDropEffect        = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
    mShellIDListOffset = ::RegisterClipboardFormat(CFSTR_SHELLIDLISTOFFSET);
    mFileName          = ::RegisterClipboardFormat(CFSTR_FILENAME);
    mFileNameW         = ::RegisterClipboardFormat(CFSTR_FILENAMEW);
    mFileNameMap       = ::RegisterClipboardFormat(CFSTR_FILENAMEMAP);
    mFileNameMapW      = ::RegisterClipboardFormat(CFSTR_FILENAMEMAPW);
    mFileContents      = ::RegisterClipboardFormat(CFSTR_FILECONTENTS);
    mFileDescriptorA   = ::RegisterClipboardFormat(CFSTR_FILEDESCRIPTORA); // CFSTR_FILEDESCRIPTOR: Unicode Not Supported
    mFileDescriptorW   = ::RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW); // CFSTR_FILEDESCRIPTOR: Unicode Not Supported
    mInetUrl           = ::RegisterClipboardFormat(CFSTR_INETURL);
}
} // namespace fxfile
