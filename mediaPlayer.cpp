// mediaPlayer.cpp : Implementation of CmediaPlayerApp and DLL registration.

#include "stdafx.h"
#include "mediaPlayer.h"
#include "SAStatusLog.h"
#include "MainFrm.h"
#include "LoginDialog.h"
#include "EditStatueFile.h"
#include "ProgressCtrlView.h"
#include "URL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CmediaPlayerApp theApp;
CSAStatusLog g_statusLog;
CEditStatueFile g_editStatusFile;
CTimeRecord g_timeRecord;

CMainFrame* g_pMainFrame = NULL;

const GUID CDECL _tlid = { 0x4078D9F8, 0xF4E0, 0x4959, { 0xB9, 0x2F, 0x4A, 0x7B, 0x94, 0x98, 0x6C, 0x42 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

typedef void (CmediaPlayerApp::*PFUN)();

CmediaPlayerApp::CmediaPlayerApp()
{
  // TODO: 在此处添加构造代码，
  // 将所有重要的初始化放置在 InitInstance 中
//   m_bConnect = false;
  m_nMenu = 0;
  m_bEditSingleFile = true;
  m_bTerminate = false;
  m_hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  memset(&m_EditTaskStatus, 0, sizeof(m_EditTaskStatus));
  m_bOnline = false;
  m_hRecvThread = INVALID_HANDLE_VALUE;
  m_hKeepAliveThread = INVALID_HANDLE_VALUE;

  m_pTaskManager = CTaskManager::Instance();
}

// CmediaPlayerApp::InitInstance - DLL initialization

BOOL CmediaPlayerApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
    if (!AfxSocketInit())
    {
      AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
      return FALSE;
    }

    g_statusLog.Init("edit_health.log");
    g_statusLog.StatusOut("g_editStatusFile.Init()");
    g_statusLog.StatusOut("g_timeRecord.Init()");

    m_pEditCs.reset(new CEditCs);

    m_hRecvThread = (HANDLE)_beginthreadex(nullptr, 0, RecvThread, this, CREATE_SUSPENDED, nullptr);
    m_hKeepAliveThread = (HANDLE)_beginthreadex(nullptr, 0, KeepAliveThread, this, CREATE_SUSPENDED, nullptr);

    g_editStatusFile.Init();
    g_timeRecord.Init();
	}

	return bInit;
}



// CmediaPlayerApp::ExitInstance - DLL termination

int CmediaPlayerApp::ExitInstance()
{
	// TODO: Add your own module termination code here.
  CloseHandle(m_hRecvThread);
  m_hRecvThread = INVALID_HANDLE_VALUE;

  SAFE_DELETE(m_pTaskManager);
//   SAFE_DELETE(m_pEditCs);

  CMainFrame* pMainFrame = (CMainFrame*)GetMainWnd();
  CProgressCtrlView* pProgressView = pMainFrame->m_pProgressView;
  if (pProgressView)
  {
    pProgressView->Release();
  }

	return COleControlModule::ExitInstance();
}

void CmediaPlayerApp::UpdateEditStatue(const EDIT_TASK_STATUS& taskStatus)
{
  if (m_pTaskManager)
  {
    m_pTaskManager->UpdateTaskStatus(taskStatus);
  }

//   memcpy(&m_EditTaskStatus, &taskStatus, sizeof(EDIT_TASK_STATUS));
//   ::SendMessage(g_pMainFrame->GetSafeHwnd(), WM_UPDATEPANETEXT, NULL, (LPARAM)(&m_EditTaskStatus));
  g_pMainFrame->UpdateStatueBar(taskStatus);
}

BOOL CmediaPlayerApp::KeepAlive()
{
  m_pEditCs->csSendKeepAliveMsg(e_edit_keepalive);
  if (FALSE == m_pEditCs->IsLinking())
  {
#if 1
    SYSTEMTIME now;
    GetLocalTime(&now);

    g_statusLog.StatusOut("%d:%d:%d:%d, offline", now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);

#endif
    PFUN pfun = &CmediaPlayerApp::Reconnect;
    (this->*pfun)();
  }

  return TRUE;
}

BOOL CmediaPlayerApp::UpLoadFile(const CString& strFilePath)
{
  return m_pEditCs->csUploadFile(strFilePath);
}

void CmediaPlayerApp::StartRecvEvent()
{
  static bool send = false;

  if (!send)
  {
    SetEvent(m_hStartEvent);
    send = true;
  }
}

unsigned int __stdcall CmediaPlayerApp::RecvThread(void* pParam)
{
  CmediaPlayerApp* pThis = (CmediaPlayerApp*)pParam;
  WaitForSingleObject(pThis->m_hStartEvent, INFINITE);
  if (pThis)
  {
    pThis->Main();
  }

  return 0;
}

unsigned int __stdcall CmediaPlayerApp::KeepAliveThread(void* pParam)
{
  while (!theApp.m_bTerminate)
  {
    theApp.KeepAlive();

    Sleep(10000);
  }

  return 0;
}

void CmediaPlayerApp::Main()
{
  while (!m_bTerminate)
  {
    m_pEditCs->csOnCmdRecv();
  }

  m_bOnline = false;
}

void CmediaPlayerApp::SetTerminate(bool bTerminate)
{
  m_bTerminate = bTerminate;
  SetEvent(m_hStartEvent);

//   ::CloseHandle(m_hRecvThread);
//   m_hRecvThread = INVALID_HANDLE_VALUE;
// 
//   ::CloseHandle(m_hStartEvent);
//   m_hStartEvent = INVALID_HANDLE_VALUE;
}

void CmediaPlayerApp::Reconnect()
{
  m_pEditCs->Close();
  g_statusLog.StatusOut(_T("CmediaPlayerApp::Reconnect()"));
  BOOL bRet = FALSE;

  bRet = m_pEditCs->csInitialize(FALSE, ::inet_addr(CURL::GetIp()));
  Sleep(10000);

  if (!bRet)
  {
    TRACE(_T("重连至服务器失败\r\n"));
    g_statusLog.StatusOut(_T("重连至服务器失败"));
  }
}


// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


