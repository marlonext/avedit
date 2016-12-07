#pragma once

#include <atlimage.h>
#include "PictureCtrl.h"


// CImagePreviewDialog dialog

class CImagePreviewDialog : public CDialog
{
	DECLARE_DYNAMIC(CImagePreviewDialog)

public:
	CImagePreviewDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImagePreviewDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMAGEPREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
  CPictureCtrl* m_pTitleImage;
  CPictureCtrl* m_pTailImage;

  CString m_strTitlePath;
  CString m_strTailPath;
  bool m_bAddTitle;
  bool m_bAddTail;


  void ShowTitleAndTailImage(CDC& dc);
public:
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnAddPic(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnPaint();
};
