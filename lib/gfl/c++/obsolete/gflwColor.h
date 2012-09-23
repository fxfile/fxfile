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

#ifndef __GFLW_COLOR_H__
#define __GFLW_COLOR_H__

class GFLW_COLOR {
	public:
		GFLW_COLOR(); 
		GFLW_COLOR(GFL_UINT8 red, GFL_UINT8 green, GFL_UINT8 blue); 
		virtual ~GFLW_COLOR(void); 
		
		GFLW_COLOR(const GFLW_COLOR & other); 
		GFLW_COLOR & operator= (const GFLW_COLOR & other); 
		
		void      Set(GFL_UINT8 red, GFL_UINT8 green, GFL_UINT8 blue); 
		
		GFL_UINT8 GetRed(void) const; 
		GFL_UINT8 GetGreen(void) const; 
		GFL_UINT8 GetBlue(void) const; 
		
		GFL_COLOR GetGFLColor(void) const; 

	private:
		GFL_UINT8 
			redComponent, greenComponent, blueComponent; 
}; 

// INLINE

inline GFLW_COLOR :: GFLW_COLOR(
	) :
	redComponent(0), 
	greenComponent(0), 
	blueComponent(0)
{
}

// ~

inline GFLW_COLOR :: GFLW_COLOR(
	GFL_UINT8 red, 
	GFL_UINT8 green, 
	GFL_UINT8 blue
	) :
	redComponent(red), 
	greenComponent(green), 
	blueComponent(blue)
{
}

// ~

inline GFLW_COLOR :: ~GFLW_COLOR(
	void
	)
{
}

// ~

inline GFLW_COLOR :: GFLW_COLOR(
	const GFLW_COLOR & other
	)
{
	redComponent = other.redComponent; 
	greenComponent = other.greenComponent; 
	blueComponent = other.blueComponent; 
}


// ~

inline GFLW_COLOR & GFLW_COLOR :: operator= (
	const GFLW_COLOR & other
	)
{
	if (this != &other)
	{
		redComponent = other.redComponent; 
		greenComponent = other.greenComponent; 
		blueComponent = other.blueComponent; 
	}

	return *this; 
}

// ~

inline void GFLW_COLOR :: Set(
	GFL_UINT8 red, 
	GFL_UINT8 green, 
	GFL_UINT8 blue
	)
{
	redComponent = red; 
	greenComponent = green; 
	blueComponent = blue; 
}

// ~

inline GFL_UINT8 GFLW_COLOR :: GetRed (
	void
	) const
{
	return redComponent; 
}

// ~

inline GFL_UINT8 GFLW_COLOR :: GetGreen (
	void
	) const
{
	return greenComponent; 
}

// ~

inline GFL_UINT8 GFLW_COLOR :: GetBlue (
	void
	) const
{
	return blueComponent; 
}

// ~

inline GFL_COLOR GFLW_COLOR :: GetGFLColor (
	void
	) const
{
	GFL_COLOR
		gfl_color; 

	gfl_color.Red		= redComponent; 
	gfl_color.Green = greenComponent; 
	gfl_color.Blue	= blueComponent; 
	return gfl_color; 
}

#endif
