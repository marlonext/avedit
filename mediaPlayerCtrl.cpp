// mediaPlayerCtrl.cpp : Implementation of the CmediaPlayerCtrl ActiveX Control class.

#include "stdafx.h"
#include "mediaPlayer.h"
#include "mediaPlayerCtrl.h"
#include "mediaPlayerPropPage.h"
#include "afxdialogex.h"
#include "mediaPlayerDoc.h"
#include "mediaPlayerView.h"
#include "MainFrm.h"
#include "LoginDialog.h"
#include "MainDialog.h"
#include "DownLoadDialog.h"
#include "URL.h"
#include "SAStatusLog.h"
#include "EditStatueFile.h"


#include <afxwin.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CmediaPlayerApp theApp;
extern CMainFrame* g_pMainFrame;
extern CSAStatusLog g_statusLog;
extern CEditStatueFile g_editStatusFile;
extern CTimeRecord g_timeRecord;

CString g_strUploadURL;

IMPLEMENT_DYNCREATE(CmediaPlayerCtrl, CActiveXDocControl)



// Message map

BEGIN_MESSAGE_MAP(CmediaPlayerCtrl, CActiveXDocControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
  ON_WM_LBUTTONDOWN()
  ON_WM_CREATE()
  ON_WM_CLOSE()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CmediaPlayerCtrl, CActiveXDocControl)
  DISP_FUNCTION_ID(CmediaPlayerCtrl, "StartEdit", dispidStartEdit, StartEdit, VT_BOOL, VTS_BSTR/*, VTS_BSTR*/)
  DISP_FUNCTION_ID(CmediaPlayerCtrl, "DownLoad", dispidDownLoad, DownLoad, VT_BOOL, VTS_BSTR)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CmediaPlayerCtrl, CActiveXDocControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CmediaPlayerCtrl, 1)
	PROPPAGEID(CmediaPlayerPropPage::guid)
END_PROPPAGEIDS(CmediaPlayerCtrl)


// BEGIN_INTERFACE_MAP(CmediaPlayerCtrl, CActiveXDocControl)  
//   INTERFACE_PART(CmediaPlayerCtrl, IID_DmediaPlayer, Dispatch)  
//   INTERFACE_PART(CmediaPlayerCtrl, IID_IObjectSafety, ObjSafe)   //这儿只添加这一句  
// END_INTERFACE_MAP()  

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CmediaPlayerCtrl, "MEDIAPLAYER.mediaPlayerCtrl.1",
	0x52768f24, 0x96ed, 0x4dd0, 0xbb, 0x6, 0xd8, 0x2a, 0x6e, 0x1c, 0x3f, 0x39)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CmediaPlayerCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID IID_DmediaPlayer = { 0x82844913, 0x35E0, 0x41F9, { 0x86, 0xFD, 0xC9, 0x80, 0x90, 0x57, 0x71, 0x64 } };
const IID IID_DmediaPlayerEvents = { 0x47973F78, 0x2E1, 0x40CF, { 0x8F, 0xCA, 0x22, 0xCA, 0xFB, 0x4A, 0x66, 0x58 } };


// Control type information

static const DWORD _dwmediaPlayerOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CmediaPlayerCtrl, IDS_MEDIAPLAYER, _dwmediaPlayerOleMisc)



// CmediaPlayerCtrl::CmediaPlayerCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CmediaPlayerCtrl

BOOL CmediaPlayerCtrl::CmediaPlayerCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MEDIAPLAYER,
			IDB_MEDIAPLAYER,
			afxRegApartmentThreading,
			_dwmediaPlayerOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CmediaPlayerCtrl::CmediaPlayerCtrl - Constructor

CmediaPlayerCtrl::CmediaPlayerCtrl()
{
	InitializeIIDs(&IID_DmediaPlayer, &IID_DmediaPlayerEvents);
	// TODO: Initialize your control's instance data here.

  m_pActiveXDoc = new CActiveXDocTemplate(
    RUNTIME_CLASS(CmediaPlayerDoc),
    RUNTIME_CLASS(CMainFrame),
    RUNTIME_CLASS(CmediaPlayerView));

  AddDocTemplate(m_pActiveXDoc);  
}

// CmediaPlayerCtrl::~CmediaPlayerCtrl - Destructor

CmediaPlayerCtrl::~CmediaPlayerCtrl()
{
	// TODO: Cleanup your control's instance data here.
  m_Mp4List.clear();
}


// CmediaPlayerCtrl::OnDraw - Drawing function

void CmediaPlayerCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: Replace the following code with your own drawing code.
// 	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
// 	pdc->Ellipse(rcBounds);
}



// CmediaPlayerCtrl::DoPropExchange - Persistence support

void CmediaPlayerCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CmediaPlayerCtrl::OnResetState - Reset control to default state

void CmediaPlayerCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CmediaPlayerCtrl message handlers


//////////////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CmediaPlayerCtrl, CActiveXDocControl) 
  INTERFACE_PART(CmediaPlayerCtrl, IID_IObjectSafety, ObjSafe) 
END_INTERFACE_MAP()  

ULONG FAR EXPORT CmediaPlayerCtrl::XObjSafe::AddRef() 
{ 
  METHOD_PROLOGUE(CmediaPlayerCtrl,ObjSafe) 
    return pThis->ExternalAddRef(); 
}  

