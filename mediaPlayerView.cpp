// mediaPlayerView.cpp : CmediaPlayerView ���ʵ��
//

#include "stdafx.h"
#include "mediaPlayer.h"

#include "mediaPlayerDoc.h"
#include "mediaPlayerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmediaPlayerView

IMPLEMENT_DYNCREATE(CmediaPlayerView, CView)

BEGIN_MESSAGE_MAP(CmediaPlayerView, CView)
END_MESSAGE_MAP()

// CmediaPlayerView ����/����

CmediaPlayerView::CmediaPlayerView()
{
	// TODO: �ڴ˴���ӹ������

}

CmediaPlayerView::~CmediaPlayerView()
{
}

BOOL CmediaPlayerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CmediaPlayerView ����

void CmediaPlayerView::OnDraw(CDC* /*pDC*/)
{
	CmediaPlayerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CmediaPlayerView ���

#ifdef _DEBUG
void CmediaPlayerView::AssertValid() const
{
	CView::AssertValid();
}

void CmediaPlayerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmediaPlayerDoc* CmediaPlayerView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmediaPlayerDoc)));
	return (CmediaPlayerDoc*)m_pDocument;
}
#endif //_DEBUG


// CmediaPlayerView ��Ϣ�������
