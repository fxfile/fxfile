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

#ifndef __GFLW_FORMAT_H__
#define __GFLW_FORMAT_H__

class GFLW_FORMAT {
	public:
		GFLW_FORMAT(); 
		GFLW_FORMAT(const char *name); 
		GFLW_FORMAT(GFL_INT32 index); 
		virtual ~GFLW_FORMAT(); 
		
		static GFL_INT32	GetNumberOfFormat(void); 
		static GFL_ERROR	FindFirst(GFLW_FORMAT &format); 
		static GFL_ERROR	FindNext(GFLW_FORMAT &format); 
		
		GFL_ERROR				FindByName(const char *name); 
		GFL_ERROR				FindByFilename(const char *filename); 
		GFL_ERROR				FindByIndex(GFL_INT32 index); 
		
		GFL_INT32				GetIndex(void) const; 
		const char *		GetName(void) const; 
		const char *		GetDescription(void) const; 
		
		GFL_BOOL				IsReadable(void) const; 
		GFL_BOOL				IsWritable(void) const; 
		
		const char *    GetDefaultExtension(void) const; 
		const char *		GetExtensionFirst(void); 
		const char *		GetExtensionNext(void); 
		
	private:
		GFL_FORMAT_INFORMATION
			bitmapInformations; 
		GFL_BOOL
			isOk; 
		GFL_INT32
			extensionIndex; 
	}; 

// INLINE

inline GFL_INT32 GFLW_FORMAT::GetNumberOfFormat(
	void
	)
{
//	ERROR_Check(isOk, ""); 
	return gflGetNumberOfFormat(); 
}

// ~

inline GFL_INT32 GFLW_FORMAT::GetIndex(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Index; 
}

// ~

inline const char *GFLW_FORMAT::GetName(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Name; 
}

// ~

inline const char *GFLW_FORMAT::GetDescription(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Description; 
}

// ~

inline GFL_BOOL GFLW_FORMAT::IsReadable(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Status & GFL_READ ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline GFL_BOOL GFLW_FORMAT::IsWritable(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Status & GFL_WRITE ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline const char *GFLW_FORMAT::GetExtensionNext(
	void
	)
{
	ERROR_Check(isOk, ""); 
	if (extensionIndex < bitmapInformations.NumberOfExtension )
		return bitmapInformations.Extension[extensionIndex++]; 
	
	return NULL; 
}

// ~

inline const char *GFLW_FORMAT::GetExtensionFirst(
	void
	)
{
	extensionIndex = 0; 
	return GetExtensionNext(); 
}

// ~

inline const char * GFLW_FORMAT::GetDefaultExtension(
	void
	) const
{
	if (bitmapInformations.NumberOfExtension)
		return bitmapInformations.Extension[0]; 
	return NULL; 
}

#endif
