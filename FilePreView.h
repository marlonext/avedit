#pragma once


// CFilePreView view
class CFilePrevDialog;
class CTaskListDialog;

class CFilePreView : public CView
{
	DECLARE_DYNCREATE(CFilePreView)

protected:
	CFilePreView();           // protected constructor used by dynamic creation
	virtual ~CFilePreView();

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
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  CTaskListDialog* GetTaskListDlg() {return m_pTaskListDlg;}

private:
//   CFilePrevDialog* m_pFilePrevDlg;
  CTaskListDialog* m_pTaskListDlg;
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  void Release();
};


