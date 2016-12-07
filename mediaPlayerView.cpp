// mediaPlayerView.cpp : CmediaPlayerView 类的实现
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

// CmediaPlayerView 构造/析构

CmediaPlayerView::CmediaPlayerView()
{
	// TODO: 在此处添加构造代码

}

CmediaPlayerView::~CmediaPlayerView()
{
}

BOOL CmediaPlayerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CmediaPlayerView 绘制

void CmediaPlayerView::OnDraw(CDC* /*pDC*/)
{
	CmediaPlayerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CmediaPlayerView 诊断

#ifdef _DEBUG
void CmediaPlayerView::AssertValid() const
{
	CView::AssertValid();
}

void CmediaPlayerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmediaPlayerDoc* CmediaPlayerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmediaPlayerDoc)));
	return (CmediaPlayerDoc*)m_pDocument;
}
#endif //_DEBUG


// CmediaPlayerView 消息处理程序
