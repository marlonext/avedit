#pragma once

class CMediaPlayerDialog;
// CForwardButton

class CForwardButton : public CButton
{
	DECLARE_DYNAMIC(CForwardButton)

public:
	CForwardButton();
	virtual ~CForwardButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
  void DrawForwardIcon(CDC& dc, const RECT& rect);
  void SetParentDlg(CMediaPlayerDialog* pDlg);

private:
  CMediaPlayerDialog* m_pMediaPlayerDlg;
public:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


