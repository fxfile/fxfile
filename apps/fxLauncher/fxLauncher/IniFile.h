//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_INI_FILE_H__
#define __FX_INI_FILE_H__
#pragma once

class IniFile  
{
public:
	enum { InvalidIndex = 0xffffffff };

public:
	IniFile(LPCTSTR lpszIniPath = NULL);
	virtual ~IniFile();

public:
	void SetCaseSensitive();
	void SetCaseInsensitive();
	
	LPCTSTR GetPath();
	void SetPath(LPCTSTR lpszIniPath);

	// ansi
	BOOL ReadFileA();
	BOOL WriteFileA(); 

	// unicode
	BOOL ReadFileW();
	BOOL WriteFileW(); 

	// ansi/unicode
	BOOL ReadFile();
	BOOL WriteFile();

	void Erase(BOOL bWithSortKey = FALSE);
	void Clear(BOOL bWithSortKey = FALSE);
	void Reset(BOOL bWithSortKey = FALSE);

	void SetComment(LPCTSTR lpcszComment);

	LPCTSTR  GetValue(size_t nKey, size_t nEntry, LPCTSTR lpcszDefault = NULL);
	LPCTSTR  GetValueS(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszDefault = NULL); 
	int      GetValueI(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, int nDefault = 0);
	UINT     GetValueU(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, UINT uDefault = 0);
	COLORREF GetValueC(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, COLORREF clrDefault);
	BOOL     GetValueB(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, BOOL bDefault = FALSE);
	double   GetValueF(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, double dbDefault = 0.0);
	CRect    GetValueR(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CRect rcDefault = CRect(0,0,0,0));
	CSize    GetValueZ(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CSize szDefault = CSize(0,0));
	CPoint   GetValueP(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CPoint ptDefault = CPoint(0,0));
	int      GetValueV(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszFormat,
				void *v1 = 0, void *v2 = 0, void *v3 = 0, void *v4 = 0,
				void *v5 = 0, void *v6 = 0, void *v7 = 0, void *v8 = 0,
				void *v9 = 0, void *v10 = 0, void *v11 = 0, void *v12 = 0,
				void *v13 = 0, void *v14 = 0, void *v15 = 0, void *v16 = 0);

	__int64          GetValueI64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, __int64 nDefault = 0i64);
	unsigned __int64 GetValueU64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, unsigned __int64 uDefault = 0ui64);

	int      GetKeyCompatibleValueI(LPCTSTR lpcszKey1, LPCTSTR lpcszKey2, LPCTSTR lpcszEntry, int nDefault = 0);
	BOOL     GetKeyCompatibleValueB(LPCTSTR lpcszKey1, LPCTSTR lpcszKey2, LPCTSTR lpcszEntry, BOOL bDefault = FALSE);
	int      GetEntryCompatibleValueI(LPCTSTR lpcszKey, LPCTSTR lpcszEntry1, LPCTSTR lpcszEntry2, int nDefault = 0);
	BOOL     GetEntryCompatibleValueB(LPCTSTR lpcszKey, LPCTSTR lpcszEntry1, LPCTSTR lpcszEntry2, BOOL bDefault = FALSE);

	BOOL SetValueS(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszValue, BOOL bCreate = TRUE);
	BOOL SetValueI(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, int nValue, BOOL bCreate = TRUE);
	BOOL SetValueU(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, UINT uValue, BOOL bCreate = TRUE);
	BOOL SetValueC(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, COLORREF clrValue, BOOL bCreate = TRUE);
	BOOL SetValueB(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, BOOL bValue, BOOL bCreate = TRUE);
	BOOL SetValueF(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, double dbValue, BOOL bCreate = TRUE);
	BOOL SetValueR(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CRect rcValue, BOOL bCreate = TRUE);
	BOOL SetValueZ(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CSize szValue, BOOL bCreate = TRUE);
	BOOL SetValueP(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CPoint ptValue, BOOL bCreate = TRUE);
	BOOL SetValueV(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszFormat, ...);
	BOOL SetValueI64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, __int64 nValue, BOOL bCreate = TRUE);
	BOOL SetValueU64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, unsigned __int64 uValue, BOOL bCreate = TRUE);

	size_t FindKey(LPCTSTR lpcszKey);
	size_t FindEntry(size_t nKey, LPCTSTR lpcszEntry);
	size_t FindEntry(LPCTSTR lpcszKey, LPCTSTR lpcszEntry);

	size_t GetKeyCount();
	size_t GetEntryCount(size_t nKey);
	size_t GetEntryCount(LPCTSTR lpcszKey);

	LPCTSTR GetEntryName(size_t nKey, size_t nEntry);
	
	size_t AddKeyName(LPCTSTR lpcszKey);
	LPCTSTR GetKeyName(size_t nKey);
	
	BOOL SetSortKey(LPCTSTR lpcszKey, size_t nSort);
	void SortKey();
	
	LPCTSTR GetValueName(size_t nKey, size_t nValue);
	LPCTSTR GetValueName(LPCTSTR lpcszKey, size_t nValue);

	BOOL SetValue(size_t nKey, size_t nEntry, LPCTSTR lpcszValue);
	BOOL DeleteEntry(LPCTSTR lpcszKey, LPCTSTR lpcszEntry);
	BOOL DeleteKey(LPCTSTR lpcszKey);
	void DeleteAllKey();

	size_t GetCommentCount();
	LPCTSTR GetComment(size_t nComment);
	BOOL DeleteComment(size_t nCommnet);
	void DeleteAllComments();;

private:
	BOOL  m_bCaseInsensitive;
	TCHAR m_szPath[MAX_PATH];

	struct Entry
	{
		LPTSTR lpszEntry;
		LPTSTR lpszValue;
	};

	struct Key
	{
		LPTSTR             lpszName;
		std::vector<Entry> dqEntries;
		size_t             nSort;
	};
	std::vector<Key>               m_dqKeys;
	std::vector<LPTSTR>            m_dqComments;
    std::map<std::tstring, size_t> m_mpSortKey;

	LPTSTR CheckCase(LPTSTR lpszString);
	int Compare(LPCTSTR lpcszString1, LPCTSTR lpcszString2);

	static bool sort_key_name(Key &elem1, Key &elem2);
};

#endif // __FX_INI_FILE_H__
