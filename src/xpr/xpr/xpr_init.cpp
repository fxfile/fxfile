//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_init.h"
#include "xpr_system.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
void initialize(void)
{
    initSystemInfo();

    ::xmlInitParser();
}

void finalize(void)
{
    ::xmlCleanupParser();
}
} // namespace xpr
