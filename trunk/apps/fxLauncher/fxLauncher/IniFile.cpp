//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "iniFile.h"

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <afxpriv.h>

#include <io.h>
#include <fcntl.h>

#if defined(WIN32)
#define IniEOLA "\r\n"
#define IniEOLW L"\r\n"
#else
#define IniEOLA "\n"
#define IniEOLW L"\n"
#endif

#define MAX_FGETS_LINE		5120
#define MAX_KEY				128
#define MAX_ENTRY			128
#define MAX_VALUE			300
#define MAX_COMMENT			2048

IniFile::IniFile(LPCTSTR lpcszIniPath)
	: m_bCaseInsensitive(TRUE)
{
	m_szPath[0] = _T('\0');

	if (lpcszIniPath)
		_tcscpy(m_szPath, lpcszIniPath);
}

IniFile::~IniFile()
{
	Erase(TRUE);
}

LPCTSTR IniFile::GetPath()
{
	return m_szPath;
}

void IniFile::SetPath(LPCTSTR lpszIniPath)
{
	if (!lpszIniPath)
		return;

	_tcscpy(m_szPath, lpszIniPath);
}

BOOL IniFile::ReadFile()
{
#ifdef _UNICODE
	return ReadFileW();
#else
	return ReadFileA();
#endif
}

BOOL IniFile::WriteFile()
{
#ifdef _UNICODE
	return WriteFileW();
#else
	return WriteFileA();
#endif
}

BOOL IniFile::ReadFileA()
{
	USES_CONVERSION;

	FILE *f = fopen(T2A(m_szPath), "rt");
	if (!f)
		return FALSE;

	char szLine[MAX_FGETS_LINE];
	char szKey[MAX_KEY];
	LPSTR lpszSub, lpszSub2;

	szKey[0] = '\0';

	while (fgets(szLine, MAX_FGETS_LINE-1, f))
	{
		if (szLine[strlen(szLine) - 1] == '\r')
			szLine[strlen(szLine) - 1] = '\0';

		if (szLine[strlen(szLine) - 1] == '\n')
			szLine[strlen(szLine) - 1] = '\0';

		if (strlen(szLine))
		{
			if (!isprint(szLine[0]))
			{
				fclose(f);
				return FALSE;
			}

			switch (szLine[0])
			{
			case '[':
				lpszSub = strchr(szLine, ']');
				if (lpszSub)
				{
					lpszSub[0] = '\0';
					strcpy(szKey, szLine+1);
					AddKeyName(A2T(szKey));
				}
				break;

			case ';':
			case '#':
				SetComment(A2T(szLine+1));
				break;

			default:
				lpszSub = strchr(szLine, '=');
				if (lpszSub)
				{
					lpszSub2 = lpszSub;

					if (lpszSub2[0] == ' ')
					{
						while (lpszSub2[0] == ' ')
							lpszSub2--;
						lpszSub2++;
					}
					lpszSub2[0] = '\0';

					while (lpszSub[0] == ' ')
						lpszSub++;
					lpszSub++;

					SetValueS(A2T(szKey), A2T(szLine), A2T(lpszSub));
				}
				break;
			}
		}
	}

	fclose(f);

	return m_dqKeys.empty() ? FALSE : TRUE;
}

