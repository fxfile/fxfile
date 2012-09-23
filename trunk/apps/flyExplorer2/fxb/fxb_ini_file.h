//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_INI_FILE_H__
#define __FXB_INI_FILE_H__
#pragma once

namespace fxb
{
class IniFile
{
public:
    enum { InvalidIndex = 0xffffffff };

public:
    IniFile(const xpr_tchar_t *aIniPath = XPR_NULL);
    virtual ~IniFile(void);

public:
    void setCaseSensitive(void);
    void setCaseInsensitive(void);

    const xpr_tchar_t *getPath(void);
    void setPath(const xpr_tchar_t *aIniPath);

    // for ansi
    xpr_bool_t readFileA(void);
    xpr_bool_t writeFileA(void); 

    // for unicode
    xpr_bool_t readFileW(void);
    xpr_bool_t writeFileW(void); 

    // for ansi/unicode
    xpr_bool_t readFile(void);
    xpr_bool_t writeFile(void);

    void erase(xpr_bool_t aWithSortKey = XPR_FALSE);
    void clear(xpr_bool_t aWithSortKey = XPR_FALSE);
    void reset(xpr_bool_t aWithSortKey = XPR_FALSE);

    void setComment(const xpr_tchar_t *aComment);

    const xpr_tchar_t * getValue(xpr_size_t aKey, xpr_size_t aEntry, const xpr_tchar_t *aDefault = XPR_NULL);
    const xpr_tchar_t * getValueS(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, const xpr_tchar_t *aDefault = XPR_NULL); 
    xpr_sint_t          getValueI(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_sint_t aDefault = 0);
    xpr_uint_t          getValueU(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_uint_t aDefault = 0);
    COLORREF            getValueC(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, COLORREF aDefault);
    xpr_bool_t          getValueB(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_bool_t aDefault = XPR_FALSE);
    xpr_double_t        getValueF(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_double_t aDefault = 0.0);
    CRect               getValueR(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, CRect aDefault = CRect(0,0,0,0));
    CSize               getValueZ(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, CSize aDefault = CSize(0,0));
    CPoint              getValueP(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, CPoint aDefault = CPoint(0,0));
    xpr_sint_t          getValueV(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, const xpr_tchar_t *aFormat,
                                  void *v1 = 0, void *v2 = 0, void *v3 = 0, void *v4 = 0,
                                  void *v5 = 0, void *v6 = 0, void *v7 = 0, void *v8 = 0,
                                  void *v9 = 0, void *v10 = 0, void *v11 = 0, void *v12 = 0,
                                  void *v13 = 0, void *v14 = 0, void *v15 = 0, void *v16 = 0);

    xpr_sint64_t getValueI64(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_sint64_t aDefault = 0i64);
    xpr_uint64_t getValueU64(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_uint64_t aDefault = 0ui64);

    xpr_sint_t getKeyCompatibleValueI(const xpr_tchar_t *aKey1, const xpr_tchar_t *aKey2, const xpr_tchar_t *aEntry, xpr_sint_t aDefault = 0);
    xpr_bool_t getKeyCompatibleValueB(const xpr_tchar_t *aKey1, const xpr_tchar_t *aKey2, const xpr_tchar_t *aEntry, xpr_bool_t aDefault = XPR_FALSE);
    xpr_sint_t getEntryCompatibleValueI(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry1, const xpr_tchar_t *aEntry2, xpr_sint_t aDefault = 0);
    xpr_bool_t getEntryCompatibleValueB(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry1, const xpr_tchar_t *aEntry2, xpr_bool_t aDefault = XPR_FALSE);

    xpr_bool_t setValueS(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, const xpr_tchar_t *aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueI(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_sint_t aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueU(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_uint_t aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueC(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, COLORREF aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueB(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_bool_t aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueF(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_double_t aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueR(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, CRect aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueZ(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, CSize aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueP(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, CPoint aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueV(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, const xpr_tchar_t *aFormat, ...);
    xpr_bool_t setValueI64(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_sint64_t aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);
    xpr_bool_t setValueU64(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry, xpr_uint64_t aValue, xpr_bool_t aCreateIfNotExist = XPR_TRUE);

    xpr_size_t findKey(const xpr_tchar_t *aKey);
    xpr_size_t findEntry(xpr_size_t aKey, const xpr_tchar_t *aEntry);
    xpr_size_t findEntry(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry);

    xpr_size_t getKeyCount(void);
    xpr_size_t getEntryCount(xpr_size_t aKey);
    xpr_size_t getEntryCount(const xpr_tchar_t *aKey);

    const xpr_tchar_t *getEntryName(xpr_size_t aKey, xpr_size_t aEntry);

    xpr_size_t addKeyName(const xpr_tchar_t *aKey);
    const xpr_tchar_t *getKeyName(xpr_size_t nKey);

    xpr_bool_t setSortKey(const xpr_tchar_t *aKey, xpr_size_t aSort);
    void sortKey(void);

    const xpr_tchar_t *getValueName(xpr_size_t aKey, xpr_size_t aValue);
    const xpr_tchar_t *getValueName(const xpr_tchar_t *aKey, xpr_size_t aValue);

    xpr_bool_t setValue(xpr_size_t aKey, xpr_size_t aEntry, const xpr_tchar_t *aValue);
    xpr_bool_t deleteEntry(const xpr_tchar_t *aKey, const xpr_tchar_t *aEntry);
    xpr_bool_t deleteKey(const xpr_tchar_t *aKey);
    void deleteAllKey(void);

    xpr_size_t getCommentCount(void);
    const xpr_tchar_t *getComment(xpr_size_t aComment);
    xpr_bool_t deleteComment(xpr_size_t aCommnet);
    void deleteAllComments(void);

private:
    xpr_bool_t  mCaseInsensitive;
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

    struct Entry
    {
        xpr_tchar_t *mEntry;
        xpr_tchar_t *mValue;
    };

    struct Key
    {
        xpr_tchar_t        *mName;
        std::vector<Entry>  mEntryVector;
        xpr_size_t          mSort;
    };
    std::vector<Key>                   mKeys;
    std::vector<xpr_tchar_t *>         mComments;
    std::map<std::tstring, xpr_size_t> mSortKey;

protected:
    xpr_tchar_t *checkCase(xpr_tchar_t *aString);
    xpr_sint_t compare(const xpr_tchar_t *aString1, const xpr_tchar_t *aString2);

    static bool sortKeyName(Key &aKey1, Key &aKey2);
};
} // namespace fxb

#endif // __FXB_INI_FILE_H__
