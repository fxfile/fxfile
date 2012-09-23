/*
 *                    Graphics File Library
 *
 *  For Windows & Un*x
 *
 *  GFL library Copyright (c) 1991-2005 Pierre-e Gougelet
 *  All rights reserved
 *
 *
 *  Commercial use is not authorized without agreement
 * 
 *  URL:     http://www.xnview.com
 *  E-Mail : webmaster@xnview.com
 */

#ifndef __GFLC_COLORMAP_H__
#define __GFLC_COLORMAP_H__

class GFLC_COLORMAP {
	public:
		GFLC_COLORMAP(GFL_COLORMAP *cmap = NULL, GFL_INT32 used=256); 
		virtual ~GFLC_COLORMAP(void); 
		
		// ~~ 
		
		GFLC_COLORMAP(const GFLC_COLORMAP & other); 
		GFLC_COLORMAP & operator= (const GFLC_COLORMAP & other); 
		
		// ~~ 
		
		void      set(GFL_COLORMAP *cmap=NULL, GFL_INT32 used=256); 
		
		GFL_INT32	getNumberOfColors(void) const; 

		GFL_UINT8 getRedAt(GFL_INT32 index) const; 
		GFL_UINT8 getGreenAt(GFL_INT32 index) const; 
		GFL_UINT8 getBlueAt(GFL_INT32 index) const; 
		
	private:
		GFL_COLORMAP
			* m_colorMap; 
		GFL_INT32
			m_colorUsed; 
}; 

// INLINE

inline GFLC_COLORMAP :: GFLC_COLORMAP(
	GFL_COLORMAP *cmap, 
	GFL_INT32 used
	) :
	m_colorMap(cmap), 
	m_colorUsed(used)
{
}

// ~

inline GFLC_COLORMAP :: ~GFLC_COLORMAP(
	void
	)
{
}

// ~

inline GFLC_COLORMAP :: GFLC_COLORMAP(
	const GFLC_COLORMAP & other
	)
{
	m_colorMap = other.m_colorMap; 
	m_colorUsed = other.m_colorUsed; 
}

// ~

inline void GFLC_COLORMAP :: set(
	GFL_COLORMAP *cmap, 
	GFL_INT32 used
	)
{
	m_colorMap = cmap; 
	m_colorUsed = used; 
}

// ~

inline GFL_INT32 GFLC_COLORMAP :: getNumberOfColors(
	void
	) const
{
	return m_colorUsed; 
}

// ~

inline GFL_UINT8 GFLC_COLORMAP :: getRedAt (
	GFL_INT32 index
	) const
{
	ERROR_Check(m_colorMap, ""); 
	ERROR_Check(index < 256, "Bad color index"); 
	
	return m_colorMap->Red[index]; 
}

// ~

inline GFL_UINT8 GFLC_COLORMAP :: getGreenAt (
	GFL_INT32 index
	) const
{
	ERROR_Check(m_colorMap, ""); 
	ERROR_Check(index < 256, "Bad color index"); 
	
	return m_colorMap->Green[index]; 
}

// ~

inline GFL_UINT8 GFLC_COLORMAP :: getBlueAt (
	GFL_INT32 index
	) const
{
	ERROR_Check(m_colorMap, ""); 
	ERROR_Check(index < 256, "Bad color index"); 
	
	return m_colorMap->Blue[index]; 
}

#endif
