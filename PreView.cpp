// PreView.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "PreView.h"
#include "MediaPlayerDialog.h"
#include "MainDialog.h"

// CPreView

IMPLEMENT_DYNCREATE(CPreView, CView)

CPreView::CPreView()
{
  m_pMediaPlayerDlg = NULL;
}

CPreView::~CPreView()
{
  SAFE_DELETE(m_pMediaPlayerDlg);
}

BEGIN_MESSAGE_MAP(CPreView, CView)
  ON_WM_SIZE()
  ON_WM_CREATE()
END_MESSAGE_MAP()


// CPreView drawing

void CPreView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

void CPreView::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  if (m_pMediaPlayerDlg)
  {
    CRect rt;
    GetClientRect(&rt);
    m_pMediaPlayerDlg->MoveWindow(&rt);
  }
}

// CPreView diagnostics

#ifdef _DEBUG
void CPreView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPreView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPreView message handlers
int CPreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Add your specialized creation code here
  if (NULL == m_pMediaPlayerDlg)
  {
    m_pMediaPlayerDlg = new CMediaPlayerDialog;
    if (!m_pMediaPlayerDlg)
    {
      return -1;
    }
  }

  if (m_pMediaPlayerDlg)
  {
    m_pMediaPlayerDlg->Create(IDD_DIALOG_PROGRESSCTRL, this);
    m_pMediaPlayerDlg->ShowWindow(SW_SHOW);
  }

  return 0;
}
