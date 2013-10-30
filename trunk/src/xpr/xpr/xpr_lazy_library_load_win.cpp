//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_lazy_library_load.h"

namespace xpr
{
LazyIconvLibrary gLazyIconvLibrary = {0};

xpr_bool_t loadIconvLibrary(void)
{
    if (gLazyIconvLibrary.mDlHandle == XPR_NULL)
    {
        gLazyIconvLibrary.mDlHandle = ::LoadLibrary(XPR_STRING_LITERAL("libiconv-2.dll"));
        if (XPR_IS_NOT_NULL(gLazyIconvLibrary.mDlHandle))
        {
            gLazyIconvLibrary.mIconOpenFunc  = (LIBICON_OPEN_FUNC )::GetProcAddress((HMODULE)gLazyIconvLibrary.mDlHandle, (const xpr_char_t *)"libiconv_open");
            gLazyIconvLibrary.mIconFunc      = (LIBICON_FUNC      )::GetProcAddress((HMODULE)gLazyIconvLibrary.mDlHandle, (const xpr_char_t *)"libiconv");
            gLazyIconvLibrary.mIconCloseFunc = (LIBICON_CLOSE_FUNC)::GetProcAddress((HMODULE)gLazyIconvLibrary.mDlHandle, (const xpr_char_t *)"libiconv_close");
        }

        if (XPR_IS_NULL(gLazyIconvLibrary.mIconOpenFunc ) ||
            XPR_IS_NULL(gLazyIconvLibrary.mIconFunc     ) ||
            XPR_IS_NULL(gLazyIconvLibrary.mIconCloseFunc))
        {
            unloadIconvLibrary();

            return XPR_FALSE;
        }
    }

    return XPR_TRUE;
}

void unloadIconvLibrary(void)
{
    if (gLazyIconvLibrary.mDlHandle != XPR_NULL)
    {
        ::FreeLibrary((HMODULE)gLazyIconvLibrary.mDlHandle);

        memset(&gLazyIconvLibrary, 0, sizeof(gLazyIconvLibrary));
    }
}
} // namespace xpr
