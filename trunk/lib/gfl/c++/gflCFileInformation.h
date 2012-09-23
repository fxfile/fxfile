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

#ifndef __GFLC_FILE_INFORMATION_H__
#define __GFLC_FILE_INFORMATION_H__

#include <string>

class GFLC_FILE_INFORMATION {
	public:
		GFLC_FILE_INFORMATION(void); 
		GFLC_FILE_INFORMATION(const char *filename); 
		GFLC_FILE_INFORMATION(const std::string &filename); 
		virtual ~GFLC_FILE_INFORMATION(void); 
		
		// ~~ 
		
		GFL_ERROR					getFromFile(const char *filename, GFL_INT32 index=-1); 
		
		// ~~ 
		
		GFL_BITMAP_TYPE		getType(void) const; 
		GFL_INT32					getWidth(void) const; 
		GFL_INT32					getHeight(void) const; 
		GFL_INT32					getIndexOfFormat(void) const; 
#ifdef __GFLC_USE_STD_STRING__
		std::string				getNameOfFormat(void) const; 
		std::string				getDescription(void) const; 
#else
		const char *			getNameOfFormat(void) const; 
		const char *			getDescription(void) const; 
#endif
		GFL_UINT16				getXdpi(void) const; 
		GFL_UINT16				getYdpi(void) const; 
		GFL_UINT16				getBitsPerComponent(void) const; 
		GFL_UINT16				getComponentsPerPixel(void) const; 
		GFL_UINT16				getNumberOfBits(void) const; 
		GFL_INT32					getNumberOfImages(void) const; 
		GFL_UINT32				getFileSize(void) const; 
		GFL_ORIGIN				getOrigin(void) const; 
		GFL_COLORMODEL    getColorModel(void) const; 
		const char *      getLabelForColorModel(void) const; 
		GFL_COMPRESSION		getCompression(void) const; 
		const char *			getCompressionDescription(void) const; 

		//GFL_BOOL          HasComment(void) const; 
		//GFL_INT32         GetNumberOfComment(void) const; 
		//const char *      GetComment(GFL_INT32) const; 
		
		friend class GFLC_BITMAP; 

	private:
		GFL_FILE_INFORMATION
			m_bitmapInformations; 
		GFL_BOOL
			m_isOk; 
}; 

// INLINE

inline GFL_BITMAP_TYPE GFLC_FILE_INFORMATION::getType(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Type; 
}

// ~

inline GFL_INT32 GFLC_FILE_INFORMATION::getWidth(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Width; 
}

// ~

inline GFL_INT32 GFLC_FILE_INFORMATION::getHeight(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Height; 
}

// ~

inline GFL_INT32 GFLC_FILE_INFORMATION::getIndexOfFormat(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.FormatIndex; 
}

// ~

#ifdef __GFLC_USE_STD_STRING__

inline std::string GFLC_FILE_INFORMATION::getNameOfFormat(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return std::string(m_bitmapInformations.FormatName); 
}

// ~

inline std::string GFLC_FILE_INFORMATION::getDescription(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return std::string(m_bitmapInformations.Description); 
}

#else

inline const char * GFLC_FILE_INFORMATION::getNameOfFormat(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.FormatName; 
}

// ~

inline const char * GFLC_FILE_INFORMATION::getDescription(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Description; 
}

#endif

// ~

inline GFL_UINT16 GFLC_FILE_INFORMATION::getXdpi(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Xdpi; 
}

// ~

inline GFL_UINT16 GFLC_FILE_INFORMATION::getYdpi(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Ydpi; 
}

// ~

inline GFL_UINT16 GFLC_FILE_INFORMATION::getBitsPerComponent(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.BitsPerComponent; 
}

// ~

inline GFL_UINT16 GFLC_FILE_INFORMATION::getComponentsPerPixel(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.ComponentsPerPixel; 
}

// ~

inline GFL_UINT16 GFLC_FILE_INFORMATION::getNumberOfBits(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.BitsPerComponent * m_bitmapInformations.ComponentsPerPixel; 
}

// ~

inline GFL_INT32 GFLC_FILE_INFORMATION::getNumberOfImages(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.NumberOfImages; 
}

// ~

inline GFL_UINT32 GFLC_FILE_INFORMATION::getFileSize(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.FileSize; 
}

// ~

inline GFL_ORIGIN GFLC_FILE_INFORMATION::getOrigin(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Origin; 
}

// ~

inline GFL_COLORMODEL GFLC_FILE_INFORMATION::getColorModel(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.ColorModel; 
}

// ~

inline const char * GFLC_FILE_INFORMATION::getLabelForColorModel(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return gflGetLabelForColorModel(m_bitmapInformations.ColorModel); 
}

// ~

inline GFL_COMPRESSION GFLC_FILE_INFORMATION::getCompression(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.Compression; 
}

// ~

inline const char * GFLC_FILE_INFORMATION::getCompressionDescription(
	void
	) const
{
	ERROR_Check(m_isOk, ""); 
	return m_bitmapInformations.CompressionDescription; 
}

// ~
/*
inline GFL_BOOL GFLC_FILE_INFORMATION::HasComment(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return Information.NumberOfComment > 0 ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline const char * GFLC_FILE_INFORMATION::GetComment(
	GFL_INT32 index
	) const
{
	ERROR_Check(isOk, ""); 
	ERROR_Check(index >= 0 && index < Information.NumberOfComment, ""); 
	return Information.Comment[index]; 
}
*/
#endif
