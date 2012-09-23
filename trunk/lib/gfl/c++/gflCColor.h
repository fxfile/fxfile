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

#ifndef __GFLC_COLOR_H__
#define __GFLC_COLOR_H__

class GFLC_COLOR {
	public:
		GFLC_COLOR(); 
		GFLC_COLOR(GFL_UINT16 red, GFL_UINT16 green, GFL_UINT16 blue, GFL_UINT16 alpha=0); 
		virtual ~GFLC_COLOR(void); 
		
		// ~~ 
		
		GFLC_COLOR(const GFLC_COLOR & other); 
		GFLC_COLOR & operator= (const GFLC_COLOR & other); 
		
		// ~~ 
		
		GFL_UINT16 operator[](int index) const; 
		GFL_UINT16 operator[](int index); 

		void				set(GFL_UINT16 red, GFL_UINT16 green, GFL_UINT16 blue, GFL_UINT16 alpha=0); 
		
		GFL_UINT16	getRed(void) const; 
		GFL_UINT16	getGreen(void) const; 
		GFL_UINT16	getBlue(void) const; 
		GFL_UINT16	getAlpha(void) const; 

		void				setRed(GFL_UINT16 red); 
		void				setGreen(GFL_UINT16 green); 
		void				setBlue(GFL_UINT16 blue); 
		void				setAlpha(GFL_UINT16 alpha); 
		
		GFL_COLOR		getGFLColor(void) const; 

	private:
		GFL_UINT16 
			m_redComponent, m_greenComponent, m_blueComponent, m_alphaComponent; 
}; 

// INLINE

inline GFLC_COLOR :: GFLC_COLOR(
	) :
	m_redComponent(0), 
	m_greenComponent(0), 
	m_blueComponent(0),
	m_alphaComponent(0)
{
}

// ~

inline GFLC_COLOR :: GFLC_COLOR(
	GFL_UINT16 red, 
	GFL_UINT16 green, 
	GFL_UINT16 blue, 
	GFL_UINT16 alpha
	) :
	m_redComponent(red), 
	m_greenComponent(green), 
	m_blueComponent(blue),
	m_alphaComponent(alpha)
{
}

// ~

inline GFLC_COLOR :: ~GFLC_COLOR(
	void
	)
{
}

// ~

inline GFLC_COLOR :: GFLC_COLOR(
	const GFLC_COLOR & other
	)
{
	m_redComponent = other.m_redComponent; 
	m_greenComponent = other.m_greenComponent; 
	m_blueComponent = other.m_blueComponent; 
	m_alphaComponent = other.m_alphaComponent; 
}

// ~

inline GFLC_COLOR & GFLC_COLOR :: operator= (
	const GFLC_COLOR & other
	)
{
	if (this != &other)
	{
		m_redComponent = other.m_redComponent; 
		m_greenComponent = other.m_greenComponent; 
		m_blueComponent = other.m_blueComponent; 
		m_alphaComponent = other.m_alphaComponent; 
	}

	return *this; 
}

// ~

inline void GFLC_COLOR :: set(
	GFL_UINT16 red, 
	GFL_UINT16 green, 
	GFL_UINT16 blue,
	GFL_UINT16 alpha
	)
{
	m_redComponent = red; 
	m_greenComponent = green; 
	m_blueComponent = blue; 
	m_alphaComponent = alpha; 
}

// ~

inline GFL_UINT16 GFLC_COLOR :: operator[](
	int index
	) const
{
	if (index == 0)
		return m_redComponent; 
	if (index == 1)
		return m_greenComponent; 
	if (index == 2)
		return m_blueComponent; 
	if (index == 3)
		return m_alphaComponent; 
	ERROR_Check(0, ""); 
}

// ~

inline GFL_UINT16 GFLC_COLOR :: operator[](
	int index
	)
{	
	if (index == 0)
		return m_redComponent; 
	if (index == 1)
		return m_greenComponent; 
	if (index == 2)
		return m_blueComponent; 
	if (index == 3)
		return m_alphaComponent; 
	ERROR_Check(0, ""); 
}

// ~

inline GFL_UINT16 GFLC_COLOR :: getRed (
	void
	) const
{
	return m_redComponent; 
}

// ~

inline GFL_UINT16 GFLC_COLOR :: getGreen (
	void
	) const
{
	return m_greenComponent; 
}

// ~

inline GFL_UINT16 GFLC_COLOR :: getBlue (
	void
	) const
{
	return m_blueComponent; 
}

// ~

inline GFL_UINT16 GFLC_COLOR :: getAlpha (
	void
	) const
{
	return m_alphaComponent; 
}

// ~

inline void GFLC_COLOR :: setRed (
	GFL_UINT16 value
	)
{
	m_redComponent = value; 
}

// ~

inline void GFLC_COLOR :: setGreen (
	GFL_UINT16 value
	)
{
	m_greenComponent = value; 
}

// ~

inline void GFLC_COLOR :: setBlue (
	GFL_UINT16 value
	)
{
	m_blueComponent = value; 
}

// ~

inline void GFLC_COLOR :: setAlpha (
	GFL_UINT16 value
	)
{
	m_alphaComponent = value; 
}

// ~

inline GFL_COLOR GFLC_COLOR :: getGFLColor (
	void
	) const
{
	GFL_COLOR
		gfl_color; 

	gfl_color.Red	= m_redComponent; 
	gfl_color.Green = m_greenComponent; 
	gfl_color.Blue	= m_blueComponent; 
	gfl_color.Alpha = m_alphaComponent; 
	return gfl_color; 
}

#endif
