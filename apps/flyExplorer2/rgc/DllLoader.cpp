/////////////////////////////////////////////////////////////////////////////
// DllLoader.cpp: implementation of the CDllLoader class.
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

#include "stdafx.h"
#include "DllLoader.h"

#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDllLoader

CDllLoader::CDllLoader( LPCTSTR lpFileName, bool bAutoFree /*=true*/ )
{
    m_hModule   = ::LoadLibrary( lpFileName );
    m_bAutoFree = bAutoFree;
}

CDllLoader::~CDllLoader()
{
    if ( m_bAutoFree && ( m_hModule != 0 ) )
    {
        VERIFY( ::FreeLibrary( m_hModule ) );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Operations

bool CDllLoader::IsLoaded()
{
    return ( m_hModule != 0 );
}

bool CDllLoader::GetVersion( DWORD* pdwVersion )
{
    DLLGETVERSIONPROC pfDllGetVersion = ( DLLGETVERSIONPROC )GetProcAddress( "DllGetVersion" );
    if ( pfDllGetVersion != 0 )
    {
        DLLVERSIONINFO dvi;
        dvi.cbSize = sizeof( dvi );

        HRESULT hr = pfDllGetVersion( &dvi );
        if ( SUCCEEDED( hr ) )
        {
            *pdwVersion = PACKVERSION( dvi.dwMajorVersion, dvi.dwMinorVersion );
            return true;
        }
    }

    return false;
}

FARPROC CDllLoader::GetProcAddress( LPCSTR lpProcName )
{
    return ( m_hModule != 0 ) ? ::GetProcAddress( m_hModule, lpProcName ) : 0;
}
