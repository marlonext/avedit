// LoginDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "LoginDialog.h"
#include "Ini.h"
#include "cs/editCs.h"
#include "mediaPlayer.h"


extern CmediaPlayerApp theApp;

#define MAX_USER_NAME_LEN   32

extern CmediaPlayerApp theApp;
// CLoginDialog dialog

IMPLEMENT_DYNAMIC(CLoginDialog, CDialog)

CLoginDialog::CLoginDialog(CEditCs& Obj, const CString& strIp, CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDialog::IDD, pParent)
  , m_EditCsRef(Obj)
  , m_strIp(strIp)
  , m_strUser(_T(""))
  , m_strPasswd(_T(""))
  , m_strUserID(_T(""))
{
  m_pContolThd = NULL;
}

CLoginDialog::~CLoginDialog()
{
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_IPADDRESS1, m_ctlHostAddr);
//   DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUser);
//   DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPasswd);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialog)
  ON_BN_CLICKED(IDOK, &CLoginDialog::OnBnClickedOk)
  ON_MESSAGE(WM_CONNECTION_FAILD,						OnConnectionFailed)
  ON_MESSAGE(WM_CONNECTION_SUCCESS,					OnConnectionSuccess)
  ON_MESSAGE(WM_LOGIN, OnLogin)
END_MESSAGE_MAP()


// CLoginDialog message handlers

void CLoginDialog::OnBnClickedOk()
{
  // TODO: Add your control notification handler code here
  OnOK();
}

void CLoginDialog::OnOK()
{
  UpdateData();

  if (m_ctlHostAddr.IsBlank())
  {
    AfxMessageBox(_T("地址不能为空"));
    return;
  }

  BYTE nField0, nField1, nField2, nField3;
  m_ctlHostAddr.GetAddress(nField0, nField1, nField2, nField3);
  
  CString strIp;
  strIp.Format("%d.%d.%d.%d", nField0, nField1, nField2, nField3);
  m_strIp = strIp;
  
  if (!m_EditCsRef.csInitialize(FALSE, inet_addr(strIp)))
  {
    DWORD dwError = GetLastError();
    GetDlgItem(IDC_CONNECTION_STATUS)->SetWindowText(_T("连接失败"));
    return;
  }

  if (!CIni::WriteString("hostname", "userIP", strIp, "server.ini"))			
    return;
  
  CDialog::OnOK();
  
//   CString str1;
//   CString str2;
// 
// 	if (m_ctlHostAddr.IsBlank())
// 	{
//     str1.Empty();
//     str2.Empty();
//     str1.LoadString(IDS_HOST_IP_CANNOTEMPTY);
//     str2.LoadString(IDS_ALERT_TITLE);
// 		MessageBox(str1, str2, MB_OK|MB_ICONEXCLAMATION);
// 		return;
// 	}
// 
// 	if (m_strUser.IsEmpty())
// 	{
//     str1.Empty();
//     str2.Empty();
//     str1.LoadString(IDS_USER_NAME_CANNOTEMPTY);
//     str2.LoadString(IDS_ALERT_TITLE);
// 
// 		MessageBox(str1, str2, MB_OK|MB_ICONEXCLAMATION);
// 		return;
// 	}
// 
// 	if (m_strPasswd.IsEmpty())
// 	{
//     str1.Empty();
//     str2.Empty();
//     str1.LoadString(IDS_USER_PASSWD_CANNOTEMPTY);
//     str2.LoadString(IDS_ALERT_TITLE);
// 
// 		MessageBox(str1, str2, MB_OK|MB_ICONEXCLAMATION);
// 		return;
// 	}
// 
// 	if (m_strUser.GetLength() >= MAX_USER_NAME_LEN)
// 	{
//     str1.Empty();
//     str2.Empty();
//     str1.LoadString(IDS_USER_NAME_TOOLONG);
//     str2.LoadString(IDS_ALERT_TITLE);
// 
// 		MessageBox(str1, str2, MB_OK|MB_ICONEXCLAMATION);
// 		return;
// 	}
// 
// 	if (m_strUser != PString(Admin_UserName) &&
// 		m_strUser != PString(SuperAdmin_UserName))
// 	{
// 		if (m_strUserID.IsEmpty())
// 		{
// 			MessageBox(XING::LoadStringFromResource(IDS_USER_ID_CANNOTEMPTY), 
// 			XING::LoadStringFromResource(IDS_ALERT_TITLE), MB_OK|MB_ICONEXCLAMATION);
// 			return;
// 		}
// 	}
// 
// 	BYTE nField0, nField1, nField2, nField3;
//     
// 	if(4 != m_ctlHostAddr.GetAddress(nField0,nField1,nField2,nField3))
// 		return;
// 
// 	PString hostAddress(PString::Printf,"%d.%d.%d.%d",nField0,nField1,nField2,nField3);
// 
// 	if (m_pContolThd != NULL)
// 		m_pContolThd = NULL;
// 	m_pContolThd = new CServiceHandler;
// 
// 	m_pContolThd->SetUserID("");
// 	m_pContolThd->SetName((const char *)m_strUser);
// 	m_pContolThd->SetPass((const char *)m_strPasswd);
// 	m_pContolThd->AttachManager(this->GetSafeHwnd(),hostAddress);
// 
// 	ACE_Synch_Options nonblocking_connect
// 		(ACE_Synch_Options::USE_REACTOR);
// 	nonblocking_connect.set (ACE_Synch_Options::USE_REACTOR |
// 		ACE_Synch_Options::USE_TIMEOUT,
// 		ACE_Time_Value (0, 600 * 1000));
// 	ACE_INET_Addr sld_addr (DefaultManagePort, (const char *)hostAddress);
// 
//  	theApp.GetClientConnn().connect(m_pContolThd,sld_addr,nonblocking_connect);
//  	GetDlgItem(IDC_CONNECTION_STATUS)->SetWindowText(XING::LoadStringFromResource(IDS_TRY_CONNECTING));
// 
// 	// ini file
// 	if (!CIni::WriteString("hostname", "name", m_strUser, "server.ini"))	
// 		return;
// 	if (!CIni::WriteString("hostname", "pass", m_strPasswd, "server.ini"))			
// 		return;
// 	if (!CIni::WriteString("hostname", "userIP", strIp, "server.ini"))			
// 		return;
}

