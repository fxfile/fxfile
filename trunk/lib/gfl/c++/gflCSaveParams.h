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

#ifndef __GFLC_SAVE_PARAMS_H__
#define __GFLC_SAVE_PARAMS_H__

class GFLC_SAVE_PARAMS {
	public:
		GFLC_SAVE_PARAMS(void); 
		virtual ~GFLC_SAVE_PARAMS(void); 
		
		// ~~ 
		
		void			setFlags(GFL_UINT32 flags); 
		void			setFormat(GFLC_FORMAT &format); 
		void			setFormat(GFL_INT32 index); 
		void			setFormat(const char *name); 
		void			setCompression(GFL_COMPRESSION compression); 
		void			setQuality(GFL_INT16 quality); 
		void			setCompressionLevel(GFL_INT16 compression_level); 
		void			setInterlaced(GFL_BOOL interlaced); 
		void			setProgressive(GFL_BOOL progressive); 
		void			setRawOffset(GFL_UINT32 offset); 
		void			setRawChannelOrder(GFL_CORDER order); 
		void			setRawChannelType(GFL_CTYPE type); 

		void      setWriteCallback(GFL_WRITE_CALLBACK write_proc); 
		void      setTellCallback(GFL_TELL_CALLBACK tell_proc); 
		void      setSeekCallback(GFL_SEEK_CALLBACK seek_proc); 
	
		friend class GFLC_BITMAP; 

	private:
		GFL_SAVE_PARAMS
			m_params; 
}; 

// INLINE

inline GFLC_SAVE_PARAMS::GFLC_SAVE_PARAMS(
	void
	)
{
	gflGetDefaultSaveParams(&m_params); 
}

// ~

inline GFLC_SAVE_PARAMS::~GFLC_SAVE_PARAMS(
	void
	)
{
}

// ~

inline void GFLC_SAVE_PARAMS::setFlags(
	GFL_UINT32 flags
	)
{
	m_params.Flags = flags; 
}

// ~

inline void GFLC_SAVE_PARAMS::setFormat(
	GFL_INT32 index
	)
{
	m_params.FormatIndex = index; 
}

// ~

inline void GFLC_SAVE_PARAMS::setFormat(
	GFLC_FORMAT &format
	)
{
	setFormat(format.getIndex()); 
}

// ~

inline void GFLC_SAVE_PARAMS::setFormat(
	const char *name
	)
{
	m_params.FormatIndex = gflGetFormatIndexByName(name); 
}

// ~

inline void GFLC_SAVE_PARAMS::setCompression(
	GFL_COMPRESSION compression
	)
{
	m_params.Compression = compression; 
}

// ~

inline void GFLC_SAVE_PARAMS::setQuality(
	GFL_INT16 quality
	)
{
	m_params.Quality = quality; 
}

// ~

inline void GFLC_SAVE_PARAMS::setCompressionLevel(
	GFL_INT16 compression_level
	)
{
	m_params.CompressionLevel = compression_level; 
}

// ~

inline void GFLC_SAVE_PARAMS::setInterlaced(
	GFL_BOOL interlaced
	)
{
	m_params.Interlaced = interlaced; 
}

// ~

inline void GFLC_SAVE_PARAMS::setProgressive(
	GFL_BOOL progressive
	)
{
	m_params.Progressive = progressive; 
}

// ~

inline void GFLC_SAVE_PARAMS::setRawOffset(
	GFL_UINT32 offset
	)
{
	m_params.Offset = offset; 
}

// ~

inline void GFLC_SAVE_PARAMS::setRawChannelOrder(
	GFL_CORDER order
	)
{
	m_params.ChannelOrder = order; 
}

// ~

inline void GFLC_SAVE_PARAMS::setRawChannelType(
	GFL_CTYPE type
	)
{
	m_params.ChannelType = type; 
}

// ~

inline void GFLC_SAVE_PARAMS::setWriteCallback(
	GFL_WRITE_CALLBACK write_proc
	)
{
	m_params.Callbacks.Write = write_proc; 
}

// ~

inline void GFLC_SAVE_PARAMS::setTellCallback(
	GFL_TELL_CALLBACK tell_proc
	)
{
	m_params.Callbacks.Tell = tell_proc; 
}

// ~

inline void GFLC_SAVE_PARAMS::setSeekCallback(
	GFL_SEEK_CALLBACK seek_proc
	)
{
	m_params.Callbacks.Seek = seek_proc; 
}

#endif