//
// further: \r\n까지 검색하여 길이 구해 line 버퍼수 결정
//
BOOL IniFile::ReadFileW()
{
	USES_CONVERSION;

	FILE *f = _tfopen(m_szPath, _T("rb"));
	if (!f)
		return FALSE;

	LPWSTR lpwszLine;
	WCHAR wszLine[MAX_FGETS_LINE] = {0};

	fread(wszLine, 2, 1, f);
	if (wszLine[0] != 0xFEFF)
	{
		fclose(f);
		return FALSE;
	}

	size_t nLen;
	size_t nRead;
	WCHAR szKey[MAX_KEY];
	LPWSTR lpwszSub, lpwszSub2;

	szKey[0] = L'\0';

	while (1)
	{
		nLen = 0;

		while (1)
		{
			nRead = fread(wszLine+nLen, sizeof(WCHAR), 1, f);
			if (nRead > 0)
				nLen++;

			if (nLen >= 1 && wszLine[nLen-1] == L'\n')
			{
				wszLine[nLen-1] = L'\0';
				nLen--;

				if (nLen >= 1 && wszLine[nLen-1] == L'\r')
				{
					wszLine[nLen-1] = L'\0';
					nLen--;
				}

				break;
			}

			if (nRead <= 0)
				break;
		}

		lpwszLine = (LPWSTR)wszLine;

		if (wcslen(lpwszLine))
		{
			switch (lpwszLine[0])
			{
			case L'[':
				lpwszSub = wcsrchr(lpwszLine, L']'); // ex) [c:\[123]]
				if (lpwszSub)
				{
					lpwszSub[0] = _T('\0');
					wcscpy(szKey, lpwszLine+1);
					AddKeyName(W2T(szKey));
				}
				break;

			case L';':
			case L'#':
				SetComment(W2T(lpwszLine+1));
				break;

			default:
				lpwszSub = wcschr(lpwszLine, L'=');
				if (lpwszSub)
				{
					lpwszSub2 = lpwszSub;

					if (lpwszSub2[0] == L' ')
					{
						while (lpwszSub2[0] == L' ')
							lpwszSub2--;
						lpwszSub2++;
					}
					lpwszSub2[0] = L'\0';

					while (lpwszSub[0] == L' ')
						lpwszSub++;
					lpwszSub++;

					SetValueS(W2T(szKey), W2T(lpwszLine), W2T(lpwszSub));
				}
				break;
			}
		}

		if (nRead <= 0)
			break;
	}

	fclose(f);
	
	return m_dqKeys.empty() ? FALSE : TRUE;
}

BOOL IniFile::WriteFileA()
{
	USES_CONVERSION;

	FILE *f = fopen(T2A(m_szPath), "wt");
	if (!f)
		return FALSE;

	{
		std::vector<LPTSTR>::iterator it = m_dqComments.begin();
		for (; it!=m_dqComments.end(); it++)
			fprintf(f, ";%s%s", T2A(*it), IniEOLA);

		if (!m_dqComments.empty())
			fprintf(f, "%s", IniEOLA);
	}

	{
		std::vector<Key>::iterator it;
		std::vector<Entry>::iterator itEntry;

		it = m_dqKeys.begin();
		for (; it!=m_dqKeys.end(); it++)
		{
			fprintf(f, "[%s]%s", T2A(it->lpszName), IniEOLA);

			itEntry = it->dqEntries.begin();
			for (; itEntry!=it->dqEntries.end(); itEntry++)
				fprintf(f, "%s=%s%s", T2A(itEntry->lpszEntry), T2A(itEntry->lpszValue), IniEOLA);

			fprintf(f, "%s", IniEOLA);
		}
	}

	fclose(f);

	return TRUE;
}

BOOL IniFile::WriteFileW()
{
	USES_CONVERSION;

	FILE *f = _tfopen(m_szPath, _T("wb"));
	if (!f)
		return FALSE;

	UINT nCode = 0xFEFF;
	fwrite(&nCode, 2, 1, f);

	WCHAR szLine[MAX_FGETS_LINE];

	{
		std::vector<LPTSTR>::iterator it = m_dqComments.begin();
		for (; it!=m_dqComments.end(); it++)
		{
			swprintf(szLine, L";%s%s", T2W(*it), IniEOLW);
			fwrite(szLine, wcslen(szLine)*sizeof(WCHAR), 1, f);
		}

		if (!m_dqComments.empty())
		{
			swprintf(szLine, L"%s", IniEOLW);
			fwrite(szLine, wcslen(szLine)*sizeof(WCHAR), 1, f);
		}
	}

	{	
		std::vector<Key>::iterator it;
		std::vector<Entry>::iterator itEntry;

		it = m_dqKeys.begin();
		for (; it!=m_dqKeys.end(); it++)
		{
			swprintf(szLine, L"[%s]%s", T2W(it->lpszName), IniEOLW);
			fwrite(szLine, wcslen(szLine)*sizeof(WCHAR), 1, f);

			itEntry = it->dqEntries.begin();
			for (; itEntry!=it->dqEntries.end(); itEntry++)
			{
				swprintf(szLine, L"%s=%s%s", T2W(itEntry->lpszEntry), T2W(itEntry->lpszValue), IniEOLW);
				fwrite(szLine, wcslen(szLine)*sizeof(WCHAR), 1, f);
			}

			swprintf(szLine, L"%s", IniEOLW);
			fwrite(szLine, wcslen(szLine)*sizeof(WCHAR), 1, f);
		}
	}

	fclose(f);

	return TRUE;
}