LRESULT CLoginDialog::OnConnectionFailed(WPARAM wParam, LPARAM lParam)
{
//   m_pContolThd = NULL;
// 
//   GetDlgItem(IDC_CONNECTION_STATUS)->SetWindowText(XING::LoadStringFromResource(IDS_CONN_FAILED));
// 
//   GetDlgItem(IDOK)->EnableWindow(TRUE);

  return 0;
}

LRESULT CLoginDialog::OnConnectionSuccess(WPARAM wParam, LPARAM lParam)
{
//   GetDlgItem(IDC_CONNECTION_STATUS)->SetWindowText(
//     XING::LoadStringFromResource(IDS_CONN_OK_TRY_LOGINING));
// 
// 	if(!(*m_ppContolThd)->Login(
// 		PString(m_strUser), PString(m_strUserID),PString(m_strPasswd)))
// 	{
// 		delete (*m_ppContolThd);
// 		(*m_ppContolThd) = NULL;
// 		GetDlgItem(IDC_CONNECTION_STATUS)->SetWindowText(
// 			XING::LoadStringFromResource(IDS_LOGIN_FAILED));
// 		return 0;
// 	}
// 
//   theApp.SetControlThd(m_pContolThd);

  CDialog::OnOK();
  return 0;
}


BOOL CLoginDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
//   TCHAR szUserIP[MAX_PATH] = {'\0'};
//   CIni::ReadString(TEXT("hostname"), TEXT("userIP"), szUserIP, MAX_PATH, TEXT("server.ini"));
  m_ctlHostAddr.SetWindowText(m_strIp);

  PostMessage(WM_LOGIN);

  CIni::WriteString("hostname", "userIP", m_strIp, "server.ini");

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT	CLoginDialog::OnLogin(WPARAM wParam, LPARAM lParam)
{
  if (!m_EditCsRef.csInitialize(FALSE, inet_addr(m_strIp)))
  {
    DWORD dwError = GetLastError();
    GetDlgItem(IDC_CONNECTION_STATUS)->SetWindowText(_T("连接失败"));
    return 0;
  }

  theApp.StartRecvEvent();
  CDialog::OnOK();

  return 0;
}


