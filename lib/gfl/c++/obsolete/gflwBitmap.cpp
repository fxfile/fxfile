/*
 *                    Graphics File Library
 *
 *  For Windows & Un*x
 *
 *  GFL library Copyright (c) 1991-2001 Pierre-e Gougelet
 *  All rights reserved
 *
 *
 *  Commercial use is not authorized without agreement
 * 
 *  URL:     http://www.xnview.com
 *  E-Mail : webmaster@xnview.com
 */

#include "gflw.h"

GFLW_BITMAP::GFLW_BITMAP(void) :
	gflBitmap(NULL), 
	colorMap()
{
	lineColor = GFLW_COLOR(0, 0, 0); 
	fillColor = GFLW_COLOR(0, 0, 0); 
	lineWidth = 1; 
}

// ~

GFLW_BITMAP::GFLW_BITMAP(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_INT32 padding)
{
	Allocate(type, width, height, 8, padding); 
	colorMap.Set(gflBitmap->ColorMap, gflBitmap->ColorUsed); 
}

// ~

GFLW_BITMAP::~GFLW_BITMAP(void)
{
	Empty(); 
}

// ~

GFLW_BITMAP::GFLW_BITMAP(const GFLW_BITMAP & other)
{
	gflBitmap = NULL; 
	Allocate(other); 
	ERROR_Check(gflBitmap, "No memory"); 
	memcpy(gflBitmap->Data, other.gflBitmap->Data, gflBitmap->Height * gflBitmap->BytesPerLine); 
	colorMap.Set(other.gflBitmap->ColorMap, other.gflBitmap->ColorUsed); 
}

// ~

GFLW_BITMAP & GFLW_BITMAP::operator = (const GFLW_BITMAP & other)
{
	if (this != &other)
	{
		Empty(); 
		Allocate(other); 
		memcpy(gflBitmap->Data, other.gflBitmap->Data, gflBitmap->Height * gflBitmap->BytesPerLine); 
		colorMap.Set(other.gflBitmap->ColorMap, other.gflBitmap->ColorUsed); 
	}
	return *this;
}

// ~

GFL_ERROR	GFLW_BITMAP::Allocate(const GFLW_BITMAP & bitmap)
{
	return Allocate(bitmap.GetType(), bitmap.GetBitsPerComponent(), bitmap.GetWidth(), bitmap.GetHeight()); 
}

// ~

GFL_ERROR	GFLW_BITMAP::Allocate(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_UINT16 bits_per_component, GFL_INT32 padding, const GFL_COLOR * color)
{
	Empty(); 
	if (bits_per_component > 8)
		gflBitmap = gflAllockBitmapEx(type, width, height, bits_per_component, padding, color); 
	else
		gflBitmap = gflAllockBitmap(type, width, height, padding, color); 
	return gflBitmap == NULL ? GFL_ERROR_NO_MEMORY : GFL_NO_ERROR; 
}


// ~

GFL_ERROR	GFLW_BITMAP::Create(const GFLW_BITMAP & bitmap)
{
	return Allocate(bitmap); 
}

// ~

GFL_ERROR	GFLW_BITMAP::Create(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_UINT16 bits_per_component, GFL_INT32 padding, const GFL_COLOR * color)
{
	return Allocate(type, width, height, bits_per_component, padding, color); 
}

// ~

void GFLW_BITMAP::Empty(void)
{
	if (gflBitmap)
		gflFreeBitmap(gflBitmap); 
	gflBitmap = NULL; 
}

// ~

GFL_ERROR GFLW_BITMAP::LoadFromFile(const char *filename, GFLW_LOAD_PARAMS &params, GFLW_FILE_INFORMATION &info)
{
	GFL_ERROR
		error; 

	Empty(); 
	error = gflLoadBitmap(filename, &gflBitmap, &params.params, &info.bitmapInformations); 
	if ( error == GFL_NO_ERROR )
	{
		info.isOk = GFL_TRUE; 
		colorMap.Set(gflBitmap->ColorMap, gflBitmap->ColorUsed); 
	}
	return error; 
}

// ~

GFL_ERROR GFLW_BITMAP::LoadFromFile(const char *filename, GFLW_LOAD_PARAMS &params)
{
	Empty(); 
	return gflLoadBitmap(filename, &gflBitmap, &params.params, NULL); 
}

// ~

