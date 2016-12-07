#pragma once
#include "afxcmn.h"


// CDownLoadDialog dialog

class CDownLoadDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDownLoadDialog)

  const static int m_nMaxPos = 200;
  CString m_strUrl;

public:
	CDownLoadDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDownLoadDialog();

  BOOL IsDownLoadSucess() {return m_bDownLoadSucess;}

//   void SetDownLoadSource(const CString& strPath);
//   void SetDownLoadSourceAlias(const CString& strSourceAlias);
  BOOL FindFiles(CString &strPath);

// Dialog Data
	enum { IDD = IDD_DIALOG_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CProgressCtrl m_Progress;
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnStartDownLoad(WPARAM wParam, LPARAM lParam);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg LRESULT OnDownLoadSuccess(WPARAM wPara, LPARAM lParam);
  afx_msg LRESULT OnDownLoadFailure(WPARAM wPara, LPARAM lParam);

private:
  int m_nFileLen;
  int m_nDownLen;
  CString m_strDir;
  CString m_StrMp4Name;
  CString m_strAlias;
  BOOL    m_bDownLoadSucess;

  HANDLE m_hHttpThread;
  static unsigned int __stdcall HttpDownLoadThread(void* pParam);
  void Main();
};
