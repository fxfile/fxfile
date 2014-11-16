//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "meta_data_file.h"
#include "conf_file_ex.h"
#include "crc32.h"
#include "md5.h"
#include "sha.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace upchecker
{
static const xpr_tchar_t kProgramKey[] = XPR_STRING_LITERAL("program");
static const xpr_tchar_t kVersionKey[] = XPR_STRING_LITERAL("version");
static const xpr_tchar_t sNewUrlKey [] = XPR_STRING_LITERAL("new_url");

/**
 * parse meta-data file
 */
xpr_bool_t MetaDataFile::parseMetaFile(const xpr::string &aMetaFilePath, MetaData &aMetaData)
{
    fxfile::base::ConfFileEx sConfFile(aMetaFilePath.c_str());

    sConfFile.setFlags(fxfile::base::ConfFile::kFlagLock);

    if (sConfFile.load() == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    fxfile::base::ConfFile::Section *sSection = sConfFile.findSection(XPR_STRING_LITERAL(""));
    XPR_ASSERT(sSection != XPR_NULL);

    aMetaData.mProgram = sConfFile.getValueS(sSection, kProgramKey, XPR_STRING_LITERAL(""));
    aMetaData.mVersion = sConfFile.getValueS(sSection, kVersionKey, XPR_STRING_LITERAL(""));
    aMetaData.mNewUrl  = sConfFile.getValueS(sSection, sNewUrlKey,  XPR_STRING_LITERAL(""));

    return XPR_TRUE;
}
} // namespace fxfile
} // namespace upchecker
