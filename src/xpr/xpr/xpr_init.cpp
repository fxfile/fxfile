//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_init.h"
#include "xpr_system.h"
#include "xpr_lazy_library_load.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

namespace xpr
{
XPR_DL_API xpr_bool_t initialize(void)
{
    xpr_bool_t sResult = XPR_FALSE;

    initSystemInfo();

    sResult = loadIconvLibrary();
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_FALSE;
    }

    ::xmlInitParser();

    return XPR_TRUE;
}

XPR_DL_API void finalize(void)
{
    ::xmlCleanupParser();

    unloadIconvLibrary();
}
} // namespace xpr
