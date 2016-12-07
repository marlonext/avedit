// FilePrevDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "FilePrevDialog.h"
#include "PreView.h"
#include "MainFrm.h"
#include "VLCPlayer.h"
#include "ProgressCtrlView.h"
#include "FileEditDialog.h"
#include "MediaPlayerDialog.h"
#include "SAStatusLog.h"


extern CmediaPlayerApp theApp;
extern CSAStatusLog g_statusLog;
// CFilePrevDialog dialog

IMPLEMENT_DYNAMIC(CFilePrevDialog, CDialog)

  CFilePrevDialog::CFilePrevDialog(CWnd* pParent /*=NULL*/)
  : CDialog(CFilePrevDialog::IDD, pParent),
  m_strMp4Name(_T(""))
{
  for (unsigned i = 0; i < MAX_EDIT_FILE_NUM; ++i)
  {
    m_UseFlagAry.push_back(0);
  }

  m_bSelect = false;
  m_nCurSelMenu = -1;
  m_nSelMp4 = -1;
  m_nLastSelMp4 = -1;
  m_bMp4Added = false;

  m_pPopupMenu.reset(new CMenu);
}

CFilePrevDialog::~CFilePrevDialog()
{
  m_filePathAry.clear();
  m_fileNameAry.clear();
  m_stFileInfoAryL.clear();
  m_stFileInfoAryS.clear();
  m_pPopupMenu->DestroyMenu();
}

void CFilePrevDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFilePrevDialog, CDialog)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_MESSAGE(WM_SETFILEICON, OnSetFileIcon)
  ON_WM_RBUTTONDOWN()
  ON_COMMAND_RANGE(ID_ADD_MP4, ID_PREVIEW_MP4, OnMP4Operation)
  ON_UPDATE_COMMAND_UI_RANGE(ID_ADD_MP4, ID_PREVIEW_MP4, OnMP4OperationUpdataUI)
  ON_WM_CTLCOLOR()
  ON_WM_SIZE()
  ON_MESSAGE(WM_ADDMP4, OnAddMp4)
  ON_MESSAGE(WM_SHOWMP4NAME, OnShowMp4Name)
END_MESSAGE_MAP()


// CFilePrevDialog message handlers


void CFilePrevDialog::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // TODO: Add your message handler code here
  // Do not call CDialog::OnPaint() for painting messages
  if (m_nSelMp4 != -1)
  {
    dc.SetTextColor(RGB(0, 0, 255));
  }
}


void CFilePrevDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  g_statusLog.StatusOut("CFilePrevDialog::OnLButtonDown");

  int nSelMp4 = -1;

  if (!m_bMp4Added || (false == IsMp4Selected(point, nSelMp4)))
  {
    return;
  }

  m_nSelMp4 = nSelMp4;

  CRect rt;
  GetClientRect(&rt);
  if (PtInRect(&rt, point))
  {
    InvalidateRect(&rt);
  }
  else
  {
    return;
  }

  CFileEditDialog* pEditDlg = NULL;
  CMediaPlayerDialog* pPlayDlg = NULL;
  CPreView* pView = (CPreView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(0, 0));
  if (pView)
  {
    pPlayDlg = pView->GetMediaPlayDlg();
    if (pPlayDlg)
    {
      pPlayDlg->SetMp4Path(m_filePathAry[m_nSelMp4]);
      ::PostMessage(pPlayDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, NULL, NULL);
    }
  }

  m_strMp4Name = m_filePathAry[m_nSelMp4];

  if (1 == m_UseFlagAry[m_nSelMp4])
  {
    return;
  }
  else
  {
    m_UseFlagAry[m_nSelMp4] = 1;
  }

  if (m_nLastSelMp4 != m_nSelMp4)
  {
    CVLCPlayer::Instance()->OpenMedia(m_strMp4Name);
    int64_t time = CVLCPlayer::Instance()->GetDuration();     //us

    g_statusLog.StatusOut("media time len: %lf", time);

    CProgressCtrlView* pView = (CProgressCtrlView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd().GetPane(0, 1));
    if (pView)
    {
      pEditDlg = pView->GetEditDlg();
      if (!pEditDlg)
      {
        return;
      }
    }

    pEditDlg->SetDuration(time);
    ::PostMessage(pEditDlg->GetSafeHwnd(), WM_DRAWTIMERSHAFT, (WPARAM)(&m_strMp4Name), (LPARAM)TRUE);
  }

  if (pEditDlg)
  {
    pEditDlg->SetCurMp4Name(m_strMp4Name);
  }

  m_bSelect = true;
  m_nLastSelMp4 = m_nSelMp4;

  CVLCPlayer::Instance()->Stop();

  CDialog::OnLButtonDown(nFlags, point);
}


