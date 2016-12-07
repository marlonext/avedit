#pragma once

#include <stdint.h>
#include <map>
#include "afxmt.h"
#include "EditTask.h"
#include "ScopeLock.h"
#include "cs/editCs.h"



class CFileEditDialog;
class CTimeEdit;
class CTimerShaft;
class CInternetSession;
class CFtpConnection;



typedef struct tagUpLoad
{
  CString m_strURL;
  CString m_strServer;
  CString m_strPath;
  CString m_strName;
}UPLOAD;


typedef struct tagBeginEndPoint
{
  POINT m_ptBegin;
  POINT m_ptEnd;
}BEGINENDPOINT, *PBEGINENDPOINT;

typedef struct tagBeginEndTimeEdit
{
  CTimeEdit* m_pTimeEditBegin;
  CTimeEdit* m_pTimeEditEnd;
}BEGINENDTIMEEDIT, *PBEGINENDTIMEEDIT;

typedef struct tagPointTimeEdit
{
  BEGINENDPOINT m_pointPair;
  BEGINENDTIMEEDIT m_TimeEditPair;

  tagPointTimeEdit& operator=(const tagPointTimeEdit& obj)
  {
    memcpy(this, &obj, sizeof(tagPointTimeEdit));
    return *this;
  }

}POINTTIMEEDIT, *PPOINTTIMEEDIT;      //时间对应的timeedit

typedef struct tagCutTimePoint
{
  CS_CUT_TIME m_CSCutTime;
  BEGINENDPOINT m_BeginEndPoint;

  tagCutTimePoint& operator=(const tagCutTimePoint& obj)
  {
    memcpy(this, &obj, sizeof(tagCutTimePoint));
    return *this;
  }

}CUTTIMEPOINT, *PCUTTIMEPOINT;

//编辑任务
typedef struct tagEditTask
{
  int m_nMilliSeconds;            //编辑任务时长
  CUTTIMEPOINT m_CuttimePoint;
}EDITTASK, *PEDITTASK;

template <class T>
struct Compare
{
  int operator()(const T& x, const T& k) const
  {
    if (x.x >= k.x) return 0;
    return 1;
  }
};

typedef std::vector<CRect> RECTLIST;
typedef std::map<CPoint, CTimeEdit*, Compare<CPoint>> POS_TIMEEDIT_MAP;
typedef std::vector<POINTTIMEEDIT> POINTTIMEEDITLIST;   //坐标点与时间
typedef std::vector<CUTTIMEPOINT> CUTTIMEPOINTLIST;     



class CTimerShaft : public CStatic
{
public:
  CTimerShaft(int nTaskId, CFileEditDialog& dlg, int64_t seconds, const char* pFileName, int nRef, std::vector<CString>& taskNameList);
  virtual ~CTimerShaft(void);

  DECLARE_MESSAGE_MAP()
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

  void DrawLine(CDC& dc, const CPoint& ptBegin, const CPoint& ptEnd);

  void Draw(CDC& dc, const CRect& rt);
  void DrawDragBkColor(CDC& dc);
  void ChangeSize(CWnd* pWnd, int cx, int cy);

  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
//   afx_msg void OnStnClicked();
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

public:
  void SetRect(const CRect& rt){m_rt = rt;}
  const CRect& GetRect() {return m_rt;}
  afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  bool IsPtInSelectedMp4Rect();
  afx_msg void OnEditOp(UINT nID);

  void Settings();
  void AddPicHead();
  void AddPicTail();
  void SetTitleDuration();
  void SetTailDuration();
  void SetFps();
  void AddEditTask();
  void RemoveEditTask();
  void StartEditTask();
  void PauseEditTask();
  void ResumeEditTask();
  void ApplySet();
  void AddPic(CString& strName, CString& strPicName);
  void UpdatePos(const CRect& rt){m_rt = rt;}

  void SetCuttimes(CS_CUT_TIME* pCutTime, int nCutTimeCount);           //设置编辑时间点

  std::vector<CString>& GetTaskNameList() 
  {
    return m_taskNameListRef;
  }

private:
  void EraseRect(const CPoint& pt);
  void ChangePos(const CPoint& pt);
  void RectSort();
  void DrawRectListMember(CDC& dc);
  void SetCutTime(float fPercent, bool bStartTime = true);
  BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
  void FormatTime(int64_t lTime, CString& strFmt);

  void UpdatePosList(const CPoint& ptCur, int nSel, bool bBegin = true);
  void ReleaseResource();

