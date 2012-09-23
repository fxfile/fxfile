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

#include "gflC.h"

GFLC_BITMAP::GFLC_BITMAP(void) :
	m_gflBitmap(NULL), 
	m_colorMap(), 
	m_lastPtr(NULL), 
	m_lastY(-1)
{
#ifdef __GFLC_DRAW_FUNCTIONS__
	m_lineColor = GFLC_COLOR(0, 0, 0); 
	m_fillColor = GFLC_COLOR(0, 0, 0); 
	m_lineWidth = 1; 
#endif
}

// ~

GFLC_BITMAP::GFLC_BITMAP(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_INT32 padding) :
	m_gflBitmap(NULL), 
	m_colorMap(),
	m_lastPtr(NULL), 
	m_lastY(-1)
{
	create(type, width, height, 8, padding); 
	m_colorMap.set(m_gflBitmap->ColorMap, m_gflBitmap->ColorUsed); 
}

// ~

GFLC_BITMAP::~GFLC_BITMAP(void)
{
	clear(); 
}

// ~

GFLC_BITMAP::GFLC_BITMAP(const GFLC_BITMAP & other)
{
	m_gflBitmap = NULL; 
	create(other); 
	ERROR_Check(m_gflBitmap, "No memory"); 
	memcpy(m_gflBitmap->Data, other.m_gflBitmap->Data, m_gflBitmap->Height * m_gflBitmap->BytesPerLine); 
	m_colorMap.set(other.m_gflBitmap->ColorMap, other.m_gflBitmap->ColorUsed); 
#ifdef __GFLC_DRAW_FUNCTIONS__
	m_lineColor = other.m_lineColor; 
	m_fillColor = other.m_fillColor; 
	m_lineWidth = other.m_lineWidth; 
#endif

	m_lastPtr = NULL; 
	m_lastY = -1; 
}

// ~

GFLC_BITMAP & GFLC_BITMAP::operator = (const GFLC_BITMAP & other)
{
	if (this != &other)
	{
		clear(); 
		create(other); 
		memcpy(m_gflBitmap->Data, other.m_gflBitmap->Data, m_gflBitmap->Height * m_gflBitmap->BytesPerLine); 
		m_colorMap.set(other.m_gflBitmap->ColorMap, other.m_gflBitmap->ColorUsed); 

#ifdef __GFLC_DRAW_FUNCTIONS__
		m_lineColor = other.m_lineColor; 
		m_fillColor = other.m_fillColor; 
		m_lineWidth = other.m_lineWidth; 
#endif
	}
	return *this;
}

// ~

GFL_ERROR	GFLC_BITMAP::create(const GFLC_BITMAP & bitmap)
{
	return create(bitmap.getType(), bitmap.getBitsPerComponent(), bitmap.getWidth(), bitmap.getHeight(), bitmap.getBitsPerComponent()); 
}

// ~

GFL_ERROR	GFLC_BITMAP::create(GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_UINT16 bits_per_component, GFL_INT32 padding, const GFL_COLOR * color)
{
	clear(); 
	if (bits_per_component > 8)
		m_gflBitmap = gflAllockBitmapEx(type, width, height, bits_per_component, padding, color); 
	else
		m_gflBitmap = gflAllockBitmap(type, width, height, padding, color); 
	return m_gflBitmap == NULL ? GFL_ERROR_NO_MEMORY : GFL_NO_ERROR; 
}


// ~

void GFLC_BITMAP::clear(void)
{
	if (m_gflBitmap)
		gflFreeBitmap(m_gflBitmap); 
	m_gflBitmap = NULL; 
	
	m_lastPtr = NULL; 
	m_lastY = -1; 
}

// ~

void * GFLC_BITMAP::getLinePtr(GFL_INT32 y)
{
	ERROR_Check(m_gflBitmap && y >= 0 && y < m_gflBitmap->Height, ""); 
	
	m_lastPtr = m_gflBitmap->Data + y * m_gflBitmap->BytesPerLine; 
	m_lastY = y; 
	return m_lastPtr; 
}

// ~

void * GFLC_BITMAP::getPixelPtr(GFL_INT32 y, GFL_INT32 x)
{
	ERROR_Check(m_gflBitmap && m_gflBitmap->Type != GFL_BINARY && y >= 0 && y < m_gflBitmap->Height && x >= 0 && x < m_gflBitmap->Width, ""); 
	
	m_lastPtr = m_gflBitmap->Data + y * m_gflBitmap->BytesPerLine + x * m_gflBitmap->BytesPerPixel; 
	m_lastY = y; 
	return m_lastPtr; 
}

