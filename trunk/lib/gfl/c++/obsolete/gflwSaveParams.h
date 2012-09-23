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

#ifndef __GFLW_SAVE_PARAMS_H__
#define __GFLW_SAVE_PARAMS_H__

class GFLW_SAVE_PARAMS {
	public:
		GFLW_SAVE_PARAMS(void); 
		virtual ~GFLW_SAVE_PARAMS(void); 
		
		void			SetFlags(GFL_UINT32 flags); 
		void			SetFormat(GFLW_FORMAT &format); 
		void			SetFormat(GFL_INT32 index); 
		void			SetFormat(const char *name); 
		void			SetCompression(GFL_COMPRESSION compression); 
		void			SetQuality(GFL_INT16 quality); 
		void			SetCompressionLevel(GFL_INT16 compression_level); 
		void			SetInterlaced(GFL_BOOL interlaced); 
		void			SetProgressive(GFL_BOOL progressive); 
		void			SetRawOffset(GFL_UINT32 offset); 
		void			SetRawChannelOrder(GFL_CORDER order); 
		void			SetRawChannelType(GFL_CTYPE type); 

		void      SetWriteCallback(GFL_WRITE_CALLBACK write_proc); 
		void      SetTellCallback(GFL_TELL_CALLBACK tell_proc); 
		void      SetSeekCallback(GFL_SEEK_CALLBACK seek_proc); 
	
		friend class GFLW_BITMAP; 

	private:
		GFL_SAVE_PARAMS
			params; 
}; 

// INLINE

inline GFLW_SAVE_PARAMS::GFLW_SAVE_PARAMS(
	void
	)
{
	gflGetDefaultSaveParams(&params); 
}

// ~

inline GFLW_SAVE_PARAMS::~GFLW_SAVE_PARAMS(
	void
	)
{
}

// ~

inline void GFLW_SAVE_PARAMS::SetFlags(
	GFL_UINT32 flags
	)
{
	params.Flags = flags; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetFormat(
	GFL_INT32 index
	)
{
	params.FormatIndex = index; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetFormat(
	GFLW_FORMAT &format
	)
{
	SetFormat(format.GetIndex()); 
}

// ~

inline void GFLW_SAVE_PARAMS::SetFormat(
	const char *name
	)
{
	params.FormatIndex = gflGetFormatIndexByName(name); 
}

// ~

inline void GFLW_SAVE_PARAMS::SetCompression(
	GFL_COMPRESSION compression
	)
{
	params.Compression = compression; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetQuality(
	GFL_INT16 quality
	)
{
	params.Quality = quality; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetCompressionLevel(
	GFL_INT16 compression_level
	)
{
	params.CompressionLevel = compression_level; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetInterlaced(
	GFL_BOOL interlaced
	)
{
	params.Interlaced = interlaced; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetProgressive(
	GFL_BOOL progressive
	)
{
	params.Progressive = progressive; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetRawOffset(
	GFL_UINT32 offset
	)
{
	params.Offset = offset; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetRawChannelOrder(
	GFL_CORDER order
	)
{
	params.ChannelOrder = order; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetRawChannelType(
	GFL_CTYPE type
	)
{
	params.ChannelType = type; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetWriteCallback(
	GFL_WRITE_CALLBACK write_proc
	)
{
	params.Callbacks.Write = write_proc; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetTellCallback(
	GFL_TELL_CALLBACK tell_proc
	)
{
	params.Callbacks.Tell = tell_proc; 
}

// ~

inline void GFLW_SAVE_PARAMS::SetSeekCallback(
	GFL_SEEK_CALLBACK seek_proc
	)
{
	params.Callbacks.Seek = seek_proc; 
}

#endif