  void SetCuttimePoint(const CRect& rt, const CPoint& pt, bool bHeadPoint = true);
  bool UpdateCuttimePoint(const CPoint& pt, int64_t& time, bool bStartTime = true);
  void GetEditCuttimeStatue(const CRect& rt, const CPoint& pt, bool& bReEditHead, bool& bReEditTail, bool& bLBtnStatue);

  CTimeEdit* CreateNewTimeEdit(int64_t time, const CPoint& pt, const CRect& rt, bool bLeft);
  void ShowTimeEdit();
  void MoveTimeEdit(CTimeEdit* pCurTimeEdit, const CPoint& pt, const CRect& rt);
  void UpdateEditTime(CTimeEdit* pCurTimeEdit, const CPoint& pt, const CRect& rt);
  void UpdatePointIimeEditList(CTimeEdit* pCurTimeEdit, const CPoint& pt, const CRect& rt);
  BOOL DelTimeBlock();

  void StoreRect(CRect& rt);

  const CS_CUT_TIME* SearchCutTimeByPoint(const CPoint& ptSel, const CRect& rt);
  CTimeEdit* SearchTimeEditByPoint(const CPoint& ptSel, const CRect& rt);

  void UpdateRectList(const CPoint& pt);
  void UpdateCuttimeList();

  int SearchRectIndex(const CPoint& pt);

  bool CanMove(const CPoint& ptBegin, const CPoint& ptEnd);
  bool IsDragMoveValid(const CPoint& ptDrag, CPoint& ptRet, const CRect& rt);
  bool IsEndPointCrossBlock(const CPoint& ptEnd, CPoint& ptOut);

  BOOL FTP_Upload(const CString& strURL, const CString& strServer, const CString& strFilePath, const CString& strFileName);
  BOOL Http_Upload(LPCTSTR strURL, LPCTSTR strLocalFileName);
  BOOL UpLoad(const CString& strURL, const CString& strServer, const CString& strFilePath, const CString& strFileName);

  void SortCuttimeRect();
  void SortPointTimeEdit();
  void SortCuttimePoint();

  void LogTimeBlock();

  void _SetTimeRecordParam();

private:
  int m_nRef;
  std::string m_strFileName;
  int64_t  m_Seconds;
  CRect m_rt;
  CFileEditDialog& m_Editdlg;
  int m_nScaleRate;
  int m_nSpace;

  CPoint m_ptBegin;
  CPoint m_ptEnd;
  CPoint m_PtSel;
  CPoint m_ptEndTemp;
  bool m_bLBtnDown;
//   CToolTipCtrl m_TimeTip;   //时间提示

  std::auto_ptr<CMenu> m_pPopupMenu;
  CEditOpts m_EditOpts;
  RECTLIST m_rtList;    //记录已经截取的矩形区域
  CMutex m_Mutex;
  std::auto_ptr<CSingleLock> m_pLock;
  CS_CUT_TIME* m_pCutTime;
//   CS_EDIT_TASK& m_EditTaskRef;
  unsigned int m_nFileArySize;
  CString m_strIP;

  int m_nTaskID;
  bool m_bReEditHead;
  bool m_bReEditTail;

  POS_TIMEEDIT_MAP* m_pPosTimeEditMap;

  int m_nPosIndex;
  CTimeEdit* m_pNewTimeEdit;
  CTimeEdit* m_pSelTimeEdit;
  CS_CUT_TIME* m_pSelCutTime;
  int m_nSelTimeEdit;
  POINTTIMEEDIT m_PointTimeEdit;
  POINTTIMEEDITLIST* m_pPointTimeEditList;
  CUTTIMEPOINT* m_pCutTimePt;
  CUTTIMEPOINTLIST* m_pCutTimePtList;
  int m_nSelRect;
  bool m_bValidArea;
  bool m_bValidHeadPoint;
  bool m_bValidTailPoint;

//   CFtpConnection *m_pFtpConnection;
//   CInternetSession *m_pInetsession;

  CS_EDIT_TASK m_EditTask;
//   HANDLE m_hUpLoad;
  HANDLE m_hUpLoadEvent;

  UPLOAD  m_ULTitle;
  UPLOAD  m_ULTail;
  CString m_strAppName;
  BOOL  m_bUpLoadSuccess;

  EDITSTATUSLIST m_editStatusList;

  BOOL m_bTaskAdd;
  BOOL m_bPauseEdit;
  bool m_bTaskHasAdd;

  CCS m_cs;

  std::vector<CString>& m_taskNameListRef;  

  static unsigned __stdcall UpLoadThread(void* pParam);
  void Main();
  BOOL _UpLoad();

public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg LRESULT OnRemoveEditTask(WPARAM wParam, LPARAM lParam);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

