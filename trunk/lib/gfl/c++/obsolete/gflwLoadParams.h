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

#ifndef __GFLW_LOAD_PARAMS_H__
#define __GFLW_LOAD_PARAMS_H__

class GFLW_LOAD_PARAMS {
	public:
		GFLW_LOAD_PARAMS(void); 
		virtual ~GFLW_LOAD_PARAMS(void); 

		void			SetFlags(GFL_UINT32 flags); 
		void			SetFormat(GFLW_FORMAT &format); 
		void			SetFormat(GFL_INT32 index); 
		void			SetFormat(const char *name); 
		void			SetColorModel(GFL_BITMAP_TYPE color_model); 
		void			SetOrigin(GFL_ORIGIN origin); 
		void			SetImageWanted(GFL_INT32 image); 
		void			SetLinePadding(GFL_UINT32 pad); 
		void			SetRawWidth(GFL_INT32 width); 
		void			SetRawHeight(GFL_INT32 height); 
		void			SetRawOffset(GFL_UINT32 offset); 
		void			SetRawChannelOrder(GFL_CORDER order); 
		void			SetRawChannelType(GFL_CTYPE type); 
		void			SetPcdBase(GFL_UINT16 base); 

		void      SetReadCallback(GFL_READ_CALLBACK read_proc); 
		void      SetTellCallback(GFL_TELL_CALLBACK tell_proc); 
		void      SetSeekCallback(GFL_SEEK_CALLBACK seek_proc); 
	
		friend class GFLW_BITMAP; 

	private:
		GFL_LOAD_PARAMS
			params; 
}; 

// INLINE

inline GFLW_LOAD_PARAMS::GFLW_LOAD_PARAMS(
	void
	)
{
	gflGetDefaultLoadParams(&params); 
}

// ~

inline GFLW_LOAD_PARAMS::~GFLW_LOAD_PARAMS(
	void
	)
{
}

// ~

inline void GFLW_LOAD_PARAMS::SetFlags(
	GFL_UINT32 flags
	)
{
	params.Flags = flags; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetFormat(
	GFL_INT32 index
	)
{
	params.FormatIndex = index; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetFormat(
	GFLW_FORMAT &format
	)
{
	SetFormat(format.GetIndex()); 
}

// ~

inline void GFLW_LOAD_PARAMS::SetFormat(
	const char *name
	)
{
	params.FormatIndex = gflGetFormatIndexByName(name); 
}

// ~

inline void GFLW_LOAD_PARAMS::SetOrigin(
	GFL_ORIGIN origin
	)
{
	params.Origin = origin; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetColorModel(
	GFL_BITMAP_TYPE color_model
	)
{
	params.ColorModel = color_model; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetImageWanted(
	GFL_INT32 image
	)
{
	params.ImageWanted = image; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetLinePadding(
	GFL_UINT32 pad
	)
{
	params.LinePadding = pad; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetRawWidth(
	GFL_INT32 width
	)
{
	params.Width = width; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetRawHeight(
	GFL_INT32 height
	)
{
	params.Height = height; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetRawOffset(
	GFL_UINT32 offset
	)
{
	params.Offset = offset; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetRawChannelOrder(
	GFL_CORDER order
	)
{
	params.ChannelOrder = order; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetRawChannelType(
	GFL_CTYPE type
	)
{
	params.ChannelType = type; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetPcdBase(
	GFL_UINT16 base
	)
{
	params.PcdBase = base; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetReadCallback(
	GFL_READ_CALLBACK read_proc
	)
{
	params.Callbacks.Read = read_proc; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetTellCallback(
	GFL_TELL_CALLBACK tell_proc
	)
{
	params.Callbacks.Tell = tell_proc; 
}

// ~

inline void GFLW_LOAD_PARAMS::SetSeekCallback(
	GFL_SEEK_CALLBACK seek_proc
	)
{
	params.Callbacks.Seek = seek_proc; 
}

#endif
