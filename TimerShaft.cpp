#include "StdAfx.h"
#include "TimerShaft.h"
#include "FileEditDialog.h"
#include "MediaPlayerDialog.h"
#include "MainFrm.h"
#include "PreView.h"
#include "mediaPlayer.h"
#include "FilePreView.h"
#include "TaskListDialog.h"
#include "Ini.h"
#include "ImagePreviewDialog.h"
#include "ImagePrview.h"
#include "TimeEdit.h"
#include "SettingDialog.h"
#include "VLCPlayer.h"
#include "SAStatusLog.h"
#include "MainFrm.h"
#include "TaskListDialog.h"
#include "EditStatueFile.h"
#include "URL.h"
#include <stdlib.h>
#include <afxinet.h>


extern CmediaPlayerApp theApp;
// extern CString g_strUploadURL;
extern CSAStatusLog g_statusLog;
extern CMainFrame* g_pMainFrame;
extern CTimeRecord g_timeRecord;
extern CEditStatueFile g_editStatusFile;

#define MAX_DIR_LEN 512
#define BKCOLOR RGB(240, 240, 240)
#define CUTCOLOR RGB(60, 160, 60)
#define CUTCOLOR_RED RGB(255, 0, 0)
#define TORANCE 3
#define UPLOAD_PORT 10380

CTimerShaft::CTimerShaft(int nTaskId, CFileEditDialog& dlg, int64_t seconds, 
  const char* pFileName, int nRef, std::vector<CString>& taskNameList)
  :m_strFileName(pFileName), 
  m_rt(0, 0, 0, 0),
  m_Editdlg(dlg),
  m_ptBegin(0, 0),
  m_ptEnd(0, 0),
  m_ptEndTemp(0, 0),
  m_PtSel(0, 0),
  m_taskNameListRef(taskNameList)
{
  m_nTaskID = nTaskId;
  m_nRef = nRef;
  m_Seconds = seconds;
  m_nScaleRate = 0;
  m_nSpace = 0;
  m_bLBtnDown = false;
  m_pPopupMenu.reset(new CMenu);
  m_pLock.reset(new CSingleLock(&m_Mutex));
  m_rtList.empty();
  m_pCutTime = NULL;
  m_nFileArySize = 0;

  TCHAR szUserIP[MAX_PATH] = {'\0'};
  CIni::ReadString("hostname", "userIP", szUserIP, MAX_PATH, "server.ini");
  m_strIP = szUserIP;
  m_bReEditHead = false;
  m_bReEditTail = false;

  m_pPosTimeEditMap = new POS_TIMEEDIT_MAP;
  m_pPointTimeEditList = new POINTTIMEEDITLIST;

  m_nPosIndex = -1;
  m_pNewTimeEdit = nullptr;
  m_pSelTimeEdit = nullptr;
  m_pSelCutTime = nullptr;
  m_nSelTimeEdit = -1;
  memset(&m_PointTimeEdit, 0, sizeof(m_PointTimeEdit));
  m_pCutTimePt = nullptr;
  m_pCutTimePtList = new CUTTIMEPOINTLIST;
  m_nSelRect = -1;
  m_bValidArea = false;
  m_bValidHeadPoint = true;
  m_bValidTailPoint = true;

  //   m_pFtpConnection = nullptr;
  //   m_pInetsession = nullptr;

  memset(&m_EditTask.editOpts, 0, sizeof(m_EditTask.editOpts));
  m_EditTask.cutTimes.clear();
  //   m_hUpLoad = INVALID_HANDLE_VALUE;
  //   m_hUpLoadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  m_strAppName = AfxGetAppName();
  m_bUpLoadSuccess = FALSE;
  m_bTaskAdd = FALSE;
  m_bPauseEdit = FALSE;

  m_bTaskHasAdd = false;
}

CTimerShaft::~CTimerShaft(void)
{
  m_pPopupMenu->DestroyMenu();

  ReleaseResource();
  SAFE_DELETE(m_pCutTimePtList);

  CS_CUT_TIME_LIST::iterator it = m_EditTask.cutTimes.begin();
  for (; it != m_EditTask.cutTimes.end(); ++it)
  {
    SAFE_DELETE(*it);
  }
  m_EditTask.cutTimes.clear();
}

BEGIN_MESSAGE_MAP(CTimerShaft, CStatic)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_SIZE()
  //   ON_CONTROL_REFLECT(STN_CLICKED, &CTimerShaft::OnStnClicked)
  ON_WM_CTLCOLOR_REFLECT()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_COMMAND_RANGE(IDS_SETTINGS, IDS_DEL_TIMEBLOCK, OnEditOp)
  ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_MESSAGE(WM_REMOVEEDITTASK, OnRemoveEditTask)
  ON_WM_TIMER()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

int CTimerShaft::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CStatic::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Add your specialized creation code here
  EnableToolTips(TRUE);
//   if (!m_TimeTip.Create(this))
//   {
//     TRACE(_T("create tooltip error\r\n"));
//     return -1;
//   }
//   m_TimeTip.Activate(TRUE);
// 
//   BOOL bRet = m_TimeTip.AddTool(this, TEXT(""));
//   m_TimeTip.SetDelayTime(200);
//   m_TimeTip.SetTipBkColor(RGB(255, 255, 180));
//   m_TimeTip.SetTipTextColor(RGB(0, 0, 200));

  return 0;
}

void CTimerShaft::Draw(CDC& dc, const CRect& rt)
{
  if (m_Seconds == 0)
  {
    return;
  }

  //   int nSpace = 0;
  //   int nMinutes = 0;
  //   if ((nMinutes = m_Seconds / 60) > 0)
  //   {
  //     if (nMinutes / 60 > 0)     //文件时长为（小时）
  //     {
  //       nSpace = nMinutes / rt.Width();
  //     }
  //     else                    //文件时长为（分）
  //     {
  //     }
  //   }
  //   else                      //文件时长为（秒）
  //   {
  //     nSpace = m_Seconds / rt.Width();
  //   }

  const CRect rtShaft = rt;
  int64_t lSeconds = m_Seconds / (int64_t)1000;
  int nSpace = rt.Width() / lSeconds;
  if (nSpace <= 1)
  {
    if (rt.Width() < lSeconds)
    {
      m_nScaleRate = lSeconds / rt.Width();
    } 
    else
    {
      m_nScaleRate = rt.Width() / lSeconds;
    }

    nSpace = 5;      
    nSpace *= m_nScaleRate;
  }

  m_nSpace = nSpace;

  dc.MoveTo(rt.left, rt.top);
  dc.LineTo(rt.right, rt.top);

  CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
  CPen* pOldPen = dc.SelectObject(&blackPen);
  CPoint ptBegin(rt.left, rt.top), ptEnd;
  ptEnd.x = ptBegin.x;
  ptEnd.y = ptBegin.y + 15;

  for (int i = 0; i < rt.Width(); i += nSpace)
  {
    if (ptBegin.x >= rt.right)
    {
      ptBegin.x = rt.right;
    }

    dc.MoveTo(ptBegin);
    dc.LineTo(ptEnd);

    ptBegin.x += nSpace;
    ptEnd.x = ptBegin.x;
  }

  dc.SelectObject(pOldPen);
}

void CTimerShaft::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // TODO: Add your message handler code here
  // Do not call CStatic::OnPaint() for painting messages

  CRect rt;

  GetClientRect(&rt);
  Draw(dc, rt);
  DrawRectListMember(dc);
}

void CTimerShaft::OnSize(UINT nType, int cx, int cy)
{
  CStatic::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  CPaintDC dc(this);
  CRect rt;
  GetClientRect(&rt);
}

void CTimerShaft::ChangeSize(CWnd* pWnd, int cx, int cy)
{
  if(pWnd)  //判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建	
  {
    CRect rect;   //获取控件变化前的大小  
    pWnd->GetWindowRect(&rect);
    ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标

    //    cx/m_rect.Width()为对话框在横向的变化比例
    rect.left=rect.left*cx / m_rt.Width();//调整控件大小
    rect.right=rect.right*cx / m_rt.Width();
    rect.top=rect.top*cy/m_rt.Height();
    rect.bottom=rect.bottom*cy / m_rt.Height();
    pWnd->MoveWindow(rect);//设置控件大小
  }
}

HBRUSH CTimerShaft::CtlColor(CDC* pDC, UINT nCtlColor)
{
  // TODO:  Change any attributes of the DC here

  // TODO:  Return a non-NULL brush if the parent's handler should not be called
  return NULL;
}

void CTimerShaft::DrawLine(CDC& dc, const CPoint& ptBegin, const CPoint& ptEnd)
{
  dc.MoveTo(ptBegin.x, ptBegin.y);
  dc.LineTo(ptEnd.x, ptEnd.y);
}

// void CTimerShaft::OnStnClicked()
// {
// TODO: Add your control notification handler code here
//   if (false == IsPtInSelectedMp4Rect())               //与文件名对应的编辑框
//   {
//     return;
//   }
// 
//   SetCapture();
// 
//   m_bLBtnDown = true;
//   m_bReEditHead = false;
//   m_bReEditTail = false;
// 
//   CPoint ptCur;
//   CRect rt;
// 
//   GetClientRect(&rt);
//   m_ptEnd = (0, 0);
//   m_ptEndTemp = (0, 0);
//   GetCursorPos(&ptCur);
//   ::MapWindowPoints(NULL, this->GetSafeHwnd(), &ptCur, 1);
//   if (ptCur.x <= 0)
//   {
//     ptCur.x = 0;
//   }
// 
//   if (ptCur.x >= rt.Width())
//   {
//     ptCur.x = rt.Width();
//   }
// 
//   GetEditCuttimeStatue(rt, ptCur, m_bReEditHead, m_bReEditTail, m_bLBtnDown);
//   if (!m_bLBtnDown)
//   {
//     return;
//   }
// 
//   ZeroMemory(&m_PointTimeEdit, sizeof(POINTTIMEEDIT));
//   CTimeEdit* pTimeEdit = nullptr;
//   if (m_pCutTimePtList->empty() || !(m_bReEditHead || m_bReEditTail))   //new crop area
//   {
// 
//     if (m_rtList.size()>= 4)
//     {
//       m_bLBtnDown = false;
//       //       AfxMessageBox(_T("最多只能有4段"));
//       return;
//     }
// 
//     m_ptBegin = ptCur;
//     m_bReEditHead = m_bReEditTail = false;
// 
//     int64_t nCurTime = (((float)m_ptBegin.x / (float)rt.Width())) * (m_Seconds/1000);    //ms
//     m_pNewTimeEdit = pTimeEdit = CreateNewTimeEdit(nCurTime, ptCur, rt, true);
//     if (nullptr == pTimeEdit)
//     {
//       return;
//     }
// 
//     m_PointTimeEdit.m_pointPair.m_ptBegin.x = ptCur.x;
//     m_PointTimeEdit.m_pointPair.m_ptBegin.y = ptCur.y;
//     m_PointTimeEdit.m_TimeEditPair.m_pTimeEditBegin = pTimeEdit;
// 
//     SetCuttimePoint(rt, m_ptBegin, true);
//   }
//   else
//   {
//     //查找选中的point对应的时间点及timeEdit
//     m_pSelCutTime = const_cast<CS_CUT_TIME*>(SearchCutTimeByPoint(ptCur, rt));
//     m_pSelTimeEdit = SearchTimeEditByPoint(ptCur, rt);
// 
//     m_nSelRect = SearchRectIndex(ptCur);    
//   }
// 
//   ChangePos(ptCur);
// }

