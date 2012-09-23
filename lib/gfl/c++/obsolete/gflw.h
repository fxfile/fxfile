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

#ifndef __GFLW_H__
#define __GFLW_H__

#include <iostream.h>

#define __GFLW_GFLE_FUNCTIONS__
#define __GFLW_DRAW_FUNCTIONS__

#include "libgfl.h"

#ifdef __GFLW_GFLE_FUNCTIONS__
	#include "libgfle.h"
#endif

#include "gflwException.h"

#define ERROR_Check(_a, _m) \
	if (!(_a)) \
	{ \
		throw GFLW_EXCEPTION(__FILE__, __LINE__, _m);\
	}

#include "gflwLibrary.h"
#include "gflwFormat.h"
#include "gflwFileInformation.h"
#include "gflwSaveParams.h"
#include "gflwLoadParams.h"
#include "gflwColor.h"
#include "gflwBitmap.h"

#endif
