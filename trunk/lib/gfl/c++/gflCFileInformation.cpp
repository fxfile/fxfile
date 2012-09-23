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

GFLC_FILE_INFORMATION :: GFLC_FILE_INFORMATION(
	void
	) :
	m_isOk(GFL_FALSE)
{
}

// ~

GFLC_FILE_INFORMATION :: GFLC_FILE_INFORMATION(
	const char *filename
	)
{
	getFromFile(filename); 
}

// ~

GFLC_FILE_INFORMATION :: GFLC_FILE_INFORMATION(
	const std::string &filename
	)
{
	getFromFile(filename.data()); 
}

// ~

GFLC_FILE_INFORMATION :: ~GFLC_FILE_INFORMATION(
	void
	)
{
	if ( m_isOk == GFL_TRUE )
		gflFreeFileInformation(&m_bitmapInformations); 
	m_isOk = GFL_FALSE; 
}

// ~

GFL_ERROR GFLC_FILE_INFORMATION :: getFromFile(
	const char *filename, 
	GFL_INT32 index
	)
{
	GFL_ERROR
		error; 

	error = gflGetFileInformation(filename, index, &m_bitmapInformations); 
	if ( error == GFL_NO_ERROR )
		m_isOk = GFL_TRUE; 

	return error; 
}
