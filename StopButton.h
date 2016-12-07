#pragma once

class CMediaPlayerDialog;
// CStopButton

class CStopButton : public CButton
{
	DECLARE_DYNAMIC(CStopButton)

  void DrawStopButton(CDC* pDC, const CRect& rect);

public:
	CStopButton();
	virtual ~CStopButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  void SetParentDlg(CMediaPlayerDialog* pDlg);

private:
  CMediaPlayerDialog* m_pMediaPlayerDlg;
public:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


