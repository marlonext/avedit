// mediaPlayerDoc.cpp : CmediaPlayerDoc ���ʵ��
//

#include "stdafx.h"
#include "mediaPlayer.h"

#include "mediaPlayerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmediaPlayerDoc

IMPLEMENT_DYNCREATE(CmediaPlayerDoc, CDocument)

BEGIN_MESSAGE_MAP(CmediaPlayerDoc, CDocument)
END_MESSAGE_MAP()


// CmediaPlayerDoc ����/����

CmediaPlayerDoc::CmediaPlayerDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CmediaPlayerDoc::~CmediaPlayerDoc()
{
}

BOOL CmediaPlayerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CmediaPlayerDoc ���л�

void CmediaPlayerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CmediaPlayerDoc ���

#ifdef _DEBUG
void CmediaPlayerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CmediaPlayerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CmediaPlayerDoc ����
