//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_op_undo.h"

#include "fxb_file_op_thread.h"
#include "fxb_bb_info_file.h"           // for BitBucket Info File

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
xpr_tchar_t *FileOpUndo::mPath  = XPR_NULL;
xpr_sint_t   FileOpUndo::mIndex = -1;
xpr_sint_t   FileOpUndo::mMode  = -1;

#define UNDO_DIR_NAME_FORMAT  XPR_STRING_LITERAL("\\undo%04d")
#define UNDO_FILE_NAME_FORMAT XPR_STRING_LITERAL("%s\\undo%04d.dat")

FileOpUndo::FileOpUndo(void)
{
    validateUndoDir();
}

FileOpUndo::~FileOpUndo(void)
{
}

void FileOpUndo::validateUndoDir(void)
{
    if (XPR_IS_NOT_NULL(mPath))
    {
        if (IsExistFile(mPath) == XPR_TRUE)
            return;

        XPR_SAFE_DELETE_ARRAY(mPath);
    }

    // ex) C:\\Documents and Settings\\flychk\\Local Settings\\Temp\\flyExplorer\\undo

    mPath = new xpr_tchar_t[XPR_MAX_PATH + 1];

    GetTempPath(XPR_MAX_PATH, mPath);
    if (mPath[_tcslen(mPath)-1] == XPR_STRING_LITERAL('\\'))
        mPath[_tcslen(mPath)-1] = XPR_STRING_LITERAL('\0');

    _tcscat(mPath, XPR_STRING_LITERAL("\\flyExplorer"));
    if (IsExistFile(mPath) == XPR_FALSE)
        ::CreateDirectory(mPath, XPR_NULL);

    _tcscat(mPath, XPR_STRING_LITERAL("\\undo"));
    if (IsExistFile(mPath) == XPR_FALSE)
        ::CreateDirectory(mPath, XPR_NULL);

    xpr_sint_t i;
    xpr_tchar_t *sFileName = mPath + _tcslen(mPath);
    for (i = 0; ; ++i)
    {
        _stprintf(sFileName, UNDO_DIR_NAME_FORMAT, i);
        if (IsExistFile(mPath) == XPR_FALSE)
        {
            ::CreateDirectory(mPath, XPR_NULL);
            break;
        }
    }

    mIndex = -1;
    mMode = -1;
}

void FileOpUndo::deleteUndoDir(void)
{
    if (XPR_IS_NOT_NULL(mPath))
    {
        mPath[_tcslen(mPath) + 1] = '\0';
        SHSilentDeleteFile(mPath);

        XPR_SAFE_DELETE_ARRAY(mPath);
    }
}

//----------------------------------------------------------------------
// Undo File Structure
//
// +---------------------+----------+
// | Undo Mode           |        4 |
// +---------------------+----------+
// | Source Count        |        4 |
// +---------------------+----------+
// | Source Total Length |        4 |
// +---------------------+----------+
// | Source              |  Str Len |
// +---------------------+----------+
// | ...                 |  Str Len |
// +---------------------+----------+
// | Target Count        |        4 |
// +---------------------+----------+
// | Target Length       |        4 |
// +---------------------+----------+
// | Target              |  Str Len |
// +---------------------+----------+

// copy -> delete
// move -> move
// recycled bin -> restore

FILE *FileOpUndo::beginAddFile(void)
{
    validateUndoDir();

    mIndex++;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _stprintf(sPath, UNDO_FILE_NAME_FORMAT, mPath, mIndex);

    return _tfopen(sPath, XPR_STRING_LITERAL("wb"));
}

void FileOpUndo::endAddFile(FILE *aFile)
{
    if (XPR_IS_NOT_NULL(aFile))
        fclose(aFile);
}

FILE *FileOpUndo::beginUndoFile(xpr_bool_t aUpdate)
{
    validateUndoDir();

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _stprintf(sPath, UNDO_FILE_NAME_FORMAT, mPath, mIndex);

    return _tfopen(sPath, XPR_STRING_LITERAL("rb"));
}

void FileOpUndo::endUndoFile(FILE *aFile, xpr_bool_t aUpdate)
{
    if (XPR_IS_NOT_NULL(aFile))
        fclose(aFile);

    if (XPR_IS_TRUE(aUpdate))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        _stprintf(sPath, UNDO_FILE_NAME_FORMAT, mPath, mIndex);

        sPath[_tcslen(sPath) + 1] = '\0';
        SHSilentDeleteFile(sPath);

        mIndex--;
        if (mIndex < -1)
            mIndex = -1;
    }
}

