// EditTaskListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "EditTaskListCtrl.h"
#include "SAStatusLog.h"
#include "EditStatueFile.h"
#include "ProgressCtrlView.h"
#include "MainFrm.h"
#include "PreView.h"
#include "FileEditDialog.h"
#include "MediaPlayerDialog.h"
#include "ScopeLock.h"
#include "VLCPlayer.h"
#include "mediaPlayer.h"
#include "EditTask.h"
#include "URL.h"
#include "TaskListDialog.h"



extern CSAStatusLog g_statusLog;
extern CEditStatueFile g_editStatusFile;
extern CTimeRecord g_timeRecord;
extern CMainFrame* g_pMainFrame;

// CEditTaskListCtrl

IMPLEMENT_DYNAMIC(CEditTaskListCtrl, CListCtrl)

unsigned int __stdcall CEditTaskListCtrl::DrawThread(void* pParam)
{
  CEditTaskListCtrl* pThis = (CEditTaskListCtrl*)pParam;
  if (pThis)
  {
    pThis->Main(pThis->m_pEditDlg);
  }

  return 0;
}


CEditTaskListCtrl::CEditTaskListCtrl()
{
  m_nTaskID = -1;
//   m_strTaskName = _T("");
//   m_strTaskAlias = _T("");
//   m_strDir = _T("");
  m_nCurPlayItem = -1;

  m_hTimeShaftThd = INVALID_HANDLE_VALUE;
}

CEditTaskListCtrl::~CEditTaskListCtrl()
{
  if (m_hTimeShaftThd)
  {
    CloseHandle(m_hTimeShaftThd);
    m_hTimeShaftThd = INVALID_HANDLE_VALUE;
  }
}


BEGIN_MESSAGE_MAP(CEditTaskListCtrl, CListCtrl)
  ON_COMMAND_RANGE(ID_REMOVE_MP4, ID_PAUSE_EDITTASK, OnMP4Operation)
  ON_NOTIFY_REFLECT(NM_CLICK, &CEditTaskListCtrl::OnNMClick)
  ON_NOTIFY_REFLECT(NM_RCLICK, &CEditTaskListCtrl::OnNMRClick)
  ON_MESSAGE(WM_ADDMP4, OnAddMp4)
  ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CEditTaskListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()



// CEditTaskListCtrl message handlers


UInt16b CEditTaskListCtrl::GetTaskId() const
{
  return m_nTaskID;
}

// const CString& CEditTaskListCtrl::GetTaskName() const
// {
//   return m_strTaskName;
// }
// 
// const CString& CEditTaskListCtrl::GetTaskAlias() const
// {
//   return m_strTaskAlias;
// }

const CString& CEditTaskListCtrl::GetTaskDir() const
{
  return m_strDir;
}

void CEditTaskListCtrl::OnMP4Operation(UINT nID)
{
  switch (nID)
  {
  case ID_REMOVE_MP4:
    {
      RemoveMp4();
      break;
    }

  case ID_CLEARALL_MP4:
    {
      ClearAllMp4();
      break;
    }

  case ID_RESUME_EDITTASK:
    {
      break;
    }

  case ID_PAUSE_EDITTASK:
    {
      break;
    }

  default:
    {
      break;
    }
  }
}

BOOL CEditTaskListCtrl::RemoveMp4()
{
  POSITION pos = GetFirstSelectedItemPosition();
  if (!pos)
  {
    return FALSE;
  }

  int nSelItem = -1;
  while (pos)
  {
    nSelItem = GetNextSelectedItem(pos);
  }

  UInt16b nTaskId = (UInt16b)Str2Int(GetItemText(nSelItem, 0));
  CTaskManager* pTaskManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
  UInt16b nTaskStatus = pTaskManager->GetTask(nTaskId)->GetTaskStatus();
  if (nTaskStatus == e_task_in_progress || nTaskStatus == e_task_in_waitting)
  {
    return FALSE;
  }

  CString strMp4Name = pTaskManager->GetTask(nTaskId)->GetSourceName();
  CFileEditDialog* pEditDlg = GetFileEditDlg();
  if (pEditDlg)
  {
    pEditDlg->SetSelTaskId(nTaskId);
    pEditDlg->SetCurMp4Name(strMp4Name);
    ::PostMessage(pEditDlg->GetSafeHwnd(), WM_DRAWTIMERSHAFT, WPARAM(pEditDlg->GetTaskID()), (LPARAM)FALSE);
  }

  g_timeRecord.Delete(nTaskId);
  g_editStatusFile.Delete(nTaskId);

  CMediaPlayerDialog* pMediaPlayDlg = GetMediaPlayerDlg();
  if (pMediaPlayDlg)
  {
    pMediaPlayDlg->ResetMediaSlider();
  }

  if (m_nCurPlayItem == nSelItem)
  {
    CVLCPlayer::Instance()->Stop();
  }

  pTaskManager->DelTask(nTaskId);

  return DeleteItem(nSelItem);
}

