// FileEditDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "FileEditDialog.h"
#include "TimerShaft.h"
#include "MainFrm.h"
#include "PreView.h"
#include "MediaPlayerDialog.h"
#include "TaskListDialog.h"
#include "FilePreView.h"
#include "EditStatueFile.h"
#include "SAStatusLog.h"
#include <memory>


// CFileEditDialog dialog

extern CMainFrame* g_pMainFrame;
extern CTimeRecord g_timeRecord;
extern CSAStatusLog g_statusLog;

IMPLEMENT_DYNAMIC(CFileEditDialog, CDialog)

  CFileEditDialog::CFileEditDialog(CWnd* pParent /*=NULL*/)
  : CDialog(CFileEditDialog::IDD, pParent)
{
  m_Duration = 0;
  m_bDrawTimershaft =false;
  m_nTimerShaftCount = 0;
  m_nImageContainerCount = 0;
  m_page = 0;
  m_nScrollPos = 0;
  
  m_hReadTimeRecrod = INVALID_HANDLE_VALUE;
  m_hReadNotifyEvent = INVALID_HANDLE_VALUE;  
  m_hReadEvent = INVALID_HANDLE_VALUE;
}

CFileEditDialog::~CFileEditDialog()
{
  TIMERSHARTLIST::iterator it = m_TimerShaftList.begin();
  for (; it != m_TimerShaftList.end(); ++it)
  {
    if (*it)
    {
      delete *it;
      *it = NULL;
    }
  }

  m_TimerShaftList.clear();
  m_editTaskList.clear();
  if (m_hReadEvent)
  {
    CloseHandle(m_hReadEvent);
    m_hReadEvent = INVALID_HANDLE_VALUE;
  }

  if (m_hReadTimeRecrod)
  {
    CloseHandle(m_hReadTimeRecrod);
    m_hReadTimeRecrod = INVALID_HANDLE_VALUE;
  }
}

void CFileEditDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFileEditDialog, CDialog)
  ON_WM_LBUTTONUP()
  ON_MESSAGE(WM_DRAWTIMERSHAFT, OnDrawTimershaft)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CLOSE()
END_MESSAGE_MAP()


// CFileEditDialog message handlers


BOOL CFileEditDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
//   CRect rc;    
//   GetClientRect(&rc);    
// 
//   const SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };    
// 
//   SCROLLINFO si;    
//   si.cbSize = sizeof(SCROLLINFO);    
//   si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;    
//   si.nPos = si.nMin = 1;    
// 
//   si.nMax = sz.cy*m_page;    
//   si.nPage = sz.cy;    
//   SetScrollInfo(SB_VERT, &si, FALSE);  //此函数将产生一个垂直滚动条  
  m_taskNameList.clear();
  m_hReadTimeRecrod = (HANDLE)_beginthreadex(NULL, 0, ReadTimeRecordThd, this, 0, NULL);
  m_hReadNotifyEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
  m_hReadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileEditDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default

  CDialog::OnLButtonUp(nFlags, point);
}

LRESULT CFileEditDialog::OnDrawTimershaft(WPARAM wParam, LPARAM lParam)
{
  SetEvent(m_hReadNotifyEvent);

  int nTaskId = (int)wParam;
  BOOL bAdd = (BOOL)lParam;

  DrawTimershaft(nTaskId, bAdd);

  return 0;
}

void CFileEditDialog::DrawTimershaft(int nTaskId, BOOL bAdd)
{
  if (bAdd)
  {
    CreateNewTimerShaft(nTaskId, m_strMp4Name, m_nTimerShaftCount);
  }
  else
  {
    m_Lock.Lock();

    TIMERSHAFTMAP::iterator it = m_TimerShaftMap.begin();
    for (; it != m_TimerShaftMap.end(); ++it)
    {
      const CString& strMp4 = it->first;
      if (m_strMp4Name == strMp4)
      {
        CTimerShaft* pShaft = it->second;

        if (pShaft && pShaft->GetSafeHwnd())
        {
          CRect rtRemove;
          DelTimershaftMember(pShaft, rtRemove);

          ::PostMessage(pShaft->GetSafeHwnd(), WM_REMOVEEDITTASK, NULL, NULL);

          SAFE_DELETE(pShaft);
          m_TimerShaftMap.erase(it);  
        }

        m_Lock.Unlock();

        ::InterlockedDecrement(&m_nTimerShaftCount);

        CTaskListDialog* pTaskListDlg = g_pMainFrame->m_pFileView->GetTaskListDlg();
        if (pTaskListDlg->GetDelItem() == pTaskListDlg->GetPlayItem())
        {
          //重绘播放界面
          CMediaPlayerDialog* pPlayDlg = g_pMainFrame->m_pPreView->GetMediaPlayDlg();
          if (pPlayDlg && pPlayDlg->GetSafeHwnd())
          {
            ::PostMessage(pPlayDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, WPARAM(false), NULL);
          }
        }

        break;
      }
    }

    //update file
//     g_timeRecord.Delete(m_nSelTaskId);
  }
}

