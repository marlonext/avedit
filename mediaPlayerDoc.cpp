// mediaPlayerDoc.cpp : CmediaPlayerDoc 类的实现
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


// CmediaPlayerDoc 构造/析构

CmediaPlayerDoc::CmediaPlayerDoc()
{
	// TODO: 在此添加一次性构造代码

}

CmediaPlayerDoc::~CmediaPlayerDoc()
{
}

BOOL CmediaPlayerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CmediaPlayerDoc 序列化

void CmediaPlayerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CmediaPlayerDoc 诊断

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


// CmediaPlayerDoc 命令