BOOL CEditTaskListCtrl::ClearAllMp4()
{
  CVLCPlayer::Instance()->Stop();
  CTaskManager* pTaskManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
  if (pTaskManager)
  {
    pTaskManager->RemoveAllTask();
  }

  g_editStatusFile.Clear();
  g_timeRecord.Clear();
  m_taskUseDict.clear();

  return FALSE;
}

BOOL CEditTaskListCtrl::Mp4PrevView()
{
  POSITION pos = GetFirstSelectedItemPosition();
  if (!pos)
  {
    return FALSE;
  }

  int nSelItem = -1;
  while (pos)
  {
    nSelItem = GetNextSelectedItem(pos);
  }

  g_statusLog.StatusOut("CFilePrevDialog::Mp4PrevView");

  int nItem = -1;
  const int nSubItem = 1;
  POSITION ps = GetFirstSelectedItemPosition();
  while (ps)
  {
    nItem = GetNextSelectedItem(ps);
  }

  if (-1 == nItem)
  {
    return FALSE;
  }

  UInt16b nTaskId = (UInt16b)Str2Int(GetItemText(nSelItem, 0));
  CTaskManager* pManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
  CString strDir = pManager->GetTask(nTaskId)->GetDir();

  //修改播放状态
  CMediaPlayerDialog* pPlayDlg = GetMediaPlayerDlg();
  if (pPlayDlg)
  {
    pPlayDlg->SetMp4Path(strDir);
    ::PostMessage(pPlayDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, WPARAM(true), NULL);
  }

  CVLCPlayer::Instance()->OpenMedia(m_strDir);
  CVLCPlayer::Instance()->Play();

  return FALSE;
}

BOOL CEditTaskListCtrl::ResumeEditTask()
{  
  return theApp.GetEditCs().csSendEditTaskCmd(m_nTaskID, e_edit_task_pause);
}

BOOL CEditTaskListCtrl::PauseEditTask()
{
  return theApp.GetEditCs().csSendEditTaskCmd(m_nTaskID, e_edit_task_pause);
}

CFileEditDialog* CEditTaskListCtrl::GetFileEditDlg()
{
  CMainFrame* pMainFrame = (CMainFrame*)theApp.GetMainWnd();
  CSplitterWnd& splitWnd = pMainFrame->GetSplitterWnd();

  CProgressCtrlView* pView = g_pMainFrame->m_pProgressView;
//   CProgressCtrlView* pView = (CProgressCtrlView*)splitWnd.GetPane(0, 1);
  CFileEditDialog* pEditDlg = pView->GetEditDlg();

  return pEditDlg;

//   return ((CProgressCtrlView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd().GetPane(0, 1)))->GetEditDlg();
}

CMediaPlayerDialog* CEditTaskListCtrl::GetMediaPlayerDlg()
{
  CMainFrame* pMainFrame = (CMainFrame*)theApp.GetMainWnd();
  CSplitterWnd& splitWnd = pMainFrame->GetSplitterWnd2();

  CPreView* pView = g_pMainFrame->m_pPreView;
  CMediaPlayerDialog* pMediaDlg = pView->GetMediaPlayDlg();

  return pMediaDlg;

//   return ((CPreView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(0, 0)))->GetMediaPlayDlg();
}

bool CEditTaskListCtrl::IsTimerShaftDraw(UInt16b nTaskID)
{
  TASK_USE_DICT::iterator it = m_taskUseDict.find(nTaskID);
  BYTE bUse = 0;
  if (it != m_taskUseDict.end())
  {
    bUse = it->second;
  }

  return bUse == 1 ? true : false;   //true已经绘制，false没有绘制
}

static bool operator!= (const SYSTEMTIME& time1, const SYSTEMTIME& time2)
{
  return (time1.wHour != time1.wHour || time1.wMinute != time2.wMinute
    || time1.wSecond != time2.wSecond);
}

void CEditTaskListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // TODO: Add your control notification handler code here
  int nItem = -1;
  const int nSubItem = 1;
  int nTaskId = -1;

  SYSTEMTIME now;
  ZeroMemory(&now, sizeof(now));
  static SYSTEMTIME lastTime = now;
  GetLocalTime(&now);

  int nSubTime = (now.wHour*3600+now.wMinute*60+now.wSecond) - 
                  (lastTime.wHour*3600+lastTime.wMinute*60+lastTime.wSecond);
  if (lastTime != now && nSubTime >= 5)
  {
    memcpy(&lastTime, &now, sizeof(SYSTEMTIME));
  }
  else
  {
    return;
  }

  CAutoLock lock(m_cs);

  POSITION ps = GetFirstSelectedItemPosition();
  while (ps) 
  {
    nItem = GetNextSelectedItem(ps);
  }

  if (nItem == -1)
  {
    return;   //强制间隔5秒点一次，否则VLC库容易发生死锁（回调HandleVLCEvents）
  }

  m_nCurPlayItem = nItem;
  m_nTaskID = nTaskId = Str2Int(GetItemText(nItem, 0));

  CTaskManager* pManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;

  CVLCPlayer::Instance()->Stop();
  CVLCPlayer::Instance()->OpenMedia(pManager->GetTaskDir(nTaskId));

  int64_t time = CVLCPlayer::Instance()->GetDuration();
  
  CFileEditDialog* pEditDlg = GetFileEditDlg();
  pEditDlg->SetCurMp4Name(pManager->GetTaskName(nTaskId));
  pEditDlg->SetSelTaskId(nTaskId);

  m_pEditDlg = pEditDlg;

  if (pEditDlg && !IsTimerShaftDraw(nTaskId))
  {
    pEditDlg->SetDuration(time);
    ::PostMessage(pEditDlg->GetSafeHwnd(), WM_DRAWTIMERSHAFT, (WPARAM)nTaskId, (LPARAM)TRUE);

    m_taskUseDict[nTaskId] = (BYTE)1;
  }

  //修改播放状态
  CMediaPlayerDialog* pPlayDlg = GetMediaPlayerDlg();
  if (pPlayDlg)
  {
    pPlayDlg->SetMp4Path(pManager->GetTaskDir(nTaskId));
    ::SendMessage(pPlayDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, WPARAM(true), NULL);
  }

  CVLCPlayer::Instance()->Play();

  *pResult = 0;
}

void CEditTaskListCtrl::Main(CFileEditDialog* pDlg)
{
  ::PostMessage(pDlg->GetSafeHwnd(), WM_DRAWTIMERSHAFT, (WPARAM)pDlg->GetTaskID(), (LPARAM)TRUE);
}

void CEditTaskListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // TODO: Add your control notification handler code here
  CMenu menu;
  VERIFY(menu.CreatePopupMenu());

  CString str;

  if (GetItemCount() != 0)
  {
    int nItem = -1;
    POSITION pos = GetFirstSelectedItemPosition();
    while (pos)
    {
      nItem = GetNextSelectedItem(pos);
    }

    int nTaskId = Str2Int(GetItemText(nItem, 0));

    CTaskManager* pTaskManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
    CEditTask* pTask = const_cast<CEditTask*>(pTaskManager->GetTask(nTaskId));

    str.Empty();
    if (pTask->GetTaskStatus() != e_task_in_progress)
    {
      str.LoadString(ID_REMOVE_MP4);
      BOOL bRet = menu.AppendMenu(MF_STRING, ID_REMOVE_MP4, str);
    }

//     str.Empty();
//     str.LoadString(ID_CLEARALL_MP4);
//     bRet = menu.AppendMenu(MF_STRING, ID_CLEARALL_MP4, str);
// 
//     str.Empty();
//     str.LoadString(ID_PREVIEW_MP4);
//     bRet = menu.AppendMenu(MF_STRING, ID_PREVIEW_MP4, str);

//     str.Empty();    
//     if (e_task_pausing == pTask->GetTaskStatus())
//     {
//       str.LoadString(IDS_RESUME_EDITTASK);
//       menu.AppendMenu(MF_STRING, ID_RESUME_EDITTASK, str);
//     }
//     else if (e_task_in_progress == pTask->GetTaskStatus())
//     {
//       str.LoadString(IDS_PAUSE_EDITTASK);
//       menu.AppendMenu(MF_STRING, ID_PAUSE_EDITTASK, str);
//     }

    CPoint point;
    GetCursorPos(&point);
    menu.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);

    menu.DestroyMenu();
  }

  *pResult = 0;
}