void CTimerShaft::OnMouseMove(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  if (false == m_bLBtnDown)
  {
    CStatic::OnMouseMove(nFlags, point);
    return;
  }

  //   ReleaseCapture();

  if (!m_bReEditHead && !m_bReEditTail)
  {
    if (!CanMove(m_ptBegin, point))
    {
      m_bValidArea = false;
      return;
    }
    else
    {
      m_bValidArea = true;
    }
  }

  CRect rt, rtTemp;
  GetClientRect(&rt);
  rtTemp = rt;

  if (point.x < rt.left)
  {
    m_bValidHeadPoint = false;
    m_bValidTailPoint = true;

    TRACE("rt.width = %d\r\n", rt.Width());
    return;
  }

  if (point.x > rt.right)
  {
    m_bValidHeadPoint = true;
    m_bValidTailPoint = false;

    TRACE("rt.width = %d, point.x > rt.right\r\n", rt.Width());
    return;
  }

  CClientDC dc(this);
  CDC MemDC;
  MemDC.CreateCompatibleDC(&dc);

  CBitmap bitmap;
  CBitmap* pOldbitmap = nullptr;

  bitmap.CreateCompatibleBitmap(&dc, rt.Width(), rt.Height());
  pOldbitmap = MemDC.SelectObject(&bitmap);

  if (m_bReEditHead)
  {
    m_rtList[m_nSelRect].left = point.x;

    CBitmap bitmap_inner;
    CBitmap* pOldbitmap = nullptr;

    rt.left = point.x;
    rt.top += 20;
    rt.bottom -= 20;

    //memdc fill color with bk color
    MemDC.FillSolidRect(0, 0, rt.Width(), rt.Height(), BKCOLOR);
    MemDC.FillSolidRect(0, 0, m_ptEnd.x-point.x, rt.Height(), CUTCOLOR);
    dc.BitBlt(0, rt.top, rt.Width(), rt.Height(), &MemDC, 0, 0, SRCCOPY);
  }
  else
  {
    if (m_bReEditTail)
    {
      m_rtList[m_nSelRect].right = point.x;
    }

    rt.left = m_ptBegin.x;
    rt.top += 20;
    rt.bottom -= 20;

    m_ptEnd = point;
    TRACE("m_ptEnd.x = %d\r\n", m_ptEnd.x);
    rt.left = m_ptBegin.x;

    //memdc fill color with bk color
    MemDC.FillSolidRect(0, 0, rt.Width(), rt.Height(), BKCOLOR);

    MemDC.FillSolidRect(0, 0, point.x, rt.Height(), BKCOLOR);
    MemDC.FillSolidRect(0, 0, point.x-m_ptBegin.x, rt.Height(), CUTCOLOR);
    //     MemDC.FillSolidRect(point.x, rt.top, rt.right-point.x, rt.Height(), BKCOLOR);
    dc.BitBlt(rt.left, rt.top, rt.Width(), rt.Height(), &MemDC, 0, 0, SRCCOPY);
  }

  DrawRectListMember(dc);

  MemDC.SelectObject(pOldbitmap);
  bitmap.DeleteObject();
  MemDC.DeleteDC();

  float fPosPercent = (float)point.x / (float)rtTemp.Width();
  double dShowTime = fPosPercent*(m_Seconds / 1000);
  CString strTime;
  strTime.Format("%.0lfs", dShowTime);
//   m_TimeTip.UpdateTipText(strTime, this);

  CStatic::OnMouseMove(nFlags, point);
}

void CTimerShaft::OnLButtonUp(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  if (!m_bLBtnDown)
  {
    return;
  }

  ReleaseCapture();

  CRect rt, rtTemp;
  GetClientRect(&rt);

  CPoint ptInner;
  CTimeEdit* pTimeEdit = nullptr;
  int64_t nCurTime = 0;

  if (m_bReEditHead || m_bReEditTail)     //重新编辑原来的区域
  {
    CPoint ptInner = point;
    if (!IsDragMoveValid(point, ptInner, rt))
    {
      nCurTime = (((float)ptInner.x / (float)rt.Width())) * (m_Seconds/1000);
    }
    else
    {
      nCurTime = (((float)point.x / (float)rt.Width())) * (m_Seconds/1000);
    }

    UpdateCuttimePoint(ptInner, nCurTime, m_bReEditHead == true);
    UpdateCuttimeList();

    UpdatePointIimeEditList(m_pSelTimeEdit, ptInner, rt);
    MoveTimeEdit(m_pSelTimeEdit, ptInner, rt);
    UpdateEditTime(m_pSelTimeEdit, ptInner, rt);
    UpdateRectList(ptInner);

    RectSort();
    CPaintDC dc(this);
    DrawRectListMember(dc);

    m_Editdlg.RedrawWindow();

    ChangePos(ptInner);
  }
  else      //添加新的编辑段
  {
    if (point.x >= rt.right)
    {
      point.x = rt.right;
    }

    CPoint ptRet;
    if (IsEndPointCrossBlock(point, ptRet))
    {
      point.x = ptRet.x;
      m_ptEnd.x = ptRet.x;
    }


    nCurTime = (((float)point.x / (float)rt.Width())) * (m_Seconds/1000);    //ms
    if (m_ptBegin.x != m_ptEnd.x && m_bValidArea)
    {
      SetCuttimePoint(rt, point, false);
      pTimeEdit = CreateNewTimeEdit(nCurTime, point, rt, false);

      m_PointTimeEdit.m_pointPair.m_ptEnd.x = point.x;
      m_PointTimeEdit.m_pointPair.m_ptEnd.y = point.y;
      m_PointTimeEdit.m_TimeEditPair.m_pTimeEditEnd = pTimeEdit;
      m_pPointTimeEditList->push_back(m_PointTimeEdit);
      StoreRect(rt);

      //排序时间段/POINTTIMEEDITLIST
      SortCuttimeRect();
      SortPointTimeEdit();
      SortCuttimePoint();

      m_pCutTime = &(m_pCutTimePt->m_CSCutTime);

      m_pLock->Lock();
      //       m_EditTaskRef.cutTimes.push_back(m_pCutTime);
      m_EditTask.cutTimes.push_back(m_pCutTime);
      m_pLock->Unlock();

      ChangePos(point);
    }
    else
    {
      SAFE_DELETE(m_pNewTimeEdit);
    }

    m_Editdlg.RedrawWindow();
  }

  m_ptBegin = (0, 0);
  m_bLBtnDown = false;

  CStatic::OnLButtonUp(nFlags, point);
}

void CTimerShaft::DrawDragBkColor(CDC& dc)
{
  CDC MemDC;
  MemDC.CreateCompatibleDC(&dc);

  CRect rt;
  GetClientRect(&rt);
  rt.left = m_ptBegin.x;
  rt.top = rt.top + 20;
  rt.right = m_ptEnd.x;
  rt.bottom = rt.bottom - 10;

#if _DEBUG
  CString strRt;
  strRt.Format("ptBegin:x=%d,y=%d\r\n", rt.left, rt.top);
  TRACE(strRt);

  strRt.Empty();
  strRt.Format("ptEnd:x=%d,y=%d\r\n", m_ptEnd.x, m_ptEnd.y);
  TRACE(strRt);
#endif

  CBitmap bitmap;
  BOOL bRet = bitmap.CreateCompatibleBitmap(&dc, rt.Width(), rt.Height());
  if (0 == bRet)
  {
    DWORD dwError = ::GetLastError();
  }
  CBitmap* pOldbitmap = MemDC.SelectObject(&bitmap);

  MemDC.FillSolidRect(0, 0, rt.Width(), rt.Height(), RGB(60, 160, 60));
  bRet = dc.BitBlt(rt.left, rt.top, rt.Width(), rt.Height(), &MemDC, 0, 0, SRCCOPY);
  MemDC.SelectObject(pOldbitmap);
}

bool CTimerShaft::IsPtInSelectedMp4Rect()
{
  const TIMERSHAFTMAP& ShaftMap = m_Editdlg.GetTimerShaftMap();
  TIMERSHAFTMAP::const_iterator it = ShaftMap.begin();
  for (; it != ShaftMap.end(); ++it)
  {
    const CString& strMp4 = it->first;
    CTimerShaft* pTimerShaft = it->second;
    const CString& strSelMp4 = m_Editdlg.GetCurMp4Name();

    if (strSelMp4 == strMp4)
    {
      if (pTimerShaft == this)
      {
        return true;
      }
    }
  } 

  return false;
}