BOOL CFilePrevDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  CRect rt;
  GetClientRect(&rt);

  const unsigned nHeight = 30;
  const unsigned nSpace = 10;

  CRect rtText = rt;

  rt.left += 5;
  rt.top += 5;
  rt.right = rt.left + 400;
  rt.bottom = rt.top + nHeight;

  rtText.top += rt.top;
  rtText.left = rt.left;
  rtText.right = rt.right;
  rtText.bottom = rtText.top + nHeight;
  //create MAX_EDIT_FILE_NUM CStatic, set icon and name
  for (unsigned i = 0; i < MAX_EDIT_FILE_NUM; ++i)
  {
    m_FileNameCtrl[i].Create(_T(""), WS_CHILD | WS_VISIBLE | SS_LEFT | 
      SS_CENTERIMAGE, rtText, this, NAME_STATIC_BASE + i);
    m_Mp4NameRect.push_back(rtText);
    rtText.top = rtText.bottom + nSpace;
    rtText.bottom = rtText.top + nHeight;
  }

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CFilePrevDialog::OnSetFileIcon(WPARAM wParam, LPARAM lParam)
{
  for (unsigned i = 0; i < m_fileNameAry.size(); ++i)
  {    
    m_FileNameCtrl[i].SetWindowText(m_fileNameAry[i]);
  }

  return 0;
}

LRESULT CFilePrevDialog::OnShowMp4Name(WPARAM wParam, LPARAM lParam)
{
  m_FileNameCtrl[static_cast<int>(wParam)].SetWindowText(m_fileNameAry[static_cast<int>(wParam)]);

  return 0;
}


void CFilePrevDialog::OnRButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  //create popup menu
  this->SetFocus();
  if (FALSE == m_pPopupMenu->CreatePopupMenu())
  {
    TRACE(_T("create popupmenu failed\r\n"));
    return;
  }

  CString str;
  str.LoadString(ID_ADD_MP4);
  BOOL bRet = m_pPopupMenu->AppendMenu(MF_STRING, ID_ADD_MP4, str);

  str.LoadString(ID_REMOVE_MP4);
  bRet = m_pPopupMenu->AppendMenu(MF_STRING, ID_REMOVE_MP4, str);

  str.LoadString(ID_CLEARALL_MP4);
  bRet = m_pPopupMenu->AppendMenu(MF_STRING, ID_CLEARALL_MP4, str);

  str.LoadString(ID_PREVIEW_MP4);
  bRet = m_pPopupMenu->AppendMenu(MF_STRING, ID_PREVIEW_MP4, str);

  CPoint pt;
  GetCursorPos(&pt);
  m_pPopupMenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);

  m_pPopupMenu->DestroyMenu();

  CDialog::OnRButtonDown(nFlags, point);
}

void CFilePrevDialog::OnMP4Operation(UINT nID)
{
  //   m_nCurSelMenu = nID - ID_ADD_MP4;

  switch (nID)
  {
  case ID_ADD_MP4:
    {
//       AddMp4();

      break;
    }

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

  default:
    {
      Mp4PrevView();
      break;
    }
  }
}

void CFilePrevDialog::OnMP4OperationUpdataUI(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_nCurSelMenu == pCmdUI->m_nID - ID_ADD_MP4);
}

