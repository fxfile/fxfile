/*
 *                    Graphics File Library
 *
 *  For Windows & Un*x
 *
 *  GFL library Copyright (c) 1991-2003 Pierre-e Gougelet
 *  All rights reserved
 *
 *
 *  Commercial use is not authorized without agreement
 * 
 *  URL:     http://www.xnview.com
 *  E-Mail : webmaster@xnview.com
 */

#ifndef __GFLW_COLORMAP_H__
#define __GFLW_COLORMAP_H__

class GFLW_COLORMAP {
	public:
		GFLW_COLORMAP(GFL_COLORMAP *cmap = NULL, GFL_INT32 used=256); 
		virtual ~GFLW_COLORMAP(void); 
		
		GFLW_COLORMAP(const GFLW_COLORMAP & other); 
		GFLW_COLORMAP & operator= (const GFLW_COLORMAP & other); 
		
		void      Set(GFL_COLORMAP *cmap = NULL, GFL_INT32 used=256); 
		
		GFL_INT32	GetNumberOfColors(void) const; 

		GFL_UINT8 GetRedAt(GFL_INT32 index) const; 
		GFL_UINT8 GetGreenAt(GFL_INT32 index) const; 
		GFL_UINT8 GetBlueAt(GFL_INT32 index) const; 
		
	private:
		GFL_COLORMAP
			* colorMap; 
		GFL_INT32
			colorUsed; 
}; 

// INLINE

inline GFLW_COLORMAP :: GFLW_COLORMAP(
	GFL_COLORMAP *cmap, 
	GFL_INT32 used
	) :
	colorMap(cmap), 
	colorUsed(used)
{
}

// ~

inline GFLW_COLORMAP :: ~GFLW_COLORMAP(
	void
	)
{
}

// ~

inline GFLW_COLORMAP :: GFLW_COLORMAP(
	const GFLW_COLORMAP & other
	)
{
	colorMap = other.colorMap; 
	colorUsed = other.colorUsed; 
}

// ~

inline void GFLW_COLORMAP :: Set(
	GFL_COLORMAP *cmap, 
	GFL_INT32 used
	)
{
	colorMap = cmap; 
	colorUsed = used; 
}

// ~

inline GFL_INT32 GFLW_COLORMAP :: GetNumberOfColors(
	void
	) const
{
	return colorUsed; 
}

// ~

inline GFL_UINT8 GFLW_COLORMAP :: GetRedAt (
	GFL_INT32 index
	) const
{
	ERROR_Check(colorMap, ""); 
	ERROR_Check(index < 256, "Bad color index"); 
	
	return colorMap->Red[index]; 
}

// ~

inline GFL_UINT8 GFLW_COLORMAP :: GetGreenAt (
	GFL_INT32 index
	) const
{
	ERROR_Check(colorMap, ""); 
	ERROR_Check(index < 256, "Bad color index"); 
	
	return colorMap->Green[index]; 
}

// ~

inline GFL_UINT8 GFLW_COLORMAP :: GetBlueAt (
	GFL_INT32 index
	) const
{
	ERROR_Check(colorMap, ""); 
	ERROR_Check(index < 256, "Bad color index"); 
	
	return colorMap->Blue[index]; 
}

#endif
