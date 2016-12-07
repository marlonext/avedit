#pragma once
#include <vector>
#include "afxwin.h"


// CFilePrevDialog dialog
#define ICON_STATIC_BASE 60000
#define NAME_STATIC_BASE 61000

class CFilePrevDialog : public CDialog
{
	DECLARE_DYNAMIC(CFilePrevDialog)

public:
	CFilePrevDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFilePrevDialog();

  afx_msg LRESULT OnAddMp4(WPARAM wParam, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  const std::vector<CString>& GetFileNameAry() {return m_fileNameAry;}
  const std::vector<CString>& GetMp4PathAry() {return m_filePathAry;}
  int GetEditFileIndex() {return m_nSelMp4;}

  void SetMp4Dir(const CString& strDir){m_strDir = strDir;}
  void SetMp4Name(const CString& strMp4){m_strMp4Name = strMp4;}
  const CString& GetPlayMp4() {return m_strMp4Name;}

  void SetCurSelMp4(int nSel) {m_nSelMp4 = nSel;}
  void UpdateUI();

// Dialog Data
	enum { IDD = IDD_DIALOG_FILEPREV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg LRESULT OnSetFileIcon(WPARAM wParam, LPARAM lParam);
  afx_msg void OnMP4Operation(UINT nID);
  afx_msg void OnMP4OperationUpdataUI(CCmdUI* pCmdUI);
  afx_msg LRESULT OnUpdateUI(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnShowMp4Name(WPARAM wParam, LPARAM lParam);

  bool AddMp4();
  bool RemoveMp4();
  bool ClearAllMp4();
  bool Mp4PrevView();
  void BegingDrag(const CPoint& point);
  HBITMAP ConvertIconToBitmap(HICON hIcon, int nWidth, int nHeight, COLORREF clrBackground);

	DECLARE_MESSAGE_MAP()

private:
  std::vector<CString> m_filePathAry;
  std::vector<CString> m_fileNameAry;
  std::vector<SHFILEINFO> m_stFileInfoAryL;
  std::vector<SHFILEINFO> m_stFileInfoAryS;
  std::vector<HICON> m_hLargeIcon;
  std::vector<HICON> m_hSmallIcon;
  CStatic m_IconCtrl[MAX_EDIT_FILE_NUM];
  CStatic m_FileNameCtrl[MAX_EDIT_FILE_NUM];

  std::vector<CRect> m_IconRect;
  std::vector<CRect> m_Mp4NameRect;
  std::auto_ptr<CMenu> m_pPopupMenu;
  std::vector<BYTE> m_UseFlagAry;

  bool m_bSelect;
  int m_nCurSelMenu;
  int m_nSelMp4;            //index of selected mp4
  int m_nLastSelMp4;        
  bool m_bMp4Added;         //whether mp4 added
  CString m_strMp4Name;     //name of selected mp4
  CPoint m_ptRemove;      
  CString m_strDir;

public:
  virtual BOOL OnInitDialog();
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  bool IsMp4Selected(const CPoint& pt, int& nIndex);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnSize(UINT nType, int cx, int cy);

private:
  void AdjustFlagAry(int nIndex);
};