//
// undo
//   - copy, move, rename: post-processing
//   - delete: pre-processing
//
void FileOpUndo::addOperation(SHFILEOPSTRUCT *aShFileOpStruct)
{
    xpr_sint_t sMode = -1;
    switch (aShFileOpStruct->wFunc)
    {
    case FO_COPY: sMode = MODE_COPY; break;
    case FO_MOVE: sMode = MODE_MOVE; break;
    case FO_DELETE:
        if (aShFileOpStruct->fFlags & FOF_ALLOWUNDO)
            sMode = MODE_TRASH;
        break;
    default: break;
    }

    if (sMode == -1)
        return;

    std::swap(mMode, sMode);

    const xpr_tchar_t *sSource = aShFileOpStruct->pFrom;
    const xpr_tchar_t *sTarget = aShFileOpStruct->pTo;
    HandleToMappings *sHandleToMappings = (HandleToMappings *)aShFileOpStruct->hNameMappings;

    std::tstring sPath;
    const xpr_tchar_t *sParsing = sSource;
    xpr_sint_t nLen = XPR_IS_NOT_NULL(sTarget) ? (xpr_sint_t)_tcslen(sTarget) : 4;

    // ex) C:\\Documents and Settings\\Adminstrator\\Application Data\\flyExplorer\\undo0000\\undo0000.dat

    FILE *sFile = beginAddFile();
    if (XPR_IS_NULL(sFile))
    {
        std::swap(mMode, sMode);
        return;
    }

    // Undo Mode
    fwrite(&mMode, sizeof(xpr_sint_t), 1, sFile);

    // Temp Source Path Total Count, Length
    fwrite(&mMode, sizeof(xpr_sint_t), 1, sFile);
    fwrite(&mMode, sizeof(xpr_sint_t), 1, sFile);

    HANDLE sFindFile = XPR_NULL;
    WIN32_FIND_DATA sWin32FindData = {0};

    std::deque<xpr_tchar_t *> sSourceDeque;
    std::deque<FILETIME> sTimeDeque;

    xpr_sint_t i;
    xpr_tchar_t *sSplit;
    while (*(sParsing) != '\0')
    {
        sPath.clear();

        if (mMode == MODE_COPY)
        {
            sSplit = (xpr_tchar_t *)_tcsrchr(sParsing, '\\');
            if (XPR_IS_NOT_NULL(sSplit))
            {
                sPath  = sTarget;
                sPath += (nLen <= 3) ? (sSplit+1) : (sSplit);

                if (XPR_IS_NOT_NULL(sHandleToMappings))
                {
                    for (i = 0; i < (xpr_sint_t)sHandleToMappings->mNumberOfMappings; ++i)
                    {
                        if (_tcsicmp(sPath.c_str(), sHandleToMappings->mSHNameMapping[i].pszOldPath) == 0)
                        {
                            sPath = sHandleToMappings->mSHNameMapping[i].pszNewPath;
                            break;
                        }
                    }
                }
            }

            if (sPath.empty() == false)
            {
                sSourceDeque.push_back(new xpr_tchar_t[sPath.length() + 1]);
                _tcscpy(*(sSourceDeque.end()-1), sPath.c_str());
            }
        }
        else if (mMode == MODE_MOVE)
        {
            sSourceDeque.push_back(new xpr_tchar_t[_tcslen(sParsing) + 1]);
            _tcscpy(*(sSourceDeque.end()-1), sParsing);
        }
        else if (mMode == MODE_TRASH)
        {
            sFindFile = ::FindFirstFile(sParsing, &sWin32FindData);
            if (sFindFile != INVALID_HANDLE_VALUE)
            {
                ::FindClose(sFindFile);

                // Creation Time
                sTimeDeque.push_back(sWin32FindData.ftCreationTime);

                // Long Path
                sSourceDeque.push_back(new xpr_tchar_t[_tcslen(sParsing) + 1]);
                _tcscpy(*(sSourceDeque.end()-1), sParsing);
            }
        }

        sParsing += _tcslen(sParsing) + 1;
    }

    xpr_sint_t sSourceLen = 0;
    xpr_sint_t sSourceCount = (xpr_sint_t)sSourceDeque.size();
    for (i = 0; i < sSourceCount; ++i)
    {
        fwrite(sSourceDeque[i], (_tcslen(sSourceDeque[i]) + 1)*sizeof(xpr_tchar_t), 1, sFile);
        sSourceLen += (xpr_sint_t)_tcslen(sSourceDeque[i]) + 1;

        delete[] sSourceDeque[i];
    }

    if (mMode == MODE_TRASH)
    {
        for (i = 0; i < sSourceCount; ++i)
            fwrite(&sTimeDeque[i], sizeof(FILETIME), 1, sFile);
    }

    sSourceDeque.clear();
    sTimeDeque.clear();

    fseek(sFile, sizeof(xpr_sint_t), SEEK_SET);

    // Source Path Count, Total Length
    fwrite(&sSourceCount, sizeof(xpr_sint_t), 1, sFile);
    fwrite(&sSourceLen,   sizeof(xpr_sint_t), 1, sFile);

    if (XPR_IS_NOT_NULL(sTarget))
    {
        fseek(sFile, 0, SEEK_END);

        // Temp Target Path Total Count, Length
        xpr_slong_t sPos = ftell(sFile);
        fwrite(&mMode, sizeof(xpr_sint_t), 1, sFile);
        fwrite(&mMode, sizeof(xpr_sint_t), 1, sFile);

        // Targer Path
        xpr_sint_t sTargetLen = 0;
        xpr_sint_t sTargetCount = 0;
        sParsing = sTarget;
        while (*(sParsing) != '\0')
        {
            fwrite(sTarget, (_tcslen(sParsing)+1) * sizeof(xpr_tchar_t), 1, sFile);
            sTargetLen += (xpr_sint_t)_tcslen(sParsing) + 1;

            sParsing += _tcslen(sParsing) + 1;
            sTargetCount++;
        }

        fseek(sFile, sPos, SEEK_SET);

        // Target Path Count
        fwrite(&sTargetCount, sizeof(xpr_sint_t), 1, sFile);

        // Target Path Total Length
        fwrite(&sTargetLen,   sizeof(xpr_sint_t), 1, sFile);
    }

    endAddFile(sFile);
}

