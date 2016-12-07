// BackwardButton.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "BackwardButton.h"
#include "MediaPlayerDialog.h"
#include "MainFrm.h"
#include "TaskListDialog.h"
#include "FilePreView.h"

extern CMainFrame* g_pMainFrame;
// CBackwardButton

IMPLEMENT_DYNAMIC(CBackwardButton, CWnd)

CBackwardButton::CBackwardButton()
{
  m_pMediaPlayerDlg = nullptr;
}

CBackwardButton::~CBackwardButton()
{
}


BEGIN_MESSAGE_MAP(CBackwardButton, CWnd)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CBackwardButton message handlers

void CBackwardButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
//   this->SetDlgItemText(IDC_BUTTON_BACK, _T("hello"));
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

  DrawFastBackwardIcon(*pDC, lpDrawItemStruct->rcItem);

  // Get the button's text.
//   CString strText = _T("hello");
//   GetWindowText(strText);

  // Draw the button text using the text color red.
//   COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(255,0,0));
//   ::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), 
//     &lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
//   ::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
  

}

void CBackwardButton::DrawFastBackwardIcon(CDC& dc, const RECT& rect)
{
  CRect rt(rect.left, rect.top, rect.right, rect.bottom);

  rt.DeflateRect(3, 3, 3, 3);

  long lTop = rt.top;
  long lTop_ = lTop;
  long lBottom = rt.bottom;
  long lBottom_ = lBottom;
  long lCenter = rt.left + rt.Width() / 2;
  long lCenter_ = lCenter;
  long lHeight = rt.bottom - rt.top;
  long lRight = rt.right - 3;

  const long lIncrease = 1L;

  while (true)
  {
    if (lTop >= lBottom)
    {
      break;
    }

    dc.MoveTo(lCenter, lTop /*+= lIncrease*/);
    dc.LineTo(lCenter, lBottom /*-= lIncrease*/);

    lCenter -= 1;
    lTop += lIncrease;
    lBottom -= lIncrease;
  }

  while (true)
  {
    if (lTop_ >= lBottom_)
    {
      break;
    }

    dc.MoveTo(lRight, lTop_ /*+= lIncrease*/);
    dc.LineTo(lRight, lBottom_ /*-= lIncrease*/);

    lRight -= 1;
    lTop_ += lIncrease;
    lBottom_ -= lIncrease;
  }
}


void CBackwardButton::SetParentDlg(CMediaPlayerDialog* pDlg)
{
  if (pDlg)
  {
    m_pMediaPlayerDlg = pDlg;
  }
}



void CBackwardButton::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
//   CSplitterWnd& split = g_pMainFrame->GetSplitterWnd2();
//   CFilePreView* pFilePrevView = (CFilePreView*)(split.GetPane(1, 0));
//   CTaskListDialog* pDlg = pFilePrevView->GetTaskListDlg();
// 
//   std::vector<CString>& Mp4PathAry = const_cast<std::vector<CString>&>(pDlg->GetMp4PathAry());
//   std::vector<CString>::reverse_iterator rit = Mp4PathAry.rbegin();
//   const CString& strMp4Path = m_pMediaPlayerDlg->GetMp4Path();
// 
//   int nSelMp4 = Mp4PathAry.size();
//   while (true)
//   {
//     if (rit == Mp4PathAry.rend())
//     {
//       nSelMp4 = Mp4PathAry.size() - 1;
//       rit = Mp4PathAry.rbegin();
//     }
// 
//     if (*rit == strMp4Path)
//     {
//       --nSelMp4;
//       ++rit;
//       break;
//     }
// 
//     --nSelMp4;
//     ++rit;
//   }
// 
//   if (rit == Mp4PathAry.rend())
//   {
//     nSelMp4 = Mp4PathAry.size() - 1;
// 
//     rit = Mp4PathAry.rbegin();
//   }
// 
//   const CString& strNextMp4 = *rit;
// 
//   pDlg->SetCurSelMp4(nSelMp4);
// 
//   CVLCPlayer::Instance()->OpenMedia(strNextMp4);
//   CVLCPlayer::Instance()->Play();

  CButton::OnLButtonDown(nFlags, point);
}