// ~

void GFLC_BITMAP::getPixel(GFL_INT32 x, GFL_INT32 y, GFLC_COLOR & color)
{
	ERROR_Check(m_gflBitmap && m_gflBitmap->Type != GFL_BINARY && y >= 0 && y < m_gflBitmap->Height && x >= 0 && x < m_gflBitmap->Width, ""); 
	
	m_lastPtr = m_gflBitmap->Data + y * m_gflBitmap->BytesPerLine; 
	m_lastY = y; 

	GFL_COLOR gfl_color; 
	gflGetColorAtEx(m_gflBitmap, (GFL_UINT8 *)m_lastPtr + x * m_gflBitmap->BytesPerPixel, &gfl_color); 
	color.set(gfl_color.Red, gfl_color.Green, gfl_color.Blue, gfl_color.Alpha); 
}

// ~

void GFLC_BITMAP::setPixel(GFL_INT32 x, GFL_INT32 y, const GFLC_COLOR & color)
{
	ERROR_Check(m_gflBitmap && m_gflBitmap->Type != GFL_BINARY && y >= 0 && y < m_gflBitmap->Height && x >= 0 && x < m_gflBitmap->Width, ""); 
	
	if ( y != m_lastY )
	{
		m_lastPtr = m_gflBitmap->Data + y * m_gflBitmap->BytesPerLine; 
		m_lastY = y; 
	}
	GFL_COLOR tmp = color.getGFLColor(); 
	gflSetColorAtEx(m_gflBitmap, (GFL_UINT8 *)m_lastPtr + x * m_gflBitmap->BytesPerPixel, &tmp); 
}

// ~

GFL_ERROR GFLC_BITMAP::loadFromFile(const char *filename, const GFLC_LOAD_PARAMS &params, GFLC_FILE_INFORMATION &info)
{
	GFL_ERROR
		error; 

	clear(); 
	error = gflLoadBitmap(filename, &m_gflBitmap, &params.m_params, &info.m_bitmapInformations); 
	if ( error == GFL_NO_ERROR )
	{
		info.m_isOk = GFL_TRUE; 
		m_colorMap.set(m_gflBitmap->ColorMap, m_gflBitmap->ColorUsed); 
	}
	return error; 
}

// ~

GFL_ERROR GFLC_BITMAP::loadFromFile(const char *filename, const GFLC_LOAD_PARAMS &params)
{
	clear(); 
	return gflLoadBitmap(filename, &m_gflBitmap, &params.m_params, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::loadFromFile(const char *filename, const char *name)
{
	GFLC_FORMAT
		format(name); 
	GFLC_LOAD_PARAMS
		load_option; 
	load_option.setFormat(format); 
	return loadFromFile(filename, load_option); 
}

// ~

GFL_ERROR	GFLC_BITMAP::loadFromFile(const char *filename, GFL_INT32 index)
{
	GFLC_FORMAT
		format(index); 
	GFLC_LOAD_PARAMS
		load_option; 
	load_option.setFormat(index); 
	return loadFromFile(filename, load_option); 
}

// ~

GFL_ERROR GFLC_BITMAP::loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, const GFLC_LOAD_PARAMS &params, GFLC_FILE_INFORMATION &info)
{
	GFL_ERROR
		error; 

	clear(); 
	error = gflLoadPreview(filename, width, height, &m_gflBitmap, &params.m_params, &info.m_bitmapInformations); 
	if ( error == GFL_NO_ERROR )
	{
		info.m_isOk = GFL_TRUE; 
		m_colorMap.set(m_gflBitmap->ColorMap, m_gflBitmap->ColorUsed); 
	}
	return error; 
}

// ~

GFL_ERROR GFLC_BITMAP::loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, const GFLC_LOAD_PARAMS &params)
{
	clear(); 
	return gflLoadPreview(filename, width, height, &m_gflBitmap, &params.m_params, NULL); 
}

// ~

GFL_ERROR GFLC_BITMAP::loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, const char *name)
{
	GFLC_FORMAT
		format(name); 
	GFLC_LOAD_PARAMS
		load_option; 
	load_option.setFormat(format); 
	return loadThumbnailFromFile(filename, width, height, load_option); 
}

