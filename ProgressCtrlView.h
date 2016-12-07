#pragma once


// CProgressCtrlView view
class CFileEditDialog;


class CProgressCtrlView : public CView
{
	DECLARE_DYNCREATE(CProgressCtrlView)

protected:
	CProgressCtrlView();           // protected constructor used by dynamic creation
	virtual ~CProgressCtrlView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
  
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnPaint();

  CFileEditDialog* GetEditDlg() {return m_pDlg;}

private:
  CFileEditDialog* m_pDlg;
public:
  afx_msg void OnSize(UINT nType, int cx, int cy);
  void Release();
};


