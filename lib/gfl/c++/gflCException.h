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

#ifndef __GFLC_EXCEPTION_H__
#define __GFLC_EXCEPTION_H__

#include <string>

class GFLC_EXCEPTION {
	public:
		GFLC_EXCEPTION(const char * file, GFL_INT32 line, const char *error); 
		virtual ~GFLC_EXCEPTION(void); 
		
		// ~~ 
		
		GFLC_EXCEPTION(const GFLC_EXCEPTION & other); 
		GFLC_EXCEPTION & operator= (const GFLC_EXCEPTION & other); 
		
		// ~~ 
		
		GFL_INT32	getLine(void) const; 
#ifdef __GFLC_USE_STD_STRING__
		const std::string & getFile(void) const; 
		const std::string & getError(void) const; 
#else
		const char * getFile(void) const; 
		const char * getError(void) const; 
#endif

	private:
		GFL_INT32
			m_lineNumber; 
		std::string
			m_fileSource; 
		std::string
			m_errorMessage; 
}; 

// INLINE

inline GFLC_EXCEPTION :: GFLC_EXCEPTION(
	const char * file, 
	GFL_INT32 line, 
	const char * error
	) :
	m_errorMessage(), 
	m_fileSource(file), 
	m_lineNumber(line)
{
	if (error)
		m_errorMessage = error; 
}

// ~

inline GFLC_EXCEPTION :: ~GFLC_EXCEPTION(
	void
	)
{
}

// ~

inline GFLC_EXCEPTION :: GFLC_EXCEPTION(
	const GFLC_EXCEPTION & other
	)
{
	m_fileSource = other.m_fileSource; 
	m_lineNumber = other.m_lineNumber; 
	m_errorMessage = other.m_errorMessage; 
}

// ~

inline GFLC_EXCEPTION & GFLC_EXCEPTION :: operator= (
	const GFLC_EXCEPTION & other
	)
{
	if (this != &other)
	{
		m_fileSource = other.m_fileSource; 
		m_lineNumber = other.m_lineNumber; 
		m_errorMessage = other.m_errorMessage; 
	}
	return *this; 
}

// ~

inline GFL_INT32 GFLC_EXCEPTION :: getLine(
	void
	) const
{
	return m_lineNumber; 
}

// ~

#ifdef __GFLC_USE_STD_STRING__

inline const std::string & GFLC_EXCEPTION :: getFile(
	void
	) const
{
	return m_fileSource; 
}

// ~

inline const std::string & GFLC_EXCEPTION :: getError(
	void
	) const
{
	return m_errorMessage; 
}

#else

// ~

inline const char * GFLC_EXCEPTION :: getFile(
	void
	) const
{
	return m_fileSource.data(); 
}

// ~

inline const char * GFLC_EXCEPTION :: getError(
	void
	) const
{
	return m_errorMessage.data(); 
}

#endif

#endif
