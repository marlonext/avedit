// MainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "MainDialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "MediaPlayerDialog.h"
#include "PreView.h"
#include "MainFrm.h"

#pragma comment(lib, "Version.lib ")

extern CMainFrame* g_pMainFrame;

// CMainDialog dialog

IMPLEMENT_DYNAMIC(CMainDialog, CDialogEx)

CMainDialog::CMainDialog(CFrameWnd* pFrameWnd, CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDialog::IDD, pParent)
{
  m_pFrameWnd = pFrameWnd;
}

CMainDialog::~CMainDialog()
{
}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDialog, CDialogEx)
  ON_WM_SIZE()
  ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMainDialog message handlers

static CString GetAppVersion()   
{   
  char cPath[200],szVersionBuffer[200];
  DWORD dwHandle,InfoSize;
  CString strVersion;
  ::GetModuleFileName(NULL,cPath,sizeof(cPath)); //首先获得版本信息资源的长度
  InfoSize = GetFileVersionInfoSize(cPath,&dwHandle); //将版本信息资源读入缓冲区
  if(InfoSize==0) return _T("None VerSion Supprot");
  char *InfoBuf = new char[InfoSize];
  GetFileVersionInfo(cPath,0,InfoSize,InfoBuf); //获得生成文件使用的代码页及文件版本
  unsigned int  cbTranslate = 0;
  struct LANGANDCODEPAGE {
    WORD wLanguage;
    WORD wCodePage;
  } *lpTranslate;
  VerQueryValue(InfoBuf, TEXT("\\VarFileInfo\\Translation"),(LPVOID*)&lpTranslate,&cbTranslate);
  // Read the file description for each language and code page.
  for( int i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
  {
    char  SubBlock[200];
    wsprintf( SubBlock,
      TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
      lpTranslate[i].wLanguage,
      lpTranslate[i].wCodePage);
    void *lpBuffer=NULL;
    unsigned int dwBytes=0;
    VerQueryValue(InfoBuf,
      SubBlock,
      &lpBuffer,
      &dwBytes);
    CString strTemp=(char *)lpBuffer;
    strVersion+=strTemp;

  }
  return strVersion; 
}

void CMainDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialogEx::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  CRect rt;
  GetClientRect(&rt);
  if (m_pFrameWnd)
  {
    m_pFrameWnd->MoveWindow(&rt);
  }
}


BOOL CMainDialog::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  this->ShowWindow(SW_HIDE | SW_SHOWMAXIMIZED);
//   CString strVersion = GetAppVersion();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMainDialog::UpdateFrameWnd(CFrameWnd* pFramWnd)
{
  if (pFramWnd)
  {
    m_pFrameWnd = pFramWnd;
  }
}

void CMainDialog::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  this->ShowWindow(SW_HIDE);
  CVLCPlayer::Instance()->Release();

  CSplitterWnd& split = g_pMainFrame->GetSplitterWnd2();
  CPreView* pFilePrevView = (CPreView*)(split.GetPane(0, 0));
  CMediaPlayerDialog* pPlayDlg = pFilePrevView->GetMediaPlayDlg();
  if (pPlayDlg)
  {
    ::SendMessage(pPlayDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, WPARAM(false), NULL);
  }

  CmediaPlayerApp* pApp = (CmediaPlayerApp*)AfxGetApp();
  pApp->SetTerminate(true);

//   CDialogEx::OnClose();
}
