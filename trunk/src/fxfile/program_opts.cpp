//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "program_opts.h"
#include "getopt.h"
#include "app_ver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
ProgramOpts::ProgramOpts(void)
    : mShowUsage(XPR_FALSE)
    , mShowVersion(XPR_FALSE)
    , mSelect(XPR_FALSE)
    , mSplitRowCount(-1)
    , mSplitColumnCount(-1)
    , mResetConf(XPR_FALSE)
{
    xpr_memset_zero(mDir, sizeof(mDir));
}

ProgramOpts::~ProgramOpts(void)
{
}

void ProgramOpts::parse(void)
{
    xpr_sint_t sResetFlag   = 0;
    xpr_sint_t sConfDirFlag = 0;
    xpr_sint_t sOptionIndex = -1;
    xpr_sint_t sChar;

    static struct option sLongOptions[] =
    {
        { XPR_STRING_LITERAL("help"),     ARG_NONE, 0,           XPR_STRING_LITERAL('h') },
        { XPR_STRING_LITERAL("version"),  ARG_NONE, 0,           XPR_STRING_LITERAL('v') },
        { XPR_STRING_LITERAL("window"),   ARG_REQ,  0,           XPR_STRING_LITERAL('w') },
        { XPR_STRING_LITERAL("dir1"),     ARG_REQ,  0,           XPR_STRING_LITERAL('1') },
        { XPR_STRING_LITERAL("dir2"),     ARG_REQ,  0,           XPR_STRING_LITERAL('2') },
        { XPR_STRING_LITERAL("dir3"),     ARG_REQ,  0,           XPR_STRING_LITERAL('3') },
        { XPR_STRING_LITERAL("dir4"),     ARG_REQ,  0,           XPR_STRING_LITERAL('4') },
        { XPR_STRING_LITERAL("dir5"),     ARG_REQ,  0,           XPR_STRING_LITERAL('5') },
        { XPR_STRING_LITERAL("dir6"),     ARG_REQ,  0,           XPR_STRING_LITERAL('6') },
        { XPR_STRING_LITERAL("select"),   ARG_NONE, 0,           XPR_STRING_LITERAL('s') },
        { XPR_STRING_LITERAL("reset"),    ARG_NONE, &sResetFlag,                       1 },
        { XPR_STRING_LITERAL("conf_dir"), ARG_REQ,  0,                                 0 },
        { ARG_NULL,                       ARG_NULL, ARG_NULL,                   ARG_NULL }
    };

    while (true)
    {
        sChar = getopt_long(__argc, __targv, _T("-w:1:2:3:4:5:6:shv"), sLongOptions, &sOptionIndex);

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

        case 1: // non-option argument
            {
                mDir[0] = optarg;
                break;
            }

        case 'w':
            {
                _stscanf(optarg, XPR_STRING_LITERAL("%dx%d"), &mSplitRowCount, &mSplitColumnCount);
                break;
            }

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
            {
                xpr_sint_t sIndex = 5 - ('6' - sChar);
                mDir[sIndex] = optarg;
                break;
            }

        case 's':
            {
                mSelect = XPR_TRUE;
                break;
            }

        case 'h':
            {
                mShowUsage = XPR_TRUE;
                break;
            }

        case 'v':
            {
                mShowVersion = XPR_TRUE;
                break;
            }

        case '?': // unknown option
            {
                mShowUsage = XPR_TRUE;
                break;
            }
        }
    }

    if (XPR_IS_TRUE(sResetFlag))
    {
        mResetConf = XPR_TRUE;
    }
}

void ProgramOpts::showUsage(void)
{
    xpr::tstring sMsg;
    sMsg += XPR_STRING_LITERAL("Usage: fxfile.exe path\r\n");
    sMsg += XPR_STRING_LITERAL("\r\n");
    sMsg += XPR_STRING_LITERAL("Available options (use --help for help):\r\n");
    sMsg += XPR_STRING_LITERAL("-h --help          Usage information\r\n");
    sMsg += XPR_STRING_LITERAL("-v --version       Show program version\r\n");
    sMsg += XPR_STRING_LITERAL("-w --window (arg)  Window split (1x1|1x2|1x3|2x1|2x2|2x3)\r\n");
    sMsg += XPR_STRING_LITERAL("--dir[1-6]         Initial directory by split window\r\n");
    sMsg += XPR_STRING_LITERAL("-s --select        Select file\r\n");
    sMsg += XPR_STRING_LITERAL("--reset            Reset configuration\r\n");
    sMsg += XPR_STRING_LITERAL("--conf_dir (arg)   Configuration directory to load\r\n");
    MessageBox(XPR_NULL, sMsg.c_str(), FXFILE_PROGRAM_NAME, MB_OK | MB_ICONINFORMATION);
}

void ProgramOpts::showVersion(void)
{
    xpr::tstring sMsg;
    sMsg = FXFILE_PROGRAM_NAME;

    xpr_tchar_t sFullVer[0xff] = {0};
    getFullAppVer(sFullVer);

    sMsg += XPR_STRING_LITERAL(' ');
    sMsg += sFullVer;

    MessageBox(XPR_NULL, sMsg.c_str(), FXFILE_PROGRAM_NAME, MB_OK | MB_ICONINFORMATION);
}
} // namespace fxfile
