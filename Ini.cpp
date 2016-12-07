// Ini.cpp: implementation of the CIni class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ini.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIni::CIni()
{

}

CIni::~CIni()
{

}

DWORD CIni::ReadString(LPCTSTR lpAppName, LPCTSTR lpKyeValue, LPTSTR lpReturnStr, 
					   DWORD nSize, LPCTSTR lpFileName, LPCTSTR lpDefault/* = NULL*/)
{
	TCHAR FilePath[255] = {".\\"};
	lstrcat(FilePath, lpFileName);

	return GetPrivateProfileString(lpAppName, lpKyeValue, lpDefault,
		lpReturnStr, nSize, FilePath);
}

BOOL CIni::WriteString(LPCTSTR lpAppName, LPCTSTR lpKeyValue, LPCTSTR lpStr, 
					   LPCTSTR lpFileName)
{
	TCHAR FilePath[255] = {".\\"};
	lstrcat(FilePath, lpFileName);

	return WritePrivateProfileString(lpAppName, lpKeyValue, lpStr, 
		FilePath);
}

int CIni::GetSections(CStringArray& arrSection, LPCTSTR lpFileName)
{ 
	TCHAR FilePath[255] = {".\\"};
	lstrcat(FilePath, lpFileName);

	TCHAR szSectionNames[2048] = {'\0'};
	char* pszSectionName = NULL;
	int j = 0;
	int count = 0;

	GetPrivateProfileSectionNames(szSectionNames, 2048, FilePath);

	for (int i=0; i<2048; ++i, j++)
	{
		if (szSectionNames[0] == '\0')			break;
		if (szSectionNames[i] == '\0')
		{
			pszSectionName = &szSectionNames[i - j];
			j = -1;

			CString strAppName = pszSectionName;
			arrSection.Add(strAppName);

			count++;
			if (szSectionNames[i+1] == '\0')	break;
		}
	}
	return count;
}