size_t IniFile::FindKey(LPCTSTR lpcszKeyName)
{
	size_t i;
	std::vector<Key>::iterator it;

	it = m_dqKeys.begin();
	for (i=0; it!=m_dqKeys.end(); it++, i++)
	{
		if (!Compare(it->lpszName, lpcszKeyName))
			return i;
	}

	return InvalidIndex;
}

size_t IniFile::FindEntry(size_t nKey, LPCTSTR lpcszEntry)
{
	if (!STL_IS_INDEXABLE(nKey, m_dqKeys))
		return InvalidIndex;

	size_t i;
	std::vector<Entry>::iterator it;

	it = m_dqKeys[nKey].dqEntries.begin();
	for (i=0; it!=m_dqKeys[nKey].dqEntries.end(); it++, i++)
	{
		if (!Compare(it->lpszEntry, lpcszEntry))
			return i;
	}

	return InvalidIndex;
}

size_t IniFile::FindEntry(LPCTSTR lpcszKey, LPCTSTR lpcszEntry)
{
	if (!lpcszKey || !lpcszEntry)
		return InvalidIndex;

	size_t nKey = FindKey(lpcszKey);
	if (nKey == InvalidIndex)
		return InvalidIndex;

	size_t i;
	std::vector<Entry>::iterator it;

	it = m_dqKeys[nKey].dqEntries.begin();
	for (i=0; it!=m_dqKeys[nKey].dqEntries.end(); it++, i++)
	{
		if (!Compare(it->lpszEntry, lpcszEntry))
			return i;
	}

	return InvalidIndex;
}

size_t IniFile::AddKeyName(LPCTSTR lpcszKey)
{
	size_t nKey = FindKey(lpcszKey);
	if (nKey == InvalidIndex)
	{
		LPTSTR lpszKey2 = new TCHAR[_tcslen(lpcszKey)+1];
		_tcscpy(lpszKey2, lpcszKey);

		Key key;
		key.lpszName = lpszKey2;
		key.nSort    = InvalidIndex;

        std::map<std::tstring, size_t>::iterator it = m_mpSortKey.find(lpcszKey);
		if (it != m_mpSortKey.end())
			key.nSort = it->second;

		m_dqKeys.resize(m_dqKeys.size() + 1, key);
		
		nKey = m_dqKeys.size() - 1;
	}

	return nKey;
}

LPCTSTR IniFile::GetKeyName(size_t nKeyIndex)
{
	if (!STL_IS_INDEXABLE(nKeyIndex, m_dqKeys))
		return NULL;

	return m_dqKeys[nKeyIndex].lpszName;
}

bool IniFile::sort_key_name(IniFile::Key &elem1, IniFile::Key &elem2)
{
	if (elem2.nSort == InvalidIndex) return false;
	if (elem1.nSort == InvalidIndex) return true;

	return elem1.nSort < elem2.nSort;
}

void IniFile::SortKey()
{
	std::sort(m_dqKeys.begin(), m_dqKeys.end(), sort_key_name);
}

BOOL IniFile::SetSortKey(LPCTSTR lpcszKey, size_t nSort)
{
	if (!lpcszKey)
		return FALSE;

	m_mpSortKey[lpcszKey] = nSort;

	size_t nKey = FindKey(lpcszKey);
	if (nKey == InvalidIndex)
		return FALSE;

	m_dqKeys[nKey].nSort = nSort;

	return TRUE;
}

size_t IniFile::GetKeyCount()
{
	return m_dqKeys.size();
}

size_t IniFile::GetEntryCount(size_t nKeyIndex)
{
	if (!STL_IS_INDEXABLE(nKeyIndex, m_dqKeys))
		return 0;

	return m_dqKeys[nKeyIndex].dqEntries.size();
}

