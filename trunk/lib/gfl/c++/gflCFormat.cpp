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

GFLC_FORMAT::GFLC_FORMAT(
	) :
	m_isOk(GFL_FALSE), 
	m_extensionIndex(0)
{
}

// ~

GFLC_FORMAT::GFLC_FORMAT(
	const char *name
	)
{
	m_isOk = GFL_TRUE; 
	gflGetFormatInformationByName(name, &m_bitmapInformations); 
}

// ~

GFLC_FORMAT::GFLC_FORMAT(
	GFL_INT32 index
	)
{
	m_isOk = GFL_TRUE; 
	gflGetFormatInformationByIndex(index, &m_bitmapInformations); 
}

// ~

GFLC_FORMAT::~GFLC_FORMAT(
	)
{
}

// ~

GFL_ERROR GFLC_FORMAT::findByName(
	const char *name
	)
{
	m_isOk = GFL_TRUE; 
	m_extensionIndex = 0; 
	return gflGetFormatInformationByName(name, &m_bitmapInformations); 
}

// ~

GFL_ERROR GFLC_FORMAT::findByFilename(
	const char *filename
	)
{
	char 
		* ptr; 
	const char
		* ext; 
	GFL_BOOL 
		ret; 
	GFLC_FORMAT
		format; 

	m_isOk = GFL_TRUE; 
#ifdef __BORLANDC__
	ptr = strrchr((char *)filename, '.'); 
#else
	ptr = (char *)strrchr(filename, '.'); 
#endif
	if (ptr)
	{
		ret = GFLC_FORMAT::findFirst(format); 
		while ( ret == GFL_NO_ERROR )
		{
			ext = format.getExtensionFirst(); 
			while (ext)
			{
				if (! strcmp(ext, ptr+1) )
					break; 

				ext = format.getExtensionNext(); 
			}
			if (ext)
				return findByIndex(format.getIndex()); 

			ret = GFLC_FORMAT::findNext(format); 
		}
	}
	
	return GFL_ERROR_BAD_FORMAT_INDEX; 
}

// ~

GFL_ERROR GFLC_FORMAT::findByIndex(
	GFL_INT32 index
	)
{
	m_isOk = GFL_TRUE; 
	m_extensionIndex = 0; 
	return gflGetFormatInformationByIndex(index, &m_bitmapInformations); 
}

// ~

GFL_ERROR GFLC_FORMAT::findFirst(
	GFLC_FORMAT &format
	)
{
	return format.findByIndex(0); 
}

// ~

GFL_ERROR GFLC_FORMAT::findNext(
	GFLC_FORMAT &format
	)
{
	GFL_INT32 
		index; 

	if (format.m_isOk)
		index = format.m_bitmapInformations.Index + 1; 
	else
		index = 0; 
	
	return format.findByIndex(index); 
}
