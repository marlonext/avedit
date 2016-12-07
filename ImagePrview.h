#pragma once


// CImagePrview view;

class CImagePreviewDialog;

class CImagePrview : public CView
{
	DECLARE_DYNCREATE(CImagePrview)

protected:
	CImagePrview();           // protected constructor used by dynamic creation
	virtual ~CImagePrview();

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

private:
  CImagePreviewDialog* m_pImagePreviewDlg;
public:
  afx_msg void OnSize(UINT nType, int cx, int cy);
  CImagePreviewDialog* GetImagePrevDlg() {return m_pImagePreviewDlg;}
};


