// Picture.cpp: implementation of the CPicture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Picture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPicture::CPicture()
: m_Width(0), m_Height(0), m_pPict(NULL)
{
	
}

CPicture::~CPicture()
{
	if ( m_pPict )
		m_pPict->Release();
}

HRESULT CPicture::LoadFromFile(LPCTSTR lpFilePath)
{
	HRESULT   hr;
	LPSTREAM  pStm = NULL;
	LONG      lSize = 0;
	
	ASSERT(lpFilePath != NULL);
	
	hr = CreateStreamOnFile(lpFilePath,&pStm,&lSize);
	if ( FAILED(hr) )
		return hr;
	hr = LoadFromIStream(pStm, lSize);
	if ( FAILED(hr) )
		return hr;
	
	pStm->Release();
	return TRUE;
}

HRESULT CPicture::LoadFromIStream(LPSTREAM pStream, LONG lSize)
{
	HRESULT             hr;
	CDC                 dc;
	OLE_XSIZE_HIMETRIC  w;
	OLE_YSIZE_HIMETRIC  h;
	CSize               size;
	
	ASSERT(pStream != NULL);
	
	if ( m_pPict )
		m_pPict->Release();
	hr = ::OleLoadPicture(pStream,lSize,TRUE,IID_IPicture,(PVOID *)&m_pPict);
	if ( FAILED(hr) )
		return hr;
	
	dc.Attach(::GetDC(NULL));
	
	m_pPict->get_Width(&w);
	m_pPict->get_Height(&h);
	size.cx = w;
	size.cy = h;
	
	dc.HIMETRICtoDP(&size);
	m_Width = size.cx;
	m_Height = size.cy;
	
	::ReleaseDC(NULL,dc.Detach());
	return S_OK;
}

void CPicture::Reset()
{
	m_Width = 0;
	m_Height = 0;
	
	if (m_pPict)
	{
		m_pPict->Release();
		m_pPict = NULL;
	}
}

void CPicture::Draw(CDC *pDC, UINT x, UINT y, UINT w, UINT h)
{
	CRect rc;
	long  hmWidth;
	long  hmHeight;
	
	ASSERT(pDC != NULL);
	ASSERT(m_pPict != NULL);
	
	if ( w == 0 )
		w = m_Width;
	if ( h == 0 )
		h = m_Height;
	rc.SetRect(x,y,x + w, y + h);
	
	m_pPict->get_Width(&hmWidth);
	m_pPict->get_Height(&hmHeight);
	
	m_pPict->Render(pDC->m_hDC,
		rc.left, 
		rc.top,
		rc.Width(), 
		rc.Height(), 
		0, 
		hmHeight - 1,
		hmWidth, 
		-hmHeight, 
		(LPCRECT)rc);
}

HRESULT CPicture::CreateStreamOnFile(LPCTSTR pszPath, LPSTREAM* ppstm, LONG* plSize)
{
	ASSERT(pszPath != NULL);
	ASSERT(ppstm != NULL);
	
	*ppstm = NULL;
	
	if (plSize != NULL)
		*plSize = 0;
	
	CFileStatus fstatus;
	CFile file;
	HRESULT hr = E_FAIL;
	LONG cb;
	
	if (file.Open(pszPath, CFile::modeRead) &&
		file.GetStatus(pszPath, fstatus) &&
		((cb = (ULONG)fstatus.m_size) != -1) )
	{
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, cb);
		LPVOID pvData = NULL;
		
		if (hGlobal != NULL)
		{
			if ((pvData = GlobalLock(hGlobal)) != NULL)
			{
				BOOL bRead = (file.Read(pvData, cb) == (ULONG)cb);
				GlobalUnlock(hGlobal);
				
				if (bRead &&
					SUCCEEDED(hr = CreateStreamOnHGlobal(hGlobal, TRUE, ppstm)))
				{
					ASSERT(*ppstm != NULL);
					if (plSize != NULL)
						*plSize = (ULONG)fstatus.m_size;
					hr = S_OK;
				}
			}
			
			if (FAILED(hr))
				GlobalFree(hGlobal);
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}
	else
	{
		hr = E_ACCESSDENIED;
	}
	
	return hr;
}