void CTimerShaft::OnRButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  if (false == IsPtInSelectedMp4Rect())
  {
    return;
  }

  this->SetFocus();
  m_PtSel = point;

  if (m_pPopupMenu->CreatePopupMenu())
  {
    CString str;
    str.Empty();

    str.LoadString(IDS_SETTINGS);
    m_pPopupMenu->AppendMenu(MF_STRING, IDS_SETTINGS, str);

    str.Empty();
    str.LoadString(IDS_REMOVE_EDITTASK);
    m_pPopupMenu->AppendMenu(MF_STRING, IDS_REMOVE_EDITTASK, str);
    //     if (!m_bTaskAdd)
    //     {
    //       m_pPopupMenu->EnableMenuItem(IDS_REMOVE_EDITTASK, MF_BYCOMMAND|MF_GRAYED);
    //     }

    str.Empty();
    str.LoadString(IDS_ADD_EDITTASK);
    m_pPopupMenu->AppendMenu(MF_STRING, IDS_ADD_EDITTASK, str);

    //     str.Empty();
    //     str.LoadString(IDS_START_EDITTASK);
    //     m_pPopupMenu->AppendMenu(MF_STRING, IDS_START_EDITTASK, str);
    //     if (!m_bTaskAdd)
    //     {
    //       m_pPopupMenu->EnableMenuItem(IDS_START_EDITTASK, MF_BYCOMMAND|MF_GRAYED);
    //     }

    //     str.Empty();
    //     str.LoadString(IDS_PAUSE_EDITTASK);
    //     m_pPopupMenu->AppendMenu(MF_STRING, IDS_PAUSE_EDITTASK, str);
    //     if (!m_bTaskAdd || m_bPauseEdit)
    //     {
    //       m_pPopupMenu->EnableMenuItem(IDS_PAUSE_EDITTASK, MF_BYCOMMAND|MF_GRAYED);
    //     }
    // 
    //     str.Empty();
    //     str.LoadString(IDS_RESUME_EDITTASK);
    //     m_pPopupMenu->AppendMenu(MF_STRING, IDS_RESUME_EDITTASK, str);
    //     if (!m_bTaskAdd)
    //     {
    //       m_pPopupMenu->EnableMenuItem(IDS_RESUME_EDITTASK, MF_BYCOMMAND|MF_GRAYED);
    //     }

    str.Empty();
    str.LoadString(IDS_DEL_TIMEBLOCK);
    m_pPopupMenu->AppendMenu(MF_STRING, IDS_DEL_TIMEBLOCK, str);

    CPoint pt;
    GetCursorPos(&pt);
    m_pPopupMenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);

    m_pPopupMenu->DestroyMenu();
  }

  CStatic::OnRButtonDown(nFlags, point);
}

void CTimerShaft::OnEditOp(UINT nID)
{
  switch (nID)
  {
  case IDS_SETTINGS:
    {
      Settings();
      break;
    }
    //   case IDS_ADD_AHEAD_PIC:
    //     {
    //       AddPicHead();
    //       break;
    //     }
    // 
    //   case IDS_ADD_TAIL_PIC:
    //     {
    //       AddPicTail();
    //       break;
    //     }
    // 
    //   case IDS_SETTITLE_DURATION:
    //     {
    //       SetTitleDuration();
    //       break;
    //     }
    // 
    //   case IDS_SETTAIL_DURATION:
    //     {
    //       SetTailDuration();
    //       break;
    //     }
    // 
    //   case IDS_SET_FPS:
    //     {
    //       SetFps();
    //       break;
    //     }

  case IDS_REMOVE_EDITTASK:
    {
      RemoveEditTask();
      break;
    }

  case IDS_ADD_EDITTASK:
    {
      AddEditTask();
      break;
    }

  case IDS_START_EDITTASK:
    {
      StartEditTask();
      break;
    }

  case IDS_PAUSE_EDITTASK:
    {
      PauseEditTask();
      break;
    }

  case IDS_DEL_TIMEBLOCK:
    {
      DelTimeBlock();
      break;
    }

  default:                //IDS_RESUME_EDITTASK
    {
      ResumeEditTask();
      break;
    }
  }
}

void CTimerShaft::Settings()
{
  //   if (m_hUpLoad)
  //   {
  //     CloseHandle(m_hUpLoad);
  //     m_hUpLoad = INVALID_HANDLE_VALUE;
  //   }
  // 
  //   m_hUpLoad = (HANDLE)_beginthreadex(nullptr, 0, UpLoadThread, this, CREATE_SUSPENDED, nullptr);
  //   if (INVALID_HANDLE_VALUE == m_hUpLoad)
  //   {
  //     return;
  //   }

  CSettingDialog dlg(this);
  if (IDOK == dlg.DoModal())
  {
    _tcsncpy(m_EditTask.editOpts.dst_file_path, dlg.GetDestFileName(), dlg.GetDestFileName().GetLength());
    m_EditTask.editOpts.dst_video_bitrate = -1;
    m_EditTask.editOpts.dst_pic_width = -1;
    m_EditTask.editOpts.dst_pic_height = -1;
    m_EditTask.editOpts.dst_frame_rate = dlg.GetFPS();
    m_EditTask.editOpts.title_duration = dlg.GetTitleDuration()*1000*1000;
    strcpy(m_EditTask.editOpts.title_pic_path, dlg.GetTitlePicName());

    m_EditTask.editOpts.tail_duration = dlg.GetTailDuration()*1000*1000;
    strcpy(m_EditTask.editOpts.tail_pic_path, dlg.GetTailPicName());
    m_EditTask.editOpts.tail_switch_mode = -1;

    //上传片头片尾至服务器上
    int nURLStart = CURL::GetURL().Find(_T("http://"));
    int nNext = CURL::GetURL().Find(_T('/'), _tcslen(_T("http://")));
    CString strUploadURL = CURL::GetURL().Left(nNext);
    CString strUploadURLTemp = strUploadURL;
    strUploadURL += _T("/vrs/editdir");

    m_ULTitle.m_strURL = strUploadURLTemp;
    m_ULTitle.m_strServer = strUploadURL;
    m_ULTitle.m_strPath = dlg.GetTitlePath();
    m_ULTitle.m_strName = dlg.GetTitlePicName();

    m_ULTail.m_strURL = strUploadURLTemp;
    m_ULTail.m_strServer = strUploadURL;
    m_ULTail.m_strPath = dlg.GetTailPath();
    m_ULTail.m_strName = dlg.GetTailPicName();

    m_bUpLoadSuccess = (UpLoad(strUploadURLTemp, strUploadURL, dlg.GetTitlePath(), dlg.GetTitlePicName()) &&
      UpLoad(strUploadURLTemp, strUploadURL, dlg.GetTailPath(), dlg.GetTailPicName()));

    if (!dlg.NeedUpload())
    {
      m_bUpLoadSuccess = TRUE;
    }
//     if (dlg.NeedUpload())
//     {
//       ResumeThread(m_hUpLoad);
//     }
//     else
//     {
//       SetEvent(m_hUpLoadEvent);
//       m_bUpLoadSuccess = TRUE;
//     }    
  }  
}

void CTimerShaft::AddPicHead()
{
  CString strPath, strPicName;
  strPath.Empty();

  AddPic(strPath, strPicName);

  if (strPath.IsEmpty())
  {
    return;
  }

  CImagePreviewDialog* pImagePrevDlg = ((CImagePrview*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(2, 0)))->GetImagePrevDlg();
  ::PostMessage(pImagePrevDlg->GetSafeHwnd(), WM_ADDPIC, WPARAM(true), (LPARAM)(&strPath));

  m_EditOpts.SetTitlePicPath(strPicName, strPicName.GetLength());
  m_EditOpts.SetAddTitle(true);
}

void CTimerShaft::AddPicTail()
{
  CString strPath, strPicName;
  strPath.Empty();

  AddPic(strPath, strPicName);
  if (strPath.IsEmpty())
  {
    return;
  }

  CImagePreviewDialog* pImagePrevDlg = ((CImagePrview*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(2, 0)))->GetImagePrevDlg();
  ::PostMessage(pImagePrevDlg->GetSafeHwnd(), WM_ADDPIC, WPARAM(false), (LPARAM)(&strPath));

  m_EditOpts.SetTailPicPath(strPicName, strPicName.GetLength());
  m_EditOpts.SetAddTail(true);
}

void CTimerShaft::SetTitleDuration()
{
  //   if (!m_EditOpts.IsTitleAdded())
  //   {
  //     return;
  //   }
  // 
  //   CDurationDialog dlg;
  //   if (IDOK == dlg.DoModal())
  //   {
  //     m_EditOpts.SetTitleDuration(dlg.GetDurations()*1000*1000);
  //   }
}

void CTimerShaft::SetTailDuration()
{
  //   if (!m_EditOpts.isTailAdded())
  //   {
  //     return;
  //   }
  // 
  //   CDurationDialog dlg;
  //   if (IDOK == dlg.DoModal())
  //   {
  //     m_EditOpts.SetTailDuration(dlg.GetDurations()*1000*1000);
  //   }
}

void CTimerShaft::SetFps()
{
  //   CFPSDialog dlg;
  //   if (IDOK == dlg.DoModal())
  //   {
  //     m_EditTaskRef.editOpts.dst_frame_rate = atoi(dlg.m_strFPS.GetBuffer());
  //   }
}

void CTimerShaft::ApplySet()
{
  CEditCsCmdFrame CmdFrame(sizeof(CS_EDIT_TASK));
  CmdFrame.SetFrameCmdType(e_edit_task_stop);
  //   CmdFrame.SetFrameData(, (UInt16b)sizeof(CS_EDIT_TASK));
  //   theApp.GetCsSkt().SendCsData();
}

void CTimerShaft::AddPic(CString& strName, CString& strPicName)
{
  TCHAR szDirectory[MAX_DIR_LEN];
  ZeroMemory(szDirectory, sizeof(szDirectory));

  GetCurrentDirectory(MAX_DIR_LEN, szDirectory);

  static TCHAR BASED_CODE szFilter[] = _T("jpg (*.jpg)|*.jpg|")_T("png (*.png)|*.png|")_T("All Files (*.*)|*.*||");

  CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_HIDEREADONLY,
    szFilter, this);
  if (IDOK == dlg.DoModal())
  {
    strName = dlg.GetPathName();   
    strPicName = dlg.GetFileName();
  }
}

void CTimerShaft::ChangePos(const CPoint& pt)
{
  CMediaPlayerDialog* pPlayDlg = NULL;
  CPreView* pView = (CPreView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(0, 0));
  if (pView)
  {
    pPlayDlg = pView->GetMediaPlayDlg();
  }

  if (pPlayDlg)
  {
    if (CVLCPlayer::Instance()->GetPlayState() != CVLCPlayer::e_Playing)
    {
      return;
    }

    CRect rt;
    GetClientRect(&rt);
    int nPosPercent = int((float)pt.x / (float)rt.Width() *100);
    ::PostMessage(pPlayDlg->GetSafeHwnd(), WM_SEEK, NULL, (LPARAM)nPosPercent);
  }
}