// ~

GFL_ERROR GFLC_BITMAP::loadThumbnailFromFile(const char *filename, GFL_INT32 width, GFL_INT32 height, GFL_INT32 index)
{
	GFLC_FORMAT
		format(index); 
	GFLC_LOAD_PARAMS
		load_option; 
	load_option.setFormat(index); 
	return loadThumbnailFromFile(filename, width, height, load_option); 
}

// ~

GFL_ERROR GFLC_BITMAP::saveIntoFile(const char *filename, const GFLC_SAVE_PARAMS &params) const
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflSaveBitmap((char *)filename, m_gflBitmap, &params.m_params); 
}

// ~

GFL_ERROR GFLC_BITMAP::saveIntoFile(const char *filename, const char *format_name) const
{
	GFLC_FORMAT
		format(format_name); 
	GFLC_SAVE_PARAMS
		save_option; 
	save_option.setFormat(format); 
	return saveIntoFile(filename, save_option); 
}

// ~

GFL_ERROR GFLC_BITMAP::saveIntoFile(const char *filename, GFL_INT32 index) const
{
	GFLC_FORMAT
		format(index); 
	GFLC_SAVE_PARAMS
		save_option; 
	save_option.setFormat(format); 
	return saveIntoFile(filename, save_option); 
}

// ~

GFL_ERROR GFLC_BITMAP::resize(GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method, GFL_UINT32 flags)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflResize( m_gflBitmap, NULL, width, height, method, flags ); 
}

// ~

GFL_ERROR GFLC_BITMAP::resize(GFLC_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_UINT32 method, GFL_UINT32 flags)
{
	ERROR_Check(m_gflBitmap, ""); 
	if ( &bitmap == this )
		return gflResize( m_gflBitmap, NULL, width, height, method, flags ); 
	return gflResize( m_gflBitmap, &bitmap.m_gflBitmap, width, height, method, flags ); 
}

// ~

GFL_ERROR GFLC_BITMAP::changeColorDepth(GFL_MODE mode, GFL_MODE_PARAMS option)
{
	GFL_ERROR
		error; 

	ERROR_Check(m_gflBitmap, ""); 
	error = gflChangeColorDepth( m_gflBitmap, NULL, mode, option ); 
	m_colorMap.set(m_gflBitmap->ColorMap, m_gflBitmap->ColorUsed); 
	return error; 
}

// ~

GFL_ERROR GFLC_BITMAP::changeColorDepth(GFLC_BITMAP &bitmap, GFL_MODE mode, GFL_MODE_PARAMS option)
{
	GFL_ERROR
		error; 

	ERROR_Check(m_gflBitmap, ""); 
	if ( &bitmap == this )
	{
		error = gflChangeColorDepth( m_gflBitmap, NULL, mode, option ); 
		m_colorMap.set(m_gflBitmap->ColorMap, m_gflBitmap->ColorUsed); 
	}
	else
	{
		error = gflChangeColorDepth( m_gflBitmap, &bitmap.m_gflBitmap, mode, option ); 
		m_colorMap.set(m_gflBitmap->ColorMap, m_gflBitmap->ColorUsed); 
	}
	return error; 
}

// ~

GFL_ERROR GFLC_BITMAP::flipVertical(void)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflFlipVertical( m_gflBitmap, NULL ); 
}

// ~

GFL_ERROR GFLC_BITMAP::flipVertical(GFLC_BITMAP &bitmap)
{
	ERROR_Check(m_gflBitmap, ""); 
	if ( &bitmap == this )
		return gflFlipVertical( m_gflBitmap, NULL ); 
	return gflFlipVertical( m_gflBitmap, &bitmap.m_gflBitmap ); 
}

// ~

GFL_ERROR GFLC_BITMAP::flipHorizontal(void)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflFlipHorizontal( m_gflBitmap, NULL ); 
}

// ~

GFL_ERROR GFLC_BITMAP::flipHorizontal(GFLC_BITMAP &bitmap)
{
	ERROR_Check(m_gflBitmap, ""); 
	if ( &bitmap == this )
		return gflFlipHorizontal( m_gflBitmap, NULL ); 
	return gflFlipHorizontal( m_gflBitmap, &bitmap.m_gflBitmap ); 
}

// ~

GFL_ERROR GFLC_BITMAP::crop(const GFL_RECT &rect)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflCrop(m_gflBitmap, NULL, &rect); 
}

