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

#include "gflCColormap.h"

#ifndef __GFLC_BITMAP_H__
#define __GFLC_BITMAP_H__

class GFLC_BITMAP {
	public:
		GFLC_BITMAP(void); 
		GFLC_BITMAP(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_INT32 padding=4); 
		virtual ~GFLC_BITMAP(void); 
		
		// ~~ 
		
		GFLC_BITMAP(const GFLC_BITMAP & other); 
		GFLC_BITMAP & operator=(const GFLC_BITMAP & other); 
		
		// ~~ 
		
		GFL_BITMAP_TYPE		getType(void) const; 
		GFL_ORIGIN				getOrigin(void) const; 
		GFL_INT32					getWidth(void) const; 
		GFL_INT32					getHeight(void) const; 
		GFL_UINT16				getBytesPerPixel(void) const; 
		GFL_UINT16				getBitsPerComponent(void) const; 
		GFL_UINT16				getComponentsPerPixel(void) const; 
		GFL_UINT32				getBytesPerLine(void) const; 
		GFL_UINT16				getXdpi(void) const; 
		void							setXdpi(GFL_UINT16 dpi); 
		GFL_UINT16				getYdpi(void) const; 
		void							setYdpi(GFL_UINT16 dpi); 
		GFL_BOOL          hasTransparentIndex(void) const; 
		GFL_INT16         getTransparentIndex(void) const; 
		void              setTransparentIndex(GFL_INT16 index=0); 
		GFL_INT32					getColorUsed(void) const; 
		GFLC_COLORMAP	&		getColorMap(void); 

		GFL_BOOL					hasComment(void) const; 
		const char *			getComment(void); 
		void							setComment(const char *comment); 
		
		GFL_BOOL					hasColorMap(void) const; 
		
		// ~~ 
		
		void *						getDataPtr(void); 
		void *						getLinePtr(GFL_INT32 y); 
		void *						getPixelPtr(GFL_INT32 y, GFL_INT32 x); 
		void							getPixel(GFL_INT32 x, GFL_INT32 y, GFLC_COLOR & color); 
		void							setPixel(GFL_INT32 x, GFL_INT32 y, const GFLC_COLOR & color); 
		
		// ~~ 
		
		void							clear(void); 
		GFL_ERROR					create(const GFLC_BITMAP & bitmap); 
		GFL_ERROR					create(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_UINT16 bits_per_component=8, GFL_INT32 padding=1, const GFL_COLOR *color=NULL); 
		
		GFL_ERROR					loadFromFile(const char *filename, const char *name=NULL); 
		GFL_ERROR					loadFromFile(const char *filename, GFL_INT32 index); 
		GFL_ERROR					loadFromFile(const char *filename, const GFLC_LOAD_PARAMS &params); 
		GFL_ERROR					loadFromFile(const char *filename, const GFLC_LOAD_PARAMS &params, GFLC_FILE_INFORMATION &info); 

		GFL_ERROR					loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, const char *name=NULL); 
		GFL_ERROR					loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFL_INT32 index); 
		GFL_ERROR					loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, const GFLC_LOAD_PARAMS &params); 
		GFL_ERROR					loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, const GFLC_LOAD_PARAMS &params, GFLC_FILE_INFORMATION &info); 

		GFL_ERROR					saveIntoFile(const char *filename, const char *name="jpeg") const; 
		GFL_ERROR					saveIntoFile(const char *filename, GFL_INT32 index) const; 
		GFL_ERROR					saveIntoFile(const char *filename, const GFLC_SAVE_PARAMS &params) const; 

		//
