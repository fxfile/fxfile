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

#include "gflwColormap.h"

#ifndef __GFLW_BITMAP_H__
#define __GFLW_BITMAP_H__

class GFLW_BITMAP {
	public:
		GFLW_BITMAP(void); 
		GFLW_BITMAP(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_INT32 padding=1); 
		virtual ~GFLW_BITMAP(void); 

		GFLW_BITMAP(const GFLW_BITMAP & other); 
		GFLW_BITMAP & operator= (const GFLW_BITMAP & other); 
		
		GFL_BITMAP_TYPE		GetType(void) const; 
		GFL_ORIGIN				GetOrigin(void) const; 
		GFL_INT32					GetWidth(void) const; 
		GFL_INT32					GetHeight(void) const; 
		GFL_UINT16				GetBytesPerPixel(void) const; 
		GFL_UINT16				GetBitsPerComponent(void) const; 
		GFL_UINT16				GetComponentsPerPixel(void) const; 
		GFL_UINT32				GetBytesPerLine(void) const; 
		GFL_UINT16				GetXdpi(void) const; 
		void              SetXdpi(GFL_UINT16 dpi); 
		GFL_UINT16				GetYdpi(void) const; 
		void              SetYdpi(GFL_UINT16 dpi); 
		GFL_BOOL          HasTransparentIndex(void) const; 
		void              SetHasTransparentIndex(GFL_BOOL flag=GFL_TRUE); 
		GFL_INT16         GetTransparentIndex(void) const; 
		void              SetTransparentIndex(GFL_INT16 index=0); 
		GFL_INT32					GetColorUsed(void) const; 
		GFLW_COLORMAP	&		GetColorMap(void); 
		void *						GetDataPointer(void); 

		GFL_BOOL					HasComment(void); 
		const char *      GetComment(void); 
		void							SetComment(const char *comment); 
		
		GFL_BOOL					HasColorMap(void) const; 

		void							Empty(); 
		GFL_ERROR					Create(const GFLW_BITMAP & bitmap); 
		GFL_ERROR					Create(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_UINT16 bits_per_component=8, GFL_INT32 padding=1, const GFL_COLOR *color=NULL); 
		GFL_ERROR					Allocate(const GFLW_BITMAP & bitmap); 
		GFL_ERROR					Allocate(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_UINT16 bits_per_component=8, GFL_INT32 padding=1, const GFL_COLOR *color=NULL); 
		
		GFL_ERROR					LoadFromFile(const char *filename, GFLW_LOAD_PARAMS &params); 
		GFL_ERROR					LoadFromFile(const char *filename, GFLW_LOAD_PARAMS &params, GFLW_FILE_INFORMATION &info); 
		GFL_ERROR					LoadPreviewFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params); 
		GFL_ERROR					LoadPreviewFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params, GFLW_FILE_INFORMATION &info); 
		GFL_ERROR					LoadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params); 
		GFL_ERROR					LoadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params, GFLW_FILE_INFORMATION &info); 
		GFL_ERROR					SaveIntoFile(char *filename, GFLW_SAVE_PARAMS &params); 

		//
/*
		GFL_UINT32				GetNumberOfColorsUsed(void); 
*/
		//

		GFL_ERROR					Resize(GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method=GFL_RESIZE_BILINEAR, GFL_UINT32 flags=0); 
		GFL_ERROR					Resize(GFLW_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method=GFL_RESIZE_BILINEAR, GFL_UINT32 flags=0); 

		//

		GFL_ERROR					ChangeColorDepth(GFL_MODE mode, GFL_MODE_PARAMS option=0); 
		GFL_ERROR					ChangeColorDepth(GFLW_BITMAP &bitmap, GFL_MODE mode, GFL_MODE_PARAMS option=0); 

		//

		GFL_ERROR					FlipVertical    (void); 
		GFL_ERROR					FlipVertical    (GFLW_BITMAP &bitmap); 
		GFL_ERROR					FlipHorizontal  (void); 
		GFL_ERROR					FlipHorizontal  (GFLW_BITMAP &bitmap); 

		//

		GFL_ERROR 				Rotate          (GFL_INT32 angle, const GFL_COLOR *background_color=NULL); 
		GFL_ERROR 				Rotate          (GFLW_BITMAP &bitmap, GFL_INT32 angle, const GFL_COLOR *background_color=NULL); 

		//

		GFL_ERROR 				Crop            (const GFL_RECT &rect); 
		GFL_ERROR 				Crop            (GFLW_BITMAP &bitmap, const GFL_RECT &rect); 
		GFL_ERROR 				AutoCrop        (const GFL_COLOR &color, GFL_INT32 tolerance); 
		GFL_ERROR 				AutoCrop        (GFLW_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance); 
		GFL_ERROR 				AutoCrop2       (const GFL_COLOR &color, GFL_INT32 tolerance); 
		GFL_ERROR 				AutoCrop2       (GFLW_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance); 
		
		//

		GFL_ERROR 				ResizeCanvas    (GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode=GFL_CANVASRESIZE_CENTER, const GFL_COLOR *color=NULL); 
		GFL_ERROR 				ResizeCanvas    (GFLW_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode=GFL_CANVASRESIZE_CENTER, const GFL_COLOR *color=NULL); 

		//