// ~

GFL_ERROR GFLC_BITMAP::crop(GFLC_BITMAP &bitmap, const GFL_RECT &rect)
{
	ERROR_Check(m_gflBitmap, ""); 
	if ( &bitmap == this )
		return gflCrop(m_gflBitmap, NULL, &rect); 
	return gflCrop(m_gflBitmap, &bitmap.m_gflBitmap, &rect); 
}

// ~

GFL_ERROR GFLC_BITMAP::autoCrop(const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflAutoCrop(m_gflBitmap, NULL, &color, tolerance); 
}

// ~

GFL_ERROR GFLC_BITMAP::autoCrop(GFLC_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflAutoCrop(m_gflBitmap, &bitmap.m_gflBitmap, &color, tolerance); 
}

// ~

GFL_ERROR GFLC_BITMAP::autoCrop2(const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflAutoCrop2(m_gflBitmap, NULL, &color, tolerance); 
}

// ~

GFL_ERROR GFLC_BITMAP::autoCrop2(GFLC_BITMAP &bitmap, const GFL_COLOR &color, GFL_INT32 tolerance)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflAutoCrop2(m_gflBitmap, &bitmap.m_gflBitmap, &color, tolerance); 
}

// ~

GFL_ERROR GFLC_BITMAP::resizeCanvas(GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode, const GFL_COLOR *color)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflResizeCanvas(m_gflBitmap, NULL, width, height, mode, color); 
}

// ~

GFL_ERROR GFLC_BITMAP::resizeCanvas(GFLC_BITMAP &bitmap, GFL_INT32 width, GFL_INT32 height, GFL_CANVASRESIZE mode, const GFL_COLOR *color)
{
	ERROR_Check(m_gflBitmap, ""); 
	if ( &bitmap == this )
		return gflResizeCanvas(m_gflBitmap, NULL, width, height, mode, color); 
	return gflResizeCanvas(m_gflBitmap, &bitmap.m_gflBitmap, width, height, mode, color); 
}

// ~

GFL_ERROR GFLC_BITMAP::rotate(GFL_INT32 angle, const GFL_COLOR *background_color)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflRotate(m_gflBitmap, NULL, angle, background_color); 
}

// ~

GFL_ERROR GFLC_BITMAP::rotate(GFLC_BITMAP &bitmap, GFL_INT32 angle, const GFL_COLOR *background_color)
{
	ERROR_Check(m_gflBitmap, ""); 
	if ( &bitmap == this )
		return gflRotate(m_gflBitmap, NULL, angle, background_color); 
	return gflRotate(m_gflBitmap, &bitmap.m_gflBitmap, angle, background_color); 
}

// ~

#ifdef __GFLC_GFLE_FUNCTIONS__

#include "libgfle.h"

GFL_ERROR GFLC_BITMAP::negative(void)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflNegative(m_gflBitmap, NULL); 
}

// ~

GFL_ERROR GFLC_BITMAP::negative(GFLC_BITMAP &bitmap)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflNegative(m_gflBitmap, &bitmap.m_gflBitmap); 
}

// ~

GFL_ERROR GFLC_BITMAP::brightness(GFL_INT32 brightness)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflBrightness(m_gflBitmap, NULL, brightness); 
}

// ~

GFL_ERROR GFLC_BITMAP::brightness(GFLC_BITMAP &bitmap, GFL_INT32 brightness)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflBrightness(m_gflBitmap, &bitmap.m_gflBitmap, brightness); 
}

// ~

GFL_ERROR GFLC_BITMAP::contrast(GFL_INT32 contrast)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflContrast(m_gflBitmap, NULL, contrast); 
}

// ~

GFL_ERROR GFLC_BITMAP::contrast(GFLC_BITMAP &bitmap, GFL_INT32 contrast)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflContrast(m_gflBitmap, &bitmap.m_gflBitmap, contrast); 
}

// ~

GFL_ERROR GFLC_BITMAP::gamma(double gamma)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflGamma(m_gflBitmap, NULL, gamma); 
}

// ~

GFL_ERROR GFLC_BITMAP::gamma(GFLC_BITMAP &bitmap, double gamma)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflGamma(m_gflBitmap, &bitmap.m_gflBitmap, gamma); 
}

// ~

