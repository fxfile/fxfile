//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_OP_UNDO_H__
#define __FXFILE_FILE_OP_UNDO_H__ 1
#pragma once

namespace fxfile
{
enum
{
    MODE_COPY = 0,
    MODE_MOVE,
    MODE_RENAME,
    MODE_TRASH,
};

class FileOpUndo
{
public:
    FileOpUndo(void);
    virtual ~FileOpUndo(void);

public:
    void addOperation(SHFILEOPSTRUCT *aShFileOpStruct);
    void addRename(const xpr_tchar_t *aSource, const xpr_tchar_t *aTarget);
    void addRename(const xpr::tstring &aSource, const xpr::tstring &aTarget);

    static void deleteUndoDir(void);

public:
    void start(void);

    static xpr_sint_t getMode(void);
    static xpr_bool_t isUndo(void);
    static xpr_sint_t getCount(void);

protected:
    SHFILEOPSTRUCT *startCopy(xpr::FileIo &aFileIo);
    SHFILEOPSTRUCT *startMove(xpr::FileIo &aFileIo);
    SHFILEOPSTRUCT *startRename(xpr::FileIo &aFileIo);
    SHFILEOPSTRUCT *startRecycleBin(xpr::FileIo &aFileIo);

    void validateUndoDir(void);

    xpr_bool_t beginAddFile(xpr::FileIo &aFileIo);
    void endAddFile(xpr::FileIo &aFileIo);

    xpr_bool_t beginUndoFile(xpr::FileIo &aFileIo, xpr_bool_t aUpdate = XPR_TRUE);
    void endUndoFile(xpr::FileIo &aFileIo, xpr_bool_t aUpdate = XPR_TRUE);

protected:
    static xpr_tchar_t *mPath;
    static xpr_sint_t   mIndex;
    static xpr_sint_t   mMode;
};
} // namespace fxfile

#endif // __FXFILE_FILE_OP_UNDO_H__