LRESULT CEditTaskListCtrl::OnAddMp4(WPARAM wParam, LPARAM lParam)
{
  CAutoLock lock(m_cs);

  CTaskManager* pManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
  _LoadAllTask(pManager);
  BOOL bSameMp4 = pManager->IsSameTask(CURL::GetTaskName());
  if (bSameMp4)
  {
    return 0;
  }

  CEditTask* pNewTask = new CEditTask;
  pNewTask->SetSourceName(CURL::GetTaskName());
  pNewTask->SetTaskAlias(CURL::GetAlias());
  pNewTask->SetDir(m_strDir);
  pNewTask->SetTaskID(pManager->AllocTaskID());

  pManager->AddTask(pNewTask);

  CString strId;
  strId.Format(_T("%d"), pNewTask->GetTaskID());
  int nRow = InsertItem(GetItemCount(), strId);
  SetItemText(nRow, 0, strId);
  SetItemText(nRow, 1, pNewTask->GetSourceAlias());
  SetItemText(nRow, 2, pNewTask->GetDestName());

  CString strTemp;
  strTemp.Format(_T("%d%%"), pNewTask->GetTaskProgress());
  SetItemText(nRow, 3, strTemp);

  CString strStatus;
  switch ((TASK_STATUS)(pNewTask->GetTaskStatus()))
  {
  case e_task_no_exist:
    {
      strStatus = _T("id不存在");
      break;
    }

  case e_task_id_repeated:
    {
      strStatus = _T("id重复");
      break;
    }

  case e_task_in_waitting:
    {
      strStatus = _T("等待编辑");
      break;
    }

  case e_task_in_progress:
    {
      strStatus = _T("正在编辑");
      break;
    }

  case e_task_pausing:
    {
      strStatus = _T("暂停中");
      break;
    }

  case e_task_completed:
    {
      strStatus = _T("编辑完成");
      break;
    }

  case e_task_failed:
    {
      strStatus = _T("编辑失败");
      break;
    }

  default:
    strStatus = _T("");
    break;
  }

  SetItemText(nRow, 4, strStatus);

  EDITSTATUS status;
  ZeroMemory(&status, sizeof(status));
//   status.m_bUseFlag = 0;
//   status.m_nEditProgress = 0;
//   ZeroMemory(status.m_szSourceFileName, sizeof(status.m_szSourceFileName));
//   ZeroMemory(status.m_szSourceAlias, sizeof(status.m_szSourceAlias));
//   ZeroMemory(status.m_szDestFileName, sizeof(status.m_szDestFileName));

  status.m_taskStatus = ENUM_TASK_STATUS;
  status.m_nTaskId = pNewTask->GetTaskID();
  _tcsncpy(status.m_szSourceFileName, pNewTask->GetSourceName(), 
    pNewTask->GetSourceName().GetLength()+sizeof(TCHAR));
  _tcsncpy(status.m_szSourceAlias, pNewTask->GetSourceAlias(), 
    pNewTask->GetSourceAlias().GetLength()+sizeof(TCHAR));
  status.m_bUseFlag = 1;

  g_editStatusFile.Write(status);

  return 0;
}

void CEditTaskListCtrl::AddTask(const CEditTask* pTask)
{
  if (!pTask)
  {
    return;
  }

  CTaskManager* pManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
  BOOL bSameMp4 = pManager->IsSameTask(CURL::GetTaskName());
  if (bSameMp4)
  {
    return;
  }

  pManager->AddTask(const_cast<CEditTask*>(pTask));

  CString strTemp;
  strTemp.Format(_T("%d"), pTask->GetTaskID());
  int nRow = InsertItem(GetItemCount(), strTemp);
  SetItemText(nRow, 0, strTemp);

  SetItemText(nRow, 1, pTask->GetSourceAlias());
  SetItemText(nRow, 2, pTask->GetDestName());

  strTemp.Format(_T("%d%%"), pTask->GetTaskProgress());
  SetItemText(nRow, 3, strTemp);

  CString strStatus;
  switch ((TASK_STATUS)(pTask->GetTaskStatus()))
  {
  case e_task_no_exist:
    {
      strStatus = _T("id不存在");
      break;
    }

  case e_task_id_repeated:
    {
      strStatus = _T("id重复");
      break;
    }

  case e_task_in_waitting:
    {
      strStatus = _T("等待编辑");
      break;
    }

  case e_task_in_progress:
    {
      strStatus = _T("正在编辑");
      break;
    }

  case e_task_pausing:
    {
      strStatus = _T("暂停中");
      break;
    }

  case e_task_completed:
    {
      strStatus = _T("编辑完成");
      break;
    }

  case e_task_failed:
    {
      strStatus = _T("编辑失败");
      break;
    }

  default:
    strStatus = _T("");
    break;
  }

  SetItemText(nRow, 4, strStatus);
}