void CFileEditDialog::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // TODO: Add your message handler code here
  // Do not call CDialog::OnPaint() for painting messages
}

void CFileEditDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default

  CDialog::OnLButtonDown(nFlags, point);
}


bool CFileEditDialog::CreateNewTimerShaft(int nTaskId, const CString& strMp4Name, int nRef)
{
  m_editTaskList.clear();

  const unsigned nHeight = 100;
  const unsigned nSpace = 15;

  CRect rt;
  GetClientRect(&rt);

  rt.top += 8;
  rt.left += 8;
  rt.right -= 8;
  rt.bottom = rt.top + nHeight;

  m_strMp4Name = strMp4Name;
  CTimerShaft* pShaft = new CTimerShaft(nTaskId, *this, m_Duration, 
    m_strMp4Name.GetBuffer(), nRef, m_taskNameList);
  if (!pShaft)
  {
    return false;
  }

  m_Lock.Lock();
  m_TimerShaftMap[strMp4Name] = pShaft;
  m_Lock.Unlock();

  rt.top += m_nTimerShaftCount*nHeight;
  rt.bottom = rt.top + nHeight;

  pShaft->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_BORDER, rt, 
                 this, TIMERSHAFT_BASE + m_nTimerShaftCount);
  pShaft->SetRect(rt);

  SYSTEMTIME time;
  GetLocalTime(&time);
  g_statusLog.StatusOut("CFileEditDialog::CreateNewTimerShaft %d:%d:%d:%d", 
    time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

  WaitForSingleObject(m_hReadEvent, INFINITE);

  GetLocalTime(&time);
  g_statusLog.StatusOut("CFileEditDialog::CreateNewTimerShaft %d:%d:%d:%d", 
    time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

  m_Lock.Lock();
  EDIT_TASK_LIST::const_iterator it = m_editTaskList.begin();
  for (; it != m_editTaskList.end(); ++it)
  {
    if ((*it)->m_nID == m_nSelTaskId)
    {
      pShaft->SetCuttimes((*it)->m_pCutTime, (*it)->m_nCutTimeCount);
      break;
    }
  }

  m_TimerShaftList.push_back(pShaft);
  m_Lock.Unlock();

  ::InterlockedIncrement(&m_nTimerShaftCount);
  InvalidateRect(&rt);

  return true;
}

void CFileEditDialog::DelTimershaftMember(CTimerShaft* pShaft, CRect& rtRemove)
{
  if (!pShaft)
  {
    return;
  }

  CRect rtPrev, rtCur;
  int i = 0, j = 0;

  m_Lock.Lock();

  TIMERSHARTLIST::iterator it = m_TimerShaftList.begin();
  for (int i = 0; i < m_TimerShaftList.size(), it != m_TimerShaftList.end(); ++i, ++it)
  {
    if (pShaft == m_TimerShaftList[i])
    {
      rtPrev = m_TimerShaftList[i]->GetRect();
      for (j = i + 1; j < m_TimerShaftList.size(); ++j)
      {
        rtCur = m_TimerShaftList[j]->GetRect();

        m_TimerShaftList[j]->SetRect(rtPrev);
        m_TimerShaftList[j]->MoveWindow(&rtPrev);
        rtPrev = rtCur;
      }
      break;
    }
  }

  m_TimerShaftList.erase(it);

  m_Lock.Unlock();
}


void CFileEditDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
}

void CFileEditDialog::ScrollClient(int nBar, int nPos)    
{    
  static int s_prevx = 1;    
  static int s_prevy = 1;    

  int cx = 0;    
  int cy = 0;    

  int& delta = cy;    
  int& prev = s_prevy;  

  delta = prev - nPos;    
  prev = nPos;    

  if(cx || cy)    
  {    
    ScrollWindow(cx, cy, NULL, NULL);    
  }    
}    