void CTimerShaft::RectSort()
{
  m_pLock->Lock();

  for (unsigned int i = 0; i < m_rtList.size() - 1; ++i)
  {
    for (unsigned int j = i + 1; j < m_rtList.size(); ++j)
    {
      if (m_rtList[i].left  > m_rtList[j].left)
      {
        CRect rtTemp = m_rtList[i];
        m_rtList[i] = m_rtList[j];
        m_rtList[j] = rtTemp;
      }
    }
  }

  m_pLock->Unlock();
}

void CTimerShaft::DrawRectListMember(CDC& dc)
{
  for (int i = 0; i < m_rtList.size(); ++i)
  {
#ifdef _DEBUG
    CString strFmt;
    strFmt.Format("left:%d, top:%d, right:%d, bottom:%d\r\n", 
      m_rtList[i].left, m_rtList[i].top, m_rtList[i].right, 
      m_rtList[i].bottom);
    TRACE(strFmt);
#endif
    dc.FillSolidRect(&m_rtList[i], CUTCOLOR);
  }
}


//通知server添加编辑任务
void CTimerShaft::AddEditTask()
{
  theApp.StartRecvEvent();

  if (!theApp.m_bOnline)
  {
    return;
  }

  if (m_bTaskHasAdd)
  {
    AfxMessageBox("编辑任务已添加完成");
    return;
  }

  if (::MessageBox(GetSafeHwnd(), _T("添加编辑任务?"), _T("提示"), MB_OKCANCEL) != IDOK)
  {
    return;
  }

  //   WaitForSingleObject(m_hUpLoadEvent, INFINITE);
  if (!m_bUpLoadSuccess)
  {
    AfxMessageBox(_T("上传失败"));
    return;
  }

  if (false == ((CMainFrame*)AfxGetMainWnd())->IsEditFinish())
  {
    AfxMessageBox(_T("请等待编辑任务完成..."));
    return;
  }

  CTaskListDialog* pDlg = g_pMainFrame->m_pFileView->GetTaskListDlg();

  EDIT_TASK_RECORD record;
  record.m_nCutTimeCount = m_pCutTimePtList->size();
  record.m_pCutTime = new CS_CUT_TIME[record.m_nCutTimeCount];

  record.m_nID = pDlg->GetListCtrl().GetTaskId();;
  memcpy(&record.m_editOpts, &m_EditTask.editOpts, sizeof(m_EditTask.editOpts));

  CUTTIMEPOINTLIST::iterator it_cp = m_pCutTimePtList->begin();
  for (int i = 0; it_cp != m_pCutTimePtList->end(); ++it_cp, ++i)
  {
    record.m_pCutTime[i].startTimeInUs = (*it_cp).m_CSCutTime.startTimeInUs;
    record.m_pCutTime[i].endTimeInUs = (*it_cp).m_CSCutTime.endTimeInUs;
    record.m_pCutTime[i].streamMapNum = (*it_cp).m_CSCutTime.streamMapNum;
    record.m_pCutTime[i].switch_mode = (*it_cp).m_CSCutTime.switch_mode;

    for (int j = 0; j < MaxAvVideoStreamNum; ++j)
    {
      memcpy(record.m_pCutTime[i].streamMaps[j].src_file_path, (*it_cp).m_CSCutTime.streamMaps[j].src_file_path, MAX_PATH_LEN);
      record.m_pCutTime[i].streamMaps[j].a_stream0_dst_id = (*it_cp).m_CSCutTime.streamMaps[j].a_stream0_dst_id;
      record.m_pCutTime[i].streamMaps[j].v_stream0_dst_id = (*it_cp).m_CSCutTime.streamMaps[j].v_stream0_dst_id;
    }
  }

  record.m_bFlag = 1;

  g_timeRecord.Write(record);

  delete [] record.m_pCutTime;
  record.m_pCutTime = NULL;

  m_bTaskAdd = theApp.GetEditCs().csAddEditTask(m_nTaskID, m_EditTask);

  if (m_bTaskAdd)
  {
    m_bTaskHasAdd = true;
    g_statusLog.StatusOut("添加编辑任务成功");
  }
  else
  {
    g_statusLog.StatusOut("添加编辑任务失败");
  }
}

//删除当前编辑任务并通知server
void CTimerShaft::RemoveEditTask()
{
  if (FALSE == theApp.GetEditCs().csRemoveEditTask(m_nTaskID))
  {
    //     AfxMessageBox(_T("连接已断开"));
    return;
  }  

  //   m_rtList.clear();
  ReleaseResource();
  m_Editdlg.RedrawWindow();

  CClientDC dc(this);

  CRect rt;
  GetClientRect(&rt);
  rt.top += 20;
  rt.bottom -= 10;
  dc.FillSolidRect(&rt, BKCOLOR);

  CSplitterWnd& split = g_pMainFrame->GetSplitterWnd2();
  CTaskListDialog* pDlg = ((CFilePreView*)(split.GetPane(1, 0)))->GetTaskListDlg();
  CEditTaskListCtrl& listCtrl = pDlg->GetListCtrl();
  POSITION pos = listCtrl.GetFirstSelectedItemPosition();
  int nItem = -1;
  while (pos)
  {
    nItem = listCtrl.GetNextSelectedItem(pos);
  }
  listCtrl.SetItemText(nItem, 2, _T(""));
  listCtrl.SetItemText(nItem, 3, _T("0%"));
  listCtrl.SetItemText(nItem, 4, _T(""));

  CTaskManager* pTaskManager = ((CmediaPlayerApp*)AfxGetApp())->m_pTaskManager;

  EDITSTATUS status;
  //   status.m_taskStatus = taskStatus.taskStatus;
  status.m_nTaskId = m_nTaskID;
  status.m_nEditProgress = 0;
  memcpy(status.m_szSourceFileName, pTaskManager->GetTask(m_nTaskID)->GetSourceName(), 
    pTaskManager->GetTask(m_nTaskID)->GetSourceName().GetLength()+sizeof(TCHAR));
  memset(status.m_szDestFileName, 0, sizeof(status.m_szDestFileName));
  memcpy(status.m_szSourceAlias, pTaskManager->GetTask(m_nTaskID)->GetSourceAlias(), 
    pTaskManager->GetTask(m_nTaskID)->GetSourceAlias().GetLength() + sizeof(TCHAR));
  status.m_bUseFlag = 1;

  g_editStatusFile.Update(status);
  g_timeRecord.Delete(m_nTaskID);

  //   RECTLIST::iterator it = m_rtList.begin();
  //   for (; it != m_rtList.end(); ++it)
  //   {
  //     const CRect& rt = *it;
  //     if (PtInRect(&(*it), m_PtSel))
  //     {
  //       bSelBlock = true;
  //       break;
  //     }
  //   }
  // 
  //   CUTTIMEPOINTLIST::iterator it_cp = m_pCutTimePtList->begin();
  // 
  //   if (bSelBlock)
  //   {
  //     m_rtList.erase(it);
  //     //删除两个时间点
  //     POINTTIMEEDITLIST::iterator it = m_pPointTimeEditList->begin();
  //     CS_CUT_TIME_LIST::iterator it_cutTimeList = m_EditTask.cutTimes.begin();
  // 
  //     for (; it != m_pPointTimeEditList->end(), it_cp != m_pCutTimePtList->end(), 
  //       it_cutTimeList != m_EditTask.cutTimes.end(); ++it, ++it_cp, ++it_cutTimeList)
  //     {
  //       if ((m_PtSel.x >= (*it).m_pointPair.m_ptBegin.x) && (m_PtSel.x <= (*it).m_pointPair.m_ptEnd.x))
  //       {
  //         SAFE_DELETE((*it).m_TimeEditPair.m_pTimeEditBegin);
  //         SAFE_DELETE((*it).m_TimeEditPair.m_pTimeEditEnd);
  // 
  //         m_pPointTimeEditList->erase(it);
  //         m_pCutTimePtList->erase(it_cp);
  //         m_EditTask.cutTimes.erase(it_cutTimeList);
  // 
  //         break;
  //       }
  //     }    
}

//通知server启动编辑任务
void CTimerShaft::StartEditTask()
{
  if (FALSE == theApp.GetEditCs().csSendEditTaskCmd(m_nTaskID, e_edit_task_start))
  {
    AfxMessageBox(_T("发送失败"));
  }  
}

//通知server暂停编辑任务
void CTimerShaft::PauseEditTask()
{
  if (FALSE == theApp.GetEditCs().csSendEditTaskCmd(m_nTaskID, e_edit_task_pause))
  {
    AfxMessageBox(_T("发送失败"));
  }

  m_bPauseEdit = TRUE;
}

//通知server继续编辑任务
void CTimerShaft::ResumeEditTask()
{
  if (FALSE == theApp.GetEditCs().csSendEditTaskCmd(m_nTaskID, e_edit_task_resume))
  {
    AfxMessageBox(_T("发送失败"));
  }

  m_bPauseEdit = FALSE;
}

BOOL CTimerShaft::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{ 
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR; 
  UINT nID = pNMHDR->idFrom;

  if (nID == this->GetDlgCtrlID()) // Fill in the text buffer
  {
    _tcscpy(pTTT->szText, _T("Tooltip text\rExtended tooltip text"));
    return TRUE;
  }

  return FALSE;
}



BOOL CTimerShaft::PreTranslateMessage(MSG* pMsg)
{
  // TODO: Add your specialized code here and/or call the base class
  if (NULL != this->GetSafeHwnd())  
  {  
//     m_TimeTip.RelayEvent(pMsg);  
  }  

  return CStatic::PreTranslateMessage(pMsg);
}

void CTimerShaft::UpdatePosList(const CPoint& ptCur, int nSel, bool bBegin/* = true*/)
{
  if (m_pPointTimeEditList)
  {
    POINTTIMEEDIT& obj = m_pPointTimeEditList->at(nSel);

    if (bBegin)
    {
      obj.m_pointPair.m_ptBegin.x = ptCur.x;
    }
    else
    {
      obj.m_pointPair.m_ptEnd.x = ptCur.x;
    }
  }
}