size_t IniFile::GetEntryCount(LPCTSTR lpcszKeyName)
{
	size_t nKeyIndex = FindKey(lpcszKeyName);
	if (nKeyIndex == InvalidIndex)
		return 0;

	return m_dqKeys[nKeyIndex].dqEntries.size();
}

LPCTSTR IniFile::GetEntryName(size_t nKey, size_t nEntry)
{
	if (!STL_IS_INDEXABLE(nKey, m_dqKeys))
		return NULL;

	if (!STL_IS_INDEXABLE(nEntry, m_dqKeys[nKey].dqEntries))
		return NULL;

	return m_dqKeys[nKey].dqEntries[nEntry].lpszEntry;
}

LPCTSTR IniFile::GetValueName(size_t nKeyIndex, size_t nValueIndex)
{
	if (!STL_IS_INDEXABLE(nKeyIndex, m_dqKeys))
		return NULL;

	if (!STL_IS_INDEXABLE(nValueIndex, m_dqKeys[nKeyIndex].dqEntries))
		return NULL;

	return m_dqKeys[nKeyIndex].dqEntries[nValueIndex].lpszEntry;
}

LPCTSTR IniFile::GetValueName(LPCTSTR lpcszKeyName, size_t nValueIndex)
{
	size_t nKeyIndex = FindKey(lpcszKeyName);
	if (nKeyIndex == InvalidIndex)
		return NULL;

	return GetValueName(nKeyIndex, nValueIndex);
}

BOOL IniFile::SetValue(size_t nKey, size_t nEntry, LPCTSTR lpcszValue)
{
	if (!STL_IS_INDEXABLE(nKey, m_dqKeys))
		return FALSE;

	if (!STL_IS_INDEXABLE(nEntry, m_dqKeys[nKey].dqEntries))
		return FALSE;

	LPTSTR lpszValue2 = new TCHAR[_tcslen(lpcszValue)+1];
	_tcscpy(lpszValue2, lpcszValue);

	delete[] m_dqKeys[nKey].dqEntries[nEntry].lpszValue;
	m_dqKeys[nKey].dqEntries[nEntry].lpszValue = lpszValue2;

	return FALSE;
}

BOOL IniFile::SetValueS(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszValue, BOOL bCreate)
{
	size_t nKey = FindKey(lpcszKey);
	if (nKey == InvalidIndex)
	{
		if (bCreate)
			nKey = AddKeyName(lpcszKey);
		else
			return FALSE;
	}
	
	size_t nEntry = FindEntry(nKey, lpcszEntry);
	if (nEntry == InvalidIndex)
	{
		if (!bCreate)
			return FALSE;

		Entry entry;
		entry.lpszEntry = new TCHAR[_tcslen(lpcszEntry)+1];
		entry.lpszValue = new TCHAR[_tcslen(lpcszValue)+1];
		_tcscpy(entry.lpszEntry, lpcszEntry);
		_tcscpy(entry.lpszValue, lpcszValue);

		m_dqKeys[nKey].dqEntries.resize(m_dqKeys[nKey].dqEntries.size() + 1, entry);
	}
	else
	{
		LPTSTR lpszValue2 = new TCHAR[_tcslen(lpcszValue)+1];
		_tcscpy(lpszValue2, lpcszValue);

		delete[] m_dqKeys[nKey].dqEntries[nEntry].lpszValue;
		m_dqKeys[nKey].dqEntries[nEntry].lpszValue = lpszValue2;
	}
	
	return TRUE;
}

BOOL IniFile::SetValueI(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, int nValue, BOOL bCreate)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%d"), nValue);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueU(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, UINT uValue, BOOL bCreate)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%u"), uValue);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueI64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, __int64 nValue, BOOL bCreate)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%I64d"), nValue);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueU64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, unsigned __int64 uValue, BOOL bCreate)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%I64u"), uValue);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueB(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, BOOL bValue, BOOL bCreate)
{
	return SetValueI(lpcszKey, lpcszEntry, (int)bValue, bCreate);
}

