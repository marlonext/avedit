#pragma once

#include "ScopeLock.h"
#include <map>

class CFileEditDialog;
class CMediaPlayerDialog;
class CEditTask;
class CTaskManager;

typedef std::map<UInt16b, BYTE> TASK_USE_DICT;
// CEditTaskListCtrl

class CEditTaskListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEditTaskListCtrl)

private:
  CCS m_cs;
  TASK_USE_DICT   m_taskUseDict;

  UInt16b m_nTaskID;
//   CString m_strTaskName;
//   CString m_strTaskAlias;
  CString m_strDir;
  int m_nCurPlayItem;

private:
  BOOL RemoveMp4();
  BOOL ClearAllMp4();
  BOOL Mp4PrevView();
  BOOL ResumeEditTask();
  BOOL PauseEditTask();

  bool IsTimerShaftDraw(UInt16b nTaskID);

  CFileEditDialog* GetFileEditDlg();
  CMediaPlayerDialog* GetMediaPlayerDlg();
  void _LoadAllTask(/*const*/ CTaskManager* pManager);

public:
	CEditTaskListCtrl();
	virtual ~CEditTaskListCtrl();

  UInt16b GetTaskId() const;
  const CString& GetTaskSourceName(int nTaskId);
  const CString& GetTaskAlias(int nTaskId);
  const CString& GetTaskDir(int nTaskId);
  const CString& GetTaskDir() const;
  void UpdataTaskStatus(UInt16b nTaskId, UInt16b nTaskStatus);

  void SetTaskDir(const CString& strDir)
  {
    m_strDir = strDir;
  }

  void AddTask(const CEditTask* pTask);

protected:
	DECLARE_MESSAGE_MAP()

public:
  void OnMP4Operation(UINT nID);
  afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg LRESULT OnAddMp4(WPARAM wParam, LPARAM lParam);
  BOOL DeleteTask(int nTaskId);

private:
  HANDLE m_hTimeShaftThd;
  CFileEditDialog* m_pEditDlg;

  static unsigned int __stdcall DrawThread(void* pParam);
  void Main(CFileEditDialog* pDlg);
public:
  afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