void CTimerShaft::ReleaseResource()
{
  //   POINTTIMEEDITLIST::iterator it = m_pPointTimeEditList->begin();
  //   for (; it != m_pPointTimeEditList->end(); ++it)
  //   {
  //     CTimeEdit* pTimeEditBegin = (*it).m_TimeEditPair.m_pTimeEditBegin;
  //     SAFE_DELETE(pTimeEditBegin);
  // 
  //     CTimeEdit* pTimeEditEnd = (*it).m_TimeEditPair.m_pTimeEditEnd;
  //     SAFE_DELETE(pTimeEditEnd);
  //   }
  //   m_pPointTimeEditList->clear();

  m_rtList.clear();

  m_pCutTimePtList->clear();

  m_rtList.clear();
  if (m_pCutTimePtList)
  {
    m_pCutTimePtList->clear();
  }

  if (m_pPointTimeEditList)
  {
    POINTTIMEEDITLIST::iterator it = m_pPointTimeEditList->begin();
    for (; it != m_pPointTimeEditList->end(); ++it)
    {
      SAFE_DELETE(it->m_TimeEditPair.m_pTimeEditBegin);
      SAFE_DELETE(it->m_TimeEditPair.m_pTimeEditEnd);
    }

    m_pPointTimeEditList->clear();
  }

  CS_CUT_TIME_LIST::iterator it_cutTimeList = m_EditTask.cutTimes.begin();
  for (; it_cutTimeList != m_EditTask.cutTimes.end(); ++it_cutTimeList)
  {
    SAFE_DELETE(*it_cutTimeList);
  }
  m_EditTask.cutTimes.clear();
}

void CTimerShaft::SetCuttimePoint(const CRect& rt, const CPoint& pt, bool bHeadPoint)
{
  float fPercent = (float)pt.x / (float)rt.Width();

  if (bHeadPoint)
  {
    m_pCutTimePt = new CUTTIMEPOINT;
    if (nullptr == m_pCutTimePt)
    {
      return;
    }
    ZeroMemory(m_pCutTimePt, sizeof(CUTTIMEPOINT));

    m_pCutTimePt->m_BeginEndPoint.m_ptBegin.x = pt.x;
    m_pCutTimePt->m_BeginEndPoint.m_ptBegin.y = pt.y;
    m_pCutTimePt->m_CSCutTime.startTimeInUs = fPercent*m_Seconds*1000;
  }
  else
  {
    m_pCutTimePt->m_BeginEndPoint.m_ptEnd.x = pt.x;
    m_pCutTimePt->m_BeginEndPoint.m_ptEnd.y = pt.y;
    m_pCutTimePt->m_CSCutTime.endTimeInUs = fPercent*m_Seconds*1000;

    //     CTaskListDialog* pDlg = ((CFilePreView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(1, 0)))->GetTaskListDlg();
    //     if (!pDlg)
    //     {
    //       return;
    //     }

    strcpy(m_pCutTimePt->m_CSCutTime.streamMaps[0].src_file_path, m_strFileName.c_str());
    m_pCutTimePt->m_CSCutTime.streamMaps[0].a_stream0_dst_id = 0;
    m_pCutTimePt->m_CSCutTime.streamMaps[0].v_stream0_dst_id = 0;
    m_pCutTimePt->m_CSCutTime.streamMapNum = 1;
    m_pCutTimePt->m_CSCutTime.switch_mode = e_SM_MidIn; // -1 for none switch mode

    m_pCutTimePtList->push_back(*m_pCutTimePt);
  }
}

//更新CUTTIMEPOINTLIST中CUTTIMEPOINT的值(time & point)
bool CTimerShaft::UpdateCuttimePoint(const CPoint& pt, int64_t& time, bool bStartTime/* = true*/)
{
  if (nullptr == m_pCutTimePtList)
  {
    return false;
  }

  CUTTIMEPOINTLIST::iterator it = m_pCutTimePtList->begin();
  for (; it != m_pCutTimePtList->end(); ++it)
  {
    if (&((*it).m_CSCutTime) == m_pSelCutTime)
    {
      if (bStartTime)
      {
        (*it).m_BeginEndPoint.m_ptBegin.x = pt.x;
        (*it).m_BeginEndPoint.m_ptBegin.y = pt.y;

        (*it).m_CSCutTime.startTimeInUs = time*1000*1000;

        return true;
      }
      else
      {
        (*it).m_BeginEndPoint.m_ptEnd.x = pt.x;
        (*it).m_BeginEndPoint.m_ptEnd.y = pt.y;

        (*it).m_CSCutTime.endTimeInUs = time*1000*1000;

        return true;
      }
    }      
  }

  return false;
}

void CTimerShaft::UpdateCuttimeList()
{
  CAutoLock l(m_cs);

  CUTTIMEPOINTLIST::iterator it1 = m_pCutTimePtList->begin();
  CS_CUT_TIME_LIST::iterator it2 = m_EditTask.cutTimes.begin();

  for (;it1 != m_pCutTimePtList->end(), it2 != m_EditTask.cutTimes.end(); ++it1,++it2)
  {

    int64_t t1 = (*it1).m_CSCutTime.startTimeInUs;
    //     int64_t t2 = (*it2)->startTimeInUs;
    int64_t t2 = (*it2)->startTimeInUs;
    (*it2)->startTimeInUs = (*it1).m_CSCutTime.startTimeInUs;
    (*it2)->endTimeInUs = (*it1).m_CSCutTime.endTimeInUs;

    (*it2)->streamMapNum = (*it1).m_CSCutTime.streamMapNum;
    (*it2)->switch_mode = (*it1).m_CSCutTime.switch_mode;
    memcpy(&((*it2)->streamMaps), &((*it1).m_CSCutTime.streamMaps), sizeof(CS_STREAM_MAP));
  }
}


void CTimerShaft::UpdatePointIimeEditList(CTimeEdit* pCurTimeEdit, const CPoint& pt,
  const CRect& rt)
{
  CAutoLock l(m_cs);

  if (m_pPointTimeEditList)
  {
    POINTTIMEEDITLIST::iterator it = m_pPointTimeEditList->begin();
    for (; it != m_pPointTimeEditList->end(); ++it)
    {
      if (pCurTimeEdit == (*it).m_TimeEditPair.m_pTimeEditBegin)
      {
        (*it).m_pointPair.m_ptBegin.x = pt.x;
        (*it).m_pointPair.m_ptBegin.y = pt.y;

        return;
      }

      if (pCurTimeEdit == (*it).m_TimeEditPair.m_pTimeEditEnd)
      {
        (*it).m_pointPair.m_ptEnd.x = pt.x;
        (*it).m_pointPair.m_ptEnd.y = pt.y;

        return;
      }
    }
  }

}

void CTimerShaft::GetEditCuttimeStatue(const CRect& rt, const CPoint& pt, bool& bReEditHead, bool& bReEditTail, bool& bLBtnStatue)
{
  CAutoLock l(m_cs);

  CUTTIMEPOINTLIST::const_iterator it = m_pCutTimePtList->begin();
  for (; it != m_pCutTimePtList->end(); ++it)
  {
    if (PtInRect(&CRect((*it).m_BeginEndPoint.m_ptBegin.x-TORANCE, rt.top, 
      (*it).m_BeginEndPoint.m_ptBegin.x+TORANCE, rt.bottom), pt))
    {
      bReEditHead = true;
      bReEditTail = false;
      bLBtnStatue = true;

      m_ptBegin.x = (*it).m_BeginEndPoint.m_ptBegin.x;

      return;
    }

    if (PtInRect(&CRect((*it).m_BeginEndPoint.m_ptEnd.x-TORANCE, rt.top, 
      (*it).m_BeginEndPoint.m_ptEnd.x+TORANCE, rt.bottom), pt))
    {
      bReEditHead = false;
      bReEditTail = true;
      bLBtnStatue = true;

      m_ptEnd.x = m_ptEndTemp.x = (*it).m_BeginEndPoint.m_ptEnd.x;

      return;
    }

    if (PtInRect(&CRect((*it).m_BeginEndPoint.m_ptBegin.x+TORANCE, rt.top, 
      (*it).m_BeginEndPoint.m_ptEnd.x-TORANCE, rt.bottom), pt))
    {
      bLBtnStatue = false;
      bReEditHead = false;
      bReEditTail = false;

      return;
    }
  }
}

CTimeEdit* CTimerShaft::CreateNewTimeEdit(int64_t time, const CPoint& pt, const CRect& rt, bool bLeft)
{
  int nHour = 0, nMinute = 0, nSecs = 0;
  while (true)
  {
    if (!(time / 60))
    {
      nSecs = time;
      break;
    }

    nMinute += time / 60;
    time %= 60;
  }

  while (true)
  {
    if (!(nMinute/60))
    {
      break;
    }

    nHour += nMinute /60;
    nMinute %= 60;
  }

  CString strCutTime;
  strCutTime.Format("%02d:%02d:%02d", nHour, nMinute, nSecs);

  static int i = 0;
  return (new CTimeEdit(this, pt, rt, i++, strCutTime, bLeft)); 
}

void CTimerShaft::ShowTimeEdit()
{
  CTaskListDialog* pDlg = ((CFilePreView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(1, 0)))->GetTaskListDlg();
  if (!pDlg)
  {
    return;
  }

  //   const std::vector<CString>& strFileNameAry = pDlg->GetFileNameAry();
  //   m_nFileArySize = strFileNameAry.size();
  // 
  //   unsigned int i = 0;
  // 
  //   m_pCutTime = new CS_CUT_TIME;
  //   if (!m_pCutTime)
  //   {
  //     return;
  //   }
  //   memset(m_pCutTime, 0, sizeof(CS_CUT_TIME));

  //   if (theApp.IsEditSingleFile())    //编辑单个文件
  //   {
  //     int nEditFile = pDlg->GetEditFileIndex();
  // 
  //     if (m_bReEditTail)
  //     {
  //       m_pCutTime->endTimeInUs =  fPercent*m_Seconds*1000;      
  //     }
  //     else
  //     {
  //       m_pCutTime->startTimeInUs =  fPercent*m_Seconds*1000;
  //     }
  // 
  //     strcpy(m_pCutTime->streamMaps[0].src_file_path, strFileNameAry[nEditFile]);
  //     m_pCutTime->streamMaps[0].a_stream0_dst_id = 0;
  //     m_pCutTime->streamMaps[0].v_stream0_dst_id = 0;
  //     m_pCutTime->streamMapNum = 1;
  //     m_pCutTime->switch_mode = e_SM_MidIn; // -1 for none switch mode
  //     double dShowTime = fPercent*m_Seconds / 1000;
  // 
  //     CString strTime;
  //     strTime.Format("%.0lfs", dShowTime);
  //     m_TimeTip.UpdateTipText(strTime, this);
}

