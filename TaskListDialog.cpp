// TaskListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "TaskListDialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "FileEditDialog.h"
#include "ProgressCtrlView.h"
#include "VLCPlayer.h"
#include "SAStatusLog.h"
#include "MediaPlayerDialog.h"
#include "PreView.h"
#include "EditStatueFile.h"
#include "ScopeLock.h"
#include "mediaPlayer.h"

extern CmediaPlayerApp theApp;
extern CSAStatusLog g_statusLog;
extern CEditStatueFile g_editStatusFile;
extern CTimeRecord g_timeRecord;
extern CEditStatueFile g_editStatusFile;

IMPLEMENT_DYNAMIC(CTaskListDialog, CDialogEx)

CTaskListDialog::CTaskListDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTaskListDialog::IDD, pParent)
{
//   m_strDir = _T("");
//   m_strMp4Name = _T("");
//   m_strAlias = _T("");
  m_nCurPlayItem = -1;
  m_nDelItem = -1;
  m_nFileCount = 0;
}

CTaskListDialog::~CTaskListDialog()
{
  m_TaskNameUseDict.clear();
  m_statusList.clear();
}

void CTaskListDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_TASKINFO, m_taskListCtrl);
}


BEGIN_MESSAGE_MAP(CTaskListDialog, CDialogEx)
  ON_WM_SIZE()
  ON_MESSAGE(WM_ADDMP4, OnAddMp4)
  ON_MESSAGE(WM_STARTPLAY, OnStartPlay)
//   ON_NOTIFY(NM_RCLICK, IDC_LIST_TASKINFO, &CTaskListDialog::OnNMRClickListTaskinfo)
//   ON_COMMAND_RANGE(ID_REMOVE_MP4, ID_PREVIEW_MP4, OnMP4Operation)
//   ON_NOTIFY(NM_CLICK, IDC_LIST_TASKINFO, &CTaskListDialog::OnNMClickListTaskinfo)
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTaskListDialog message handlers


BOOL CTaskListDialog::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  theApp.StartRecvThd();
  _InitListCtrl();

  if (!g_editStatusFile.IsEmpty())
  {
    CString strDir = _T("D:\\avEdit\\mp4");
    if (!g_editStatusFile.IsInit())
    {
      g_editStatusFile.Init();
    }

    g_editStatusFile.Read(m_statusList); 
    for (int i = 0; i < m_statusList.size(); ++i)
    {
      CEditTask* pNewTask = new CEditTask;
      pNewTask->SetTaskID(m_statusList[i].m_nTaskId);
      pNewTask->SetSourceName(m_statusList[i].m_szSourceFileName);
      pNewTask->SetTaskAlias(m_statusList[i].m_szSourceAlias);
      pNewTask->SetTaskStatus(m_statusList[i].m_taskStatus);
      pNewTask->SetTaskProgress(m_statusList[i].m_nEditProgress);
      pNewTask->SetDestName(m_statusList[i].m_szDestFileName);
      pNewTask->SetDir(strDir + "\\" + m_statusList[i].m_szSourceFileName);

      m_taskListCtrl.AddTask(pNewTask);
    }
  }

  AutoSize();

  _Release(/*m_statusList*/);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskListDialog::_InitListCtrl()
{
  DWORD dwStyle = m_taskListCtrl.GetExtendedStyle();
  dwStyle |= LVS_EX_FULLROWSELECT;              
  dwStyle |= LVS_EX_GRIDLINES;   
//   dwStyle |= LVS_SHOWSELALWAYS;
  m_taskListCtrl.SetExtendedStyle(dwStyle);         

  int nCol = 0;
  m_taskListCtrl.InsertColumn(nCol++, TEXT("ID"), LVCFMT_LEFT, 50);
  m_taskListCtrl.InsertColumn(nCol++, TEXT("源"), 20);
  m_taskListCtrl.InsertColumn(nCol++, TEXT("目标节目名称"), 80);
  m_taskListCtrl.InsertColumn(nCol++, TEXT("编辑进度"), 50);
  m_taskListCtrl.InsertColumn(nCol++, TEXT("编辑状态"), 50);
}

void CTaskListDialog::_SetEditStatus(int nStatus, CString& strStatus)
{
  switch (nStatus)
  {
  case e_task_no_exist:		// id不存在
    {
      strStatus = _T("任务不存在");      
      break;
    }
  case e_task_id_repeated:		// id重复
    {
      strStatus = _T("任务ID重复");
      break;
    }

  case e_task_in_waitting:		// 等待编辑
    {
      strStatus = _T("等待编辑");
      break;
    }

  case e_task_in_progress:		// 正在编辑
    {
      strStatus = _T("正在编辑");
      break;
    }

  case e_task_pausing:			// 暂停中
    {
      strStatus = _T("暂停");
      break;
    }

  case e_task_completed:		// 编辑完成
    {
      strStatus = _T("编辑完成");
      break;
    }

  case e_task_failed:			// 编辑失败
    {
      strStatus = _T("编辑失败");
      break;
    }

  default:
    strStatus = _T("");
    break;
  }
}


void CTaskListDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialogEx::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  CRect rect;
  GetClientRect(&rect);

  if (m_taskListCtrl.GetSafeHwnd())
  {
    m_taskListCtrl.MoveWindow(&rect);
  }
}


LRESULT CTaskListDialog::OnAddMp4(WPARAM wParam, LPARAM lParam)
{
//   CTaskManager* pManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
//   bool bSameMp4 = pManager->IsSameTask(m_strMp4Name);
//   if (bSameMp4)
//   {
//     return 0;
//   }
// 
//   CEditTask* pNewTask = new CEditTask;
//   pNewTask->SetSourceName(m_strMp4Name);
//   pNewTask->SetTaskAlias(m_strAlias);
//   pNewTask->SetDir(m_strDir);
// 
//   pManager->AddTask(pNewTask);
// 
//   CString strId;
//   strId.Format(_T("%d"), pNewTask->GetTaskID());
//   int nRow = m_taskListCtrl.InsertItem(m_taskListCtrl.GetItemCount(), strId);
//   m_taskListCtrl.SetItemText(nRow, 1, m_strMp4Name);
// 
//   EDITSTATUS status;
//   status.m_bUseFlag = 0;
//   status.m_nEditProgress = 0;
//   ZeroMemory(status.m_szDestFileName, sizeof(status.m_szDestFileName));
//   status.m_taskStatus = ENUM_TASK_STATUS;
//   status.m_nTaskId = pNewTask->GetTaskID();
//   _tcsncpy(status.m_szSourceFileName, pNewTask->GetSourceName(), pNewTask->GetSourceName().GetLength()+sizeof(TCHAR));
//   _tcsncpy(status.m_szSourceAlias, pNewTask->GetSourceAlias(), pNewTask->GetSourceAlias().GetLength()+sizeof(TCHAR));
//   status.m_bUseFlag = 1;
// 
//   g_editStatusFile.Write(status);

  return 0;
}

void CTaskListDialog::_SetNamePathDict(const NAME_PATH& namePath, int nItemIndex)
{
//   NAME_PATH_DICT::const_iterator it = m_namePathDict.find(m_TaskIdList[nItemIndex]);
//   if (it == m_namePathDict.end())
//   {
//     m_namePathDict[m_TaskIdList[nItemIndex]] = namePath;
//   }
}

void CTaskListDialog::AutoSize()
{
  CHeaderCtrl* pHeaderCtrl = m_taskListCtrl.GetHeaderCtrl();
  if (pHeaderCtrl)
  {
    int nCols = pHeaderCtrl->GetItemCount();
    for (int i = 0; i < nCols; ++i)
    {
      m_taskListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
    }
  }
}

bool CTaskListDialog::RemoveMp4()
{
  int nSelItem = -1;
  const int nSubItem = 1;

  CAutoLock l(m_cs);

  POSITION pos = m_taskListCtrl.GetFirstSelectedItemPosition();
  while (pos)
  {
    nSelItem = m_taskListCtrl.GetNextSelectedItem(pos);
  }

  if (nSelItem < 0)
  {
    return false;
  }

  m_nDelItem = nSelItem;

  UInt16b nID = Str2Int(m_taskListCtrl.GetItemText(nSelItem, 0));

  CTaskManager* pManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;
  if (pManager)
  {
    pManager->DelTask(nID);
  }

  CString strMp4Name;
  strMp4Name = GetMp4Name(nSelItem, nSubItem);

  CFileEditDialog* pEditDlg = GetFileEditDlg();
  if (pEditDlg)
  {
    pEditDlg->SetSelTaskId(Str2Int(m_taskListCtrl.GetItemText(nSelItem, 0)));
    ::SendMessage(pEditDlg->GetSafeHwnd(), WM_DRAWTIMERSHAFT, WPARAM(&strMp4Name), (LPARAM)FALSE);
  }

  g_timeRecord.Delete(Str2Int(m_taskListCtrl.GetItemText(nSelItem, 0)));
  g_editStatusFile.Delete(Str2Int(m_taskListCtrl.GetItemText(nSelItem, 0)));

//     AdjustRandList(nSelItem, m_TaskInfo.GetItemCount());

  CMediaPlayerDialog* pMediaPlayDlg = GetMediaPlayerDlg();
  if (pMediaPlayDlg)
  {
    pMediaPlayDlg->ResetMediaSlider();
  }

  if (m_nCurPlayItem == m_nDelItem)
  {
    CVLCPlayer::Instance()->Stop();
  }

//     DelElementFromIDNamePathDict(Str2Int(m_TaskInfo.GetItemText(nSelItem, 0)));
//     DelElementFromTaskNameUseDict(strMp4Name);
  m_taskListCtrl.DeleteItem(nSelItem);

  return true;
}