/*
		GFL_UINT32				getNumberOfColorsUsed(void); 
*/
		//

		GFL_ERROR					resize(GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method=GFL_RESIZE_BILINEAR, GFL_UINT32 flags=0); 
		GFL_ERROR					resize(GFLC_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method=GFL_RESIZE_BILINEAR, GFL_UINT32 flags=0); 

		//

		GFL_ERROR					changeColorDepth(GFL_MODE mode, GFL_MODE_PARAMS option=0); 
		GFL_ERROR					changeColorDepth(GFLC_BITMAP &bitmap, GFL_MODE mode, GFL_MODE_PARAMS option=0); 

		//

		GFL_ERROR					flipVertical    (void); 
		GFL_ERROR					flipVertical    (GFLC_BITMAP &bitmap); 
		GFL_ERROR					flipHorizontal  (void); 
		GFL_ERROR					flipHorizontal  (GFLC_BITMAP &bitmap); 

		//

		GFL_ERROR 				rotate          (GFL_INT32 angle, const GFL_COLOR *background_color=NULL); 
		GFL_ERROR 				rotate          (GFLC_BITMAP &bitmap, GFL_INT32 angle, const GFL_COLOR *background_color=NULL); 

		//

		GFL_ERROR 				crop            (const GFL_RECT &rect); 
		GFL_ERROR 				crop            (GFLC_BITMAP &bitmap, const GFL_RECT &rect); 
		GFL_ERROR 				autoCrop        (const GFL_COLOR &color, GFL_INT32 tolerance); 
		GFL_ERROR 				autoCrop        (GFLC_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance); 
		GFL_ERROR 				autoCrop2       (const GFL_COLOR &color, GFL_INT32 tolerance); 
		GFL_ERROR 				autoCrop2       (GFLC_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance); 
		
		//

		GFL_ERROR 				resizeCanvas    (GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode=GFL_CANVASRESIZE_CENTER, const GFL_COLOR *color=NULL); 
		GFL_ERROR 				resizeCanvas    (GFLC_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode=GFL_CANVASRESIZE_CENTER, const GFL_COLOR *color=NULL); 

		//
