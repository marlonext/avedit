#pragma once
#include "afxcmn.h"
#include "cs/editCsSkt.h"
#include <string>

#define WM_LOGIN    (WM_USER + 2000)

class CServiceHandler;
class CEditCs;
// CLoginDialog dialog

class CLoginDialog : public CDialog
{
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog(CEditCs& Obj, const CString& strIP, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg LRESULT	OnConnectionFailed(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT	OnConnectionSuccess(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT	OnLogin(WPARAM wParam, LPARAM lParam);
  virtual BOOL OnInitDialog();
  virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();

  const CString& GetHostAddr() const {return m_strIp;}

private:
  CIPAddressCtrl m_ctlHostAddr;
  CServiceHandler* m_pContolThd;
  CString m_strUser;
  CString m_strPasswd;
  CString m_strUserID;
  CEditCs& m_EditCsRef;

  CString m_strIp;
};