void CTimerShaft::StoreRect(CRect& rt)
{
  rt.left = m_ptBegin.x;
  rt.top += 20;
  rt.right = m_ptEnd.x;
  rt.bottom -= 20;

  m_pLock->Lock();
  m_rtList.push_back(rt);
  m_pLock->Unlock();
}


const CS_CUT_TIME* CTimerShaft::SearchCutTimeByPoint(const CPoint& ptSel, const CRect& rt)
{
  CAutoLock l(m_cs);

  if (m_pCutTimePtList)
  {
    CUTTIMEPOINTLIST::const_iterator it = m_pCutTimePtList->begin();
    for (; it != m_pCutTimePtList->end(); ++it)
    {
      if (PtInRect(&CRect((*it).m_BeginEndPoint.m_ptBegin.x-TORANCE, rt.top,
        (*it).m_BeginEndPoint.m_ptBegin.x+TORANCE, rt.bottom), ptSel))
      {
        m_ptBegin.x = (*it).m_BeginEndPoint.m_ptBegin.x;
        m_ptBegin.y = (*it).m_BeginEndPoint.m_ptBegin.y;

        return &((*it).m_CSCutTime);
      }

      if (PtInRect(&CRect((*it).m_BeginEndPoint.m_ptEnd.x-TORANCE, rt.top,
        (*it).m_BeginEndPoint.m_ptEnd.x+TORANCE, rt.bottom), ptSel))
      {
        m_ptBegin.x = (*it).m_BeginEndPoint.m_ptBegin.x;
        m_ptBegin.y = (*it).m_BeginEndPoint.m_ptBegin.y;

        m_ptEndTemp.x = (*it).m_BeginEndPoint.m_ptEnd.x;
        m_ptEndTemp.y = (*it).m_BeginEndPoint.m_ptEnd.y;

        return &((*it).m_CSCutTime);
      }
    }
  }

  return nullptr;
}

CTimeEdit* CTimerShaft::SearchTimeEditByPoint(const CPoint& ptSel, const CRect& rt)
{
  CAutoLock l(m_cs);

  if (m_pPointTimeEditList)
  {
    POINTTIMEEDITLIST::const_iterator it = m_pPointTimeEditList->begin();
    for (; it != m_pPointTimeEditList->end(); ++it)
    {
      if (PtInRect(&CRect((*it).m_pointPair.m_ptBegin.x-TORANCE, rt.top,
        (*it).m_pointPair.m_ptBegin.x+TORANCE, rt.bottom), ptSel))
      {
        return (*it).m_TimeEditPair.m_pTimeEditBegin;
      }

      if (PtInRect(&CRect((*it).m_pointPair.m_ptEnd.x-TORANCE, rt.top,
        (*it).m_pointPair.m_ptEnd.x+TORANCE, rt.bottom), ptSel))
      {
        return (*it).m_TimeEditPair.m_pTimeEditEnd;
      }
    }
  }

  return nullptr;
}

void CTimerShaft::MoveTimeEdit(CTimeEdit* pCurTimeEdit, const CPoint& pt, const CRect& rt)
{
  if (nullptr == pCurTimeEdit)
  {
    return;
  }

  if (m_bReEditHead)
  {
    pCurTimeEdit->MoveWindow(pt.x, rt.bottom - TIMEBOX_HEIGHT, TIMEBOX_WIDTH, TIMEBOX_HEIGHT);
  }

  if (m_bReEditTail)
  {
    m_pSelTimeEdit->MoveWindow(pt.x-TIMEBOX_WIDTH, rt.bottom-TIMEBOX_HEIGHT, TIMEBOX_WIDTH, TIMEBOX_HEIGHT);
  }
}

void CTimerShaft::UpdateEditTime(CTimeEdit* pCurTimeEdit, const CPoint& pt, const CRect& rt)
{
  if (!pCurTimeEdit)
  {
    return;
  }

  int64_t nCurTime = (((float)pt.x / (float)rt.Width())) * (m_Seconds/1000);    //ms
  int nHour = 0, nMinute = 0, nSecs = 0;

  while (true)
  {
    if (!(nCurTime / 60))
    {
      nSecs = nCurTime;
      break;
    }

    nMinute += nCurTime / 60;
    nCurTime %= 60;
  }

  while (true)
  {
    if (!(nMinute/60))
    {
      break;
    }

    nHour += nMinute /60;
    nMinute %= 60;
  }

  CString strCutTime;
  strCutTime.Format("%02d:%02d:%02d", nHour, nMinute, nSecs);

  pCurTimeEdit->UpdateTime(strCutTime);
}


int CTimerShaft::SearchRectIndex(const CPoint& pt)
{
  CRect rt;
  GetClientRect(&rt);

  //   const int TORANCE = 5;
  int nIndex = -1;


  CAutoLock l(m_cs);

  RECTLIST::const_iterator it = m_rtList.begin();
  for (; it != m_rtList.end(); ++it)
  {
    ++nIndex;

    if (PtInRect(&CRect((*it).left-TORANCE, rt.top, (*it).left+TORANCE, rt.bottom), pt))
    {
      break;
    }

    if (PtInRect(&CRect((*it).right-TORANCE, rt.top, (*it).right+TORANCE, rt.bottom), pt))
    {
      break;
    }
  }

  return nIndex;
}

void CTimerShaft::UpdateRectList(const CPoint& pt)
{
  CAutoLock l(m_cs);

  if (m_rtList.empty())
  {
    return;
  }

  if (m_bReEditHead)
  {
    m_rtList[m_nSelRect].left = pt.x;
  }

  if (m_bReEditTail)
  {
    m_rtList[m_nSelRect].right = pt.x;
  }
}

//检查是否从后往前拖动生成时间段
bool CTimerShaft::CanMove(const CPoint& ptBegin, const CPoint& ptEnd)
{
  if (ptBegin.x >= ptEnd.x - 10)
  {
    return false;
  }

  return true;
}

//针对重新编辑，检查当前拖动的头部或尾部是否越过了原来的时间段
bool CTimerShaft::IsDragMoveValid(const CPoint& ptDrag, CPoint& ptRet, const CRect& rt)
{
  const int nValidSpace = 2;
  int nAreaCount = m_pPointTimeEditList->size();

  CAutoLock l(m_cs);

  const POINTTIMEEDIT& ptEdit = m_pPointTimeEditList->at(m_nSelRect);
  if (m_bReEditHead)
  {
    //编辑第1个时间段
    if (0 == m_nSelRect)
    {
      if (ptDrag.x <= 0)
      {
        ptRet.x = 0;
        ptRet.y = ptEdit.m_pointPair.m_ptEnd.y;        
      }

      if (ptDrag.x >= ptEdit.m_pointPair.m_ptEnd.x)
      {
        ptRet.x = ptEdit.m_pointPair.m_ptBegin.x;
        ptRet.y = ptEdit.m_pointPair.m_ptBegin.y;
      }

      return false;
    }

    if (m_nSelRect > 0 && m_nSelRect < nAreaCount)
    {
      if (ptDrag.x >= ptEdit.m_pointPair.m_ptEnd.x-nValidSpace)
      {
        ptRet.x = ptEdit.m_pointPair.m_ptBegin.x;
        ptRet.y = ptEdit.m_pointPair.m_ptBegin.y;

        //         ptRet.x = ptEdit.m_pointPair.m_ptEnd.x-nValidSpace;
        //         ptRet.y = ptEdit.m_pointPair.m_ptEnd.y;
        return false;
      }

      const POINTTIMEEDIT& ptEditPrev = m_pPointTimeEditList->at(m_nSelRect-1);
      if (ptDrag.x <= ptEditPrev.m_pointPair.m_ptEnd.x + nValidSpace)
      {
        CString str;
        str.Format(_T("ReEditTail ptDrag.x=%d,m_ptEnd.x=%d, false"), ptDrag.x, ptEditPrev.m_pointPair.m_ptEnd.x);
        TRACE(str);

        ptRet.x = ptEditPrev.m_pointPair.m_ptEnd.x + nValidSpace;
        ptRet.y = ptEditPrev.m_pointPair.m_ptEnd.y;

        return false;
      }
    }
  }

  if (m_bReEditTail)
  {
    //编辑最后一个时间段
    if (m_nSelRect == nAreaCount-1)
    {
      if (ptDrag.x > rt.right)
      {
        ptRet.x = rt.right;
        ptRet.y = ptEdit.m_pointPair.m_ptEnd.y;
      }

      if (ptDrag.x <= ptEdit.m_pointPair.m_ptBegin.x)
      {
        ptRet.x = ptEdit.m_pointPair.m_ptEnd.x;
        ptRet.y = ptEdit.m_pointPair.m_ptEnd.y;
      }

      return false;
    }

    int nNext = m_nSelRect + 1;
    if (nNext >= nAreaCount)
    {
      TRACE(_T("nNext >= nAreaCount false"));
      return false;
    }

    if (ptDrag.x <= ptEdit.m_pointPair.m_ptBegin.x + nValidSpace)
    {
      //       ptRet.x = ptEdit.m_pointPair.m_ptBegin.x + nValidSpace;
      //       ptRet.y = ptEdit.m_pointPair.m_ptBegin.y;
      ptRet.x = ptEdit.m_pointPair.m_ptEnd.x;
      ptRet.y = ptEdit.m_pointPair.m_ptEnd.y;

      return false;
    }

    const POINTTIMEEDIT& ptEditNext = m_pPointTimeEditList->at(nNext);
    if (ptDrag.x >= ptEditNext.m_pointPair.m_ptBegin.x-nValidSpace)
    {
      CString str;
      str.Format(_T("ReEditTail ptDrag.x=%d,m_ptBegin.x=%d, false"), ptDrag.x, ptEditNext.m_pointPair.m_ptBegin.x);
      TRACE(str);

      ptRet.x = ptEditNext.m_pointPair.m_ptBegin.x-nValidSpace;
      ptRet.y = ptEditNext.m_pointPair.m_ptBegin.y;

      return false;
    }
  }

  return true;
}

