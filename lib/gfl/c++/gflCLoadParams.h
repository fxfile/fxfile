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

#ifndef __GFLC_LOAD_PARAMS_H__
#define __GFLC_LOAD_PARAMS_H__

class GFLC_LOAD_PARAMS {
	public:
		GFLC_LOAD_PARAMS(void); 
		virtual ~GFLC_LOAD_PARAMS(void); 
		
		// ~~ 
		
		void			setFlags(GFL_UINT32 flags); 
		void			setFormat(GFLC_FORMAT &format); 
		void			setFormat(GFL_INT32 index); 
		void			setFormat(const char *name); 
		void			setColorModel(GFL_BITMAP_TYPE color_model); 
		void			setOrigin(GFL_ORIGIN origin); 
		void			setImageWanted(GFL_INT32 image); 
		void			setLinePadding(GFL_UINT32 pad); 
		void			setRawWidth(GFL_INT32 width); 
		void			setRawHeight(GFL_INT32 height); 
		void			setRawOffset(GFL_UINT32 offset); 
		void			setRawChannelOrder(GFL_CORDER order); 
		void			setRawChannelType(GFL_CTYPE type); 
		void			setPcdBase(GFL_UINT16 base); 

		void      setReadCallback(GFL_READ_CALLBACK read_proc); 
		void      setTellCallback(GFL_TELL_CALLBACK tell_proc); 
		void      setSeekCallback(GFL_SEEK_CALLBACK seek_proc); 
	
		friend class GFLC_BITMAP; 

	private:
		GFL_LOAD_PARAMS
			m_params; 
}; 

// INLINE

inline GFLC_LOAD_PARAMS::GFLC_LOAD_PARAMS(
	void
	)
{
	gflGetDefaultLoadParams(&m_params); 
}

// ~

inline GFLC_LOAD_PARAMS::~GFLC_LOAD_PARAMS(
	void
	)
{
}

// ~

inline void GFLC_LOAD_PARAMS::setFlags(
	GFL_UINT32 flags
	)
{
	m_params.Flags = flags; 
}

// ~

inline void GFLC_LOAD_PARAMS::setFormat(
	GFL_INT32 index
	)
{
	m_params.FormatIndex = index; 
}

// ~

inline void GFLC_LOAD_PARAMS::setFormat(
	GFLC_FORMAT &format
	)
{
	setFormat(format.getIndex()); 
}

// ~

inline void GFLC_LOAD_PARAMS::setFormat(
	const char *name
	)
{
	m_params.FormatIndex = gflGetFormatIndexByName(name); 
}

// ~

inline void GFLC_LOAD_PARAMS::setOrigin(
	GFL_ORIGIN origin
	)
{
	m_params.Origin = origin; 
}

// ~

inline void GFLC_LOAD_PARAMS::setColorModel(
	GFL_BITMAP_TYPE color_model
	)
{
	m_params.ColorModel = color_model; 
}

// ~

inline void GFLC_LOAD_PARAMS::setImageWanted(
	GFL_INT32 image
	)
{
	m_params.ImageWanted = image; 
}

// ~

inline void GFLC_LOAD_PARAMS::setLinePadding(
	GFL_UINT32 pad
	)
{
	m_params.LinePadding = pad; 
}

// ~

inline void GFLC_LOAD_PARAMS::setRawWidth(
	GFL_INT32 width
	)
{
	m_params.Width = width; 
}

// ~

inline void GFLC_LOAD_PARAMS::setRawHeight(
	GFL_INT32 height
	)
{
	m_params.Height = height; 
}

// ~

inline void GFLC_LOAD_PARAMS::setRawOffset(
	GFL_UINT32 offset
	)
{
	m_params.Offset = offset; 
}

// ~

inline void GFLC_LOAD_PARAMS::setRawChannelOrder(
	GFL_CORDER order
	)
{
	m_params.ChannelOrder = order; 
}

// ~

inline void GFLC_LOAD_PARAMS::setRawChannelType(
	GFL_CTYPE type
	)
{
	m_params.ChannelType = type; 
}

// ~

inline void GFLC_LOAD_PARAMS::setPcdBase(
	GFL_UINT16 base
	)
{
	m_params.PcdBase = base; 
}

// ~

inline void GFLC_LOAD_PARAMS::setReadCallback(
	GFL_READ_CALLBACK read_proc
	)
{
	m_params.Callbacks.Read = read_proc; 
}

// ~

inline void GFLC_LOAD_PARAMS::setTellCallback(
	GFL_TELL_CALLBACK tell_proc
	)
{
	m_params.Callbacks.Tell = tell_proc; 
}

// ~

inline void GFLC_LOAD_PARAMS::setSeekCallback(
	GFL_SEEK_CALLBACK seek_proc
	)
{
	m_params.Callbacks.Seek = seek_proc; 
}

#endif
