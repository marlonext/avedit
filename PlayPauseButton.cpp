// DrawButton.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "PlayPauseButton.h"
#include "MediaPlayerDialog.h"

// CDrawButton

IMPLEMENT_DYNAMIC(CPlayPauseButton, CButton)

CPlayPauseButton::CPlayPauseButton()
{
  m_bPlay = false;
  m_bMp4Opened = false;
  m_pMediaPlayDlg = nullptr;
}

CPlayPauseButton::~CPlayPauseButton()
{
}


BEGIN_MESSAGE_MAP(CPlayPauseButton, CButton)
  ON_WM_LBUTTONDOWN()
  ON_MESSAGE(WM_DRAWCIRCULARBTN, OnDrawCircularButton)
END_MESSAGE_MAP()



// CDrawButton message handlers

BOOL CPlayPauseButton::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Add your specialized code here and/or call the base class
  ModifyStyle(0, GetStyle() | BS_OWNERDRAW);

  return CButton::PreCreateWindow(cs);
}

void CPlayPauseButton::DrawCircularButton(CDC& dc, const CPoint& ptBegin, 
                                  const CPoint& ptEnd, COLORREF color)
{
  dc.SetBkMode(TRANSPARENT);

  CPen LightBluePen(PS_SOLID, 2, color);
  CPen* pOldPen = dc.SelectObject(&LightBluePen);

  dc.Ellipse(ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y);
  if (pOldPen)
  {
    dc.SelectObject(pOldPen);
  }

  if (m_bPlay)
  {
    DrawPauseIcon(dc, ptBegin, ptEnd, color);
  }
  else
  {
    DrawPlayIcon(dc, ptBegin, ptEnd, color);
  }  
}

void CPlayPauseButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  // TODO:  Add your code to draw the specified item
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  RECT rect = lpDrawItemStruct->rcItem;

  int nSubWidth = rect.right - rect.left;
  int nSubHeight = rect.bottom - rect.top;
  if (nSubWidth < nSubHeight)
  {
    rect.top += (nSubHeight - nSubWidth) / 2;
    rect.bottom = rect.top + nSubWidth;
  }

  if (nSubWidth > nSubHeight)
  {
    rect.left += (nSubWidth - nSubHeight) / 2;
    rect.right = rect.left + nSubHeight;
  }

  UINT uStyle = DFCS_BUTTONPUSH;

  // This code only works with buttons.
  ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

  COLORREF BlackColor = RGB(0, 0, 0);
  // If drawing selected, add the pushed style to DrawFrameControl.
  if (lpDrawItemStruct->itemState & ODS_SELECTED)
  {    
    CRect rtPressed = rect;
    rtPressed.top += 2;
    rtPressed.left += 2;
    rtPressed.right -= 2;
    rtPressed.bottom -= 2;

    DrawCircularButton(*pDC, CPoint(rtPressed.left, rtPressed.top), 
                    CPoint(rtPressed.right, rtPressed.bottom), BlackColor);
  }
  else
  {
    DrawCircularButton(*pDC, CPoint(rect.left, rect.top), 
                      CPoint(rect.right, rect.bottom), BlackColor);

  }
}

void CPlayPauseButton::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  CRect rect;
  GetClientRect(&rect);
  InvalidateRect(&rect);

  const CString& strMp4Path = m_pMediaPlayDlg->GetMp4Path();
  if (strMp4Path.IsEmpty())
  {
    return;
  }

  if (!m_bPlay)
  {
    if (!m_bMp4Opened)
    {
      CVLCPlayer::Instance()->OpenMedia(strMp4Path);
    }

    CVLCPlayer::Instance()->Play();
    m_bPlay = true;

    ::PostMessage(m_pMediaPlayDlg->GetSafeHwnd(), WM_RESETBTNSTATUS, NULL, NULL);
  }
  else
  {
    m_bMp4Opened = true;
    m_bPlay = false;
    CVLCPlayer::Instance()->Pause();
  }  

  CButton::OnLButtonDown(nFlags, point);
}

void CPlayPauseButton::DrawPlayIcon(CDC& dc, const CPoint& ptBegin, 
                               const CPoint& ptEnd,  COLORREF color)
{
  long lStart = ptBegin.x + (ptEnd.x - ptBegin.x) / 5*2;
  long lEnd = lStart + (ptEnd.x - ptBegin.x) / 7 * 3;
  long lCenter = (ptEnd.y - ptBegin.y) / 6 * 3;
  long lCentUp = lCenter;
  long lCentDown = lCenter;

  dc.MoveTo(lStart, lCenter);
  dc.LineTo(lEnd, lCenter);

  while (true) 
  {    
    if (lEnd <= lStart)
    {
      break;
    }

    dc.MoveTo(lStart, --lCentUp);
    dc.LineTo(lEnd-=2, lCentUp);  

    dc.MoveTo(lStart, ++lCentDown);
    dc.LineTo(lEnd, lCentDown);  
  }
}

void CPlayPauseButton::DrawPauseIcon(CDC& dc, const CPoint& ptBegin, 
                                const CPoint& ptEnd, COLORREF color)
{
  long lHeight = (ptEnd.y - ptBegin.y) / 2;
  long lCenter_x = ptBegin.x + (ptEnd.x - ptBegin.x) / 3 - 2;
  long lCenter_y = ptBegin.y + (ptEnd.y - ptBegin.y) / 2;
  long ly1 = lCenter_y - lHeight / 2;
  long ly2 = lCenter_y + lHeight / 2;
  
  for (int i = 0; i < 6; i++)
  {
    dc.MoveTo(lCenter_x, ly1);
    dc.LineTo(lCenter_x, ly2);

    lCenter_x += 1;
  }

  lCenter_x += 3;

  for (int i = 0; i < 6; i++)
  {
    dc.MoveTo(lCenter_x, ly1);
    dc.LineTo(lCenter_x, ly2);

    lCenter_x += 1;
  }
}


void CPlayPauseButton::SetParentDlg(CMediaPlayerDialog* pDlg) 
{
  if (pDlg)
  {
    m_pMediaPlayDlg = pDlg;
  }
}


LRESULT CPlayPauseButton::OnDrawCircularButton(WPARAM wParam, LPARAM lParam)
{
  CClientDC dc(this);
  CRect rect;

  GetClientRect(&rect);

  COLORREF BlackColor = RGB(0, 0, 0);
  CRect rtPressed = rect;
  rtPressed.top += 2;
  rtPressed.left += 2;
  rtPressed.right -= 2;
  rtPressed.bottom -= 2;
  
  m_bPlay = bool(wParam);

  InvalidateRect(NULL);

  DrawCircularButton(dc, CPoint(rtPressed.left, rtPressed.top), 
    CPoint(rtPressed.right, rtPressed.bottom), BlackColor);

  return 0;
}