GFL_ERROR GFLW_BITMAP::LoadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params, GFLW_FILE_INFORMATION &info)
{
	GFL_ERROR
		error; 

	Empty(); 
	error = gflLoadPreview(filename, width, height, &gflBitmap, &params.params, &info.bitmapInformations); 
	if ( error == GFL_NO_ERROR )
	{
		info.isOk = GFL_TRUE; 
		colorMap.Set(gflBitmap->ColorMap, gflBitmap->ColorUsed); 
	}
	return error; 
}

// ~

GFL_ERROR GFLW_BITMAP::LoadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params)
{
	Empty(); 
	return gflLoadPreview(filename, width, height, &gflBitmap, &params.params, NULL); 
}

// ~

GFL_ERROR GFLW_BITMAP::LoadPreviewFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params, GFLW_FILE_INFORMATION &info)
{
	return LoadThumbnailFromFile(filename, width, height, params, info); 
}

// ~

GFL_ERROR GFLW_BITMAP::LoadPreviewFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFLW_LOAD_PARAMS &params)
{
	return LoadThumbnailFromFile(filename, width, height, params); 
}

// ~

GFL_ERROR GFLW_BITMAP::SaveIntoFile(char *filename, GFLW_SAVE_PARAMS &params)
{
	ERROR_Check(gflBitmap, ""); 
	return gflSaveBitmap(filename, gflBitmap, &params.params); 
}

// ~

GFL_ERROR GFLW_BITMAP::Resize(GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method, GFL_UINT32 flags)
{
	ERROR_Check(gflBitmap, ""); 
	return gflResize( gflBitmap, NULL, width, height, method, flags ); 
}

// ~

GFL_ERROR GFLW_BITMAP::Resize(GFLW_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method, GFL_UINT32 flags)
{
	ERROR_Check(gflBitmap, ""); 
	if ( &bitmap == this )
		return gflResize( gflBitmap, NULL, width, height, method, flags ); 
	return gflResize( gflBitmap, &bitmap.gflBitmap, width, height, method, flags ); 
}

// ~

GFL_ERROR GFLW_BITMAP::ChangeColorDepth(GFL_MODE mode, GFL_MODE_PARAMS option)
{
	GFL_ERROR
		error; 

	ERROR_Check(gflBitmap, ""); 
	error = gflChangeColorDepth( gflBitmap, NULL, mode, option ); 
	colorMap.Set(gflBitmap->ColorMap, gflBitmap->ColorUsed); 
	return error; 
}

// ~

GFL_ERROR GFLW_BITMAP::ChangeColorDepth(GFLW_BITMAP &bitmap, GFL_MODE mode, GFL_MODE_PARAMS option)
{
	GFL_ERROR
		error; 

	ERROR_Check(gflBitmap, ""); 
	if ( &bitmap == this )
	{
		error = gflChangeColorDepth( gflBitmap, NULL, mode, option ); 
		colorMap.Set(gflBitmap->ColorMap, gflBitmap->ColorUsed); 
	}
	else
	{
		error = gflChangeColorDepth( gflBitmap, &bitmap.gflBitmap, mode, option ); 
		colorMap.Set(gflBitmap->ColorMap, gflBitmap->ColorUsed); 
	}
	return error; 
}

// ~

GFL_ERROR GFLW_BITMAP::FlipVertical(void)
{
	ERROR_Check(gflBitmap, ""); 
	return gflFlipVertical( gflBitmap, NULL ); 
}

// ~

GFL_ERROR GFLW_BITMAP::FlipVertical(GFLW_BITMAP &bitmap)
{
	ERROR_Check(gflBitmap, ""); 
	if ( &bitmap == this )
		return gflFlipVertical( gflBitmap, NULL ); 
	return gflFlipVertical( gflBitmap, &bitmap.gflBitmap ); 
}

// ~

GFL_ERROR GFLW_BITMAP::FlipHorizontal(void)
{
	ERROR_Check(gflBitmap, ""); 
	return gflFlipHorizontal( gflBitmap, NULL ); 
}

// ~

GFL_ERROR GFLW_BITMAP::FlipHorizontal(GFLW_BITMAP &bitmap)
{
	ERROR_Check(gflBitmap, ""); 
	if ( &bitmap == this )
		return gflFlipHorizontal( gflBitmap, NULL ); 
	return gflFlipHorizontal( gflBitmap, &bitmap.gflBitmap ); 
}

// ~

GFL_ERROR GFLW_BITMAP::Crop(const GFL_RECT &rect)
{
	ERROR_Check(gflBitmap, ""); 
	return gflCrop(gflBitmap, NULL, &rect); 
}

// ~

