// Ini.h: interface for the CIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__A98BECFC_7CD3_435A_826B_A515EC67A92B__INCLUDED_)
#define AFX_INI_H__A98BECFC_7CD3_435A_826B_A515EC67A92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIni  
{
public:
	CIni();
	virtual ~CIni();

public:
	static DWORD ReadString(LPCTSTR lpAppName, LPCTSTR lpKyeValue, LPTSTR lpReturnStr, 
							DWORD nSize, LPCTSTR lpFileName, LPCTSTR lpDefault = NULL);
	static BOOL WriteString(LPCTSTR lpAppName, LPCTSTR lpKeyValue, LPCTSTR lpStr, 
							LPCTSTR lpFileName);
	static int GetSections(CStringArray& arrSection, LPCTSTR lpFileName);
};

#endif // !defined(AFX_INI_H__A98BECFC_7CD3_435A_826B_A515EC67A92B__INCLUDED_)
