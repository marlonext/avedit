#include "StdAfx.h"
#include "URL.h"

CString CURL::m_strURL;
CString CURL::m_strIP;
CString CURL::m_strFileName;
CString CURL::m_strAlias;

CURL::CURL(const CString& strURL)
{
  m_strURL.Empty();
  m_strIP.Empty();
  m_strFileName.Empty();
  m_strAlias.Empty();

  ExtraIP(strURL);
  ExtraFileName(strURL);
  ExtraAlias(strURL);
  ExtraURL(strURL);
}


CURL::~CURL(void)
{
}

void CURL::ExtraIP(const CString& strSource)
{
  const TCHAR szHttp[] = _T("http://");
  if (strSource.IsEmpty())
  {
    return;
  }

  m_strIP.Empty();

  const TCHAR* pIpStart = strSource;
  pIpStart += _tcslen(szHttp);
  while (*pIpStart != TEXT('/'))
  {
    m_strIP += *pIpStart++;
  }
}

void CURL::ExtraFileName(const CString& strSource)
{
  if (strSource.IsEmpty())
  {
    return;
  }

  int nNameStart = strSource.ReverseFind(_T('/'));
  const TCHAR* pName = strSource;
  pName += nNameStart;

  CString strTemp;
  while (*pName++ != '\0')
  {
    strTemp += *pName;
  }

  int nIndex = strTemp.Find('|');
  m_strFileName = strTemp.Left(nIndex);
}

void CURL::ExtraAlias(const CString& strSource)
{
  int nIndex = nIndex = strSource.Find('|');
  if (-1 == nIndex)
  {
    return;
  }

  m_strAlias = strSource.Right(strSource.GetLength() - (nIndex + 1));
}


void CURL::ExtraURL(const CString& strSource)
{
  int nIndex = nIndex = strSource.Find('|');
  if (-1 == nIndex)
  {
    return;
  }

  m_strURL = strSource.Left(nIndex);
}

