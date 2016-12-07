// SettingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "SettingDialog.h"
#include "afxdialogex.h"
#include "ImagePreviewDialog.h"
#include "ImagePrview.h"
#include "MainFrm.h"
#include "TimerShaft.h"

#include <algorithm>

extern CmediaPlayerApp theApp;
#define MAX_DIR_LEN 512

// CSettingDialog dialog

IMPLEMENT_DYNAMIC(CSettingDialog, CDialog)

CSettingDialog::CSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDialog::IDD, pParent)
  , m_strTitlePath(_T(""))
  , m_strTailPath(_T(""))
  , m_strTitleTime(_T(""))
  , m_strTailTime(_T(""))
  , m_strFPS(_T(""))
  , m_strDestFile(_T(""))
  , m_timerShaft((CTimerShaft&)*(CTimerShaft*)pParent)
{

}

CSettingDialog::~CSettingDialog()
{
}

void CSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

  DDX_Text(pDX, IDC_EDIT_TITLEPATH, m_strTitlePath);
  DDX_Text(pDX, IDC_EDIT_TAILPATH, m_strTailPath);
  DDX_Text(pDX, IDC_EDIT_TITLETIME, m_strTitleTime);
  DDX_Text(pDX, IDC_EDIT_TAILTIME, m_strTailTime);
  DDX_Text(pDX, IDC_EDIT_FPS, m_strFPS);
  DDX_Text(pDX, IDC_EDIT_DESTFILE, m_strDestFile);
}


BEGIN_MESSAGE_MAP(CSettingDialog, CDialog)
  ON_BN_CLICKED(IDOK, &CSettingDialog::OnBnClickedOk)
  ON_WM_KEYDOWN()
  ON_BN_CLICKED(IDC_BUTTON_TITLE, &CSettingDialog::OnBnClickedButtonTitle)
  ON_BN_CLICKED(IDC_BUTTON_TAIL, &CSettingDialog::OnBnClickedButtonTail)
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CSettingDialog message handlers

static int StrToInt(const CString& str)
{
  int nRet = 0;

  for (int i = 0; i < str.GetLength(); ++i)
  {
    nRet = nRet * 10 + str[i] - '0';
  }

  return nRet;
}

BOOL CSettingDialog::IsSameTaskName()
{
  std::vector<CString>::iterator it = m_timerShaft.GetTaskNameList().begin();
  for (; it != m_timerShaft.GetTaskNameList().end(); ++it)
  {
    if (*it == m_strDestFile)
    {
      return TRUE;
    }
  }

  return FALSE;
}

void CSettingDialog::OnBnClickedOk()
{
  // TODO: Add your control notification handler code here

  UpdateData();

//   if (((m_strTitlePath.IsEmpty() || *m_strTitlePath == _T('\0')) && (!m_strTitleTime.IsEmpty() || *m_strTitleTime == _T('\0')))
//       || ((m_strTailPath.IsEmpty() || *m_strTailPath == _T('\0')) && (!m_strTailTime.IsEmpty() || *m_strTailTime == _T('\0'))) 
//       || ((!m_strTitlePath.IsEmpty() || *m_strTitlePath == _T('\0')) && (m_strTitleTime.IsEmpty() || *m_strTitleTime == _T('\0'))) 
//       || ((!m_strTailPath.IsEmpty() || *m_strTailPath == _T('\0')) && (m_strTailTime.IsEmpty() || *m_strTailTime == _T('\0'))))
//   {
//     if (FALSE == IsDestFileNameLegal())
//     {
//       AfxMessageBox(_T("文件名不规范，只能由字母、数字、下划线组成"));
//     }
// 
//     return;
//   }
  if (m_strFPS.IsEmpty() || *m_strFPS == _T('\0'))
  {
    return;
  }

  //检查是否重名  
  if (IsSameTaskName())
  {
    AfxMessageBox("已存在相同的任务名");
    return;
  }

  m_timerShaft.GetTaskNameList().push_back(m_strDestFile);

  m_nTitleDuration = StrToInt(m_strTitleTime);
  m_nTailDuration = StrToInt(m_strTailTime);

  CImagePreviewDialog* pImagePrevDlg = ((CImagePrview*)(((CMainFrame*)theApp.GetMainWnd())->GetSplitterWnd2().GetPane(2, 0)))->GetImagePrevDlg();
  ::SendMessage(pImagePrevDlg->GetSafeHwnd(), WM_ADDPIC, WPARAM(true), (LPARAM)(&m_strTitlePath));
  ::SendMessage(pImagePrevDlg->GetSafeHwnd(), WM_ADDPIC, WPARAM(false), (LPARAM)(&m_strTailPath));

  if (!m_strDestFile.IsEmpty())
  {
    m_EditOpts.SetDstFilePath(m_strDestFile, m_strDestFile.GetLength());
  }
  m_EditOpts.SetTitlePicPath(m_strTitlePicName, m_strTitlePicName.GetLength());
  m_EditOpts.SetAddTitle(true);

  m_EditOpts.SetTailPicPath(m_strTailPath, m_strTailPath.GetLength());
  m_EditOpts.SetAddTail(true);

  CDialog::OnOK();
}

int CSettingDialog::IsLegalInput(const CString& str)
{
  if (str.IsEmpty())
  {
    return e_empty;
  }

  for (int i = 0; i < str.GetLength(); ++i)
  {
    if (str[i] < '0' || str[i] > '9')
    {
      return e_notallnumber;
    }
  }

  return e_legal;
}



void CSettingDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // TODO: Add your message handler code here and/or call default
  int nRet = -1;
  switch (nChar)
  {
  case VK_TAB:
    {
      if (GetFocus() == GetDlgItem(IDC_EDIT_TITLEPATH))
      {
        nRet = IsLegalInput(m_strTitlePath);
        if (e_empty == nRet)
        {
          AfxMessageBox(_T("输入为空"));
        }

        if (e_notallnumber == nRet)
        {
          AfxMessageBox(_T("必须都为0-9的数字"));
        }
      }

      if (GetFocus() == GetDlgItem(IDC_EDIT_TAILPATH))
      {
        nRet = IsLegalInput(m_strTailPath);
        if (e_empty == nRet)
        {
          AfxMessageBox(_T("输入为空"));
        }

        if (e_notallnumber == nRet)
        {
          AfxMessageBox(_T("必须都为0-9的数字"));
        }
      }

      if (GetFocus() == GetDlgItem(IDC_EDIT_TITLETIME))
      {
        nRet = IsLegalInput(m_strTitleTime);
        if (e_empty == nRet)
        {
          AfxMessageBox(_T("输入为空"));
        }

        if (e_notallnumber == nRet)
        {
          AfxMessageBox(_T("必须都为0-9的数字"));
        }
      }

      if (GetFocus() == GetDlgItem(IDC_EDIT_TAILTIME))
      {
        nRet = IsLegalInput(m_strTailTime);
        if (e_empty == nRet)
        {
          AfxMessageBox(_T("输入为空"));
        }

        if (e_notallnumber == nRet)
        {
          AfxMessageBox(_T("必须都为0-9的数字"));
        }
      }

      if (GetFocus() == GetDlgItem(IDC_EDIT_FPS))
      {
        nRet = IsLegalInput(m_strFPS);
        if (e_empty == nRet)
        {
          AfxMessageBox(_T("输入为空"));
        }

        if (e_notallnumber == nRet)
        {
          AfxMessageBox(_T("必须都为0-9的数字"));
        }
      }
      
  	  break;
    }

  default:
    break;
  }

  CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CSettingDialog::OnBnClickedButtonTitle()
{
  // TODO: Add your control notification handler code here
  m_strTitlePath.Empty();
  m_strTitlePicName.Empty();

  AddPic(m_strTitlePath, m_strTitlePicName);

  UpdateData(FALSE);
}


void CSettingDialog::OnBnClickedButtonTail()
{
  // TODO: Add your control notification handler code here
  m_strTailPath.Empty();
  m_strTailPicName.Empty();

  AddPic(m_strTailPath, m_strTailPicName);

  UpdateData(FALSE);
}

void CSettingDialog::AddPic(CString& strName, CString& strPicName)
{
  static TCHAR BASED_CODE szFilter[] = _T("jpg (*.jpg)|*.jpg|")_T("png (*.png)|*.png|")_T("All Files (*.*)|*.*||");

  CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_HIDEREADONLY,
                  szFilter, this);
  if (IDOK == dlg.DoModal())
  {
    strName = dlg.GetPathName();   
    strPicName = dlg.GetFileName();
  }
}


BOOL CSettingDialog::PreTranslateMessage(MSG* pMsg)
{
  // TODO: Add your specialized code here and/or call the base class
  if (WM_KEYDOWN == pMsg->message)
  {
    if (GetDlgItem(IDC_EDIT_TITLETIME)->GetSafeHwnd() == pMsg->hwnd)
    {
      if (m_strTitlePath.IsEmpty())
      {
        AfxMessageBox(_T("未添加片头"));
        return TRUE;
      }
    }

    if (GetDlgItem(IDC_EDIT_TAILTIME)->GetSafeHwnd() == pMsg->hwnd)
    {
      if (m_strTailPath.IsEmpty())
      {
        AfxMessageBox(_T("未添加片尾"));
        return TRUE;
      }
    }

  }

  if (GetDlgItem(IDC_EDIT_DESTFILE)->GetSafeHwnd() == pMsg->hwnd)
  {
    m_ToolTip.RelayEvent(pMsg);
  }

  return CDialog::PreTranslateMessage(pMsg);
}


BOOL CSettingDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  BOOL bRet = EnableToolTips(TRUE);
  bRet = m_ToolTip.Create(this, TTS_ALWAYSTIP);
  m_ToolTip.Activate(TRUE);

  CWnd* pWnd = GetDlgItem(IDC_EDIT_DESTFILE);
  bRet = m_ToolTip.AddTool(pWnd, IDS_DESTFILE);
  m_ToolTip.SetDelayTime(300);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSettingDialog::OnMouseMove(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default

  CDialog::OnMouseMove(nFlags, point);
}

BOOL CSettingDialog::IsDestFileNameLegal()
{
  BOOL bLegal = FALSE;
  //合法文件名只能是字母、数字、下划线
  if (!m_strDestFile.IsEmpty())
  {
    for (int i = 0; i < m_strDestFile.GetLength(); ++i)
    {
      if ((m_strDestFile[i] >= 'a' && m_strDestFile[i] <= 'z') || 
         (m_strDestFile[i] >= 'A' && m_strDestFile[i] <= 'Z') ||
         (m_strDestFile[i] >= '0' && m_strDestFile[i] <= '9') ||
         m_strDestFile[i] == '-')
      {
        bLegal = TRUE;
      }
      else
      {
        bLegal = FALSE;
        break;
      }
    }
  }

  return bLegal;
}