void FileOpUndo::addRename(const std::tstring &aSource, const std::tstring &aTarget)
{
    addRename(aSource.c_str(), aTarget.c_str());
}

void FileOpUndo::addRename(const xpr_tchar_t *aSource, const xpr_tchar_t *aTarget)
{
    FILE *sFile = beginAddFile();
    if (XPR_IS_NULL(sFile))
        return;

    mMode = MODE_RENAME;

    // Undo Mode
    fwrite(&mMode, sizeof(xpr_sint_t), 1, sFile);

    // Source Path Count
    xpr_sint_t sSourceCount = 1;
    fwrite(&sSourceCount, sizeof(xpr_sint_t), 1, sFile);

    // Source Path Length
    xpr_sint_t sSourceLen = (xpr_sint_t)_tcslen(aSource) + 1;
    fwrite(&sSourceLen, sizeof(xpr_sint_t), 1, sFile);

    // Source Path
    fwrite(aSource, sSourceLen * sizeof(xpr_tchar_t), 1, sFile);

    // Target Path Count
    xpr_sint_t sTargetCount = 1;
    fwrite(&sTargetCount, sizeof(xpr_sint_t), 1, sFile);

    // Target Path Length
    xpr_sint_t sTargetLen = (xpr_sint_t)_tcslen(aTarget) + 1;
    fwrite(&sTargetLen, sizeof(xpr_sint_t), 1, sFile);

    // Target Path
    fwrite(aTarget, sTargetLen * sizeof(xpr_tchar_t), 1, sFile);

    endAddFile(sFile);
}

xpr_bool_t FileOpUndo::isUndo(void)
{
    return XPR_IS_NOT_NULL(mPath) && (mIndex > -1);
}

xpr_sint_t FileOpUndo::getMode(void)
{
    return mMode;
}

xpr_sint_t FileOpUndo::getCount(void)
{
    return mIndex + 1;
}

