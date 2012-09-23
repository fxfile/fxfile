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

#ifndef __GFLW_EXCEPTION_H__
#define __GFLW_EXCEPTION_H__

class GFLW_EXCEPTION {
	public:
		GFLW_EXCEPTION(const char * file, GFL_INT32 line, const char *error); 
		virtual ~GFLW_EXCEPTION(void); 
		
		GFLW_EXCEPTION(const GFLW_EXCEPTION & other); 
		GFLW_EXCEPTION & operator= (const GFLW_EXCEPTION & other); 

		GFL_INT32	GetLine(void) const; 
		const char * GetFile(void) const; 
		const char * GetError(void) const; 

	private:
		GFL_INT32
			lineNumber; 
		char 
			* fileSource; 
		char
			* errorMessage; 
}; 

// INLINE

inline GFLW_EXCEPTION :: GFLW_EXCEPTION(
	const char * file, 
	GFL_INT32 line, 
	const char *error
	) :
	errorMessage(NULL)
{
	fileSource = strdup(file); 
	lineNumber = line; 
	if (error)
		errorMessage = strdup(error); 
}

// ~

inline GFLW_EXCEPTION :: ~GFLW_EXCEPTION(
	void
	)
{
	free(fileSource); 
	if (errorMessage)
		free(errorMessage); 
}

// ~

inline GFLW_EXCEPTION :: GFLW_EXCEPTION(
	const GFLW_EXCEPTION & other
	)
{
	fileSource = strdup(other.fileSource); 
	lineNumber = other.lineNumber; 
	if (errorMessage)
		errorMessage = strdup(other.errorMessage); 
	else
		errorMessage = NULL; 
}

// ~

inline GFLW_EXCEPTION & GFLW_EXCEPTION :: operator= (
	const GFLW_EXCEPTION & other
	)
{
	if (this != &other)
	{
		fileSource = strdup(other.fileSource); 
		lineNumber = other.lineNumber; 
		if (errorMessage)
			errorMessage = strdup(other.errorMessage); 
		else
			errorMessage = NULL; 
	}
	return *this; 
}

// ~

inline GFL_INT32 GFLW_EXCEPTION :: GetLine(
	void
	) const
{
	return lineNumber; 
}

// ~

inline const char * GFLW_EXCEPTION :: GetFile(
	void
	) const
{
	return fileSource; 
}

// ~

inline const char * GFLW_EXCEPTION :: GetError(
	void
	) const
{
	return errorMessage; 
}

#endif
