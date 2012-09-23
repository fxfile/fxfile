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

#ifndef __GFLW_FILE_INFORMATION_H__
#define __GFLW_FILE_INFORMATION_H__

class GFLW_FILE_INFORMATION {
	public:
		GFLW_FILE_INFORMATION(void); 
		GFLW_FILE_INFORMATION(const char *filename); 
		virtual ~GFLW_FILE_INFORMATION(void); 
		
		GFL_ERROR					GetFromFile(const char *filename, GFL_INT32 index=-1); 
		
		GFL_BITMAP_TYPE		GetType(void) const; 
		GFL_INT32					GetWidth(void) const; 
		GFL_INT32					GetHeight(void) const; 
		GFL_INT32					GetIndexOfFormat(void) const; 
		const char *			GetNameOfFormat(void) const; 
		const char *			GetDescription(void) const; 
		GFL_UINT16				GetXdpi(void) const; 
		GFL_UINT16				GetYdpi(void) const; 
		GFL_UINT16				GetBitsPerComponent(void) const; 
		GFL_UINT16				GetComponentsPerPixel(void) const; 
		GFL_UINT16				GetNumberOfBits(void) const; 
		GFL_INT32					GetNumberOfImages(void) const; 
		GFL_UINT32				GetFileSize(void) const; 
		GFL_ORIGIN				GetOrigin(void) const; 
		GFL_COLORMODEL    GetColorModel(void) const; 
		const char *      GetLabelForColorModel(void) const; 
		GFL_COMPRESSION		GetCompression(void) const; 
		const char *			GetCompressionDescription(void) const; 

		//GFL_BOOL          HasComment(void) const; 
		//GFL_INT32         GetNumberOfComment(void) const; 
		//const char *      GetComment(GFL_INT32) const; 
		
		friend class GFLW_BITMAP; 

	private:
		GFL_FILE_INFORMATION
			bitmapInformations; 
		GFL_BOOL
			isOk; 
}; 

// INLINE

inline GFL_BITMAP_TYPE GFLW_FILE_INFORMATION::GetType(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Type; 
}

// ~

inline GFL_INT32 GFLW_FILE_INFORMATION::GetWidth(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Width; 
}

// ~

inline GFL_INT32 GFLW_FILE_INFORMATION::GetHeight(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Height; 
}

// ~

inline GFL_INT32 GFLW_FILE_INFORMATION::GetIndexOfFormat(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.FormatIndex; 
}

// ~

inline const char * GFLW_FILE_INFORMATION::GetNameOfFormat(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.FormatName; 
}

// ~

inline const char * GFLW_FILE_INFORMATION::GetDescription(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Description; 
}

// ~

inline GFL_UINT16 GFLW_FILE_INFORMATION::GetXdpi(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Xdpi; 
}

// ~

inline GFL_UINT16 GFLW_FILE_INFORMATION::GetYdpi(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Ydpi; 
}

// ~

inline GFL_UINT16 GFLW_FILE_INFORMATION::GetBitsPerComponent(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.BitsPerComponent; 
}

// ~

inline GFL_UINT16 GFLW_FILE_INFORMATION::GetComponentsPerPixel(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.ComponentsPerPixel; 
}

// ~

inline GFL_UINT16 GFLW_FILE_INFORMATION::GetNumberOfBits(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.BitsPerComponent * bitmapInformations.ComponentsPerPixel; 
}

// ~

inline GFL_INT32 GFLW_FILE_INFORMATION::GetNumberOfImages(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.NumberOfImages; 
}

// ~

inline GFL_UINT32 GFLW_FILE_INFORMATION::GetFileSize(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.FileSize; 
}

// ~

inline GFL_ORIGIN GFLW_FILE_INFORMATION::GetOrigin(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Origin; 
}

// ~

inline GFL_COLORMODEL GFLW_FILE_INFORMATION::GetColorModel(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.ColorModel; 
}

// ~

inline const char * GFLW_FILE_INFORMATION::GetLabelForColorModel(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return gflGetLabelForColorModel(bitmapInformations.ColorModel); 
}

// ~

inline GFL_COMPRESSION GFLW_FILE_INFORMATION::GetCompression(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.Compression; 
}

// ~

inline const char * GFLW_FILE_INFORMATION::GetCompressionDescription(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return bitmapInformations.CompressionDescription; 
}

// ~
/*
inline GFL_BOOL GFLW_FILE_INFORMATION::HasComment(
	void
	) const
{
	ERROR_Check(isOk, ""); 
	return Information.NumberOfComment > 0 ? GFL_TRUE : GFL_FALSE; 
}

// ~

inline const char * GFLW_FILE_INFORMATION::GetComment(
	GFL_INT32 index
	) const
{
	ERROR_Check(isOk, ""); 
	ERROR_Check(index >= 0 && index < Information.NumberOfComment, ""); 
	return Information.Comment[index]; 
}
*/
#endif
