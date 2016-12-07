// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "mediaPlayer.h"

#include "MainFrm.h"
#include "ProgressCtrlView.h"
#include "FilePreView.h"
#include "PreView.h"
#include "ImagePrview.h"
#include "SAStatusLog.h"
#include "TaskListDialog.h"
#include "EditStatueFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStatusBar  CMainFrame::m_wndStatusBar;
extern CmediaPlayerApp theApp;
extern CSAStatusLog g_statusLog;
extern CMainFrame* g_pMainFrame;
extern CEditStatueFile g_editStatusFile;
extern CTimeRecord g_timeRecord;

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
  ON_WM_TIMER()
  ON_WM_CLOSE()
//   ON_UPDATE_COMMAND_UI(ID_NEW_PANE, OnUpdatePane)
  ON_MESSAGE(WM_UPDATEPANETEXT, OnUpdatePaneText)
  ON_MESSAGE(WM_OFFLINE, OnOffline)
  ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,

  ID_NEW_PANE
};

static char * UnicodeToANSI( const wchar_t* str ) 
{ 
  char* result; 
  int textlen; 
  textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL ); 
  result =(char *)malloc((textlen+1)*sizeof(char)); 
  memset( result, 0, sizeof(char) * ( textlen + 1 ) ); 
  WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL ); 
  return result; 
} 

static void Convert(const char* strIn,char* strOut, int sourceCodepage, int targetCodepage)  
{  
  int len=lstrlen(strIn);  
  int unicodeLen=MultiByteToWideChar(sourceCodepage,0,strIn,-1,NULL,0);  
  wchar_t* pUnicode;  
  pUnicode=new wchar_t[unicodeLen+1];  
  memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
  MultiByteToWideChar(sourceCodepage,0,strIn,-1,(LPWSTR)pUnicode,unicodeLen);  
  BYTE * pTargetData = NULL;  
  int targetLen=WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,0,NULL,NULL);  
  pTargetData=new BYTE[targetLen+1];  
  memset(pTargetData,0,targetLen+1);  
  WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,targetLen,NULL,NULL);  
  lstrcpy(strOut,(char*)pTargetData);  
  delete pUnicode;  
  delete pTargetData;  
}  

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
  m_bEditFinish = true;
  g_pMainFrame = this;
  m_pFileView = nullptr;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD  | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
    /*|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)*/)
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	// TODO: 如果不需要工具栏可停靠，则删除这三行
// 	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
// 	DockControlBar(&m_wndToolBar);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
  cs.style |= WS_MAXIMIZE;

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
  // TODO: Add your specialized code here and/or call the base class
//   g_pMainFrame = this;
  
  m_splitWnd1.CreateStatic(this, 1, 2);

  CRect rt;
  GetClientRect(&rt);

  BOOL bRet = m_splitWnd1.CreateView(0, 1, RUNTIME_CLASS(CProgressCtrlView), CSize(200, 300), pContext);
  m_splitWnd1.SetColumnInfo(0, 200, 300);

  m_pProgressView = (CProgressCtrlView*)(m_splitWnd1.GetPane(0, 1));

  bRet = m_splitWnd2.CreateStatic(&m_splitWnd1, 3, 1, WS_CHILD | WS_VISIBLE, m_splitWnd1.IdFromRowCol(0, 0));
  bRet = m_splitWnd2.CreateView(0, 0, RUNTIME_CLASS(CPreView), CSize(200, 300), pContext);
  bRet = m_splitWnd2.CreateView(1, 0, RUNTIME_CLASS(CFilePreView), CSize(200, 300), pContext);
  bRet = m_splitWnd2.CreateView(2, 0, RUNTIME_CLASS(CImagePrview), CSize(200, 300), pContext);
  m_splitWnd2.SetColumnInfo(0, 200, 300);

  m_pPreView = (CPreView*)(m_splitWnd2.GetPane(0, 0));
  m_pFileView = (CFilePreView*)(m_splitWnd2.GetPane(1, 0));

  return TRUE;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
  // TODO: Add your message handler code here and/or call default
  if (nIDEvent == KEEPALIVE_EVENT)
  {
    theApp.KeepAlive();
  }

  CFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  KillTimer(KEEPALIVE_EVENT);
  theApp.SetTerminate(true);

  CFrameWnd::OnClose();
}

