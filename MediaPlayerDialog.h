#pragma once
#include "VLCPlayer.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "stdint.h"
#include "PlayPauseButton.h"
#include "StopButton.h"
#include "ForwardButton.h"
#include "BackwardButton.h"


// CMediaPlayerDialog dialog

class CMediaPlayerDialog : public CDialog
{
	DECLARE_DYNAMIC(CMediaPlayerDialog)

public:
	CMediaPlayerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMediaPlayerDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROGRESSCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg LRESULT OnStartPlay(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnStopPlay(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSeek(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnResetBtnStatus(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnResetPlayStatus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
  virtual BOOL OnInitDialog();
  void UpdatePosition();
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnPaint();
  void SetSeekPos(float fPercent){m_fPercent =  fPercent;}
  void SetMp4Path(const CString& strPath){m_strMp4Path = strPath;}

  bool IsPlaying() {return m_bPlaying;}
  const CString& GetMp4Path() {return m_strMp4Path;}
  void ResetMediaSlider(){m_mediaSlider.SetPos(0);}
  CStatic& GetVLCCtrl() {return m_vlcControl;}
  
  static CVLCPlayer* m_pVLCPlayer;

private:
  bool m_bPlaying;
  bool m_muteFlag;
  int64_t m_nlength;   

  CStatic m_mediaPosition;
  CSliderCtrl m_mediaSlider;
  CSliderCtrl m_volumeSlider;
  CStatic m_vlcControl;
  CStatic m_volumeLevel;
  CStatic m_SoundIcon;

  HICON m_hMuteIcon;
  HICON m_hVolumeIcon;
  HICON m_hPlayIcon;
  HICON m_hStopIcon;
  HICON m_hPauseIcon;  

  float m_fPercent;
  CString m_strMp4Path;

  const static int m_nMaxPos = 100;

  CStopButton m_StopBtn;
  CPlayPauseButton m_PlayPauseBtn;
  CForwardButton m_ForwardBtn;
  CBackwardButton m_BackwardBtn;

public:
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg LRESULT OnLoopPlay(WPARAM wParam, LPARAM lParam);

  afx_msg void OnStnClickedStaticSound();
  afx_msg void OnNMCustomdrawSliderVolume(NMHDR *pNMHDR, LRESULT *pResult);
};
