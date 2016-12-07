// ProgressCtrlView.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "ProgressCtrlView.h"
#include "MediaPlayerDialog.h"
#include "FileEditDialog.h"

// CProgressCtrlView

IMPLEMENT_DYNCREATE(CProgressCtrlView, CView)

CProgressCtrlView::CProgressCtrlView()
{
  m_pDlg = NULL;
}

CProgressCtrlView::~CProgressCtrlView()
{
  SAFE_DELETE(m_pDlg);
}

BEGIN_MESSAGE_MAP(CProgressCtrlView, CView)
  ON_WM_SIZE()
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_SIZE()
END_MESSAGE_MAP()


// CProgressCtrlView drawing

void CProgressCtrlView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CProgressCtrlView diagnostics

#ifdef _DEBUG
void CProgressCtrlView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CProgressCtrlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CProgressCtrlView message handlers


int CProgressCtrlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Add your specialized creation code here
  if (!m_pDlg)
  {
    m_pDlg = new CFileEditDialog;
    if (NULL == m_pDlg)
    {
      return -1;
    }

    if (!m_pDlg->Create(IDD_DIALOG_EDIT, this))
    {
      return -1;
    }    
  }

  const unsigned nHeight = 100;

  CRect rt;
  GetClientRect(&rt);

  rt.bottom = rt.top + MAX_TASKLIST_NUM * nHeight;

  m_pDlg->MoveWindow(&rt);
  m_pDlg->ShowWindow(SW_SHOW);

  return 0;
}


void CProgressCtrlView::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // TODO: Add your message handler code here
  // Do not call CView::OnPaint() for painting messages
  
}


void CProgressCtrlView::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  if (m_pDlg)
  {
    const unsigned nHeight = 100;

    CRect rt;
    GetClientRect(&rt);

    rt.bottom = rt.top + MAX_TASKLIST_NUM * nHeight;

    m_pDlg->MoveWindow(&rt);
  }
}

void CProgressCtrlView::Release()
{
  m_pDlg->Release();
  
}