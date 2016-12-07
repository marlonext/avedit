// TimeEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "TimeEdit.h"


// CTimeEdit

IMPLEMENT_DYNAMIC(CTimeEdit, CEdit)

CTimeEdit::CTimeEdit(CWnd* pParent, const CPoint& pt, const CRect& rt, int nID, const CString& strTime, bool bLeft)
:m_strCutTime(strTime),
  m_ptShowTime(pt)
{
  CRect rtPlace = rt;

  if (bLeft)
  {
    rtPlace.left = pt.x;
  }
  else
  {
    rtPlace.left = pt.x - TIMEBOX_WIDTH;
  }
  rtPlace.top = rtPlace.bottom - TIMEBOX_HEIGHT;
  rtPlace.right = rtPlace.left + TIMEBOX_WIDTH;

  Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED, rtPlace, pParent, nID);

  this->SetWindowText(strTime);
  ::SetWindowPos(pParent->GetSafeHwnd(), HWND_TOPMOST, pt.x, pt.y, 
                 pt.x + TIMEBOX_WIDTH, pt.y + TIMEBOX_HEIGHT, 
                 SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

  ShowWindow(SW_SHOW);
}

CTimeEdit::~CTimeEdit()
{
//   delete this;
}


BEGIN_MESSAGE_MAP(CTimeEdit, CEdit)
END_MESSAGE_MAP()



// CTimeEdit 消息处理程序


void CTimeEdit::UpdateTime(const CString& strTime)
{
  if (strTime.IsEmpty())
  {
    return;
  }

  m_strCutTime = strTime;
  this->SetWindowText(strTime);
}