GFL_ERROR GFLC_BITMAP::average(GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflAverage(m_gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::average(GFLC_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflAverage(m_gflBitmap, &bitmap.m_gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::soften(GFL_INT32 percentage)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflSoften(m_gflBitmap, NULL, percentage); 
}

// ~

GFL_ERROR GFLC_BITMAP::soften(GFLC_BITMAP &bitmap, GFL_INT32 percentage)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflSoften(m_gflBitmap, &bitmap.m_gflBitmap, percentage); 
}

// ~

GFL_ERROR GFLC_BITMAP::blur(GFL_INT32 percentage)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflBlur(m_gflBitmap, NULL, percentage); 
}

// ~

GFL_ERROR GFLC_BITMAP::blur(GFLC_BITMAP &bitmap, GFL_INT32 percentage)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflBlur(m_gflBitmap, &bitmap.m_gflBitmap, percentage); 
}

// ~

GFL_ERROR GFLC_BITMAP::gaussianBlur(GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflGaussianBlur(m_gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::gaussianBlur(GFLC_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflGaussianBlur(m_gflBitmap, &bitmap.m_gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::maximum(GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMaximum(m_gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::maximum(GFLC_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMaximum(m_gflBitmap, &bitmap.m_gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::minimum(GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMinimum(m_gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::minimum(GFLC_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMinimum(m_gflBitmap, &bitmap.m_gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::medianBox(GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMedianBox(m_gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::medianBox(GFLC_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMedianBox(m_gflBitmap, &bitmap.m_gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::medianCross(GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMedianCross(m_gflBitmap, NULL, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::medianCross(GFLC_BITMAP &bitmap, GFL_INT32 filter_size)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflMedianCross(m_gflBitmap, &bitmap.m_gflBitmap, filter_size); 
}

// ~

GFL_ERROR GFLC_BITMAP::sharpen(GFL_INT32 percentage)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflSharpen(m_gflBitmap, NULL, percentage); 
}

// ~

GFL_ERROR GFLC_BITMAP::sharpen(GFLC_BITMAP &bitmap, GFL_INT32 percentage)
{
	ERROR_Check(m_gflBitmap, ""); 
	return gflSharpen(m_gflBitmap, &bitmap.m_gflBitmap, percentage); 
}

#endif

#ifdef __GFLC_DRAW_FUNCTIONS__

#include "libgfle.h"

GFL_ERROR	GFLC_BITMAP::drawPoint(GFL_INT32 x0, GFL_INT32 y0)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	return gflDrawPointColor(m_gflBitmap, x0, y0, m_lineWidth, &line_color, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::drawLine(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 x1, GFL_INT32 y1)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	return gflDrawLineColor(m_gflBitmap, x0, y0, x1, y1, m_lineWidth, &line_color, m_lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::drawPolyline(const GFL_POINT points[], GFL_INT32 num_points)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	return gflDrawPolylineColor(m_gflBitmap, points, num_points, m_lineWidth, &line_color, m_lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::drawPolygon(const GFL_POINT points[], GFL_INT32 num_points)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	GFL_COLOR fill_color = m_fillColor.GetGFLColor(); 
	return gflDrawPolygonColor(m_gflBitmap, points, num_points, &fill_color, m_lineWidth, &line_color, m_lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::drawRectangle(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	return gflDrawRectangleColor(m_gflBitmap, x0, y0, width, height, NULL, m_lineWidth, &line_color, m_lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::drawFillRectangle(GFL_INT32 x0, GFL_INT32 y0, GFL_INT32 width, GFL_INT32 height)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	GFL_COLOR fill_color = m_fillColor.GetGFLColor(); 
	return gflDrawRectangleColor(m_gflBitmap, x0, y0, width, height, &fill_color, m_lineWidth, &line_color, m_lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::drawCircle(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	return gflDrawCircleColor(m_gflBitmap, x, y, radius, NULL, m_lineWidth, &line_color, m_lineStyle, NULL); 
}

// ~

GFL_ERROR	GFLC_BITMAP::drawFillCircle(GFL_INT32 x, GFL_INT32 y, GFL_INT32 radius)
{
	ERROR_Check(m_gflBitmap, ""); 
	GFL_COLOR line_color = m_lineColor.GetGFLColor(); 
	GFL_COLOR fill_color = m_fillColor.GetGFLColor(); 
	return gflDrawCircleColor(m_gflBitmap, x, y, radius, &fill_color, m_lineWidth, &line_color, m_lineStyle, NULL); 
}

#endif
