#pragma once


// CTimeEdit


class CTimeEdit : public CEdit
{
	DECLARE_DYNAMIC(CTimeEdit)

public:
	CTimeEdit(CWnd* pParent, const CPoint& pt, const CRect& rt, int nID, const CString& strTime, bool bLeft);
	virtual ~CTimeEdit();

  void UpdateTime(const CString& strTime);

protected:
	DECLARE_MESSAGE_MAP()

private:
  CString m_strCutTime;
  CPoint m_ptShowTime;
};


