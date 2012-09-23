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

#ifndef __GFLC_H__
#define __GFLC_H__

#include <iostream.h>

// If you want GFLE feature
//#define __GFLC_GFLE_FUNCTIONS__
// If you want DRAW feature
//#define __GFLC_DRAW_FUNCTIONS__

//#define __GFLC_USE_STD_STRING__

#include "libgfl.h"

#ifdef __GFLC_GFLE_FUNCTIONS__
	#include "libgfle.h"
#endif

#include "gflCException.h"

#define ERROR_Check(_a, _m) \
	if (!(_a)) \
	{ \
		throw GFLC_EXCEPTION(__FILE__, __LINE__, _m);\
	}

#include "gflCLibrary.h"
#include "gflCFormat.h"
#include "gflCFileInformation.h"
#include "gflCSaveParams.h"
#include "gflCLoadParams.h"
#include "gflCColor.h"
#include "gflCBitmap.h"

#endif