GFL_ERROR GFLW_BITMAP::Crop(GFLW_BITMAP &bitmap, const GFL_RECT &rect)
{
	ERROR_Check(gflBitmap, ""); 
	if ( &bitmap == this )
		return gflCrop(gflBitmap, NULL, &rect); 
	return gflCrop(gflBitmap, &bitmap.gflBitmap, &rect); 
}

// ~

GFL_ERROR GFLW_BITMAP::AutoCrop(const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(gflBitmap, ""); 
	return gflAutoCrop(gflBitmap, NULL, &color, tolerance); 
}

// ~

GFL_ERROR GFLW_BITMAP::AutoCrop(GFLW_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(gflBitmap, ""); 
	return gflAutoCrop(gflBitmap, &bitmap.gflBitmap, &color, tolerance); 
}

// ~

GFL_ERROR GFLW_BITMAP::AutoCrop2(const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(gflBitmap, ""); 
	return gflAutoCrop2(gflBitmap, NULL, &color, tolerance); 
}

// ~

GFL_ERROR GFLW_BITMAP::AutoCrop2(GFLW_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(gflBitmap, ""); 
	return gflAutoCrop2(gflBitmap, &bitmap.gflBitmap, &color, tolerance); 
}

// ~

GFL_ERROR GFLW_BITMAP::ResizeCanvas(GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode, const GFL_COLOR *color)
{
	ERROR_Check(gflBitmap, ""); 
	return gflResizeCanvas(gflBitmap, NULL, width, height, mode, color); 
}

// ~

GFL_ERROR GFLW_BITMAP::ResizeCanvas(GFLW_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode, const GFL_COLOR *color)
{
	ERROR_Check(gflBitmap, ""); 
	if ( &bitmap == this )
		return gflResizeCanvas(gflBitmap, NULL, width, height, mode, color); 
	return gflResizeCanvas(gflBitmap, &bitmap.gflBitmap, width, height, mode, color); 
}

// ~

GFL_ERROR GFLW_BITMAP::Rotate(GFL_INT32 angle, const GFL_COLOR *background_color)
{
	ERROR_Check(gflBitmap, ""); 
	return gflRotate(gflBitmap, NULL, angle, background_color); 
}

// ~

GFL_ERROR GFLW_BITMAP::Rotate(GFLW_BITMAP &bitmap, GFL_INT32 angle, const GFL_COLOR *background_color)
{
	ERROR_Check(gflBitmap, ""); 
	if ( &bitmap == this )
		return gflRotate(gflBitmap, NULL, angle, background_color); 
	return gflRotate(gflBitmap, &bitmap.gflBitmap, angle, background_color); 
}

// ~

#ifdef __GFLW_GFLE_FUNCTIONS__

#include "libgfle.h"

GFL_ERROR GFLW_BITMAP::Negative(void)
{
	ERROR_Check(gflBitmap, ""); 
	return gflNegative(gflBitmap, NULL); 
}

// ~

GFL_ERROR GFLW_BITMAP::Negative(GFLW_BITMAP &bitmap)
{
	ERROR_Check(gflBitmap, ""); 
	return gflNegative(gflBitmap, &bitmap.gflBitmap); 
}

// ~

GFL_ERROR GFLW_BITMAP::Brightness(GFL_INT32 brightness)
{
	ERROR_Check(gflBitmap, ""); 
	return gflBrightness(gflBitmap, NULL, brightness); 
}

// ~

GFL_ERROR GFLW_BITMAP::Brightness(GFLW_BITMAP &bitmap, GFL_INT32 brightness)
{
	ERROR_Check(gflBitmap, ""); 
	return gflBrightness(gflBitmap, &bitmap.gflBitmap, brightness); 
}

// ~

GFL_ERROR GFLW_BITMAP::Contrast(GFL_INT32 contrast)
{
	ERROR_Check(gflBitmap, ""); 
	return gflContrast(gflBitmap, NULL, contrast); 
}

// ~

GFL_ERROR GFLW_BITMAP::Contrast(GFLW_BITMAP &bitmap, GFL_INT32 contrast)
{
	ERROR_Check(gflBitmap, ""); 
	return gflContrast(gflBitmap, &bitmap.gflBitmap, contrast); 
}

// ~

GFL_ERROR GFLW_BITMAP::Gamma(double gamma)
{
	ERROR_Check(gflBitmap, ""); 
	return gflGamma(gflBitmap, NULL, gamma); 
}

// ~