int CFileEditDialog::GetScrollPos(int nBar, UINT nSBCode)  
{  
  SCROLLINFO si;  
  si.cbSize = sizeof(SCROLLINFO);  
  si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;  
  GetScrollInfo(nBar, &si);  

  const int minPos = si.nMin;  
  const int maxPos = si.nMax - (si.nPage - 1);  

  int result = -1;  

  switch(nSBCode)  
  {  
  case SB_LINEUP /*SB_LINELEFT*/:  
    result = max(si.nPos - 1, minPos);  
    break;  
  case SB_LINEDOWN /*SB_LINERIGHT*/:  
    result = min(si.nPos + 1, maxPos);  
    break;  
  case SB_PAGEUP /*SB_PAGELEFT*/:  
    result = max(si.nPos - (int)si.nPage, minPos);  
    break;  
  case SB_PAGEDOWN /*SB_PAGERIGHT*/:  
    result = min(si.nPos + (int)si.nPage, maxPos);  
    break;  
  case SB_THUMBPOSITION:  
    // do nothing   
    break;  
  case SB_THUMBTRACK:  
    result = si.nTrackPos;  
    break;  
  case SB_TOP /*SB_LEFT*/:  
    result = minPos;  
    break;  
  case SB_BOTTOM /*SB_RIGHT*/:  
    result = maxPos;  
    break;  
  case SB_ENDSCROLL:  
    // do nothing   
    break;  
  }  

  return result;  
}  


void CFileEditDialog::VScrool(int nBar, UINT nSBCode)  
{  
  const int scrollPos = GetScrollPos(nBar,nSBCode);  

  if(scrollPos == -1)  
    return;  

  SetScrollPos(nBar, scrollPos, TRUE);  
  ScrollClient(nBar, scrollPos);  
}  



void CFileEditDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // TODO: Add your message handler code here and/or call default
  VScrool(SB_VERT,nSBCode);  

  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CFileEditDialog::Release()
{
  EDIT_TASK_LIST::iterator it1 = m_editTaskList.begin();
  for (; it1 != m_editTaskList.end(); ++it1)
  {
    delete [] (*it1)->m_pCutTime;
    (*it1)->m_pCutTime = NULL;
  }

  TIMERSHAFTMAP::iterator it2 = m_TimerShaftMap.begin();
//   for (; it2 != m_TimerShaftMap.end(); ++it2)
//   {
//     delete it2->second;
//     it2->second = nullptr;
//   }
  m_TimerShaftMap.clear();

  m_editTaskList.clear();
  m_taskNameList.clear();

  if (m_hReadEvent)
  {
    ::CloseHandle(m_hReadEvent);
    m_hReadEvent = INVALID_HANDLE_VALUE;
  }

  if (m_hReadNotifyEvent)
  {
    ::CloseHandle(m_hReadNotifyEvent);
    m_hReadNotifyEvent = INVALID_HANDLE_VALUE;
  }

  if (m_hReadTimeRecrod)
  {
    ::CloseHandle(m_hReadTimeRecrod);
    m_hReadTimeRecrod = INVALID_HANDLE_VALUE;
  }
}

void CFileEditDialog::RemoveTimeShaft(const CString& strTaskName)
{
  if (strTaskName.IsEmpty())
  {
    return;
  }

  m_Lock.Lock();

  TIMERSHAFTMAP::iterator it = m_TimerShaftMap.find(strTaskName);
  if (it != m_TimerShaftMap.end())
  {
    const CTimerShaft* pShaft = it->second;
    ::PostMessage(pShaft->GetSafeHwnd(), WM_REMOVEEDITTASK, NULL, NULL);

    TIMERSHARTLIST::iterator _it = m_TimerShaftList.begin();
    for (; _it != m_TimerShaftList.end(); ++_it)
    {
      if ((*_it) == pShaft)
      {
        m_TimerShaftList.erase(_it);
        break;
      }
    }

    m_TimerShaftMap.erase(it);
  }

  m_Lock.Unlock();
}

unsigned int __stdcall CFileEditDialog::ReadTimeRecordThd(void* pParam)
{
  CFileEditDialog* pThis = (CFileEditDialog*)pParam;

  SYSTEMTIME time;
  GetLocalTime(&time);
  g_statusLog.StatusOut("CFileEditDialog::ReadTimeRecordThd %d:%d:%d:%d", 
    time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

  while (!theApp.IsTerminate())
  {
    WaitForSingleObject(pThis->m_hReadNotifyEvent, INFINITE);
    g_timeRecord.Read(pThis->m_editTaskList);
    SetEvent(pThis->m_hReadEvent);
  }

  return 0;
}


void CFileEditDialog::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  g_statusLog.StatusOut("CFileEditDialog::OnClose()");
  Release();

  CDialog::OnClose();
}