void CEditTaskListCtrl::_LoadAllTask(/*const*/ CTaskManager* pManager)
{
  if (!pManager)
  {
    return;
  }

  this->DeleteAllItems();
  int nCount = GetItemCount();

  TASKLISTDICT& dict = pManager->GetTaskListDict();
  TASKLISTDICT::const_iterator it = dict.begin();
  for (; it != dict.end(); ++it)
  {
    CEditTask* pTask = it->second;

    CString strTemp;
    strTemp.Format(_T("%d"), pTask->GetTaskID());
    int nRow = InsertItem(GetItemCount(), strTemp);

    SetItemText(nRow, 0, strTemp);

    SetItemText(nRow, 1, pTask->GetSourceAlias());
    SetItemText(nRow, 2, pTask->GetDestName());

    strTemp.Format(_T("%d%%"), pTask->GetTaskProgress());
    SetItemText(nRow, 3, strTemp);

    CString strStatus;
    switch ((TASK_STATUS)(pTask->GetTaskStatus()))
    {
    case e_task_no_exist:
      {
        strStatus = _T("id不存在");
        break;
      }

    case e_task_id_repeated:
      {
        strStatus = _T("id重复");
        break;
      }

    case e_task_in_waitting:
      {
        strStatus = _T("等待编辑");
        break;
      }

    case e_task_in_progress:
      {
        strStatus = _T("正在编辑");
        break;
      }

    case e_task_pausing:
      {
        strStatus = _T("暂停中");
        break;
      }

    case e_task_completed:
      {
        strStatus = _T("编辑完成");
        break;
      }

    case e_task_failed:
      {
        strStatus = _T("编辑失败");
        break;
      }

    default:
      strStatus = _T("");
      break;
    }

    SetItemText(nRow, 4, strStatus);
  }
}
extern CmediaPlayerApp theApp;
const CString& CEditTaskListCtrl::GetTaskSourceName(int nTaskId)
{
  CTaskManager* pManager = ((CmediaPlayerApp*)&theApp)->m_pTaskManager;
  const CEditTask* pTask = pManager->GetTask(nTaskId);

  return pTask->GetSourceName();
}

const CString& CEditTaskListCtrl::GetTaskAlias(int nTaskId)
{
  CTaskManager* pManager = ((CmediaPlayerApp*)&theApp)->m_pTaskManager;
  const CEditTask* pTask = pManager->GetTask(nTaskId);

  return pTask->GetSourceAlias();
}

const CString& CEditTaskListCtrl::GetTaskDir(int nTaskId)
{
  CTaskManager* pManager = ((CmediaPlayerApp*)&theApp)->m_pTaskManager;
  const CEditTask* pTask = pManager->GetTask(nTaskId);

  return pTask->GetDir();
}

BOOL CEditTaskListCtrl::DeleteTask(int nTaskId)
{
  CAutoLock lock(m_cs);

  BOOL bFindTask = FALSE;

  int nItemCount = GetItemCount();
  int i = 0;
  for (; i < nItemCount; ++i)
  {
    if (nTaskId == (UInt16b)Str2Int(GetItemText(i, 0)))
    {
      bFindTask = TRUE;
      break;
    }
  }

  if (!bFindTask)
  {
    g_statusLog.StatusOut("CEditTaskListCtrl::DeleteTask, task not found");
    return FALSE;
  }

  CTaskManager* pManager = ((CmediaPlayerApp*)&theApp)->m_pTaskManager;
  if (!pManager)
  {
    return FALSE;
  }

//   CString strMp4Name = pTaskManager->GetTask(nTaskId)->GetSourceName();
  CFileEditDialog* pEditDlg = GetFileEditDlg();
  if (pEditDlg && pEditDlg->GetSafeHwnd())
  {
    pEditDlg->SetSelTaskId(nTaskId);
    ::PostMessage(pEditDlg->GetSafeHwnd(), WM_DRAWTIMERSHAFT, WPARAM(nTaskId), (LPARAM)FALSE);
  }

  g_timeRecord.Delete(nTaskId);
  g_editStatusFile.Delete(nTaskId);

  CMediaPlayerDialog* pMediaPlayDlg = GetMediaPlayerDlg();
  if (pMediaPlayDlg && pMediaPlayDlg->GetSafeHwnd())
  {
    pMediaPlayDlg->ResetMediaSlider();
  }

  pManager->DelTask(nTaskId);

  DeleteItem(i);

  if (m_nCurPlayItem == i)
  {
    CVLCPlayer::Instance()->Stop();
  }

  return TRUE;
}

