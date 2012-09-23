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

GFLW_FORMAT::GFLW_FORMAT(
	) :
	isOk(GFL_FALSE), 
	extensionIndex(0)
{
}

// ~

GFLW_FORMAT::GFLW_FORMAT(
	const char *name
	)
{
	isOk = GFL_TRUE; 
	gflGetFormatInformationByName(name, &bitmapInformations); 
}

// ~

GFLW_FORMAT::GFLW_FORMAT(
	GFL_INT32 index
	)
{
	isOk = GFL_TRUE; 
	gflGetFormatInformationByIndex(index, &bitmapInformations); 
}

// ~

GFLW_FORMAT::~GFLW_FORMAT(
	)
{
}

// ~

GFL_ERROR GFLW_FORMAT::FindByName(
	const char *name
	)
{
	isOk = GFL_TRUE; 
	extensionIndex = 0; 
	return gflGetFormatInformationByName(name, &bitmapInformations); 
}

// ~

GFL_ERROR GFLW_FORMAT::FindByFilename(
	const char *filename
	)
{
	char 
		* ptr; 
	const char
		* ext; 
	GFL_BOOL 
		ret; 
	GFLW_FORMAT
		format; 

	isOk = GFL_TRUE; 
#ifdef __BORLANDC__
	ptr = strrchr((char *)filename, '.'); 
#else
	ptr = strrchr(filename, '.'); 
#endif
	if (ptr)
	{
		ret = GFLW_FORMAT::FindFirst(format); 
		while ( ret == GFL_NO_ERROR )
		{
			ext = format.GetExtensionFirst(); 
			while (ext)
			{
				if (! strcmp(ext, ptr+1) )
					break; 

				ext = format.GetExtensionNext(); 
			}
			if (ext)
				return FindByIndex(format.GetIndex()); 

			ret = GFLW_FORMAT::FindNext(format); 
		}
	}
	
	return GFL_ERROR_BAD_FORMAT_INDEX; 
}

// ~

GFL_ERROR GFLW_FORMAT::FindByIndex(
	GFL_INT32 index
	)
{
	isOk = GFL_TRUE; 
	extensionIndex = 0; 
	return gflGetFormatInformationByIndex(index, &bitmapInformations); 
}

// ~

GFL_ERROR GFLW_FORMAT::FindFirst(
	GFLW_FORMAT &format
	)
{
	return format.FindByIndex(0); 
}

// ~

GFL_ERROR GFLW_FORMAT::FindNext(
	GFLW_FORMAT &format
	)
{
	GFL_INT32 
		index; 

	if (format.isOk)
		index = format.bitmapInformations.Index + 1; 
	else
		index = 0; 
	
	return format.FindByIndex(index); 
}