bool CTaskListDialog::ClearAllMp4()
{
//   CVLCPlayer::Instance()->Stop();
// 
//   m_bSelect = false;
//   m_nSelMp4 = -1;
//   m_nLastSelMp4 = -1;
//   m_bMp4Added = false;
//   m_strMp4Name.Empty();
// 
//   g_editStatusFile.Clear();
//   g_timeRecord.Clear();
// 
//   m_TaskNameUseDict.clear();

  return true;
}

bool CTaskListDialog::Mp4PrevView()
{
//   g_statusLog.StatusOut("CFilePrevDialog::Mp4PrevView");
// 
//   int nItem = -1;
//   const int nSubItem = 1;
//   POSITION ps = m_taskListCtrl.GetFirstSelectedItemPosition();
//   while (ps)
//   {
//     nItem = m_taskListCtrl.GetNextSelectedItem(ps);
//   }
// 
//   if (nItem != -1)
//   {
//     /*m_strMp4Name*/m_strDir = GetMp4Path(nItem, nSubItem);
//   }
// 
//   //修改播放状态
//   CMediaPlayerDialog* pPlayDlg = GetMediaPlayerDlg();
//   if (pPlayDlg)
//   {
//     pPlayDlg->SetMp4Path(m_strMp4Name);
//     ::SendMessage(pPlayDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, WPARAM(true), NULL);
//   }
// 
//   CVLCPlayer::Instance()->OpenMedia(m_strMp4Name);
//   CVLCPlayer::Instance()->Play();

  return true;
}

const CString CTaskListDialog::GetMp4Path(int nItem, int nSubItem)
{
//   m_AddMp4Lock.Lock();
// 
//   int nTaskID = Str2Int(m_TaskInfo.GetItemText(nItem, 0));
//   ID_NAMEPATH_DICT::const_iterator it = m_IdNamePathDict.find(nTaskID);
//   if (it != m_IdNamePathDict.end())
//   {
//     m_AddMp4Lock.Unlock();
//     return it->second.m_strPath;
//   }
// 
//   m_AddMp4Lock.Unlock();
  return _T("");
}

const CString CTaskListDialog::GetMp4Name(int nItem, int nSubItem)
{
  return m_taskListCtrl.GetItemText(nItem, nSubItem);
}


CFileEditDialog* CTaskListDialog::GetFileEditDlg()
{
  return ((CProgressCtrlView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd().GetPane(0, 1)))->GetEditDlg();
}

CMediaPlayerDialog* CTaskListDialog::GetMediaPlayerDlg()
{
  return ((CPreView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(0, 0)))->GetMediaPlayDlg();
}

bool CTaskListDialog::IsTimerShaftDraw(const CString& strMp4)
{
  TASKNAME_USE_DICT::iterator it = m_TaskNameUseDict.find(strMp4);
  BYTE bUse = 0;
  if (it != m_TaskNameUseDict.end())
  {
    bUse = it->second;
  }

  return bUse == 1 ? true : false;   //true已经绘制，false没有绘制
}

void CTaskListDialog::DelElementFromTaskNameUseDict(const CString& strMp4Name)
{
  TASKNAME_USE_DICT::iterator it = m_TaskNameUseDict.begin();
  for (; it != m_TaskNameUseDict.end(); ++it)
  {
    if (it->first == strMp4Name)
    {
      m_TaskNameUseDict.erase(it);
      break;
    }
  }
}

void CTaskListDialog::DelElementFromIDNamePathDict(int nTaskId)
{
//   ID_NAMEPATH_DICT::iterator it = m_IdNamePathDict.find(nTaskId);
//   if (it != m_IdNamePathDict.end())
//   {
//     m_IdNamePathDict.erase(it);
//   }
}


int CTaskListDialog::GetActiveTaskId()
{
  CString strId = m_taskListCtrl.GetItemText(0, 0); /*m_taskListCtrl.GetItemText(m_nSelMp4, 0);*/

  return _ttoi(strId);
}

