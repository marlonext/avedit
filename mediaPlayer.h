#pragma once

// mediaPlayer.h : main header file for mediaPlayer.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols
#include "cs/editCs.h"
#include "ScopeLock.h"

// CmediaPlayerApp : See mediaPlayer.cpp for implementation.
class CTaskManager;

class CmediaPlayerApp : public COleControlModule
{
public:
  CmediaPlayerApp();
	BOOL InitInstance();
	int ExitInstance();

  CEditCs& GetEditCs() 
  {
    return *m_pEditCs;
  }

  bool IsEditSingleFile() {return m_bEditSingleFile;}
  void SetTerminate(bool bTerminate);

  void UpdateEditStatue(const EDIT_TASK_STATUS& taskStatus);

  BOOL KeepAlive();

  void StartRecvEvent();
  inline void StartRecvThd() {::ResumeThread(m_hRecvThread);}
  inline void StartKeepAliveThd() {::ResumeThread(m_hKeepAliveThread);}
  BOOL UpLoadFile(const CString& strFilePath);
  bool IsTerminate() const {return m_bTerminate;}

  void Reconnect();

  std::auto_ptr<CEditCs> m_pEditCs;
  bool m_bOnline;

  CTaskManager* m_pTaskManager;

private:
  bool m_bEditSingleFile;
  int m_nMenu;

  bool m_bTerminate;
  HANDLE m_hRecvThread;
  HANDLE m_hStartEvent;
  EDIT_TASK_STATUS m_EditTaskStatus;  

  HANDLE m_hKeepAliveThread;
  HANDLE  m_hReconnectThread;

  static unsigned int __stdcall RecvThread(void* pParam);
  static unsigned int __stdcall KeepAliveThread(void* pParam);
  void Main();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

extern CmediaPlayerApp theApp;
