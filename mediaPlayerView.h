// mediaPlayerView.h : CmediaPlayerView ��Ľӿ�
//


#pragma once


class CmediaPlayerView : public CView
{
protected: // �������л�����
	CmediaPlayerView();
	DECLARE_DYNCREATE(CmediaPlayerView)

// ����
public:
	CmediaPlayerDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CmediaPlayerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // mediaPlayerView.cpp �еĵ��԰汾
inline CmediaPlayerDoc* CmediaPlayerView::GetDocument() const
   { return reinterpret_cast<CmediaPlayerDoc*>(m_pDocument); }
#endif

