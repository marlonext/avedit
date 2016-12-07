#pragma once

#include "resource.h"


class CFrameWnd;
// CMainDialog dialog

class CMainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDialog)

  CFrameWnd* m_pFrameWnd;

public:
	CMainDialog(CFrameWnd* pFrameWnd, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnSize(UINT nType, int cx, int cy);
  virtual BOOL OnInitDialog();
  void UpdateFrameWnd(CFrameWnd* pFramWnd);
  afx_msg void OnClose();
};