bool CTimerShaft::IsEndPointCrossBlock(const CPoint& ptEnd, CPoint& ptOut)
{
  CAutoLock l(m_cs);

  POINTTIMEEDITLIST::const_iterator it = m_pPointTimeEditList->begin();
  for (; it != m_pPointTimeEditList->end(); ++it)
  {
    if (m_ptBegin.x < it->m_pointPair.m_ptBegin.x)
    {
      if (ptEnd.x > it->m_pointPair.m_ptBegin.x)
      {
        ptOut = it->m_pointPair.m_ptBegin;
        ptOut.x -= TORANCE;

        return true;
      }
    }
    else if (m_ptBegin.x > it->m_pointPair.m_ptEnd.x)
    {
      for (; it != m_pPointTimeEditList->end(); ++it)
      {
        if (m_ptBegin.x > it->m_pointPair.m_ptEnd.x)
        {
          continue;
        }
        else
        {
          break;
        }
      }

      if (it == m_pPointTimeEditList->end())
      {
        return false;
      }

      if (ptEnd.x > it->m_pointPair.m_ptBegin.x)
      {
        ptOut = it->m_pointPair.m_ptBegin;
        ptOut.x -= TORANCE;

        return true;
      }
    }
  }

  return false;
}

BOOL CTimerShaft::FTP_Upload(const CString& strURL, const CString& strServer, const CString& strFilePath, const CString& strFileName)
{
  const TCHAR szUser[] = _T("ftp");
  const TCHAR szPwd[] = _T("50171714");

  BOOL bRet = FALSE;

  CString strURL_ = strURL.Right(strURL.GetLength() - _tcslen(_T("http://")));
  CInternetSession* pInetsession = nullptr;
  CFtpConnection* pFtpConnection = nullptr;

  try
  {
    pInetsession = new CInternetSession(m_strAppName, 1, PRE_CONFIG_INTERNET_ACCESS);
    //这里用你的用户名，密码连接到ftp服务器 ,其中usr是用户名，pwd是密码，ftpSite.com，是你想要连接的ftp服务器.   
    pFtpConnection = pInetsession->GetFtpConnection(strURL_, szUser, szPwd, INTERNET_INVALID_PORT_NUMBER, TRUE);
    if (nullptr == pFtpConnection)
    {
      g_statusLog.StatusOut(_T("pInetsession->GetFtpConnection null"));
      return false;
    }

    strURL_ += _T("/vrs/editdir");
    CString strURL_Inner = _T("/vrs/editdir");
    bRet = pFtpConnection->SetCurrentDirectory(strURL_Inner);
    if (FALSE == bRet)
    {
      g_statusLog.StatusOut(_T("SetCurrentDirectory failed"));

      DWORD dwError = GetLastError();
      CString strDir;
      bRet = pFtpConnection->GetCurrentDirectoryAsURL(strDir);
    }
  }
  catch(CInternetException *pEx)
  {
    pEx->ReportError(MB_ICONEXCLAMATION);
    pFtpConnection = NULL;
    pEx->Delete();

    g_statusLog.StatusOut(_T("无法与服务器建立连接"));
  }

  CFileFind Finder;
  CString strFileName_;
  CString strFileRoot;
  // 这里c:\\Myfile.bmp 是你要上传的文件名称
  // 文件没有必要是一个bmp图片，你可以根据需要上传任何图片  
  // CString strFileName 确保上传到服务器上的文件保持原来的名称，当然，你也可以改成任何其他的  
  // 名称

  if(Finder.FindFile(strFilePath, INTERNET_FLAG_DONT_CACHE)==TRUE)
  {
    Finder.FindNextFile();
    strFileName_ = Finder.GetFileName();
    strFileRoot = Finder.GetRoot();
    Finder.Close();
  }

  strFileRoot += strFileName;

  if (pFtpConnection)
  {
    bRet = pFtpConnection->PutFile(strFilePath, strFileName, FTP_TRANSFER_TYPE_BINARY, 1);
    if (FALSE == bRet)
    {
      g_statusLog.StatusOut(_T("pFtpConnection->PutFile fail, errorcode = %d"), ::GetLastError());
    }
  }

  delete pInetsession;
  pInetsession = nullptr;

  if(bRet)
  {
    g_statusLog.StatusOut(_T("上传成功"));
  }
  else 
  {
    g_statusLog.StatusOut(_T("上传失败"));
  }

  return bRet == 0 ? false : true;
}

BOOL CTimerShaft::Http_Upload(LPCTSTR strURL, LPCTSTR strLocalFileName)
{
  ASSERT(strURL != NULL && strLocalFileName != NULL);

  BOOL bResult = FALSE;
  DWORD dwType = 0;
  CString strServer;
  CString strObject;
  INTERNET_PORT wPort = 0;
  DWORD dwFileLength = 0;
  char * pFileBuff = NULL;
  CHttpConnection * pHC = NULL;
  CHttpFile * pHF = NULL;
  CInternetSession* pInetsession = new CInternetSession(m_strAppName, 1, PRE_CONFIG_INTERNET_ACCESS);

  bResult =  AfxParseURL(strURL, dwType, strServer, strObject, wPort);
  if(!bResult)
    return FALSE;
  CFile file;
  try
  {
    if(!file.Open(strLocalFileName, CFile::shareDenyNone | CFile::modeRead))
      return FALSE;
    dwFileLength = file.GetLength();
    if(dwFileLength <= 0)
      return FALSE;
    pFileBuff = new char[dwFileLength];
    memset(pFileBuff, 0, sizeof(char) * dwFileLength);
    file.Read(pFileBuff, dwFileLength);
    const int nTimeOut = 5000;
    pInetsession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut); //联接超时设置
    pInetsession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);  //重试1次
    pHC = pInetsession->GetHttpConnection(strServer, wPort);  //取得一个Http联接
    pHF = pHC->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
    if(!pHF->SendRequest(NULL, 0, pFileBuff, dwFileLength))
    {
      delete[]pFileBuff;
      pFileBuff = NULL;
      pHF->Close();
      pHC->Close();
      pInetsession->Close();
      return FALSE;
    }
    DWORD dwStateCode = 0;
    pHF->QueryInfoStatusCode(dwStateCode);
    if(dwStateCode == HTTP_STATUS_OK)
      bResult = TRUE;
  }
  catch(CInternetException * pEx)
  {
    char sz[256] = "";
    pEx->GetErrorMessage(sz, 25);
    CString str;
    str.Format("InternetException occur!\r\n%s", sz);
    AfxMessageBox(str);
  }
  catch(CFileException& fe)
  {
    CString str;
    str.Format("FileException occur!\r\n%d", fe.m_lOsError);
    AfxMessageBox(str);
  }
  catch(...)
  {
    DWORD dwError = GetLastError();
    CString str;
    str.Format("Unknow Exception occur!\r\n%d", dwError);
    AfxMessageBox(str);
  }

  delete [] pFileBuff;
  pFileBuff = NULL;

  file.Close();
  pHF->Close();
  pHC->Close();
  pInetsession->Close();
  delete pInetsession;
  pInetsession = nullptr;

  return bResult;
}

BOOL CTimerShaft::UpLoad(const CString& strURL, const CString& strServer, const CString& strFilePath, const CString& strFileName)
{
  return theApp.UpLoadFile(strFilePath);
}


BOOL CTimerShaft::DelTimeBlock()
{
  // #ifdef _DEBUG
  //   g_statusLog.StatusOut(_T("DelTimeBlock before:"));
  //   LogTimeBlock();
  // #endif

  m_bReEditHead = m_bReEditTail = false;
  bool bSelBlock = false;

  CAutoLock l(m_cs);

  if (m_rtList.empty())
  {
    return FALSE;
  }

  RECTLIST::iterator it = m_rtList.begin();
  for (; it != m_rtList.end(); ++it)
  {
    const CRect& rt = *it;
    if (PtInRect(&(*it), m_PtSel))
    {
      bSelBlock = true;
      break;
    }
  }

  CUTTIMEPOINTLIST::iterator it_cp = m_pCutTimePtList->begin();

  if (bSelBlock)
  {
    m_rtList.erase(it);
    //删除两个时间点
    POINTTIMEEDITLIST::iterator it = m_pPointTimeEditList->begin();
    CS_CUT_TIME_LIST::iterator it_cutTimeList = m_EditTask.cutTimes.begin();

    for (; it != m_pPointTimeEditList->end(), it_cp != m_pCutTimePtList->end(), 
      it_cutTimeList != m_EditTask.cutTimes.end(); ++it, ++it_cp, ++it_cutTimeList)
    {
      if ((m_PtSel.x >= (*it).m_pointPair.m_ptBegin.x) && (m_PtSel.x <= (*it).m_pointPair.m_ptEnd.x))
      {
        SAFE_DELETE((*it).m_TimeEditPair.m_pTimeEditBegin);
        SAFE_DELETE((*it).m_TimeEditPair.m_pTimeEditEnd);

        m_pPointTimeEditList->erase(it);
        m_pCutTimePtList->erase(it_cp);
        m_EditTask.cutTimes.erase(it_cutTimeList);

        break;
      }
    }    

    m_Editdlg.RedrawWindow();

    return TRUE;
  }

  return FALSE;
}

void CTimerShaft::SortCuttimeRect()
{
  CAutoLock l(m_cs);

  if (m_rtList.empty())
  {
    return;
  }

  for (unsigned i = 0; i < m_rtList.size() - 1; ++i)
  {
    for (unsigned j = i + 1; j < m_rtList.size(); ++j)
    {
      if (m_rtList[i].left > m_rtList[j].left)
      {
        CRect rtTemp = m_rtList[i];
        m_rtList[i] = m_rtList[j];
        m_rtList[j] = rtTemp;
      }
    }
  }
}

