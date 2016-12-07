// mediaPlayerPropPage.cpp : Implementation of the CmediaPlayerPropPage property page class.

#include "stdafx.h"
#include "mediaPlayer.h"
#include "mediaPlayerPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CmediaPlayerPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CmediaPlayerPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CmediaPlayerPropPage, "MEDIAPLAYER.mediaPlayerPropPage.1",
	0x7552d4e5, 0xeed5, 0x4880, 0xa1, 0x44, 0x12, 0xe1, 0xeb, 0x7d, 0xef, 0x9e)



// CmediaPlayerPropPage::CmediaPlayerPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CmediaPlayerPropPage

BOOL CmediaPlayerPropPage::CmediaPlayerPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MEDIAPLAYER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CmediaPlayerPropPage::CmediaPlayerPropPage - Constructor

CmediaPlayerPropPage::CmediaPlayerPropPage() :
	COlePropertyPage(IDD, IDS_MEDIAPLAYER_PPG_CAPTION)
{
}



// CmediaPlayerPropPage::DoDataExchange - Moves data between page and properties

void CmediaPlayerPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CmediaPlayerPropPage message handlers
