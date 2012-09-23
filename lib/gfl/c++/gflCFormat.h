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

#ifndef __GFLC_FORMAT_H__
#define __GFLC_FORMAT_H__

class GFLC_FORMAT {
	public:
		GFLC_FORMAT(); 
		GFLC_FORMAT(const char *name); 
		GFLC_FORMAT(GFL_INT32 index); 
		virtual ~GFLC_FORMAT(); 
		
		// ~~ 
		
		static GFL_INT32	getNumberOfFormat(void); 
		static GFL_ERROR	findFirst(GFLC_FORMAT &format); 
		static GFL_ERROR	findNext(GFLC_FORMAT &format); 
		
		GFL_ERROR				findByName(const char *name); 
		GFL_ERROR				findByFilename(const char *filename); 
		GFL_ERROR				findByIndex(GFL_INT32 index); 
		
		GFL_INT32				getIndex(void) const; 
		const char *		getName(void) const; 
		const char *		getDescription(void) const; 
		
		GFL_BOOL				isReadable(void) const; 
		GFL_BOOL				isWritable(void) const; 
		
		const char *    getDefaultExtension(void) const; 
		const char *		getExtensionFirst(void); 
		const char *		getExtensionNext(void); 
		
	private:
		GFL_FORMAT_INFORMATION
			m_bitmapInformations; 
		GFL_BOOL
			m_isOk; 
		GFL_INT32
			m_extensionIndex; 
	}; 

// INLINE

inline GFL_INT32 GFLC_FORMAT::getNumberOfFormat(
	void
	)
{
	return gflGetNumberOfFormat(); 
}

// ~

inline GFL_INT32 GFLC_FORMAT::getIndex(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Index; 
}

// ~

inline const char *GFLC_FORMAT::getName(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Name; 
}

// ~

inline const char *GFLC_FORMAT::getDescription(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Description; 
}

// ~

inline GFL_BOOL GFLC_FORMAT::isReadable(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Status & GFL_READ ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline GFL_BOOL GFLC_FORMAT::isWritable(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Status & GFL_WRITE ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline const char *GFLC_FORMAT::getExtensionNext(
	void
	)
{
	ERROR_Check(m_isOk, ""); 
	if (m_extensionIndex < m_bitmapInformations.NumberOfExtension )
		return m_bitmapInformations.Extension[m_extensionIndex++]; 
	
	return NULL; 
}

// ~

inline const char *GFLC_FORMAT::getExtensionFirst(
	void
	)
{
	m_extensionIndex = 0; 
	return getExtensionNext(); 
}

// ~

inline const char * GFLC_FORMAT::getDefaultExtension(
	void
	) const
{
	if (m_bitmapInformations.NumberOfExtension)
		return m_bitmapInformations.Extension[0]; 
	return NULL; 
}

#endif