#ifdef __GFLW_GFLE_FUNCTIONS__
		GFL_ERROR 				Negative        (void); 
		GFL_ERROR 				Negative        (GFLW_BITMAP &bitmap); 
		GFL_ERROR 				Brightness      (GFL_INT32 brightness); 
		GFL_ERROR 				Brightness      (GFLW_BITMAP &bitmap, GFL_INT32 brightness); 
		GFL_ERROR 				Contrast        (GFL_INT32 contrast); 
		GFL_ERROR 				Contrast        (GFLW_BITMAP &bitmap, GFL_INT32 contrast); 
		GFL_ERROR 				Gamma           (double gamma); 
		GFL_ERROR 				Gamma           (GFLW_BITMAP &bitmap, double gamma); 

		// Filter

		GFL_ERROR 				Average         (GFL_INT32 filter_size); 
		GFL_ERROR 				Average         (GFLW_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				Soften          (GFL_INT32 percentage); 
		GFL_ERROR 				Soften          (GFLW_BITMAP &bitmap, GFL_INT32 percentage); 
		GFL_ERROR 				Blur            (GFL_INT32 percentage); 
		GFL_ERROR 				Blur            (GFLW_BITMAP &bitmap, GFL_INT32 percentage); 
		GFL_ERROR 				GaussianBlur    (GFL_INT32 filter_size); 
		GFL_ERROR 				GaussianBlur    (GFLW_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				Maximum         (GFL_INT32 filter_size); 
		GFL_ERROR 				Maximum         (GFLW_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				Minimum         (GFL_INT32 filter_size); 
		GFL_ERROR 				Minimum         (GFLW_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				MedianBox       (GFL_INT32 filter_size); 
		GFL_ERROR 				MedianBox       (GFLW_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				MedianCross     (GFL_INT32 filter_size); 
		GFL_ERROR 				MedianCross     (GFLW_BITMAP &bitmap, GFL_INT32 filter_size); 
		GFL_ERROR 				Sharpen         (GFL_INT32 percentage); 
		GFL_ERROR 				Sharpen         (GFLW_BITMAP &bitmap, GFL_INT32 percentage); 
#endif
#ifdef __GFLW_DRAW_FUNCTIONS__
		void							SetColor				(const GFLW_COLOR & color); 
		void							SetFillColor    (const GFLW_COLOR & color); 
		void							SetLineWidth    (GFL_INT32 line_width); 
		void							SetLineStyle    (GFL_LINE_STYLE line_style); 
		GFL_ERROR					DrawPoint				(GFL_INT32 x0, GFL_INT32 y0);
		GFL_ERROR					DrawLine				(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 x1, GFL_INT32 y1);
		GFL_ERROR					DrawPolyline		(const GFL_POINT points[], GFL_INT32 num_points);
		GFL_ERROR					DrawPolygon			(const GFL_POINT points[], GFL_INT32 num_points);
		GFL_ERROR					DrawRectangle		(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height);
		GFL_ERROR					DrawFillRectangle	(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height);
		GFL_ERROR					DrawCircle			(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius);
		GFL_ERROR					DrawFillCircle	(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius);
#endif

	private:
		GFL_BITMAP
			* gflBitmap; 
		GFLW_COLORMAP
			colorMap; 

		GFLW_COLOR
			lineColor, 
			fillColor; 
		GFL_INT32
			lineWidth; 
		GFL_LINE_STYLE
			lineStyle; 
}; 

// INLINE

inline GFL_BOOL GFLW_BITMAP::HasColorMap(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Type == GFL_COLORS ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline GFL_BITMAP_TYPE GFLW_BITMAP::GetType(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Type; 
}

// ~

inline GFL_ORIGIN GFLW_BITMAP::GetOrigin(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Origin; 
}

// ~

inline GFL_INT32 GFLW_BITMAP::GetWidth(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Width; 
}

// ~

inline GFL_INT32 GFLW_BITMAP::GetHeight(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Height; 
}

// ~

inline GFL_UINT16 GFLW_BITMAP::GetBytesPerPixel(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->BytesPerPixel; 
}

// ~

inline GFL_UINT16 GFLW_BITMAP::GetBitsPerComponent(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->BitsPerComponent; 
}

// ~

inline GFL_UINT16 GFLW_BITMAP::GetComponentsPerPixel(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->ComponentsPerPixel; 
}

// ~

inline GFL_UINT32 GFLW_BITMAP::GetBytesPerLine(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->BytesPerLine; 
}

// ~

inline GFL_UINT16 GFLW_BITMAP::GetXdpi(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Xdpi; 
}

// ~

inline void GFLW_BITMAP::SetXdpi(
	GFL_UINT16 dpi
	) 
{
	ERROR_Check(gflBitmap, ""); 
	gflBitmap->Xdpi = dpi; 
}

// ~

inline GFL_UINT16 GFLW_BITMAP::GetYdpi(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Ydpi; 
}

// ~

inline void GFLW_BITMAP::SetYdpi(
	GFL_UINT16 dpi
	) 
{
	ERROR_Check(gflBitmap, ""); 
	gflBitmap->Ydpi = dpi; 
}

// ~

inline GFL_BOOL GFLW_BITMAP::HasTransparentIndex(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->TransparentIndex >= 0 ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline void GFLW_BITMAP::SetHasTransparentIndex(
	GFL_BOOL flag
	) 
{
	ERROR_Check(gflBitmap, ""); 
	gflBitmap->TransparentIndex = flag; 
}

// ~

inline GFL_INT16 GFLW_BITMAP::GetTransparentIndex(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->TransparentIndex >= 0 ? gflBitmap->TransparentIndex : -1; 
}

// ~

inline void GFLW_BITMAP::SetTransparentIndex(
	GFL_INT16 index
	) 
{
	ERROR_Check(gflBitmap, ""); 
	gflBitmap->TransparentIndex = index; 
}

// ~

inline GFL_INT32 GFLW_BITMAP::GetColorUsed(
	void
	) const
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->ColorUsed; 
}

// ~

inline GFLW_COLORMAP & GFLW_BITMAP::GetColorMap(
	void
	)
{
	ERROR_Check(gflBitmap, ""); 
	return colorMap; 
}

// ~

inline void * GFLW_BITMAP :: GetDataPointer(
	void
	)
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Data; 
}

// ~

inline GFL_BOOL	GFLW_BITMAP :: HasComment(
	void
	)
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Comment ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline const char * GFLW_BITMAP :: GetComment(
	void
	)
{
	ERROR_Check(gflBitmap, ""); 
	return gflBitmap->Comment; 
}

// ~

inline void	GFLW_BITMAP :: SetComment(
	const char *comment
	)
{
	ERROR_Check(gflBitmap, ""); 
	gflBitmapSetComment(gflBitmap, comment); 
}

// ~
/*
inline GFL_UINT32 GFLW_BITMAP::GetNumberOfColorsUsed(
	void
	)
{
	ERROR_Check(bitmap, ""); 
	return gflGetNumberOfColorsUsed( Bitmap ); 
}
*/

// ~

inline void GFLW_BITMAP :: SetColor(
	const GFLW_COLOR & color
	)
{
	lineColor = color; 
}

// ~

inline void GFLW_BITMAP :: SetFillColor(
	const GFLW_COLOR & color
	)
{
	fillColor = color; 
}

// ~

inline void GFLW_BITMAP :: SetLineWidth(
	GFL_INT32 line_width
	)
{
	lineWidth = line_width; 
}

// ~

inline void GFLW_BITMAP :: SetLineStyle(
	GFL_LINE_STYLE line_style
	)
{
	lineStyle = line_style; 
}

#endif
