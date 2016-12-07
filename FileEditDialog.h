#pragma once

#include <vector>
#include <stdint.h>
#include "resource.h"
#include <map>
#include "cs/editCsSkt.h"
#include "EditStatueFile.h"

class CTimerShaft;
class CImageContainer;

typedef std::vector<CTimerShaft*> TIMERSHARTLIST;
typedef std::map<CString, CTimerShaft*> TIMERSHAFTMAP;
typedef std::vector<CImageContainer*> IMAGECONTAINER_LIST;

static unsigned int PIC_WIDTH = 160;
static unsigned int PIC_HEIGHT = 160;

// CFileEditDialog dialog

#define VERNIER_STATIC_BASE 62000

class CFileEditDialog : public CDialog
{
	DECLARE_DYNAMIC(CFileEditDialog)

public:
	CFileEditDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileEditDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnDrawTimershaft(WPARAM wParam, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  bool CreateNewTimerShaft(int nTaskId, const CString& strMp4Name, int nRef);
  void DrawTimershaft(int nTaskId, BOOL bAdd);
  void SetDuration(int64_t duration){m_Duration = duration;} 
  void SetDrawTimershaft(bool b){m_bDrawTimershaft = b;}
  void DelTimershaftMember(CTimerShaft* pShaft, CRect& rtRemove);
  const TIMERSHAFTMAP& GetTimerShaftMap() {return m_TimerShaftMap;}
  void SetCurMp4Name(const CString& strMp4Name) {m_strMp4Name = strMp4Name;}
  const CString& GetCurMp4Name() {return m_strMp4Name;}
  void ResetTimerShaft();
  void SetSelTaskId(int nTaskId) {m_nSelTaskId = nTaskId;}
  int GetTaskID() const {return m_nSelTaskId;}
  void RemoveTimeShaft(const CString& strTaskName);
  void Release();

private:
  void ClearTaskList();
  int  GetScrollPos(int nBar, UINT nSBCode);  
  void ScrollClient(int nBar, int nPos) ;  
  void VScrool(int nBar, UINT nSBCode) ;   

  static unsigned int __stdcall ReadTimeRecordThd(void* pParam);

private:
  TIMERSHARTLIST m_TimerShaftList;
  TIMERSHAFTMAP m_TimerShaftMap;
  IMAGECONTAINER_LIST m_ImageContainerList;
  int64_t m_Duration;     //ms
  bool m_bDrawTimershaft;
  unsigned int m_nTimerShaftCount;
  unsigned int m_nImageContainerCount;
  CString m_strMp4Name;

  CCriticalSection m_Lock;
  int m_page; 
  int m_nScrollPos;

  EDIT_TASK_LIST m_editTaskList;
  int m_nSelTaskId;
  HANDLE m_hReadTimeRecrod;
  HANDLE m_hReadEvent;
  HANDLE m_hReadNotifyEvent;

  std::vector<CString>  m_taskNameList;
public:
  afx_msg void OnSize(UINT nType, int cx, int cy);   
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnClose();
};