#ifdef __GFLC_GFLE_FUNCTIONS__
		GFL_ERROR 				negative        (void); 
		GFL_ERROR 				negative        (GFLC_BITMAP &bitmap); 
		GFL_ERROR 				brightness      (GFL_INT32 brightness); 
		GFL_ERROR 				brightness      (GFLC_BITMAP &bitmap, GFL_INT32 brightness); 
		GFL_ERROR 				contrast        (GFL_INT32 contrast); 
		GFL_ERROR 				contrast        (GFLC_BITMAP &bitmap, GFL_INT32 contrast); 
		GFL_ERROR 				gamma           (double gamma); 
		GFL_ERROR 				gamma           (GFLC_BITMAP &bitmap, double gamma); 

		// Filter

		GFL_ERROR 				average         (GFL_INT32 filter_size); 
		GFL_ERROR 				average         (GFLC_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				soften          (GFL_INT32 percentage); 
		GFL_ERROR 				soften          (GFLC_BITMAP &bitmap, GFL_INT32 percentage); 
		GFL_ERROR 				blur            (GFL_INT32 percentage); 
		GFL_ERROR 				blur            (GFLC_BITMAP &bitmap, GFL_INT32 percentage); 
		GFL_ERROR 				gaussianBlur    (GFL_INT32 filter_size); 
		GFL_ERROR 				gaussianBlur    (GFLC_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				maximum         (GFL_INT32 filter_size); 
		GFL_ERROR 				maximum         (GFLC_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				minimum         (GFL_INT32 filter_size); 
		GFL_ERROR 				minimum         (GFLC_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				medianBox       (GFL_INT32 filter_size); 
		GFL_ERROR 				medianBox       (GFLC_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				medianCross     (GFL_INT32 filter_size); 
		GFL_ERROR 				medianCross     (GFLC_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				sharpen         (GFL_INT32 percentage); 
		GFL_ERROR 				sharpen         (GFLC_BITMAP &bitmap, GFL_INT32 percentage); 
#endif
#ifdef __GFLC_DRAW_FUNCTIONS__
		void							setColor				(const GFLC_COLOR & color); 
		void							setFillColor		(const GFLC_COLOR & color); 
		void							setLineWidth		(GFL_INT32 line_width); 
		void							setLineStyle		(GFL_LINE_STYLE line_style); 

		GFL_ERROR					drawPoint				(GFL_INT32 x0, GFL_INT32 y0);
		GFL_ERROR					drawLine				(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 x1, GFL_INT32 y1);
		GFL_ERROR					drawPolyline		(const GFL_POINT points[], GFL_INT32 num_points);
		GFL_ERROR					drawPolygon			(const GFL_POINT points[], GFL_INT32 num_points);
		GFL_ERROR					drawRectangle		(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height);
		GFL_ERROR					drawFillRectangle	(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height);
		GFL_ERROR					drawCircle			(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius);
		GFL_ERROR					drawFillCircle	(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius);
#endif

	private:
		GFL_BITMAP
			* m_gflBitmap; 
		GFLC_COLORMAP
			m_colorMap; 

		void
			* m_lastPtr; 
		GFL_INT32
			m_lastY; 

#ifdef __GFLC_DRAW_FUNCTIONS__
		GFLC_COLOR
			m_lineColor, 
			m_fillColor; 
		GFL_INT32
			m_lineWidth; 
		GFL_LINE_STYLE
			m_lineStyle; 
#endif
}; 

// INLINE

inline GFL_BOOL GFLC_BITMAP::hasColorMap(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Type == GFL_COLORS ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline GFL_BITMAP_TYPE GFLC_BITMAP::getType(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Type; 
}

// ~

inline GFL_ORIGIN GFLC_BITMAP::getOrigin(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Origin; 
}

// ~

inline GFL_INT32 GFLC_BITMAP::getWidth(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Width; 
}

// ~

inline GFL_INT32 GFLC_BITMAP::getHeight(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Height; 
}

// ~

inline GFL_UINT16 GFLC_BITMAP::getBytesPerPixel(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->BytesPerPixel; 
}

// ~

inline GFL_UINT16 GFLC_BITMAP::getBitsPerComponent(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->BitsPerComponent; 
}

// ~

inline GFL_UINT16 GFLC_BITMAP::getComponentsPerPixel(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->ComponentsPerPixel; 
}

// ~

inline GFL_UINT32 GFLC_BITMAP::getBytesPerLine(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->BytesPerLine; 
}

// ~

inline GFL_UINT16 GFLC_BITMAP::getXdpi(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Xdpi; 
}

// ~

inline void GFLC_BITMAP::setXdpi(
	GFL_UINT16 dpi
	) 
{
	ERROR_Check(m_gflBitmap, ""); 
	m_gflBitmap->Xdpi = dpi; 
}

// ~

inline GFL_UINT16 GFLC_BITMAP::getYdpi(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Ydpi; 
}

// ~

inline void GFLC_BITMAP::setYdpi(
	GFL_UINT16 dpi
	) 
{
	ERROR_Check(m_gflBitmap, ""); 
	m_gflBitmap->Ydpi = dpi; 
}

// ~

inline GFL_BOOL GFLC_BITMAP::hasTransparentIndex(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->TransparentIndex >= 0 ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline GFL_INT16 GFLC_BITMAP::getTransparentIndex(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->TransparentIndex >= 0 ? m_gflBitmap->TransparentIndex : -1; 
}

// ~

inline void GFLC_BITMAP::setTransparentIndex(
	GFL_INT16 index
	) 
{
	ERROR_Check(m_gflBitmap, ""); 
	m_gflBitmap->TransparentIndex = index; 
}

// ~

inline GFL_INT32 GFLC_BITMAP::getColorUsed(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->ColorUsed; 
}

// ~

inline GFLC_COLORMAP & GFLC_BITMAP::getColorMap(
	void
	)
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_colorMap; 
}

// ~

inline void * GFLC_BITMAP :: getDataPtr(
	void
	)
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Data; 
}

// ~

inline GFL_BOOL	GFLC_BITMAP :: hasComment(
	void
	) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Comment ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline const char * GFLC_BITMAP :: getComment(
	void
	)
{
	ERROR_Check(m_gflBitmap, ""); 
	return m_gflBitmap->Comment; 
}

// ~

inline void	GFLC_BITMAP :: setComment(
	const char *comment
	)
{
	ERROR_Check(m_gflBitmap, ""); 
	gflBitmapSetComment(m_gflBitmap, comment); 
}

// ~
/*
inline GFL_UINT32 GFLC_BITMAP::getNumberOfColorsUsed(
	void
	)
{
	ERROR_Check(bitmap, ""); 
	return gflGetNumberOfColorsUsed( Bitmap ); 
}
*/

// ~

#ifdef __GFLC_DRAW_FUNCTIONS__

inline void GFLC_BITMAP :: setColor(
	const GFLC_COLOR & color
	)
{
	m_lineColor = color; 
}

// ~

inline void GFLC_BITMAP :: setFillColor(
	const GFLC_COLOR & color
	)
{
	m_fillColor = color; 
}

// ~

inline void GFLC_BITMAP :: setLineWidth(
	GFL_INT32 line_width
	)
{
	m_lineWidth = line_width; 
}

// ~

inline void GFLC_BITMAP :: setLineStyle(
	GFL_LINE_STYLE line_style
	)
{
	m_lineStyle = line_style; 
}

#endif

#endif