void CTimerShaft::SortPointTimeEdit()
{
  CAutoLock l(m_cs);

  for (unsigned i = 0; i < m_pPointTimeEditList->size() - 1; ++i)
  {
    for (unsigned j = i + 1; j < m_pPointTimeEditList->size(); ++j)
    {
      if ((*m_pPointTimeEditList)[i].m_pointPair.m_ptBegin.x > (*m_pPointTimeEditList)[j].m_pointPair.m_ptBegin.x)
      {
        POINTTIMEEDIT ptTemp = (*m_pPointTimeEditList)[i];
        (*m_pPointTimeEditList)[i] = (*m_pPointTimeEditList)[j];
        (*m_pPointTimeEditList)[j] = ptTemp;
      }
    }
  }
}

void CTimerShaft::SortCuttimePoint()
{
  CAutoLock l(m_cs);

  for (unsigned i = 0; i < m_pCutTimePtList->size() - 1; ++i)
  {
    for (unsigned j = i + 1; j < m_pCutTimePtList->size(); ++j)
    {
      if ((*m_pCutTimePtList)[i].m_BeginEndPoint.m_ptBegin.x > (*m_pCutTimePtList)[j].m_BeginEndPoint.m_ptBegin.x)
      {
        CUTTIMEPOINT temp = (*m_pCutTimePtList)[i];
        (*m_pCutTimePtList)[i] = (*m_pCutTimePtList)[j];
        (*m_pCutTimePtList)[j] = temp;
      }
    }
  }
}


void CTimerShaft::LogTimeBlock()
{
  CString strFmt;

  RECTLIST::const_iterator it = m_rtList.begin();
  for (; it != m_rtList.end(); ++it)
  {    
    strFmt.Format(_T("L:%d, T:%d, R:%d, B:%d"), it->left, it->top, it->right, it->bottom);
    g_statusLog.StatusOut(strFmt);
  }

  //   // 剪切时间段
  //   typedef struct _CS_CUT_TIME
  //   {
  //     int64_t startTimeInUs;      // 当前剪切时间段的开始时间，源文件中的时间
  //     int64_t endTimeInUs;        // 当前剪切时间段的结束时间，源文件中的时间
  //     // endTimeInUs为，代表使用整个文件
  // 
  //     CS_STREAM_MAP streamMaps[MaxAvVideoStreamNum];
  //     int streamMapNum;
  // 
  //     int switch_mode;            // 转场效果, -1 for none switch mode
  //   } CS_CUT_TIME;
  // 
  //   typedef struct tagPointTimeEdit
  //   {
  //     BEGINENDPOINT m_pointPair;
  //     BEGINENDTIMEEDIT m_TimeEditPair;

  strFmt.Empty();
  CUTTIMEPOINTLIST::const_iterator it_cp = m_pCutTimePtList->begin();
  for (; it_cp != m_pCutTimePtList->end(); ++it_cp)
  {
    strFmt.Format(_T("startTimeInUs: %l64d \
                     endTimeInUs: %l64d \
                     src_file_path: %s \
                     a_stream0_dst_id: %d \
                     v_stream0_dst_id: %d \
                     streamMapNum: %d \
                     switch_mode: %d \
                     ptBegin: %l, %l \
                     ptEnd: %l, %l"), 
                     it_cp->m_CSCutTime.startTimeInUs, 
                     it_cp->m_CSCutTime.endTimeInUs, 
                     it_cp->m_CSCutTime.streamMaps->src_file_path, 
                     it_cp->m_CSCutTime.streamMaps->a_stream0_dst_id, 
                     it_cp->m_CSCutTime.streamMaps->v_stream0_dst_id, 
                     it_cp->m_CSCutTime.streamMapNum, 
                     it_cp->m_CSCutTime.switch_mode, 
                     it_cp->m_BeginEndPoint.m_ptBegin.x, 
                     it_cp->m_BeginEndPoint.m_ptBegin.y, 
                     it_cp->m_BeginEndPoint.m_ptEnd.x, 
                     it_cp->m_BeginEndPoint.m_ptEnd.y);

    g_statusLog.StatusOut(strFmt);
  }
}


unsigned __stdcall CTimerShaft::UpLoadThread(void* pParam)
{
  CTimerShaft* pThis = (CTimerShaft*)pParam;
  if (pThis)
  {
    pThis->Main();
  }

  return 0;
}

void CTimerShaft::Main()
{
  m_bUpLoadSuccess = _UpLoad();
  ::SetEvent(m_hUpLoadEvent);
}


BOOL CTimerShaft::_UpLoad()
{
  return (UpLoad(m_ULTitle.m_strURL, m_ULTitle.m_strServer, m_ULTitle.m_strPath, m_ULTitle.m_strName) &&
    UpLoad(m_ULTail.m_strURL, m_ULTail.m_strServer, m_ULTail.m_strPath, m_ULTail.m_strName));
}

void CTimerShaft::SetCuttimes(CS_CUT_TIME* pCutTime, int nCutTimeCount)
{
  CAutoLock l(m_cs);

  if (!m_pCutTimePtList)
  {
    return;
  }

  for (int i = 0; i < nCutTimeCount; ++i)
  {
    CUTTIMEPOINT* pCp = new CUTTIMEPOINT;
    memcpy(&pCp->m_CSCutTime, &pCutTime[i], sizeof(CS_CUT_TIME));

    //算坐标
    float dScaleRate = (float)m_rt.Width() / (float)(m_Seconds/1000);  //vlc获取时间值截短了，为ms级
    int nBeginX = (pCutTime[i].startTimeInUs/(1000*1000)) * dScaleRate;
    int nEndX = (pCutTime[i].endTimeInUs/(1000*1000)) * dScaleRate;

    CRect rt;
    GetClientRect(&rt);
    rt.top += 20;
    rt.bottom -= 20;
    rt.left = nBeginX;
    rt.right = nEndX;

    CRect rtTimeEdit = rt;
    rtTimeEdit.top = rt.bottom;
    rtTimeEdit.bottom = rtTimeEdit.top + 20;

    m_PointTimeEdit.m_pointPair.m_ptBegin.x = nBeginX;
    m_PointTimeEdit.m_pointPair.m_ptBegin.y = rtTimeEdit.top;
    m_PointTimeEdit.m_pointPair.m_ptEnd.x = nEndX;
    m_PointTimeEdit.m_pointPair.m_ptEnd.y = rtTimeEdit.bottom;


    m_PointTimeEdit.m_TimeEditPair.m_pTimeEditBegin = CreateNewTimeEdit(pCutTime[i].startTimeInUs/(1000*1000), CPoint(nBeginX, rt.bottom), rtTimeEdit, true);
    m_PointTimeEdit.m_TimeEditPair.m_pTimeEditEnd = CreateNewTimeEdit(pCutTime[i].endTimeInUs/(1000*1000), CPoint(nEndX, rt.bottom), rtTimeEdit, false);
    m_pPointTimeEditList->push_back(m_PointTimeEdit);

    pCp->m_BeginEndPoint.m_ptBegin.x = nBeginX;
    pCp->m_BeginEndPoint.m_ptBegin.y = rt.top;
    pCp->m_BeginEndPoint.m_ptEnd.x = nEndX;
    pCp->m_BeginEndPoint.m_ptEnd.y = rt.bottom;

    m_pCutTimePtList->push_back(*pCp);
    m_rtList.push_back(rt);
  }
}


LRESULT CTimerShaft::OnRemoveEditTask(WPARAM wParam, LPARAM lParam)
{
  RemoveEditTask();

  return 0;
}




void CTimerShaft::OnTimer(UINT_PTR nIDEvent)
{
  // TODO: Add your message handler code here and/or call default
  //超过10秒没有发送数据，关闭uploadsocket

  CStatic::OnTimer(nIDEvent);
}


void CTimerShaft::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  if (false == IsPtInSelectedMp4Rect())               //与文件名对应的编辑框
  {
    return;
  }

  SetCapture();

  m_bLBtnDown = true;
  m_bReEditHead = false;
  m_bReEditTail = false;

  //   CPoint ptCur;
  CRect rt;

  GetClientRect(&rt);
  m_ptEnd = (0, 0);
  m_ptEndTemp = (0, 0);
  //   GetCursorPos(&ptCur);
  //   ::MapWindowPoints(NULL, this->GetSafeHwnd(), &ptCur, 1);
  //   if (ptCur.x <= 0)
  //   {
  //     ptCur.x = 0;
  //   }
  // 
  //   if (ptCur.x >= rt.Width())
  //   {
  //     ptCur.x = rt.Width();
  //   }

  GetEditCuttimeStatue(rt, point, m_bReEditHead, m_bReEditTail, m_bLBtnDown);
  if (!m_bLBtnDown)
  {
    return;
  }

  ZeroMemory(&m_PointTimeEdit, sizeof(POINTTIMEEDIT));
  CTimeEdit* pTimeEdit = nullptr;
  if (m_pCutTimePtList->empty() || !(m_bReEditHead || m_bReEditTail))   //new crop area
  {

    if (m_rtList.size()>= 4)
    {
      m_bLBtnDown = false;
      //       AfxMessageBox(_T("最多只能有4段"));
      return;
    }

    m_ptBegin = point;
    m_bReEditHead = m_bReEditTail = false;

    int64_t nCurTime = (((float)m_ptBegin.x / (float)rt.Width())) * (m_Seconds/1000);    //ms
    m_pNewTimeEdit = pTimeEdit = CreateNewTimeEdit(nCurTime, point, rt, true);
    if (nullptr == pTimeEdit)
    {
      return;
    }

    m_PointTimeEdit.m_pointPair.m_ptBegin.x = point.x;
    m_PointTimeEdit.m_pointPair.m_ptBegin.y = point.y;
    m_PointTimeEdit.m_TimeEditPair.m_pTimeEditBegin = pTimeEdit;

    SetCuttimePoint(rt, m_ptBegin, true);
  }
  else
  {
    //查找选中的point对应的时间点及timeEdit
    m_pSelCutTime = const_cast<CS_CUT_TIME*>(SearchCutTimeByPoint(point, rt));
    m_pSelTimeEdit = SearchTimeEditByPoint(point, rt);

    m_nSelRect = SearchRectIndex(point);    
  }

  ChangePos(point);

  CStatic::OnLButtonDown(nFlags, point);
}
