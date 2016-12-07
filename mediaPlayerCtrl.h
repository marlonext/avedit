#pragma once

#include "ActivDoc.h"
// mediaPlayerCtrl.h : Declaration of the CmediaPlayerCtrl ActiveX Control class.
#include "cs/editCs.h"
#include "LoginDialog.h"
#include "MainFrm.h"
#include <ObjSafe.h>


class CActiveXDocTemplate;
class CMainDialog;
extern CMainFrame* g_pMainFrame;

typedef std::vector<CString> MP4LIST;


// class HttpDownload;

// CmediaPlayerCtrl : See mediaPlayerCtrl.cpp for implementation.

class CmediaPlayerCtrl : public CActiveXDocControl
{
	DECLARE_DYNCREATE(CmediaPlayerCtrl)

  CActiveXDocTemplate* m_pActiveXDoc;
// Constructor
public:
	CmediaPlayerCtrl();

  DECLARE_INTERFACE_MAP()
  BEGIN_INTERFACE_PART(ObjSafe, IObjectSafety)   
    STDMETHOD_(HRESULT,   GetInterfaceSafetyOptions)   (     
    /*   [in]   */   REFIID   riid,   
    /*   [out]   */   DWORD   __RPC_FAR   *pdwSupportedOptions,   
    /*   [out]   */   DWORD   __RPC_FAR   *pdwEnabledOptions   
    );  

    STDMETHOD_(HRESULT,   SetInterfaceSafetyOptions)   (     
      /*   [in]   */   REFIID   riid,   
      /*   [in]   */   DWORD   dwOptionSetMask,   
      /*   [in]   */   DWORD   dwEnabledOptions   
      );  
  END_INTERFACE_PART(ObjSafe);

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CmediaPlayerCtrl();

	DECLARE_OLECREATE_EX(CmediaPlayerCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CmediaPlayerCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CmediaPlayerCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CmediaPlayerCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()
  

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs

private:
  std::auto_ptr<CEditCs> m_pEditCs;
  MP4LIST m_Mp4List;


//   std::auto_ptr<HttpDownload> m_pHttpDown;
protected:
  VARIANT_BOOL StartEdit(LPCTSTR sourceName/*, LPCTSTR sourceAlias*/);
  BOOL IsSameMp4(const CString& strMp4);

  enum 
  {
    dispidDownLoad = 2L,
    dispidStartEdit = 1L
  };
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnClose();
protected:
  VARIANT_BOOL DownLoad(LPCTSTR sourceName);
};

