#pragma once

class CMediaPlayerDialog;
// CPlayPauseButton

class CPlayPauseButton : public CButton
{
	DECLARE_DYNAMIC(CPlayPauseButton)

public:
	CPlayPauseButton();
	virtual ~CPlayPauseButton();

  void SetRoundStyle();
  void DrawCircularButton(CDC& dc, const CPoint& ptBegin, const CPoint& ptEnd,
                        COLORREF color);

  void DrawPlayIcon(CDC& dc, const CPoint& ptBegin, const CPoint& ptEnd, 
                    COLORREF color);
  void DrawPauseIcon(CDC& dc, const CPoint& ptBegin, const CPoint& ptEnd, 
                     COLORREF color);

  bool IsPlay(){return m_bPlay;}
  void SetPlayState(bool bPlay){m_bPlay = bPlay;}
  afx_msg LRESULT OnDrawCircularButton(WPARAM wParam, LPARAM lParam);

  void SetParentDlg(CMediaPlayerDialog* pDlg);

protected:
	DECLARE_MESSAGE_MAP()
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
  bool m_bPlay;
  bool m_bMp4Opened;

  CMediaPlayerDialog* m_pMediaPlayDlg;
};


