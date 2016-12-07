// StopButton.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "StopButton.h"
#include "MediaPlayerDialog.h"

// CStopButton

IMPLEMENT_DYNAMIC(CStopButton, CButton)

CStopButton::CStopButton()
{

}

CStopButton::~CStopButton()
{
}


BEGIN_MESSAGE_MAP(CStopButton, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CStopButton message handlers




void CStopButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  // TODO: Add your message handler code here and/or call default
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  ASSERT(pDC != NULL);

  UINT uStyle = DFCS_BUTTONPUSH;

  // This code only works with buttons.
  ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

  // If drawing selected, add the pushed style to DrawFrameControl.
  if (lpDrawItemStruct->itemState & ODS_SELECTED)
    uStyle |= DFCS_PUSHED;
  // Draw the button frame.
  ::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
                      DFC_BUTTON, uStyle);

  DrawStopButton(pDC, lpDrawItemStruct->rcItem);
}

void CStopButton::DrawStopButton(CDC* pDC, const CRect& rect)
{
  int nWidth = rect.Width() / 2;
  int nHeight = nWidth;

  int nXStart = rect.left + (rect.Width() - nWidth) / 2;
  int nYStart = rect.top + (rect.Height() - nHeight) / 2;
  int nXStart_  = nXStart;

  do 
  {
    pDC->MoveTo(nXStart, nYStart);
    pDC->LineTo(nXStart, nYStart + nHeight);
  } while (++nXStart < nXStart_ + nWidth);
}

BOOL CStopButton::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Add your specialized code here and/or call the base class
  ModifyStyle(0, GetStyle() | BS_OWNERDRAW);

  return CButton::PreCreateWindow(cs);
}


void CStopButton::SetParentDlg(CMediaPlayerDialog* pDlg)
{
  if (pDlg)
  {
    m_pMediaPlayerDlg = pDlg;
  }
}



void CStopButton::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  ::PostMessage(m_pMediaPlayerDlg->GetSafeHwnd(), WM_RESETPLAYSTATUS, WPARAM(false), NULL);

  CButton::OnLButtonDown(nFlags, point);
}
