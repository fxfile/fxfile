/////////////////////////////////////////////////////////////////////////////
// DllLoader.h: interface for the CDllLoader class.
//
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001-2002 by Nikolay Denisov. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Please email bug reports, bug fixes, enhancements, requests and
// comments to: acnick@mail.lanck.net
/////////////////////////////////////////////////////////////////////////////

#ifndef __DLLLOADER_H__
#define __DLLLOADER_H___

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDllLoader

#define PACKVERSION( major, minor ) MAKELONG( minor, major )

class CDllLoader
{
// Constructors
public:
    CDllLoader( LPCTSTR lpFileName, bool bAutoFree = true );
    ~CDllLoader();

// Operations
public:
    bool IsLoaded();
    bool GetVersion( DWORD* pdwVersion );
    FARPROC GetProcAddress( LPCSTR lpProcName );

// Implementation data
protected:
    HMODULE m_hModule;
    bool    m_bAutoFree;
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__DLLLOADER_H___
