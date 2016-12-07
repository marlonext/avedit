#pragma once
#include "afxcmn.h"
#include "EditTaskListCtrl.h"
#include "ScopeLock.h"
#include <map>
#include <vector>

typedef struct tagName_Path
{
  CString m_strName;
  CString m_strPath;
}NAME_PATH;


typedef std::vector<int> TASKIDLIST;
typedef std::map<CString, BYTE> TASKNAME_USE_DICT;
typedef std::vector<NAME_PATH>  TASKNAME_PATH_LIST;

typedef std::map<int, NAME_PATH> ID_NAMEPATH_DICT;

// CTaskListDialog dialog

class CFileEditDialog;
class CMediaPlayerDialog;

class CTaskListDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskListDialog)
//   void RandIdList();

public:
	CTaskListDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTaskListDialog();

  int GetEditFileIndex() {return m_nSelMp4;}

  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT OnAddMp4(WPARAM wParam, LPARAM lParam);

//   void SetMp4Dir(const CString& strDir){m_strDir = strDir;}
//   void SetMp4Name(const CString& strMp4){m_strMp4Name = strMp4;}
//   void SetMp4Alias(const CString& strAlias) {m_strAlias = strAlias;}

  void SetCurSelMp4(int nSel) {m_nSelMp4 = nSel;}
  void UpdateUI();

//   const CString& GetSelMp4Name() const {return m_strMp4Name;}
  CEditTaskListCtrl& GetListCtrl() {return m_taskListCtrl;}

  int GetActiveItem() {return m_nSelMp4;}
  int GetActiveTaskId();
  int GetPlayItem() const {return m_nCurPlayItem;}
  int GetDelItem() const {return m_nDelItem;}

// Dialog Data
	enum { IDD = IDD_DIALOG_TASKLIST };

protected:
  
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
  int m_nCurPlayItem;
  int m_nDelItem;
  bool m_bSelect;
  int m_nCurSelMenu;
  int m_nSelMp4;            //index of selected mp4
  int m_nLastSelMp4;        

  static const int MAX_TASK_NUM = 20;
  TASKNAME_USE_DICT   m_TaskNameUseDict;

  int m_nFileCount;

  CCriticalSection    m_AddMp4Lock;
  EDITSTATUSLIST      m_statusList;
  CEditTaskListCtrl m_taskListCtrl;

  CCS m_cs;

private:
  bool AddMp4();
  bool RemoveMp4();
  bool ClearAllMp4();
  bool Mp4PrevView();
  void AutoSize();

  void DelElementFromIDNamePathDict(int nTaskId);
  void DelElementFormTaskNamePathList(const CString& strMp4Name);
  void DelElementFromTaskNameUseDict(const CString& strMp4Name);

  bool IsTimerShaftDraw(const CString& strMp4);

  const CString GetMp4Path(int nItem, int nSubItem);
  const CString GetMp4Name(int nItem, int nSubItem);

  CFileEditDialog* GetFileEditDlg();
  CMediaPlayerDialog* GetMediaPlayerDlg();

  void _SetEditStatus(int nStatus, CString& strStatus);
  void _InitListCtrl();
  void _SetNamePathDict(const NAME_PATH& strName, int nItemIndex);

  void _Release(/*EDITSTATUSLIST& list*/);

  afx_msg LRESULT OnStartPlay(WPARAM wParam, LPARAM lParam);
// public:
//   afx_msg void OnNMRClickListTaskinfo(NMHDR *pNMHDR, LRESULT *pResult);
//   afx_msg void OnNMClickListTaskinfo(NMHDR *pNMHDR, LRESULT *pResult);
//   void OnMP4Operation(UINT nID);
public:
  afx_msg void OnClose();
};
