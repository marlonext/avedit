// FilePreView.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "FilePreView.h"
#include "FilePrevDialog.h"
#include "TaskListDialog.h"



// CFilePreView

IMPLEMENT_DYNCREATE(CFilePreView, CView)

CFilePreView::CFilePreView()
{
  m_pTaskListDlg = nullptr;
}

CFilePreView::~CFilePreView()
{
  SAFE_DELETE(m_pTaskListDlg);
}

BEGIN_MESSAGE_MAP(CFilePreView, CView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CFilePreView drawing

void CFilePreView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CFilePreView diagnostics

#ifdef _DEBUG
void CFilePreView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFilePreView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFilePreView message handlers


BOOL CFilePreView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
  // TODO: Add your specialized code here and/or call the base class

  return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


int CFilePreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Add your specialized creation code here

  if (nullptr == m_pTaskListDlg)
  {
    m_pTaskListDlg = new CTaskListDialog;
    if (!m_pTaskListDlg)
    {
      return -1;
    }
  }

  CRect rt;
  GetClientRect(&rt);
  m_pTaskListDlg->Create(IDD_DIALOG_TASKLIST, this);
  m_pTaskListDlg->MoveWindow(&rt);
  m_pTaskListDlg->ShowWindow(SW_SHOW);

  return 0;
}


void CFilePreView::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here
  CRect rt;
  GetClientRect(&rt);

  if (m_pTaskListDlg)
  {
    m_pTaskListDlg->MoveWindow(&rt);
  }
}


void CFilePreView::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default

  CView::OnLButtonDown(nFlags, point);
}

void CFilePreView::Release()
{
}
