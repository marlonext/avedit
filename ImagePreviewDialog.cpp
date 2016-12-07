// ImagePreviewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "ImagePreviewDialog.h"
#include "afxdialogex.h"



// CImagePreviewDialog dialog

IMPLEMENT_DYNAMIC(CImagePreviewDialog, CDialog)

CImagePreviewDialog::CImagePreviewDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CImagePreviewDialog::IDD, pParent)
{
  m_pTitleImage = m_pTailImage = NULL;
  m_bAddTitle = m_bAddTail = false;
}

CImagePreviewDialog::~CImagePreviewDialog()
{
  m_pTitleImage->FreeData();
  m_pTailImage->FreeData();

  SAFE_DELETE(m_pTitleImage);
  SAFE_DELETE(m_pTailImage);

}

void CImagePreviewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImagePreviewDialog, CDialog)
  ON_WM_SIZE()
  ON_MESSAGE(WM_ADDPIC, OnAddPic)
  ON_WM_PAINT()
END_MESSAGE_MAP()


// CImagePreviewDialog message handlers


BOOL CImagePreviewDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  CRect rt;
  GetClientRect(&rt);

  RECT rtTitle = {0}, rtTail = {0};

  const int SPACE = 3;
  //   rtTitle = rt;
  rtTitle.top += rt.top + SPACE;
  rtTail.top = rtTitle.top;

  rtTitle.left += rt.left + SPACE;

  rtTitle.bottom = rt.bottom - SPACE;
  rtTail.bottom = rtTitle.bottom;

  if (rt.Width() > rt.Height())
  {
    rtTitle.right = rtTitle.left + (rt.Height()-2*SPACE);
    rtTail.left = rtTitle.right + SPACE;
    rtTail.right = rtTail.left + (rt.Height()-2*SPACE);
  }
  else
  {
    rtTitle.right = rtTitle.left + (rt.Width()-2*SPACE);
    rtTail.left = rtTitle.right + SPACE;
    rtTail.right = rtTail.left + (rt.Width()-2*SPACE);
  }

  m_pTitleImage = new CPictureCtrl;
  if (!m_pTitleImage)
  {
    return FALSE;
  }
  m_pTitleImage->Create(_T(""), WS_CHILD | WS_VISIBLE/* | WS_BORDER*/, rtTitle, this, IMAGECONTAINER_BASE);

  m_pTailImage = new CPictureCtrl;
  if (!m_pTailImage)
  {
    return FALSE;
  }
  m_pTailImage->Create(_T(""), WS_CHILD | WS_VISIBLE/* | WS_BORDER*/, rtTail, this, IMAGECONTAINER_BASE + 1);

  ATL::CImage TitleImage, TailImage;
  TitleImage.Load(m_strTitlePath);
  TailImage.Load(m_strTailPath);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CImagePreviewDialog::OnAddPic(WPARAM wParam, LPARAM lParam)
{
  bool bAddPicTitle = (bool)wParam;
  CString strPath = *((CString*)lParam);

  BOOL bAdd = FALSE;

  CBitmap* pBitmap = NULL;
  CRect rtTitle, rtTail;

  m_pTitleImage->GetWindowRect(&rtTitle);
  m_pTailImage->GetWindowRect(&rtTail);

  m_pTitleImage->InvalidateRect(&rtTitle);
  m_pTailImage->InvalidateRect(&rtTail);

  if (bAddPicTitle)
  {
    m_strTitlePath = strPath;
    bAdd = m_pTitleImage->LoadFromFile(strPath);    
  }
  else
  {
    m_strTailPath = strPath;
    bAdd = m_pTailImage->LoadFromFile(strPath);
  }
// 
//   HDC hDcCtrl = NULL;
//   if (bAddPicTitle)
//   {
//     m_bAddTitle = true;
//     m_strTitlePath = strPath;
// 
//     m_TitleImage.Destroy();
//     HRESULT hr = m_TitleImage.Load(strPath);
//     pBitmap = CBitmap::FromHandle(m_TitleImage);
//     hDcCtrl = ::GetDC(m_pTitleImage->GetSafeHwnd());    
//   }
//   else
//   {
//     m_bAddTail = true;
//     m_strTailPath = strPath;
// 
//     m_TailImage.Destroy();
//     HRESULT hr = m_TailImage.Load(strPath);
//     pBitmap = CBitmap::FromHandle(m_TailImage);
//     hDcCtrl = ::GetDC(m_pTailImage->GetSafeHwnd());
//   }
// 
//   CDC* pDC = CDC::FromHandle(hDcCtrl);
// 
//   CDC MemDC;
//   MemDC.CreateCompatibleDC(pDC);
//   CBitmap* pOldBitmap = MemDC.SelectObject(pBitmap);
// 
//   BOOL bRet = ::StretchBlt(hDcCtrl, 0, 0, rtTitle.Width(), 
//     rtTitle.Height(), MemDC.GetSafeHdc(), 0, 0, m_TitleImage.GetWidth(), 
//     m_TitleImage.GetHeight(), SRCCOPY);
// 
//   MemDC.SelectObject(pOldBitmap);
//   MemDC.DeleteDC();

  return 0;
}



void CImagePreviewDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  CRect rt;
  GetClientRect(&rt);

  RECT rtTitle = {0}, rtTail = {0};

  const int SPACE = 3;
  rtTitle.top += rt.top + SPACE;
  rtTail.top = rtTitle.top;

  rtTitle.left += rt.left + SPACE;
  
//   rtTitle.bottom = rt.bottom - SPACE;
//   rtTail.bottom = rtTitle.bottom;

  int nWidth = 0;
  if (rt.Width() > rt.Height())
  {
    nWidth = (rt.Width() - 3*SPACE) / 2;
    rtTitle.right = rtTitle.left + nWidth;
    rtTail.left = rtTitle.right + SPACE;
    rtTail.right = rtTail.left + nWidth;
  }
  
  if (rt.Width() < rt.Height())
  {
    nWidth = (rt.Height() - 3*SPACE) / 2;
    rtTitle.right = rtTitle.left + nWidth;
    rtTail.left = rtTitle.right + SPACE;
    rtTail.right = rtTail.left + nWidth;
  }

  rtTitle.bottom = rt.top + SPACE + nWidth;
  rtTail.bottom = rtTitle.bottom;

  if (m_pTitleImage)
  {
    m_pTitleImage->MoveWindow(&rtTitle);
  }

  if (m_pTailImage)
  {
    m_pTailImage->MoveWindow(&rtTail);
  }
}


void CImagePreviewDialog::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // TODO: Add your message handler code here
  // Do not call CDialogEx::OnPaint() for painting messages
  ShowTitleAndTailImage(dc);
}

void CImagePreviewDialog::ShowTitleAndTailImage(CDC& dc)
{
  if (!m_strTitlePath.IsEmpty() && m_pTitleImage)
  {
    m_pTitleImage->LoadFromFile(m_strTitlePath);
  }

  if (!m_strTailPath.IsEmpty() && m_pTailImage)
  {
    m_pTailImage->LoadFromFile(m_strTailPath);
  }
}