void CEditTaskListCtrl::UpdataTaskStatus(UInt16b nTaskId, UInt16b nTaskStatus)
{
  CTaskManager* pManager = ((CmediaPlayerApp*)&theApp)->m_pTaskManager;
  CEditTask* pTask = const_cast<CEditTask*>(pManager->GetTask(nTaskId));
  pTask->SetTaskStatus(nTaskStatus);
}

void CEditTaskListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  // TODO: Add your control notification handler code here
  *pResult = 0;

  NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

  // Take the default processing unless we set this to something else below.
  *pResult = CDRF_DODEFAULT;
  // First thing - check the draw stage. If it's the control's prepaint
  // stage, then tell Windows we want messages for every item.
  if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    *pResult = CDRF_NOTIFYITEMDRAW;
  }
  else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    // This is the notification message for an item.  We'll request
    // notifications before each subitem's prepaint stage.

    *pResult = CDRF_NOTIFYSUBITEMDRAW;
  }
  else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
  {

    COLORREF clrNewTextColor, clrNewBkColor;

    int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

    BOOL bSelect = FALSE;
    POSITION pos = GetFirstSelectedItemPosition(); 
    while(pos) 
    { 
      int index = GetNextSelectedItem(pos); 
      if(index==nItem) 
      { 
        bSelect = TRUE; 
        break; 
      } 
    }     
    if(bSelect)
    {
      //clrNewTextColor = RGB(255,0,0);     //Set the text to red
      clrNewTextColor = RGB(0,255,0);     //Set the text to red
      clrNewBkColor = RGB(0,0,255);     //Set the bkgrnd color to blue
    }
    else
    {    
      clrNewTextColor = RGB(0,0,0);     //Leave the text black
      clrNewBkColor = RGB(255,255,255);    //leave the bkgrnd color white
    }

    pLVCD->clrText = clrNewTextColor;
    pLVCD->clrTextBk = clrNewBkColor;


    // Tell Windows to paint the control itself.
    *pResult = CDRF_DODEFAULT;

  }
}

//焦点发生变化时树型控件自绘（选中行一直高亮）
// void CEditTaskListCtrl::OnNMCustomdrawTree1(NMHDR *pNMHDR, LRESULT *pResult)
// {
//   LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//   // TODO: 在此添加控件通知处理程序代码
//   *pResult = 0;
// 
//   LPNMTVCUSTOMDRAW pDraw=(LPNMTVCUSTOMDRAW)pNMHDR;
//   DWORD dwDrawStage=pDraw->nmcd.dwDrawStage;
//   UINT uItemState =pDraw->nmcd.uItemState;
//   //
//   *pResult=CDRF_NOTIFYITEMDRAW;
//   //|CDRF_NOTIFYPOSTPAINT|CDRF_NOTIFYSUBITEMDRAW|CDRF_NOTIFYPOSTERASE;
//   CDC* pdc=CDC::FromHandle(pDraw->nmcd.hdc);
//   CRect rc;
//   HTREEITEM hItem=(HTREEITEM) pDraw->nmcd.dwItemSpec;
//   m_tree.GetItemRect(hItem,&rc,TRUE);//FALSE);text only
//   CString txt=m_tree.GetItemText(hItem);
//   if((dwDrawStage & CDDS_ITEM) && (uItemState & CDIS_SELECTED))
//   {// 
//     pdc->FillSolidRect(&rc,RGB(49,106,197));//clr);
//     //
//     pdc->SetTextColor(RGB(255,255,255));//white
//     pdc->SetBkColor(RGB(49,106,197));//clr);
//     //
// 
//     CFont* pfnt=pdc->GetCurrentFont();
//     //
//     pdc->TextOut(rc.left+2,rc.top+2,txt);
//     pdc->SelectObject(pfnt);
// 
//     *pResult |= CDRF_SKIPDEFAULT;
//     // afxDump << "1\n";
//   }
//   else // without these ,1st blue !
//   {
//     pdc->FillSolidRect(&rc, GetSysColor(COLOR_WINDOW));
//     pdc->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
//     pdc->TextOut(rc.left+2, rc.top+2, txt);
//     // afxDump << "2\n";
//   }
// }
