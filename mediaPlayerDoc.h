// mediaPlayerDoc.h : CmediaPlayerDoc ��Ľӿ�
//


#pragma once


class CmediaPlayerDoc : public CDocument
{
protected: // �������л�����
	CmediaPlayerDoc();
	DECLARE_DYNCREATE(CmediaPlayerDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CmediaPlayerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


