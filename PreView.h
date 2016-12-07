#pragma once

class CMediaPlayerDialog;

// CPreView view

class CPreView : public CView
{
	DECLARE_DYNCREATE(CPreView)

protected:
	CPreView();           // protected constructor used by dynamic creation
	virtual ~CPreView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  CMediaPlayerDialog* GetMediaPlayDlg() { return m_pMediaPlayerDlg; }

private:
  CMediaPlayerDialog* m_pMediaPlayerDlg;

protected:
	DECLARE_MESSAGE_MAP()
};


