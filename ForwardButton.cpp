// ForwardButton.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "ForwardButton.h"
#include "MediaPlayerDialog.h"

// CForwardButton

IMPLEMENT_DYNAMIC(CForwardButton, CButton)

CForwardButton::CForwardButton()
{
  m_pMediaPlayerDlg = nullptr;
}

CForwardButton::~CForwardButton()
{
}


BEGIN_MESSAGE_MAP(CForwardButton, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CForwardButton message handlers



void CForwardButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

  // TODO:  Add your code to draw the specified item
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

  DrawForwardIcon(*pDC, lpDrawItemStruct->rcItem);
}

void CForwardButton::DrawForwardIcon(CDC& dc, const RECT& rect)
{
  CRect rt(rect.left, rect.top, rect.right, rect.bottom);

  rt.DeflateRect(3, 3, 3, 3);

  long lTop = rt.top;
  long lTop_ = lTop;
  long lBottom = rt.bottom;
  long lBottom_ = lBottom;
  long lCenter = rt.left + (rt.right - rt.left) / 2;
  long lCenter_ = lCenter;
  long lHeight = rt.bottom - rt.top;
  long lRight = rt.right - 3;
  long lLeft = rt.left + 3;

  const long lIncrease = 1L;

  while (true)
  {
    if (lTop >= lBottom)
    {
      break;
    }

    dc.MoveTo(lLeft, lTop);
    dc.LineTo(lLeft, lBottom);

    lLeft += 1;
    lTop += lIncrease;
    lBottom -= lIncrease;
  }

  while (true)
  {
    if (lTop_ >= lBottom_)
    {
      break;
    }

    dc.MoveTo(lCenter, lTop_);
    dc.LineTo(lCenter, lBottom_);

    lCenter += 1;
    lTop_ += lIncrease;
    lBottom_ -= lIncrease;
  }
}


void CForwardButton::SetParentDlg(CMediaPlayerDialog* pDlg)
{
  if (pDlg)
  {
    m_pMediaPlayerDlg = pDlg;
  }
}



void CForwardButton::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default

  CButton::OnLButtonDown(nFlags, point);
}
