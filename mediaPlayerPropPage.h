#pragma once

// mediaPlayerPropPage.h : Declaration of the CmediaPlayerPropPage property page class.


// CmediaPlayerPropPage : See mediaPlayerPropPage.cpp for implementation.

class CmediaPlayerPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CmediaPlayerPropPage)
	DECLARE_OLECREATE_EX(CmediaPlayerPropPage)

// Constructor
public:
	CmediaPlayerPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MEDIAPLAYER };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

