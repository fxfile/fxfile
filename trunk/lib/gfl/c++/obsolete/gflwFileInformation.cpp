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

GFLW_FILE_INFORMATION :: GFLW_FILE_INFORMATION(
	void
	) :
	isOk(GFL_FALSE)
{
}

// ~

GFLW_FILE_INFORMATION :: GFLW_FILE_INFORMATION(
	const char *filename
	)
{
	GetFromFile(filename); 
}

// ~

GFLW_FILE_INFORMATION :: ~GFLW_FILE_INFORMATION(
	void
	)
{
	if ( isOk == GFL_TRUE )
		gflFreeFileInformation(&bitmapInformations); 
	isOk = GFL_FALSE; 
}

// ~

GFL_ERROR GFLW_FILE_INFORMATION :: GetFromFile(
	const char *filename, 
	GFL_INT32 index
	)
{
	GFL_ERROR
		error; 

	error = gflGetFileInformation(filename, index, &bitmapInformations); 
	if ( error == GFL_NO_ERROR )
		isOk = GFL_TRUE; 

	return error; 
}
