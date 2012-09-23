//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DlgStateMgr.h"

#include "CfgPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DlgStateMgr::DlgStateMgr(void)
{
}

DlgStateMgr::~DlgStateMgr(void)
{
}

xpr_bool_t DlgStateMgr::getName(xpr_uint_t aType, std::tstring &aName)
{
    aName.clear();

    switch (aType)
    {
    case TypeAttrTime:      aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.attr_time"));        break;
    case TypeBatchCreate:   aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.batch_create"));     break;
    case TypeCrc:           aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.crc"));              break;
    case TypeDosCmd:        aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.dos_cmd"));          break;
    case TypeDriveSel:      aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.drive"));            break;
    case TypeFileScrap:     aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.file_scrap"));       break;
    case TypeFileCombine:   aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.file_combine"));     break;
    case TypeFileMerge:     aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.file_merge"));       break;
    case TypeFileSplit:     aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.file_split"));       break;
    case TypeHistory:       aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.history"));          break;
    case TypeGoPath:        aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.go_path"));          break;
    case TypeBookmarkEdit:  aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.bookmark_edit"));    break;
    case TypeParamExec:     aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.param_exec"));       break;
    case TypePicConv:       aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.pic_conv"));         break;
    case TypePicViewer:     aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.pic_viewer"));       break;
    case TypeRename:        aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.batch_rename"));     break;
    case TypeSearch:        aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.search_bar"));       break;
    case TypeSelFilter:     aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.select_by_filter")); break;
    case TypeSelName:       aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.select_by_name"));   break;
    case TypeSharedFile:    aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.shared_process"));   break;
    case TypeSyncDirs:      aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.folder_sync"));      break;
    case TypeTextMerge:     aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.text_merge"));       break;
    case TypeTextOut:       aName = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.type.file_list"));        break;
    }

    return (aName.empty() == XPR_TRUE) ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t DlgStateMgr::getPath(xpr_uint_t aType, xpr_uint_t aSubType, std::tstring &aPath)
{
    aPath.clear();

    switch (aType)
    {
    case TypeAttrTime:      aPath = XPR_STRING_LITERAL("dlgAttrTime");      break;
    case TypeDosCmd:        aPath = XPR_STRING_LITERAL("dlgDosCmd");        break;
    case TypeDriveSel:      aPath = XPR_STRING_LITERAL("dlgDrive");         break;
    case TypeFileCombine:   aPath = XPR_STRING_LITERAL("dlgFileCombine");   break;
    case TypeFileMerge:     aPath = XPR_STRING_LITERAL("dlgFileMerge");     break;
    case TypeFileSplit:     aPath = XPR_STRING_LITERAL("dlgFileSplit");     break;
    case TypeHistory:       aPath = XPR_STRING_LITERAL("dlgHistory");       break;
    case TypeGoPath:        aPath = XPR_STRING_LITERAL("dlgGoPath");        break;
    case TypeBookmarkEdit:  aPath = XPR_STRING_LITERAL("dlgLink");          break;
    case TypeParamExec:     aPath = XPR_STRING_LITERAL("dlgParamExec");     break;
    case TypePicConv:       aPath = XPR_STRING_LITERAL("dlgPicCon");        break;
    case TypePicViewer:     aPath = XPR_STRING_LITERAL("dlgPicViewer");     break;
    case TypeSharedFile:    aPath = XPR_STRING_LITERAL("dlgSharedFile");    break;
    case TypeSyncDirs:      aPath = XPR_STRING_LITERAL("dlgSyncDirs");      break;
    case TypeTextMerge:     aPath = XPR_STRING_LITERAL("dlgTextJoin");      break;
    case TypeTextOut:       aPath = XPR_STRING_LITERAL("dlgTextOut");       break;

    case TypeBatchCreate:
        {
            switch (aSubType)
            {
            case SubTypeBatchCreate:       aPath = XPR_STRING_LITERAL("dlgBatchCreate");       break;
            case SubTypeBatchCreateFormat: aPath = XPR_STRING_LITERAL("dlgBatchCreateFormat"); break;
            }
            break;
        }

    case TypeCrc:
        {
            switch (aSubType)
            {
            case SubTypeCrcCreate: aPath = XPR_STRING_LITERAL("dlgCRC");      break;
            case SubTypeCrcCheck:  aPath = XPR_STRING_LITERAL("dlgCRCCheck"); break;
            }
            break;
        }

    case TypeFileScrap:
        {
            switch (aSubType)
            {
            case SubTypeFileScrap:     aPath = XPR_STRING_LITERAL("dlgFileScrap");     break;
            case SubTypeFileScrapDrop: aPath = XPR_STRING_LITERAL("dlgFileScrapDrop"); break;
            }
            break;
        }

    case TypeRename:
        {
            switch (aSubType)
            {
            case SubTypeRename:            aPath = XPR_STRING_LITERAL("dlgRename");             break;
            case SubTypeRename1:           aPath = XPR_STRING_LITERAL("dlgRename1");            break;
            case SubTypeRename2:           aPath = XPR_STRING_LITERAL("dlgRename2");            break;
            case SubTypeRename3:           aPath = XPR_STRING_LITERAL("dlgRename3");            break;
            case SubTypeRename4:           aPath = XPR_STRING_LITERAL("dlgRename4");            break;
            case SubTypeRename5:           aPath = XPR_STRING_LITERAL("dlgRename5");            break;
            case SubTypeRenameDirectInput: aPath = XPR_STRING_LITERAL("dlgRename-DirectInput"); break;
            case SubTypeRenameEdit:        aPath = XPR_STRING_LITERAL("dlgRenameEdit");         break;
            }
            break;
        }

    case TypeSearch:
        {
            switch (aSubType)
            {
            case SubTypeSearch: aPath = XPR_STRING_LITERAL("dlgSearch"); break;
            case SubTypeSchLoc: aPath = XPR_STRING_LITERAL("dlgSchLoc"); break;
            }
            break;
        }

    case TypeSelFilter:
        {
            switch (aSubType)
            {
            case SubTypeSelFilter:   aPath = XPR_STRING_LITERAL("dlgSelFilter");   break;
            case SubTypeUnSelFilter: aPath = XPR_STRING_LITERAL("dlgUnSelFilter"); break;
            }
            break;
        }

    case TypeSelName:
        {
            switch (aSubType)
            {
            case SubTypeSelName:   aPath = XPR_STRING_LITERAL("dlgSelName");   break;
            case SubTypeUnSelName: aPath = XPR_STRING_LITERAL("dlgUnSelName"); break;
            }
            break;
        }
    }

    if (aPath.empty() == true)
        return XPR_FALSE;

    aPath += XPR_STRING_LITERAL(".ini");

    return XPR_TRUE;
}

xpr_bool_t DlgStateMgr::getPathList(xpr_uint_t aType, PathDeque &aPathDeque)
{
    aPathDeque.clear();

    xpr_uint_t sSubType1 = SubTypeNone;
    xpr_uint_t sSubType2 = SubTypeNone+1;

    switch (aType)
    {
    case TypeAttrTime:
    case TypeDosCmd:
    case TypeDriveSel:
    case TypeFileCombine:
    case TypeFileMerge:
    case TypeFileSplit:
    case TypeHistory:
    case TypeGoPath:
    case TypeBookmarkEdit:
    case TypeParamExec:
    case TypePicConv:
    case TypePicViewer:
    case TypeSharedFile:
    case TypeSyncDirs:
    case TypeTextMerge:
    case TypeTextOut: break;

    case TypeBatchCreate: sSubType1 = SubTypeBatchCreateBegin; sSubType2 = SubTypeBatchCreateEnd; break;
    case TypeCrc:         sSubType1 = SubTypeCrcBegin;         sSubType2 = SubTypeCrcEnd;         break;
    case TypeFileScrap:   sSubType1 = SubTypeFileScrapBegin;   sSubType2 = SubTypeFileScrapEnd;   break;
    case TypeRename:      sSubType1 = SubTypeRenameBegin;      sSubType2 = SubTypeRenameEnd;      break;
    case TypeSearch:      sSubType1 = SubTypeSearchBegin;      sSubType2 = SubTypeSearchEnd;      break;
    case TypeSelFilter:   sSubType1 = SubTypeSelFilterBegin;   sSubType2 = SubTypeSelFilterEnd;   break;
    case TypeSelName:     sSubType1 = SubTypeSelNameBegin;     sSubType2 = SubTypeSelNameEnd;     break;
    }

    xpr_uint_t sSubType;
    std::tstring sPath;

    for (sSubType = sSubType1; sSubType < sSubType2; ++sSubType)
    {
        if (getPath(aType, sSubType, sPath))
            aPathDeque.push_back(sPath);
    }

    return (aPathDeque.empty() == true) ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t DlgStateMgr::clear(xpr_uint_t aType)
{
    PathDeque sPathDeque;
    if (getPathList(aType, sPathDeque) == XPR_FALSE)
        return XPR_FALSE;

    std::tstring sDir;
    if (CfgPath::instance().getLoadDir(CfgPath::TypeSettings, sDir) == XPR_FALSE)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    if (sPathDeque.empty() == false)
    {
        xpr_tchar_t *sFileNames = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sPathDeque.size() + 100];
        if (XPR_IS_NOT_NULL(sFileNames))
        {
            sFileNames[0] = XPR_STRING_LITERAL('\0');
            sFileNames[1] = XPR_STRING_LITERAL('\0');

            xpr_size_t sOffset = 0;
            PathDeque::iterator sIterator;

            sIterator = sPathDeque.begin();
            for (; sIterator != sPathDeque.end(); ++sIterator)
            {
                _stprintf(sFileNames + sOffset, XPR_STRING_LITERAL("%s\\%s"), sDir.c_str(), sIterator->c_str());
                sOffset += _tcslen(sFileNames + sOffset) + 1;
            }

            // double null
            sFileNames[sOffset] = XPR_STRING_LITERAL('\0');

            fxb::SHSilentDeleteFile(sFileNames);

            XPR_SAFE_DELETE_ARRAY(sFileNames);

            sResult = XPR_TRUE;
        }
    }

    sPathDeque.clear();

    return sResult;
}

void DlgStateMgr::clearAll(void)
{
    xpr_uint_t sType;
    for (sType = TypeBegin; sType < TypeEnd; ++sType)
        clear(sType);
}
