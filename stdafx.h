#pragma once

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _ATL_NO_AUTOMATIC_NAMESPACE             // avoid class name conflicts

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Comon Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
#ifndef _WIN64

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#endif // _WIN64

#include <afxsock.h>		// MFC Ì×½Ó×ÖÀ©Õ¹
#include <afxcontrolbars.h>

#include <atlimage.h>
#include <afxwin.h>
#include <afx.h>


// #include <gdiplus.h>  
// using namespace Gdiplus; 
// 
// #if defined(_WIN64)
// typedef unsigned __int64 ULONG_PTR;
// #else
// typedef unsigned long ULONG_PTR;
// #endif
// 
// // Initialize GDI+  
// ULONG_PTR g_gdiplusToken;  
// Gdiplus::GdiplusStartupInput gdiplusStartupInput;
// GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);  

#define WM_STARTPLAY            (WM_USER + 1)
#define WM_SETFILEICON          (WM_USER + 2)
#define WM_DRAWICONANDNAME_BK   (WM_USER + 3)
#define WM_UPDATAUI             (WM_USER + 4)
#define WM_DRAWPLAYICON         (WM_USER + 5)
#define WM_DRAWSTOPICON         (WM_USER + 6)
#define WM_DRAWTIMERSHAFT       (WM_USER + 7)
#define WM_SEEK                 (WM_USER + 8)
#define WM_REDRAWTIMERSHAFT     (WM_USER + 9)
#define WM_CONNECTION_FAILD     (WM_USER + 10)
#define WM_CONNECTION_SUCCESS   (WM_USER + 11)
#define WM_ADDPIC               (WM_USER + 12)
#define WM_UPDATEPANETEXT       (WM_USER + 13)
#define WM_OFFLINE              (WM_USER + 14)
#define WM_START_DOWNLOAD       (WM_USER + 15)
#define WM_UPDATE_DOWNLOAD_PROCESS (WM_USER + 16)
#define WM_ADDMP4               (WM_USER + 17)
#define WM_SHOWMP4NAME          (WM_USER + 18)
#define WM_LOOPPLAY             (WM_USER + 19)
#define WM_DOWNLOADSUCESS       (WM_USER + 20)
#define WM_DOWNLOADFAILURE      (WM_USER + 21)
#define WM_RESETBTNSTATUS       (WM_USER + 22)
#define WM_RESETPLAYSTATUS      (WM_USER + 23)
#define WM_DRAWCIRCULARBTN      (WM_USER + 24)
#define WM_ADDTASKNAME          (WM_USER + 25)
#define WM_REMOVEEDITTASK       (WM_USER + 26)
#define WM_AUTODELETE_EDITTASK  (WM_USER + 27)
#define WM_RECONNECT            (WM_USER + 28)

#define KEEPALIVE_EVENT   10000
#define WM_LOOPPLAY_TIMER 10001
#define WM_UPLOAD_TIMER   10002

#define TIMERSHAFT_BASE         66000
#define IMAGECONTAINER_BASE     67000

#define MAX_TASKLIST_NUM 20

#define MAX_EDIT_FILE_NUM       10
#define SAFE_DELETE(p) if(p){delete (p);(p)=NULL;}

#define RECVPACK_SIZE 2048

const int TIMEBOX_WIDTH = 60;
const int TIMEBOX_HEIGHT = 20;


bool DownloadSaveFiles(char* url,char *strSaveFile);
bool UploadFile(LPCTSTR strURL, LPCTSTR strLocalFileName);

typedef long long int64_t;
typedef unsigned short int UInt16b;

#define MAX_PATH_LEN 256

#include <vector>
typedef struct tagEditStatus
{
  UInt16b m_nTaskId;
  UInt16b m_taskStatus;
  UInt16b m_nEditProgress;
  char    m_szSourceFileName[MAX_PATH_LEN];
  char    m_szDestFileName[MAX_PATH_LEN];
  char    m_szSourceAlias[MAX_PATH_LEN];

  UInt16b m_bUseFlag;
}EDITSTATUS;

typedef std::vector<EDITSTATUS> EDITSTATUSLIST;


static int Str2Int(const CString& str)
{
  int nRet = 0;

  for (int i = 0; i < str.GetLength(); ++i)
  {
    nRet = nRet * 10 + str[i] - '0';
  }

  return nRet;
}

static CString Int2Str(int n)
{
  CString strFmt;
  strFmt.Format("%d", n);

  return strFmt;
}