BOOL IniFile::SetValueC(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, COLORREF clrValue, BOOL bCreate)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%d,%d,%d"), GetRValue(clrValue), GetGValue(clrValue), GetBValue(clrValue));
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueF(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, double dbValue, BOOL bCreate)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%f"), dbValue);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueR(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CRect rcValue, BOOL bCreate/* = TRUE*/)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%d,%d,%d,%d"), rcValue.left, rcValue.top, rcValue.right, rcValue.bottom);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueZ(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CSize size, BOOL bCreate/* = TRUE*/)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%d,%d"), size.cx, size.cy);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueP(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CPoint ptValue, BOOL bCreate/* = TRUE*/)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%d,%d"), ptValue.x, ptValue.y);
	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

BOOL IniFile::SetValueV(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszFormat, ...)
{
	va_list args;
	TCHAR szValue[MAX_VALUE];
	
	va_start(args, lpcszFormat);
	_vstprintf(szValue, lpcszFormat, args);
	va_end(args);

	return SetValueS(lpcszKey, lpcszEntry, szValue);
}

LPCTSTR IniFile::GetValue(size_t nKey, size_t nEntry, LPCTSTR lpcszDefault)
{
	if (!STL_IS_INDEXABLE(nKey, m_dqKeys))
		return lpcszDefault;

	if (!STL_IS_INDEXABLE(nEntry, m_dqKeys[nKey].dqEntries))
		return lpcszDefault;

	return m_dqKeys[nKey].dqEntries[nEntry].lpszValue;
}

LPCTSTR IniFile::GetValueS(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszDefault)
{
	size_t nKey = FindKey(lpcszKey);
	if (nKey == InvalidIndex)
		return lpcszDefault;
	
	size_t nEntry = FindEntry(nKey, lpcszEntry);
	if (nEntry == InvalidIndex)
		return lpcszDefault;
	
	return m_dqKeys[nKey].dqEntries[nEntry].lpszValue;
}

int IniFile::GetValueI(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, int nDefault)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%d"), nDefault);
	return _ttoi(GetValueS(lpcszKey, lpcszEntry, szValue)); 
}

UINT IniFile::GetValueU(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, UINT uDefault)
{
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%u"), uDefault);
	return _ttoi(GetValueS(lpcszKey, lpcszEntry, szValue)); 
}

__int64 IniFile::GetValueI64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, __int64 nDefault)
{
	LPCTSTR lpcszValue = GetValueS(lpcszKey, lpcszEntry, NULL);
	if (!lpcszValue)
		return nDefault;

	__int64 nValue = nDefault;
	_stscanf(lpcszValue, _T("%I64d"), &nValue);

	return nValue;
}

unsigned __int64 IniFile::GetValueU64(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, unsigned __int64 uDefault)
{
	LPCTSTR lpcszValue = GetValueS(lpcszKey, lpcszEntry, NULL);
	if (!lpcszValue)
		return uDefault;

	unsigned __int64 uValue = uDefault;
	_stscanf(lpcszValue, _T("%I64u"), &uValue);

	return uValue;
}

BOOL IniFile::GetValueB(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, BOOL bDefault)
{
	return (BOOL)GetValueI(lpcszKey, lpcszEntry, (int)bDefault);
}

int IniFile::GetKeyCompatibleValueI(LPCTSTR lpcszKey1, LPCTSTR lpcszKey2, LPCTSTR lpcszEntry, int nDefault)
{
	size_t nEntry = FindEntry(lpcszKey1, lpcszEntry);
	if (nEntry != InvalidIndex)
		return GetValueI(lpcszKey1, lpcszEntry, nDefault);

	return GetValueI(lpcszKey2, lpcszEntry, nDefault);
}

BOOL IniFile::GetKeyCompatibleValueB(LPCTSTR lpcszKey1, LPCTSTR lpcszKey2, LPCTSTR lpcszEntry, BOOL bDefault)
{
	size_t nEntry = FindEntry(lpcszKey1, lpcszEntry);
	if (nEntry != InvalidIndex)
		return GetValueB(lpcszKey1, lpcszEntry, bDefault);

	return GetValueB(lpcszKey2, lpcszEntry, bDefault);
}

