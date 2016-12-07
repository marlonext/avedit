#pragma once

class CMediaPlayerDialog;
// CBackwardButton

class CBackwardButton : public CButton
{
	DECLARE_DYNAMIC(CBackwardButton)

public:
	CBackwardButton();
	virtual ~CBackwardButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  void DrawFastBackwardIcon(CDC& dc, const RECT& rect);

  void SetParentDlg(CMediaPlayerDialog* pDlg);

private:
  CMediaPlayerDialog* m_pMediaPlayerDlg;
public:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


