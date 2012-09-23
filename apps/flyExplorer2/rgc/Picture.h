// 
// Picture.h: interface for the CPicture class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTURE_H__34AF39DA_2789_4716_88F7_88904B4717EE__INCLUDED_)
#define AFX_PICTURE_H__34AF39DA_2789_4716_88F7_88904B4717EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPicture  
{
public:
	CPicture();
	virtual ~CPicture();
	HRESULT   LoadFromFile(LPCTSTR lpFilePath);
	HRESULT   LoadFromIStream(LPSTREAM pStream, LONG lSize = 0);
	UINT      Width(){ return m_Width; }
	UINT      Height(){ return m_Height; }
	void      Reset();
	void      Draw(CDC * pDC, UINT x = 0, UINT y = 0, UINT w = 0, UINT h = 0);
	
	LPPICTURE m_pPict;
	
private:
	HRESULT CreateStreamOnFile(LPCTSTR pszPath, LPSTREAM* ppstm, LONG* plSize);
	
	UINT    m_Width;
	UINT    m_Height;
};

#endif // !defined(AFX_PICTURE_H__34AF39DA_2789_4716_88F7_88904B4717EE__INCLUDED_)