GFL_ERROR GFLW_BITMAP::Gamma(GFLW_BITMAP &bitmap, double gamma)
{
	ERROR_Check(gflBitmap, ""); 
	return gflGamma(gflBitmap, &bitmap.gflBitmap, gamma); 
}

// ~

GFL_ERROR GFLW_BITMAP::Average(GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflAverage(gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::Average(GFLW_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflAverage(gflBitmap, &bitmap.gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::Soften(GFL_INT32 percentage)
{
	ERROR_Check(gflBitmap, ""); 
	return gflSoften(gflBitmap, NULL, percentage); 
}

// ~

GFL_ERROR GFLW_BITMAP::Soften(GFLW_BITMAP &bitmap, GFL_INT32 percentage)
{
	ERROR_Check(gflBitmap, ""); 
	return gflSoften(gflBitmap, &bitmap.gflBitmap, percentage); 
}

// ~

GFL_ERROR GFLW_BITMAP::Blur(GFL_INT32 percentage)
{
	ERROR_Check(gflBitmap, ""); 
	return gflBlur(gflBitmap, NULL, percentage); 
}

// ~

GFL_ERROR GFLW_BITMAP::Blur(GFLW_BITMAP &bitmap, GFL_INT32 percentage)
{
	ERROR_Check(gflBitmap, ""); 
	return gflBlur(gflBitmap, &bitmap.gflBitmap, percentage); 
}

// ~

GFL_ERROR GFLW_BITMAP::GaussianBlur(GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflGaussianBlur(gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::GaussianBlur(GFLW_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflGaussianBlur(gflBitmap, &bitmap.gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::Maximum(GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMaximum(gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::Maximum(GFLW_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMaximum(gflBitmap, &bitmap.gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::Minimum(GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMinimum(gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::Minimum(GFLW_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMinimum(gflBitmap, &bitmap.gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::MedianBox(GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMedianBox(gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::MedianBox(GFLW_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMedianBox(gflBitmap, &bitmap.gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::MedianCross(GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMedianCross(gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::MedianCross(GFLW_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(gflBitmap, ""); 
	return gflMedianCross(gflBitmap, &bitmap.gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLW_BITMAP::Sharpen(GFL_INT32 percentage)
{
	ERROR_Check(gflBitmap, ""); 
	return gflSharpen(gflBitmap, NULL, percentage); 
}

// ~

GFL_ERROR GFLW_BITMAP::Sharpen(GFLW_BITMAP &bitmap, GFL_INT32 percentage)
{
	ERROR_Check(gflBitmap, ""); 
	return gflSharpen(gflBitmap, &bitmap.gflBitmap, percentage); 
}

#endif

#ifdef __GFLW_DRAW_FUNCTIONS__

#include "libgfle.h"

GFL_ERROR	GFLW_BITMAP::DrawPoint(GFL_INT32 x0, GFL_INT32 y0)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawPointColor(gflBitmap, x0, y0, lineWidth, &lineColor.GetGFLColor(), NULL); 
}

// ~

GFL_ERROR	GFLW_BITMAP::DrawLine(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 x1, GFL_INT32 y1)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawLineColor(gflBitmap, x0, y0, x1, y1, lineWidth, &lineColor.GetGFLColor(), lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLW_BITMAP::DrawPolyline(const GFL_POINT points[], GFL_INT32 num_points)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawPolylineColor(gflBitmap, points, num_points, lineWidth, &lineColor.GetGFLColor(), lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLW_BITMAP::DrawPolygon(const GFL_POINT points[], GFL_INT32 num_points)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawPolygonColor(gflBitmap, points, num_points, &fillColor.GetGFLColor(), lineWidth, &lineColor.GetGFLColor(), lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLW_BITMAP::DrawRectangle(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawRectangleColor(gflBitmap, x0, y0, width, height, NULL, lineWidth, &lineColor.GetGFLColor(), lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLW_BITMAP::DrawFillRectangle(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawRectangleColor(gflBitmap, x0, y0, width, height, &fillColor.GetGFLColor(), lineWidth, &lineColor.GetGFLColor(), lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLW_BITMAP::DrawCircle(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawCircleColor(gflBitmap, x, y, radius, NULL, lineWidth, &lineColor.GetGFLColor(), lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLW_BITMAP::DrawFillCircle(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius)
{
	ERROR_Check(gflBitmap, ""); 
	return gflDrawCircleColor(gflBitmap, x, y, radius, &fillColor.GetGFLColor(), lineWidth, &lineColor.GetGFLColor(), lineStyle, NULL); 
}

#endif
