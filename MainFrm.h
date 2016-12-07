// MainFrm.h : CMainFrame 类的接口
//


#pragma once


typedef unsigned short int UInt16b;

class CEditTaskListCtrl;
class CFilePreView;
class CProgressCtrlView;
class CPreView;

class CMainFrame : public CFrameWnd
{	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:
  CFilePreView* m_pFileView;
  CProgressCtrlView* m_pProgressView;
  CPreView* m_pPreView;

// 操作
public:
  CSplitterWnd& GetSplitterWnd(){return m_splitWnd1;}
  CSplitterWnd& GetSplitterWnd2(){return m_splitWnd2;}
  void UpdateStatueBar(const EDIT_TASK_STATUS& taskStatus);
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  void OnUpdatePane(CCmdUI* pCmdUI);
  afx_msg LRESULT OnUpdatePaneText(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnOffline(WPARAM wParam, LPARAM lParam);

  bool IsEditFinish() {return m_bEditFinish;}

  void AutoDeleteTask(CEditTaskListCtrl& listCtrl,int nTaskId);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	/*static*/ CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
  CSplitterWnd m_splitWnd1;
  CSplitterWnd m_splitWnd2;
  bool m_bEditFinish;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnClose();
  afx_msg void OnSize(UINT nType, int cx, int cy);
};