ULONG FAR EXPORT CmediaPlayerCtrl::XObjSafe::Release() 
{ 
  METHOD_PROLOGUE(CmediaPlayerCtrl,ObjSafe) 
    return pThis->ExternalRelease(); 
}  

HRESULT FAR EXPORT CmediaPlayerCtrl::XObjSafe::QueryInterface(REFIID iid,void FAR* FAR* ppvObj) 
{ 
  METHOD_PROLOGUE(CmediaPlayerCtrl,ObjSafe) 
    return (HRESULT)pThis->ExternalQueryInterface(&iid,ppvObj); 
}  

const DWORD dwSupportedBits = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA; 
const DWORD dwNotSupportedBits=~dwSupportedBits;  

HRESULT STDMETHODCALLTYPE   
  CmediaPlayerCtrl::XObjSafe::GetInterfaceSafetyOptions(   
  /*[in]*/ REFIID riid, 
  /*[out]*/ DWORD __RPC_FAR *pdwSupportedOptions, 
  /*[out]*/ DWORD __RPC_FAR *pdwEnabledOptions) 
{ 
  METHOD_PROLOGUE(CmediaPlayerCtrl,ObjSafe) 
    HRESULT retval = ResultFromScode(S_OK); 

  IUnknown FAR* punkInterface; 
  retval = pThis->ExternalQueryInterface(&riid, (void **)&punkInterface); 
  if(retval != E_NOINTERFACE) 
  { 
    punkInterface->Release(); 
  } 

  *pdwSupportedOptions=*pdwEnabledOptions=dwSupportedBits; 
  return retval; 
}  

HRESULT STDMETHODCALLTYPE   
  CmediaPlayerCtrl::XObjSafe::SetInterfaceSafetyOptions(   
  /*[in]*/ REFIID riid, 
  /*[in]*/ DWORD dwOptionSetMask, 
  /*[in]*/ DWORD dwEnabledOptions) 
{ 
  METHOD_PROLOGUE(CmediaPlayerCtrl, ObjSafe) 

    IUnknown FAR* punkInterface; 
  pThis->ExternalQueryInterface(&riid, (void **)&punkInterface); 
  if(punkInterface)
  { 
    punkInterface->Release(); 
  } 
  else
  { 
    return ResultFromScode(E_NOINTERFACE); 
  }  

  if(dwOptionSetMask & dwNotSupportedBits)  
  {   
    return ResultFromScode(E_FAIL); 
  }  

  dwEnabledOptions&=dwSupportedBits; 
  if((dwOptionSetMask&dwEnabledOptions)!=dwOptionSetMask) 
  { 
    return ResultFromScode(E_FAIL); 
  } 

  return ResultFromScode(S_OK); 
}

VARIANT_BOOL CmediaPlayerCtrl::StartEdit(LPCTSTR sourceName/*, LPCTSTR sourceAlias*/)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here  
  CURL URL(sourceName);
//   g_strUploadURL = URL.GetURL();

  g_statusLog.StatusOut(_T("URL: %s, IP: %s, name: %s, alias: %s"), 
    URL.GetURL(), URL.GetIp(), 
    URL.GetTaskName(), URL.GetAlias());

  if (theApp.m_bOnline)
  {
    m_pActiveXDoc->GetMainDlg()->ShowWindow(SW_SHOW);

    CDownLoadDialog DownLoadDlg;
    DownLoadDlg.DoModal();
  }
  else
  {
    std::auto_ptr<CLoginDialog> pLoginDlg(new CLoginDialog(*(theApp.m_pEditCs), URL.GetIp()));

    if (IDOK == pLoginDlg->DoModal())
    {    
      theApp.m_bOnline = true;

      m_pActiveXDoc->GetMainDlg()->ShowWindow(SW_SHOW);

      CDownLoadDialog DownLoadDlg;
      DownLoadDlg.DoModal();

      if (DownLoadDlg.IsDownLoadSucess())
      {
        if (!IsSameMp4(URL.GetTaskName()))
        {
          m_Mp4List.push_back(URL.GetTaskName());
        }
      }

      theApp.StartRecvThd();
      theApp.StartKeepAliveThd();

      return TRUE;
    }
    else
    {
      theApp.m_bOnline = false;
    }
  }

  return VARIANT_TRUE;
}


int CmediaPlayerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CActiveXDocControl::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Add your specialized creation code here

  return 0;
}


void CmediaPlayerCtrl::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  m_pActiveXDoc->GetMainDlg()->ShowWindow(SW_HIDE);

  CActiveXDocControl::OnClose(OLECLOSE_SAVEIFDIRTY);
}


VARIANT_BOOL CmediaPlayerCtrl::DownLoad(LPCTSTR sourceName)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  CURL URL(sourceName);

  CDownLoadDialog DownLoadDlg;
//   DownLoadDlg.SetDownLoadSource(sourceName);
  DownLoadDlg.DoModal();

  return VARIANT_TRUE;
}

BOOL CmediaPlayerCtrl::IsSameMp4(const CString& strMp4)
{
  MP4LIST::const_iterator it = m_Mp4List.begin();
  for (; it != m_Mp4List.end(); ++it)
  {
    if (*it == strMp4)
    {
      return TRUE;
    }
  }

  if (m_Mp4List.empty())
  {
    return FALSE;
  }

  return FALSE;
}