void CMainFrame::UpdateStatueBar(const EDIT_TASK_STATUS& taskStatus)
{
//   m_wndStatusBar.SetPaneInfo(4, ID_NEW_PANE, SBPS_NORMAL, 60);

//   CSplitterWnd& split = g_pMainFrame->GetSplitterWnd2();
//   CTaskListDialog* pDlg = ((CFilePreView*)(split.GetPane(1, 0)))->GetTaskListDlg();
  CTaskListDialog* pDlg = g_pMainFrame->m_pFileView->GetTaskListDlg();

  CEditTaskListCtrl& ListCtrl = pDlg->GetListCtrl();
  int nItemCount = ListCtrl.GetItemCount();  

  CString strTaskId;
  CString strSource, strAlias;
  int i = 0;
  for (; i < nItemCount;)
  {
    strTaskId = ListCtrl.GetItemText(i, 0);
    int nTaskId = Str2Int(strTaskId);

    strSource = ListCtrl.GetTaskSourceName(nTaskId);
    strAlias = ListCtrl.GetTaskAlias(nTaskId);
    g_statusLog.StatusOut("nTaskId = %d, taskStatus.taskId = %d", nTaskId, taskStatus.taskId);
    if (nTaskId == taskStatus.taskId)
    {
      g_statusLog.StatusOut("find same task_id:%d", taskStatus.taskId);
      break;
    }

    ++i;
  }

  if (i >= nItemCount)
  {
    g_statusLog.StatusOut("task not found, item = %d, itemCount = %d", i, nItemCount);
    return;
  }

  ListCtrl.UpdataTaskStatus(Str2Int(strTaskId), taskStatus.taskStatus);

  char szBillName[MAX_PATH_LEN] = {0};
  Convert(taskStatus.billName, szBillName, CP_UTF8, CP_ACP);
  ListCtrl.SetItemText(i, 2, szBillName);

  CString strFmt;
  strFmt.Format("%d%%", taskStatus.editProgress);
  ListCtrl.SetItemText(i, 3, strFmt);
  g_statusLog.StatusOut("编辑进度 = %d", taskStatus.editProgress);

  strFmt.Empty();
  switch (taskStatus.taskStatus)
  {
  case e_task_no_exist:
    {
      strFmt = _T("id不存在");
      break;
    }

  case e_task_id_repeated:
    {
      strFmt = _T("id重复");
      break;
    }

  case e_task_in_waitting:
    {
      strFmt = _T("等待编辑");
      break;
    }

  case e_task_in_progress:
    {
      strFmt = _T("正在编辑");
      break;
    }

  case e_task_pausing:
    {
      strFmt = _T("暂停中");
      break;
    }

  case e_task_completed:
    {
      strFmt = _T("编辑完成");
      break;
    }

  case e_task_failed:
    {
      strFmt = _T("编辑失败");
      break;
    }

  default:
    break;
  }

  if (100 == taskStatus.editProgress)
  {
    strFmt = _T("编辑完成");
  }

  EDITSTATUS status;
  ZeroMemory(&status, sizeof(status));
  status.m_taskStatus = taskStatus.taskStatus;
  status.m_nTaskId = taskStatus.taskId;
  status.m_nEditProgress = taskStatus.editProgress;
  memcpy(status.m_szSourceFileName, strSource, strSource.GetLength()+sizeof(TCHAR));
  memcpy(status.m_szDestFileName, szBillName, _tcslen(szBillName));
  memcpy(status.m_szSourceAlias, strAlias, strAlias.GetLength() + sizeof(TCHAR));
  status.m_bUseFlag = 1;

  g_editStatusFile.Update(status);
  ListCtrl.SetItemText(i, 4, strFmt);

  if (taskStatus.taskStatus == e_task_completed || 100 == taskStatus.editProgress)
  {
    g_statusLog.StatusOut("start delete task");
    AutoDeleteTask(ListCtrl, taskStatus.taskId);
  }
}

void CMainFrame::OnUpdatePane(CCmdUI* pCmdUI)
{
  pCmdUI->Enable();
}

LRESULT CMainFrame::OnUpdatePaneText(WPARAM wParam, LPARAM lParam)
{
  UpdateStatueBar(*(EDIT_TASK_STATUS*)lParam);

  return 0;
}

LRESULT CMainFrame::OnOffline(WPARAM wParam, LPARAM lParam)
{
  m_wndStatusBar.SetPaneText(4, _T("连接断开"));

//   theApp.m_bConnect = false;

  return 0;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
  CFrameWnd::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
    CRect rt;
    GetClientRect(&rt);

    if (m_splitWnd1.GetSafeHwnd() && m_splitWnd2.GetSafeHwnd())
    {
      m_splitWnd1.SetColumnInfo(0, rt.Width()/6, 200);
      m_splitWnd1.RecalcLayout();

      m_splitWnd2.SetColumnInfo(0, rt.Width()/6*5, 200);
      m_splitWnd2.RecalcLayout();
    }
}

void CMainFrame::AutoDeleteTask(CEditTaskListCtrl& listCtrl, int nTaskId)
{
  if (theApp.GetEditCs().csRemoveEditTask(nTaskId))
  {
    listCtrl.DeleteTask(nTaskId);

    g_statusLog.StatusOut("CMainFrame::AutoDeleteTask");
  } 
}

