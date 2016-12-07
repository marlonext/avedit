#pragma once

#include "EditTask.h"

class CTimerShaft;

class CSettingDialog : public CDialog
{
	DECLARE_DYNAMIC(CSettingDialog)

  typedef enum _INPUT_CHAR_IDENT
  {
    e_empty,
    e_notallnumber,
    e_legal
  }INPUT_CHAR_IDENT;

public:
	CSettingDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDialog();

  bool NeedUpload() {return (!m_strTitlePath.IsEmpty() || !m_strTailPath.IsEmpty());}
// Dialog Data
	enum { IDD = IDD_DIALOG_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
  CString m_strTitlePath;
  CString m_strTailPath;
  CString m_strTitleTime;
  CString m_strTailTime;
  CString m_strFPS;

  CString m_strTitlePicName;
  CString m_strTailPicName;

  CString m_strDestFile;

  CEditOpts m_EditOpts; 

  int64_t m_nTitleDuration;
  int64_t m_nTailDuration;

  CToolTipCtrl m_ToolTip;
  CTimerShaft&  m_timerShaft;

  int IsLegalInput(const CString& str);
  void AddPic(CString& strName, CString& strPicName);
  BOOL IsDestFileNameLegal();
  BOOL IsSameTaskName();

public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnBnClickedButtonTitle();
  afx_msg void OnBnClickedButtonTail();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  int GetFPS() 
  {
    return StrToInt(m_strFPS);
  }
  int GetTitleDuration()
  {
    return m_nTitleDuration;
  }
  const CString& GetTitlePicName()
  {
    return m_strTitlePicName;
  }

  int GetTailDuration()
  {
    return m_nTailDuration;
  }
  const CString& GetTailPicName()
  {
    return m_strTailPicName;
  }
  const CString& GetDestFileName() {return m_strDestFile;}

  const CString& GetTitlePath() {return m_strTitlePath;}
  const CString& GetTailPath() {return m_strTailPath;}
  virtual BOOL OnInitDialog();
  
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