void FileOpUndo::start(void)
{
    FILE *sFile = beginUndoFile();
    if (XPR_IS_NULL(sFile))
        return;

    fread(&mMode, sizeof(xpr_sint_t), 1, sFile);

    SHFILEOPSTRUCT *sShFileOpStruct = XPR_NULL;
    switch (mMode)
    {
    case MODE_COPY:   sShFileOpStruct = startCopy(sFile);       break;
    case MODE_MOVE:   sShFileOpStruct = startMove(sFile);       break;
    case MODE_RENAME: sShFileOpStruct = startRename(sFile);     break;
    case MODE_TRASH:  sShFileOpStruct = startRecycleBin(sFile); break;
    }

    if (XPR_IS_NOT_NULL(sShFileOpStruct))
    {
        FileOpThread *sFileOpThread = new FileOpThread;
        sFileOpThread->setUndo(XPR_FALSE);

        sFileOpThread->start(sShFileOpStruct);
    }

    endUndoFile(sFile);

    // Update Mode
    sFile = beginUndoFile(XPR_FALSE);

    if (XPR_IS_NOT_NULL(sFile))
        fread(&mMode, sizeof(xpr_sint_t), 1, sFile);
    else
        mMode = -1;

    endUndoFile(sFile, XPR_FALSE);
}

SHFILEOPSTRUCT *FileOpUndo::startCopy(FILE *aFile)
{
    xpr_sint_t sSourceCount = 0;
    fread(&sSourceCount, sizeof(xpr_sint_t), 1, aFile);

    xpr_sint_t sSourceLen = 0;
    fread(&sSourceLen, sizeof(xpr_sint_t), 1, aFile);

    if (sSourceCount <= 0 || sSourceLen <= 0)
        return XPR_NULL;

    xpr_tchar_t *sSource2 = new xpr_tchar_t[sSourceLen + XPR_MAX_PATH + 1];
    sSource2[0] = '\0';

    fread(sSource2, sSourceLen * sizeof(xpr_tchar_t), 1, aFile);
    sSource2[sSourceLen  ] = '\0';
    sSource2[sSourceLen+1] = '\0';

    SHFILEOPSTRUCT *sShFileOpStruct = XPR_NULL;
    if (sSource2[0] != '\0')
    {
        sShFileOpStruct = new SHFILEOPSTRUCT;
        memset(sShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
        sShFileOpStruct->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
        sShFileOpStruct->wFunc  = FO_DELETE;
        sShFileOpStruct->fFlags = 0;
        sShFileOpStruct->pFrom  = sSource2;
        sShFileOpStruct->pTo    = XPR_NULL;
    }
    else
    {
        XPR_SAFE_DELETE_ARRAY(sSource2);
    }

    return sShFileOpStruct;
}

SHFILEOPSTRUCT *FileOpUndo::startMove(FILE *aFile)
{
    xpr_sint_t sSourceCount = 0;
    fread(&sSourceCount, sizeof(xpr_sint_t), 1, aFile);

    xpr_sint_t sSourceLen = 0;
    fread(&sSourceLen, sizeof(xpr_sint_t), 1, aFile);

    if (sSourceCount <= 0 || sSourceLen <= 0)
        return XPR_NULL;

    xpr_tchar_t *sSourceTemp = new xpr_tchar_t[sSourceLen + XPR_MAX_PATH + 1];
    fread(sSourceTemp, sSourceLen * sizeof(xpr_tchar_t), 1, aFile);
    sSourceTemp[sSourceLen  ] = '\0';
    sSourceTemp[sSourceLen+1] = '\0';

    xpr_sint_t sTargetCount = 0;
    fread(&sTargetCount, sizeof(xpr_sint_t), 1, aFile);
    if (sSourceCount != sTargetCount)
        sTargetCount = 1;

    xpr_sint_t sTargetLen = 0;
    fread(&sTargetLen, sizeof(xpr_sint_t), 1, aFile);

    xpr_tchar_t *sTargetTemp = new xpr_tchar_t[sTargetLen + XPR_MAX_PATH + 1];
    fread(sTargetTemp, sTargetLen * sizeof(xpr_tchar_t), 1, aFile);

    xpr_tchar_t *sSource2 = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sSourceCount];
    xpr_tchar_t *sTarget2 = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sSourceCount];
    sSource2[0] = '\0';
    sTarget2[0] = '\0';

    xpr_tchar_t *sSplit = XPR_NULL;
    xpr_tchar_t *sSourceEnum = sSource2;
    xpr_tchar_t *sTargetEnum = sTarget2;
    xpr_tchar_t *sSourceTempEnum = sSourceTemp;
    xpr_tchar_t *sTargetTempEnum = sTargetTemp;

    if (sTargetCount == 1 && XPR_IS_NOT_NULL(sTargetTemp))
    {
        if (sTargetTemp[_tcslen(sTargetTemp) - 1] == '\\')
            sTargetTemp[_tcslen(sTargetTemp) - 1] = '\0';
    }

    xpr_sint_t i;
    xpr_sint_t sSourceTempLen = 0;
    xpr_tchar_t *sSourceDir = XPR_NULL;
    xpr_tchar_t *sTargetDir = XPR_NULL;
    xpr_tchar_t *sFileName  = XPR_NULL;
    for (i = 0; i < sSourceCount; ++i)
    {
        sSourceTempLen = (xpr_sint_t)_tcslen(sSourceTempEnum);
        sSplit = _tcsrchr(sSourceTempEnum, '\\');
        if (XPR_IS_NOT_NULL(sSplit))
        {
            *sSplit = '\0';
            sSplit++;
        }
        else
        {
            continue;
        }

        sSourceDir = sTargetTemp;
        sTargetDir = sSourceTemp;
        sFileName  = sSplit;

        // consist of source
        if (sTargetCount == 1)
            _tcscpy(sSourceEnum, sSourceDir);
        else
        {
            _tcscpy(sSourceEnum, sTargetTempEnum);
            if (sSourceEnum[_tcslen(sSourceEnum) - 1] == '\\')
                sSourceEnum[_tcslen(sSourceEnum) - 1] = '\0';
        }

        _tcscat(sSourceEnum, XPR_STRING_LITERAL("\\"));
        _tcscat(sSourceEnum, sFileName);

        // consist of target
        _tcscpy(sTargetEnum, sTargetDir);
        _tcscat(sTargetEnum, XPR_STRING_LITERAL("\\"));
        _tcscat(sTargetEnum, sFileName);

        sSourceEnum += _tcslen(sSourceEnum) + 1;
        sTargetEnum += _tcslen(sTargetEnum) + 1;

        sSourceTempEnum += sSourceTempLen + 1;
        if (sTargetCount > 1)
            sTargetTempEnum += _tcslen(sTargetTempEnum) + 1;
    }

    // last double null
    sSourceEnum[0] = '\0';
    sTargetEnum[0] = '\0';

    // free temp buffer
    XPR_SAFE_DELETE_ARRAY(sSourceTemp);
    XPR_SAFE_DELETE_ARRAY(sTargetTemp);

    SHFILEOPSTRUCT *sShFileOpStruct = XPR_NULL;
    if (sSource2[0] != '\0' && sTarget2[0] != '\0')
    {
        sShFileOpStruct = new SHFILEOPSTRUCT;
        memset(sShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
        sShFileOpStruct->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
        sShFileOpStruct->wFunc  = FO_MOVE;
        sShFileOpStruct->fFlags = FOF_MULTIDESTFILES;
        sShFileOpStruct->pFrom  = sSource2;
        sShFileOpStruct->pTo    = sTarget2;
    }
    else
    {
        XPR_SAFE_DELETE_ARRAY(sSource2);
        XPR_SAFE_DELETE_ARRAY(sTarget2);
    }

    return sShFileOpStruct;
}