int IniFile::GetEntryCompatibleValueI(LPCTSTR lpcszKey, LPCTSTR lpcszEntry1, LPCTSTR lpcszEntry2, int nDefault)
{
	size_t nEntry = FindEntry(lpcszKey, lpcszEntry1);
	if (nEntry != InvalidIndex)
		return GetValueI(lpcszKey, lpcszEntry1, nDefault);

	return GetValueI(lpcszKey, lpcszEntry2, nDefault);
}

BOOL IniFile::GetEntryCompatibleValueB(LPCTSTR lpcszKey, LPCTSTR lpcszEntry1, LPCTSTR lpcszEntry2, BOOL bDefault)
{
	size_t nEntry = FindEntry(lpcszKey, lpcszEntry1);
	if (nEntry != InvalidIndex)
		return GetValueB(lpcszKey, lpcszEntry1, bDefault);

	return GetValueB(lpcszKey, lpcszEntry2, bDefault);
}

COLORREF IniFile::GetValueC(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, COLORREF clrDefault)
{
	TCHAR szValue[MAX_VALUE];

	int r, g, b;
	_stprintf(szValue, _T("%d,%d,%d"), GetRValue(clrDefault), GetGValue(clrDefault), GetBValue(clrDefault));
	_stscanf(GetValueS(lpcszKey, lpcszEntry, szValue), _T("%d,%d,%d"), &r, &g, &b);

	return RGB((BYTE)r, (BYTE)g, (BYTE)b);
}

double IniFile::GetValueF(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, double dbDefault)
{
	USES_CONVERSION;
	TCHAR szValue[MAX_VALUE];
	_stprintf(szValue, _T("%f"), dbDefault);

#ifdef _UNICODE
	return atof(T2A(GetValueS(lpcszKey, lpcszEntry, szValue))); 
#else
	return atof(GetValueS(lpcszKey, lpcszEntry, szValue)); 
#endif
}

CRect IniFile::GetValueR(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CRect rcDefault/* = CRect(0,0,0,0)*/)
{
	TCHAR szValue[MAX_PATH];

	CRect rect;
	_stprintf(szValue, _T("%d,%d,%d,%d"), rcDefault.left, rcDefault.top, rcDefault.right, rcDefault.bottom);
	_stscanf(GetValueS(lpcszKey, lpcszEntry, szValue), _T("%d,%d,%d,%d"), &rect.left, &rect.top, &rect.right, &rect.bottom);

	return rect;
}

CSize IniFile::GetValueZ(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CSize szDefault/* = CSize(0,0)*/)
{
	TCHAR szValue[MAX_PATH];

	CSize size;
	_stprintf(szValue, _T("%d,%d"), szDefault.cx, szDefault.cy);
	_stscanf(GetValueS(lpcszKey, lpcszEntry, szValue), _T("%d,%d"), &size.cx, &size.cy);

	return size;
}

CPoint IniFile::GetValueP(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, CPoint ptDefault/* = CSize(0,0)*/)
{
	TCHAR szValue[MAX_PATH];

	CPoint pt;
	_stprintf(szValue, _T("%d,%d"), ptDefault.x, ptDefault.y);
	_stscanf(GetValueS(lpcszKey, lpcszEntry, szValue), _T("%d,%d"), &pt.x, &pt.y);

	return pt;
}

int IniFile::GetValueV(LPCTSTR lpcszKey, LPCTSTR lpcszEntry, LPCTSTR lpcszFormat,
					   void *v1, void *v2, void *v3, void *v4,
					   void *v5, void *v6, void *v7, void *v8,
					   void *v9, void *v10, void *v11, void *v12,
					   void *v13, void *v14, void *v15, void *v16)
{
	LPCTSTR lpcszValue = GetValueS(lpcszKey, lpcszEntry);
	if (_tcslen(lpcszValue))
		return FALSE;

	int nVals = _stscanf(lpcszValue, lpcszFormat,
		v1, v2, v3, v4, v5, v6, v7, v8,
		v9, v10, v11, v12, v13, v14, v15, v16);
	
	return nVals;
}

