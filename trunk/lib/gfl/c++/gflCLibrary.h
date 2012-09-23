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

#ifndef __GFLC_LIBRARY_H__
#define __GFLC_LIBRARY_H__

class GFLC_LIBRARY {
	public:
		static GFL_ERROR		initialise(void); 
		static void					exit(void); 
		static const char * getVersion(void); 
		static const char * getVersionOfLibformat(void); 
		static void					enableLZW(GFL_BOOL flag=GFL_FALSE); 
}; 

// INLINE

inline GFL_ERROR GFLC_LIBRARY::initialise(
	void
	)
{
	return gflLibraryInit(); 
}

// ~

inline void GFLC_LIBRARY::exit(
	void
	)
{
	gflLibraryExit(); 
}

// ~

inline const char * GFLC_LIBRARY::getVersion(
	void
	)
{
	return gflGetVersion(); 
}

// ~

inline const char * GFLC_LIBRARY::getVersionOfLibformat(
	void
	)
{
	return gflGetVersionOfLibformat(); 
}

// ~

inline void GFLC_LIBRARY::enableLZW(
	GFL_BOOL flag
	)
{
	gflEnableLZW(flag); 
}

#endif
