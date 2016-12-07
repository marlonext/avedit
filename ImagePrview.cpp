// ImagePrview.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "ImagePrview.h"
#include "ImagePreviewDialog.h"


// CImagePrview

IMPLEMENT_DYNCREATE(CImagePrview, CView)

CImagePrview::CImagePrview()
{
  m_pImagePreviewDlg = NULL;
}

CImagePrview::~CImagePrview()
{
//   SAFE_DELETE(m_pImagePreviewDlg);
}

BEGIN_MESSAGE_MAP(CImagePrview, CView)
  ON_WM_CREATE()
  ON_WM_SIZE()
END_MESSAGE_MAP()


// CImagePrview drawing

void CImagePrview::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CImagePrview diagnostics

#ifdef _DEBUG
void CImagePrview::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CImagePrview::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CImagePrview message handlers


int CImagePrview::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Add your specialized creation code here
  if (NULL == m_pImagePreviewDlg)
  {
    m_pImagePreviewDlg = new CImagePreviewDialog;
    if (!m_pImagePreviewDlg)
    {
      return -1;
    }
  }

  if (m_pImagePreviewDlg)
  {
    CRect rt;
    GetClientRect(&rt);
    m_pImagePreviewDlg->Create(IDD_DIALOG_IMAGEPREVIEW, this);
    m_pImagePreviewDlg->MoveWindow(&rt);
    m_pImagePreviewDlg->ShowWindow(SW_SHOW);
  }

  return 0;
}


void CImagePrview::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  CRect rt;
  GetClientRect(&rt);

  if (m_pImagePreviewDlg)
  {
    m_pImagePreviewDlg->MoveWindow(&rt);
  }
}
