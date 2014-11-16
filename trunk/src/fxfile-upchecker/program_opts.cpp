//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "program_opts.h"
#include "getopt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace upchecker
{
ProgramOpts::ProgramOpts(void)
{
}

ProgramOpts::~ProgramOpts(void)
{
}

void ProgramOpts::parse(void)
{
    xpr_sint_t sOptionIndex = -1;
    xpr_sint_t sChar;

    static struct option sLongOptions[] =
    {
        { XPR_STRING_LITERAL("conf_dir"), ARG_REQ,  0,               0 },
        { ARG_NULL,                       ARG_NULL, ARG_NULL, ARG_NULL }
    };

    while (true)
    {
        sChar = getopt_long(__argc, __targv, XPR_STRING_LITERAL(""), sLongOptions, &sOptionIndex);

        if (sChar == -1)
        {
            break;
        }

        switch (sChar)
        {
        case 0:
            {
                if (_tcscmp(sLongOptions[sOptionIndex].name, XPR_STRING_LITERAL("conf_dir")) == 0)
                {
                    mConfDir = optarg;
                }
                break;
            }
        }
    }
}
} // namespace upchecker
} // namespace fxfile