SHFILEOPSTRUCT *FileOpUndo::startRename(FILE *aFile)
{
    xpr_sint_t sSourceCount = 0;
    fread(&sSourceCount, sizeof(xpr_sint_t), 1, aFile);

    xpr_sint_t sSourceLen = 0;
    fread(&sSourceLen, sizeof(xpr_sint_t), 1, aFile);

    if (sSourceCount <= 0 || sSourceLen <= 0)
        return XPR_NULL;

    xpr_tchar_t *sSource2 = new xpr_tchar_t[sSourceLen + XPR_MAX_PATH + 1];
    sSource2[0] = '\0';

    fread(sSource2, sSourceLen * sizeof(xpr_tchar_t), 1, aFile);

    xpr_sint_t sTargetCount = 0;
    fread(&sTargetCount, sizeof(xpr_sint_t), 1, aFile);

    xpr_sint_t sTargetLen = 0;
    fread(&sTargetLen, sizeof(xpr_sint_t), 1, aFile);

    xpr_tchar_t *sTarget2 = new xpr_tchar_t[sTargetLen + XPR_MAX_PATH + 1];
    sTarget2[0] = '\0';

    fread(sTarget2, sTargetLen * sizeof(xpr_tchar_t), 1, aFile);

    sSource2[_tcslen(sSource2) + 1] = '\0';
    sTarget2[_tcslen(sTarget2) + 1] = '\0';

    SHFILEOPSTRUCT *sShFileOpStruct = XPR_NULL;
    if (sSource2[0] != '\0' && sTarget2[0] != '\0')
    {
        sShFileOpStruct = new SHFILEOPSTRUCT;
        memset(sShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
        sShFileOpStruct->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
        sShFileOpStruct->wFunc  = FO_RENAME;
        sShFileOpStruct->fFlags = 0;
        sShFileOpStruct->pFrom  = sTarget2;
        sShFileOpStruct->pTo    = sSource2;
    }
    else
    {
        XPR_SAFE_DELETE_ARRAY(sSource2);
        XPR_SAFE_DELETE_ARRAY(sTarget2);
    }

    return sShFileOpStruct;
}

SHFILEOPSTRUCT *FileOpUndo::startRecycleBin(FILE *aFile)
{
    xpr_sint_t sSourceCount = 0;
    fread(&sSourceCount, sizeof(xpr_sint_t), 1, aFile);

    xpr_sint_t sSourceLen = 0;
    fread(&sSourceLen, sizeof(xpr_sint_t), 1, aFile);

    if (sSourceCount <= 0 || sSourceLen <= 0)
        return XPR_NULL;

    xpr_tchar_t *sSource = new xpr_tchar_t[sSourceLen + XPR_MAX_PATH + 1];
    fread(sSource, sSourceLen * sizeof(xpr_tchar_t), 1, aFile);
    sSource[sSourceLen  ] = '\0';
    sSource[sSourceLen+1] = '\0';

    FILETIME *sFileTime = new FILETIME[sSourceCount];
    fread(sFileTime, sizeof(FILETIME)*sSourceCount, 1, aFile);

    xpr_sint_t i = 0;
    xpr_tchar_t **sSourceElem = new xpr_tchar_t *[sSourceCount];
    xpr_tchar_t *sSourceEnum = sSource;
    while (*sSourceEnum != '\0')
    {
        sSourceElem[i++] = sSourceEnum;
        sSourceEnum += _tcslen(sSourceEnum) + 1;
    }

    xpr_tchar_t *sSource2 = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sSourceCount];
    xpr_tchar_t *sTarget2 = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sSourceCount];
    sSource2[0] = '\0';
    sTarget2[0] = '\0';

    xpr_tchar_t sDrive;
    xpr_bool_t sSucceeded;
    BBInfoFile sBBInfoFile;

    xpr_tchar_t *sSource2Enum = sSource2;
    xpr_tchar_t *sTarget2Enum = sTarget2;
    for (i = 0; i < sSourceCount; ++i)
    {
        sDrive = sSourceElem[i][0];
        if (sBBInfoFile.open(sDrive) == XPR_FALSE)
            continue;

        sSucceeded = sBBInfoFile.findOriginalPath(sSourceElem[i], sFileTime[i], sTarget2Enum);
        if (XPR_IS_TRUE(sSucceeded))
        {
            _tcscpy(sSource2Enum, sSourceElem[i]);

            sSource2Enum += _tcslen(sSource2Enum) + 1;
            sTarget2Enum += _tcslen(sTarget2Enum) + 1;
        }

        sBBInfoFile.close();
    }

    sSource2Enum[0] = '\0';
    sTarget2Enum[0] = '\0';

    XPR_SAFE_DELETE_ARRAY(sSource);
    XPR_SAFE_DELETE_ARRAY(sSourceElem);
    XPR_SAFE_DELETE_ARRAY(sFileTime);

    SHFILEOPSTRUCT *sShFileOpStruct = XPR_NULL;
    if (sSource2[0] != '\0' && sTarget2[0] != '\0')
    {
        sShFileOpStruct = new SHFILEOPSTRUCT;
        memset(sShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
        sShFileOpStruct->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
        sShFileOpStruct->wFunc  = FO_MOVE;
        sShFileOpStruct->fFlags = FOF_MULTIDESTFILES;
        sShFileOpStruct->pFrom  = sTarget2;
        sShFileOpStruct->pTo    = sSource2;
    }
    else
    {
        XPR_SAFE_DELETE_ARRAY(sSource2);
        XPR_SAFE_DELETE_ARRAY(sTarget2);
    }

    return sShFileOpStruct;
}
} // namespace fxb