void CTaskListDialog::_Release(/*EDITSTATUSLIST& list*/)
{
  m_statusList.clear();
}

LRESULT CTaskListDialog::OnStartPlay(WPARAM wParam, LPARAM lParam)
{
  CVLCPlayer::Instance()->Play();

  return 0;
}


void CTaskListDialog::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  g_statusLog.StatusOut("CTaskListDialog::OnClose()");
  _Release();

  CDialogEx::OnClose();
}


// void CTaskListDialog::OnNMRClickListTaskinfo(NMHDR *pNMHDR, LRESULT *pResult)
// {
//   LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//   // TODO: Add your control notification handler code here
//   if (pNMItemActivate->iItem != -1)
//   {
//     DWORD dwPos = GetMessagePos();
//     CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
// 
//     CMenu menu;
//     menu.CreatePopupMenu();
//     CString str;
// 
//     str.Empty();
//     str.LoadString(ID_REMOVE_MP4);
//     BOOL bRet = menu.AppendMenu(MF_STRING, ID_REMOVE_MP4, str);
// 
//     str.Empty();
//     str.LoadString(ID_CLEARALL_MP4);
//     bRet = menu.AppendMenu(MF_STRING, ID_CLEARALL_MP4, str);
// 
//     str.Empty();
//     str.LoadString(ID_PREVIEW_MP4);
//     bRet = menu.AppendMenu(MF_STRING, ID_PREVIEW_MP4, str);
// 
//     CPoint pt;
//     GetCursorPos(&pt);
//     menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
// 
//     menu.DestroyMenu();
//   }
// 
//   *pResult = 0;
// }


// void CTaskListDialog::OnMP4Operation(UINT nID)
// {
//   switch (nID)
//   {
//   case ID_REMOVE_MP4:
//     {
//       RemoveMp4();
//       break;
//     }
// 
//   case ID_CLEARALL_MP4:
//     {
//       ClearAllMp4();
//       break;
//     }
// 
//   default:
//     {
//       Mp4PrevView();
//       break;
//     }
//   }
// }

// bool CTaskListDialog::AddMp4()
// {
//   static TCHAR BASED_CODE szFilter[] = _T("mp4 (*.mp4)|*.mp4|")_T("All Files (*.*)|*.*||");
// 
//   CFileDialog dlg(TRUE, NULL, NULL,
//     OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_HIDEREADONLY,
//     szFilter, this);
// 
//   dlg.m_ofn.nMaxFile = 20 * MAX_PATH;  //open 20 files
//   TCHAR* pszFile = new TCHAR[dlg.m_ofn.nMaxFile];
// 
//   if (pszFile)
//   {
//     dlg.m_ofn.lpstrFile = pszFile;
//     ZeroMemory(dlg.m_ofn.lpstrFile, dlg.m_ofn.nMaxFile * sizeof(TCHAR));
//   }
// 
//   CString strPath, strFileName;
//   SHFILEINFO stFileInfo = { 0 };
// 
//   if (IDOK == dlg.DoModal())
//   {
//     //get first file positon
//     POSITION ps;
//     ps = dlg.GetStartPosition();
//     HICON hIconLarge = NULL, hIconSmall = NULL;
// 
//     while (ps != NULL)
//     {
//       strPath = dlg.GetNextPathName(ps);
//       int nPos = strPath.ReverseFind(_T('\\'));
//       TCHAR* pPath = strPath.GetBuffer() + nPos + sizeof(TCHAR);
// 
//       //get file name
//       while (true)
//       {
//         if (*pPath == _T('\0'))
//         {
//           break;
//         }
// 
//         strFileName += *(pPath++);
//       }
//       strFileName += _T('\0');
// 
//       //check whether add the same file
//       bool bSameFile = false;
//       if (m_filePathAry.size() != 0)
//       {
//         std::vector<CString>::iterator it = m_filePathAry.begin();
// 
//         for (; it != m_filePathAry.end(); ++it)
//         {
//           if (*it == strPath)
//           {
//             bSameFile = true;
//             continue;
//           }
//         }
//       }
// 
//       if (!bSameFile)
//       {
// //         m_filePathAry.push_back(strPath);
// //         m_fileNameAry.push_back(strFileName);
// 
//         strPath.Empty();
//         strFileName.Empty();
//       }          
//     }
// 
//     PostMessage(WM_SETFILEICON);
//     m_bMp4Added = true;
// 
//     delete [] pszFile;
//     pszFile = NULL;
// 
//     return true;
//   }
// 
//   delete [] pszFile;
//   pszFile = NULL;
// 
//   return false;
// }