bool CFilePrevDialog::AddMp4()
{
  static TCHAR BASED_CODE szFilter[] = _T("mp4 (*.mp4)|*.mp4|")_T("All Files (*.*)|*.*||");

  CFileDialog dlg(TRUE, NULL, NULL,
    OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_HIDEREADONLY,
    szFilter, this);

  dlg.m_ofn.nMaxFile = 20 * MAX_PATH;  //open 20 files
  TCHAR* pszFile = new TCHAR[dlg.m_ofn.nMaxFile];

  if (pszFile)
  {
    dlg.m_ofn.lpstrFile = pszFile;
    ZeroMemory(dlg.m_ofn.lpstrFile, dlg.m_ofn.nMaxFile * sizeof(TCHAR));
  }

  CString strPath, strFileName;
  SHFILEINFO stFileInfo = { 0 };

  if (IDOK == dlg.DoModal())
  {
    //get first file positon
    POSITION ps;
    ps = dlg.GetStartPosition();
    HICON hIconLarge = NULL, hIconSmall = NULL;

    while (ps != NULL)
    {
      strPath = dlg.GetNextPathName(ps);
      int nPos = strPath.ReverseFind(_T('\\'));
      TCHAR* pPath = strPath.GetBuffer() + nPos + sizeof(TCHAR);

      //get file name
      while (true)
      {
        if (*pPath == _T('\0'))
        {
          break;
        }

        strFileName += *(pPath++);
      }
      strFileName += _T('\0');

      //check whether add the same file
      bool bSameFile = false;
      if (m_filePathAry.size() != 0)
      {
        std::vector<CString>::iterator it = m_filePathAry.begin();

        for (; it != m_filePathAry.end(); ++it)
        {
          if (*it == strPath)
          {
            bSameFile = true;
            continue;
          }
        }
      }

      if (!bSameFile)
      {
        m_filePathAry.push_back(strPath);
        m_fileNameAry.push_back(strFileName);

        //get large icon and small icon
        DWORD_PTR nRet = ::SHGetFileInfo(strPath, 0, &stFileInfo, sizeof(stFileInfo),
          SHGFI_LARGEICON | SHGFI_OPENICON);
        m_stFileInfoAryL.push_back(stFileInfo);

        nRet = ::SHGetFileInfo(strPath, 0, &stFileInfo, sizeof(stFileInfo),
          SHGFI_SMALLICON | SHGFI_OPENICON);
        m_stFileInfoAryS.push_back(stFileInfo);

        strPath.Empty();
        strFileName.Empty();
      }          
    }

    PostMessage(WM_SETFILEICON);
    m_bMp4Added = true;

    delete [] pszFile;
    pszFile = NULL;

    return true;
  }

  delete [] pszFile;
  pszFile = NULL;

  return false;
}


bool CFilePrevDialog::RemoveMp4(const CPoint& pt)
{
  bool bSelected = false;
  int i = 0, j = 0;

  std::vector<CRect>::const_iterator it1 = m_Mp4NameRect.begin();
  //   std::vector<CRect>::const_iterator it2 = m_IconRect.begin();

  for (; it1 != m_Mp4NameRect.end(); ++it1)
  {
    if (PtInRect(&*it1, pt))
    {
      bSelected = true;
      break;
    }

    i++;
  }

  if (bSelected)
  {
    std::vector<CString>::iterator it3 = m_fileNameAry.begin();
    std::vector<CString>::iterator it4 = m_filePathAry.begin();
    std::vector<SHFILEINFO>::iterator it5 = m_stFileInfoAryL.begin();
    std::vector<SHFILEINFO>::iterator it6 = m_stFileInfoAryS.begin();
    for (; it3 != m_fileNameAry.end(); ++it3, ++it4, ++it5, ++it6)
    {
      if (i == j++)
      {

        SendMessage(WM_UPDATAUI, WPARAM(i), LPARAM(FALSE));
        CFileEditDialog* pEditDlg = ((CProgressCtrlView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd().GetPane(0, 1)))->GetEditDlg();
        if (pEditDlg)
        {
          ::SendMessage(pEditDlg->GetSafeHwnd(), WM_DRAWTIMERSHAFT, WPARAM(&m_filePathAry[i]), (LPARAM)FALSE);
        }

        m_fileNameAry.erase(it3);
        m_filePathAry.erase(it4);
        m_stFileInfoAryL.erase(it5);
        m_stFileInfoAryS.erase(it6);

        return true;
      }
    }
  }

  return false;
}

bool CFilePrevDialog::ClearAllMp4()
{
  CVLCPlayer::Instance()->Stop();

  SendMessage(WM_UPDATAUI, WPARAM(m_filePathAry.size()), LPARAM(TRUE));

  for (unsigned i = 0; i < m_UseFlagAry.size(); ++i)
  {
    m_UseFlagAry[i] = 0;
  }

  m_bSelect = false;
  m_nSelMp4 = -1;
  m_nLastSelMp4 = -1;
  m_bMp4Added = false;
  m_strMp4Name.Empty();
  m_fileNameAry.clear();
  m_filePathAry.clear();
  m_stFileInfoAryL.clear();
  m_stFileInfoAryS.clear();

  CFileEditDialog* pEditDlg = ((CProgressCtrlView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd().GetPane(0, 1)))->GetEditDlg();
  if (pEditDlg)
  {
    pEditDlg->ResetTimerShaft();
  }

  return true;
}

