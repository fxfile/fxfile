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

#ifndef __GFLW_LIBRARY_H__
#define __GFLW_LIBRARY_H__

class GFLW_LIBRARY {
	public:
		static GFL_ERROR		Initialise(void); 
		static void					Exit(void); 
		static const char * GetVersion(void); 
		static const char * GetVersionOfLibformat(void); 
		static void					EnableLZW(GFL_BOOL flag=GFL_FALSE); 
}; 

// INLINE

inline GFL_ERROR GFLW_LIBRARY::Initialise(
	void
	)
{
	return gflLibraryInit(); 
}

// ~

inline void GFLW_LIBRARY::Exit(
	void
	)
{
	gflLibraryExit(); 
}

// ~

inline const char * GFLW_LIBRARY::GetVersion(
	void
	)
{
	return gflGetVersion(); 
}

// ~

inline const char * GFLW_LIBRARY::GetVersionOfLibformat(
	void
	)
{
	return gflGetVersionOfLibformat(); 
}

// ~

inline void GFLW_LIBRARY::EnableLZW(
	GFL_BOOL flag
	)
{
	gflEnableLZW(flag); 
}

#endif