BOOL IniFile::DeleteEntry(LPCTSTR lpcszKey, LPCTSTR lpcszEntry)
{
	size_t nKey = FindKey(lpcszKey);
	if (nKey == InvalidIndex)
		return FALSE;
	
	size_t nEntry = FindEntry(nKey, lpcszEntry);
	if (nEntry == InvalidIndex)
		return FALSE;
	
	std::vector<Entry>::iterator it = m_dqKeys[nKey].dqEntries.begin() + nEntry;

	delete[] it->lpszEntry;
	delete[] it->lpszValue;

	m_dqKeys[nKey].dqEntries.erase(it);

	return TRUE;
}

BOOL IniFile::DeleteKey(LPCTSTR lpcszKey)
{
	size_t nKey = FindKey(lpcszKey);
	if (nKey == InvalidIndex)
		return FALSE;

	std::vector<Entry>::iterator it;

	it = m_dqKeys[nKey].dqEntries.begin();
	for (; it!=m_dqKeys[nKey].dqEntries.end(); it++)
	{
		delete[] it->lpszEntry;
		delete[] it->lpszValue;
	}
	m_dqKeys[nKey].dqEntries.clear();

	delete[] m_dqKeys[nKey].lpszName;

	m_dqKeys.erase(m_dqKeys.begin() + nKey);

	return TRUE;
}

void IniFile::DeleteAllKey()
{
	std::vector<Key>::iterator it;
	std::vector<Entry>::iterator itEntry;

	it = m_dqKeys.begin();
	for (; it!=m_dqKeys.end(); it++)
	{
		itEntry = it->dqEntries.begin();
		for (; itEntry!=it->dqEntries.end(); itEntry++)
		{
			delete[] itEntry->lpszEntry;
			delete[] itEntry->lpszValue;
		}
		it->dqEntries.clear();

		delete[] it->lpszName;
	}

	m_dqKeys.clear();
}

void IniFile::Erase(BOOL bWithSortKey/* = FALSE*/)
{
	{
		std::vector<Key>::iterator it;
		std::vector<Entry>::iterator itEntry;

		it = m_dqKeys.begin();
		for (; it!=m_dqKeys.end(); it++)
		{
			delete[] it->lpszName;

			itEntry = it->dqEntries.begin();
			for (; itEntry!=it->dqEntries.end(); itEntry++)
			{
				delete[] itEntry->lpszEntry;
				delete[] itEntry->lpszValue;
			}
		}
	}

	{
		std::vector<LPTSTR>::iterator it = m_dqComments.begin();
		for (; it!=m_dqComments.end(); it++)
		{
			delete[] *it;
		}
	}

	m_dqKeys.clear();
	m_dqComments.clear();

	if (bWithSortKey)
		m_mpSortKey.clear();
}

void IniFile::Clear(BOOL bWithSortKey/* = FALSE*/)
{
	Erase(bWithSortKey);
}

void IniFile::Reset(BOOL bWithSortKey/* = FALSE*/)
{
	Erase(bWithSortKey);
}

size_t IniFile::GetCommentCount()
{
	return m_dqComments.size();
}

void IniFile::SetComment(LPCTSTR lpcszComment)
{
	LPTSTR lpszComment2 = new TCHAR[_tcslen(lpcszComment)+1];
	_tcscpy(lpszComment2, lpcszComment);

	m_dqComments.resize(m_dqComments.size() + 1, lpszComment2);
}

LPCTSTR IniFile::GetComment(size_t nComment)
{
	if (!STL_IS_INDEXABLE(nComment, m_dqComments))
		return NULL;

	return m_dqComments[nComment];
}

BOOL IniFile::DeleteComment(size_t nComment)
{
	if (!STL_IS_INDEXABLE(nComment, m_dqComments))
		return FALSE;

	std::vector<LPTSTR>::iterator it = m_dqComments.begin() + nComment;

	delete[] *it;

	m_dqComments.erase(it);

	return TRUE;
}

void IniFile::DeleteAllComments()
{
	std::vector<LPTSTR>::iterator it;

	it = m_dqComments.begin();
	for (; it!=m_dqComments.end(); it++)
	{
		delete[] *it;
	}

	m_dqComments.clear();
}

int IniFile::Compare(LPCTSTR lpcszString1, LPCTSTR lpcszString2)
{
	if (m_bCaseInsensitive)
		return _tcsicmp(lpcszString1, lpcszString2);

	return _tcscmp(lpcszString1, lpcszString2);
}