bool CFilePrevDialog::Mp4PrevView(const CPoint& pt)
{
  g_statusLog.StatusOut("CFilePrevDialog::Mp4PrevView");

  unsigned int i = 0;
  bool bSel = false;
  std::vector<CRect>::const_iterator it = m_Mp4NameRect.begin();

  for (i = 0; i < m_filePathAry.size(); ++i)
  {
    if (PtInRect(&*it++, pt))
    {
      bSel = true;
      break;
    }
  }

  if (false == bSel)
  {
    return false;
  }

  m_nSelMp4 = i;
  m_strMp4Name = m_filePathAry[i];

  CFileEditDialog* pEditDlg = NULL;
  CProgressCtrlView* pView = (CProgressCtrlView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd().GetPane(0, 1));
  if (pView)
  {
    pEditDlg = pView->GetEditDlg();
    if (!pEditDlg)
    {
      return false;
    }
  }
  if (pEditDlg)
  {
    pEditDlg->SetCurMp4Name(m_strMp4Name);
  }

  CRect rt;
  GetClientRect(&rt);
  InvalidateRect(&rt);
  CDC* pDC = GetDC();
  if (pDC)
  {
    pDC->SetTextColor(RGB(0, 0, 255));
  }

  //ÐÞ¸Ä²¥·Å×´Ì¬
  CMediaPlayerDialog* pPlayDlg = ((CPreView*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(0, 0)))->GetMediaPlayDlg();
  if (pPlayDlg)
  {
    ::SendMessage(pPlayDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, WPARAM(true), NULL);
  }

  const CString& strFileName = m_filePathAry[i];
  CVLCPlayer::Instance()->OpenMedia(strFileName);
  CVLCPlayer::Instance()->Play();

  return true;
}


bool CFilePrevDialog::IsMp4Selected(const CPoint& pt, int& nIndex)
{
  for (unsigned i = 0; i < m_filePathAry.size(); ++i)
  {
    if (PtInRect(&m_Mp4NameRect[i], pt)/* || PtInRect(&m_IconRect[i], pt)*/)
    {
      nIndex = i;
      return true;
    }
  }

  return false;
}


HBRUSH CFilePrevDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  // TODO:  Change any attributes of the DC here
  if (pWnd->GetDlgCtrlID() == NAME_STATIC_BASE + m_nSelMp4)
  {
    pDC->SetTextColor(RGB(0, 0, 255));
  }
  else
  {
    pDC->SetTextColor(RGB(0, 0, 0));
  }

  // TODO:  Return a different brush if the default is not desired
  return hbr;
}

void CFilePrevDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
}

void CFilePrevDialog::AdjustFlagAry(int nIndex)
{
  m_UseFlagAry[nIndex] = 0;
}

LRESULT CFilePrevDialog::OnAddMp4(WPARAM wParam, LPARAM lParam)
{
  bool bSameMp4 = false;

  for (int i = 0; i < m_fileNameAry.size(); i++)
  {
    if (m_strMp4Name == m_fileNameAry[i])
    {
      bSameMp4 = true;
      break;
    }
  }

  if (m_fileNameAry.empty())
  {
    m_filePathAry.push_back(m_strDir + _T("\\") + m_strMp4Name);
    m_fileNameAry.push_back(m_strMp4Name);

    PostMessage(WM_SHOWMP4NAME, (WPARAM)0, NULL);
    m_bMp4Added = true;
    
    return 0;
  }

  if (!bSameMp4)
  {
    m_filePathAry.push_back(m_strDir + _T("\\") + m_strMp4Name);
    m_fileNameAry.push_back(m_strMp4Name);

    PostMessage(WM_SHOWMP4NAME, (WPARAM)(m_fileNameAry.size()-1), NULL);
    m_bMp4Added = true;
  }

  return 0;
}

void CFilePrevDialog::UpdateUI()
{
  PostMessage(WM_CTLCOLOR);
}
