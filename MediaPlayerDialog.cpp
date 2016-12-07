// MediaPlayerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "MediaPlayerDialog.h"
#include "MainFrm.h"
#include "SAStatusLog.h"


extern CSAStatusLog g_statusLog;



// CMediaPlayerDialog dialog
CVLCPlayer* CMediaPlayerDialog::m_pVLCPlayer = /*CVLCPlayer::Instance()*/nullptr;

IMPLEMENT_DYNAMIC(CMediaPlayerDialog, CDialog)

static void HandleVLCEvents(const VLCEvent* pEvt, void* pUserData)
{       
  CMediaPlayerDialog* pDlg = reinterpret_cast<CMediaPlayerDialog*>(pUserData); 

  switch(pEvt->type)
  {
  case libvlc_MediaPlayerTimeChanged:
    {
      if(pDlg)
        pDlg->UpdatePosition();
      break;
    }

  case libvlc_MediaPlayerPaused:
    {
      break;
    }

  case libvlc_MediaPlayerStopped:
    {
      
      break;
    }

  case libvlc_Ended:
    {
//       CVLCPlayer::Instance()->Play();
      break;
    }

  default:
    break;
  }    
}

CMediaPlayerDialog::CMediaPlayerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMediaPlayerDialog::IDD, pParent),
  m_strMp4Path(_T(""))
{
  m_bPlaying = false;
  m_hMuteIcon = NULL;
  m_hVolumeIcon = NULL;
  m_hPlayIcon = NULL;
  m_hStopIcon = NULL;
  m_hPauseIcon = NULL;
  m_fPercent = 0.0f;
}

CMediaPlayerDialog::~CMediaPlayerDialog()
{
  SAFE_DELETE(m_pVLCPlayer);
}

void CMediaPlayerDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_STATIC_POSITION, m_mediaPosition);
  DDX_Control(pDX, IDC_SLIDER_MEDIA, m_mediaSlider);
  DDX_Control(pDX, IDC_SLIDER_VOLUME, m_volumeSlider);
  DDX_Control(pDX, IDC_STATIC_VOLUME, m_volumeLevel);
  DDX_Control(pDX, IDC_STATIC_PLAYWINDOW, m_vlcControl);
  DDX_Control(pDX, IDC_BUTTON_STOP, m_StopBtn);
  DDX_Control(pDX, IDC_BUTTON_PLAYPAUSE, m_PlayPauseBtn);
  DDX_Control(pDX, IDC_BUTTON_PREV, m_BackwardBtn);
  DDX_Control(pDX, IDC_BUTTON_NEXT, m_ForwardBtn);
  DDX_Control(pDX, IDC_STATIC_SOUND, m_SoundIcon);
}

BEGIN_MESSAGE_MAP(CMediaPlayerDialog, CDialog)
  ON_MESSAGE(WM_STARTPLAY, OnStartPlay)
  ON_WM_HSCROLL()
  ON_WM_PAINT()
//   ON_BN_CLICKED(IDC_BUTTON_PLAY, &CMediaPlayerDialog::OnBnClickedButtonPlay)
//   ON_BN_CLICKED(IDC_BUTTON_STOP, &CMediaPlayerDialog::OnBnClickedButtonStop)
  ON_MESSAGE(WM_SEEK, OnSeek)
  ON_WM_SIZE()
  ON_WM_TIMER()
  ON_MESSAGE(WM_LOOPPLAY, OnLoopPlay)
  ON_MESSAGE(WM_RESETBTNSTATUS, OnResetBtnStatus)
  ON_MESSAGE(WM_RESETPLAYSTATUS, OnResetPlayStatus)
  ON_STN_CLICKED(IDC_STATIC_SOUND, &CMediaPlayerDialog::OnStnClickedStaticSound)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_VOLUME, &CMediaPlayerDialog::OnNMCustomdrawSliderVolume)
END_MESSAGE_MAP()


// CMediaPlayerDialog message handlers
LRESULT CMediaPlayerDialog::OnStartPlay(WPARAM wParam, LPARAM lParam)
{
  CVLCPlayer::Instance()->Play();

  return 0;
}

LRESULT CMediaPlayerDialog::OnStopPlay(WPARAM wParam, LPARAM lParam)
{
  CVLCPlayer::Instance()->Stop();
  return 0;
}


BOOL CMediaPlayerDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  CVLCPlayer::Instance()->SetPlayHwnd((void*)m_vlcControl.GetSafeHwnd());
  CVLCPlayer::Instance()->SetEventHandler(&HandleVLCEvents, this);

  m_volumeSlider.SetRange(0, m_nMaxPos);
  m_mediaSlider.SetRange(0, m_nMaxPos);

  int nVolume = 50;
  m_volumeSlider.SetPos(nVolume);

  CString strVolume;
  strVolume.Format("%d", nVolume);
  m_volumeLevel.SetWindowText(strVolume);

  m_SoundIcon.ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE|SS_NOTIFY);
  m_hVolumeIcon = AfxGetApp()->LoadIcon(IDI_ICON_VOLUMN);
  m_hMuteIcon = AfxGetApp()->LoadIcon(IDI_ICON_NO_VOLUMN);

  SetIcon(m_hVolumeIcon, TRUE);
  SetIcon(m_hMuteIcon, TRUE);
  m_SoundIcon.SetIcon(m_hVolumeIcon);

  this->ShowWindow(SW_SHOWMAXIMIZED);

  m_PlayPauseBtn.SetParentDlg(this);
  m_StopBtn.SetParentDlg(this);
  m_BackwardBtn.SetParentDlg(this);
  m_ForwardBtn.SetParentDlg(this);

  m_StopBtn.EnableWindow(FALSE);
  m_BackwardBtn.EnableWindow(FALSE);
  m_ForwardBtn.EnableWindow(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMediaPlayerDialog::UpdatePosition()
{
  TRACE(_T("CMediaPlayerDialog::UpdatePosition\r\n"));

  m_nlength = CVLCPlayer::Instance()->GetLength();
  int64_t newPosition = CVLCPlayer::Instance()->GetTime();

  CTimeSpan length(static_cast<time_t>(m_nlength/1000));
  CTimeSpan actualPosition(static_cast<time_t>(newPosition/1000));

  CString lengthString;
  lengthString.Format("[%02d:%02d:%02d/%02d:%02d:%02d]",
    actualPosition.GetHours(), actualPosition.GetMinutes(), 
    actualPosition.GetSeconds(), length.GetHours(), length.GetMinutes(), 
    length.GetSeconds());

  if (m_mediaPosition.GetSafeHwnd())
  {
    m_mediaPosition.SetWindowText(lengthString);
  }

  try
  {
    int newSliderPos = m_nlength ? static_cast<int>((static_cast<double>(newPosition)/static_cast<double>(m_nlength) * 100)) : 0;

    if (m_mediaSlider.GetSafeHwnd())
    {
      m_mediaSlider.SetPos(newSliderPos);
    }
  }
  catch (...)
  {
  }
}

void CMediaPlayerDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // TODO: Add your message handler code here and/or call default
  CSliderCtrl* pSlider=reinterpret_cast<CSliderCtrl*>(pScrollBar);

  if(*pSlider == m_volumeSlider)
  {
    int pos = m_volumeSlider.GetPos();
    CVLCPlayer::Instance()->SetVolume(pos);
    CString volume;
    volume.Format("%d", pos);
    m_volumeLevel.SetWindowText(volume);
    m_muteFlag = false;
//     m_PlayPauseBtn.SetIcon(m_hMuteIcon);
  }

  if(*pSlider == m_mediaSlider)
  {
    int pos = m_mediaSlider.GetPos();
    CVLCPlayer::Instance()->SetTime(m_nlength/100*pos);
  }

  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMediaPlayerDialog::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // TODO: Add your message handler code here
  // Do not call CDialog::OnPaint() for painting messages
  if (IsZoomed())
  {
    m_vlcControl.RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
  }
}

LRESULT CMediaPlayerDialog::OnSeek(WPARAM wParam, LPARAM lParam)
{
  float fPercent = (float)lParam;

  int nMin = 0, nMax = 0;
  m_mediaSlider.GetRange(nMin, nMax);
  float fNewPos = fPercent /100 * nMax;
  float fPlayPos = fPercent/100;

  if (1.0f == fPlayPos)
  {
    fPlayPos = 0.999;
  }
  CVLCPlayer::Instance()->Seek(fPlayPos);
  m_mediaSlider.SetPos(static_cast<int>(fNewPos));

  return 0;
}


void CMediaPlayerDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  const int SPACE = 8;

  // TODO: Add your message handler code here
  if (m_vlcControl.GetSafeHwnd())
  {
    CRect rt, rtBtn;
    GetClientRect(&rt);
    rtBtn = rt;

    rt.DeflateRect(1, 1, 1, 60);
    m_vlcControl.MoveWindow(&rt);

    CRect rtPos = rt, rtVolume = rt;

    rtPos.top = rt.bottom;
    rtPos.bottom = rt.bottom + 20;
    rtPos.right = rtPos.left + rt.Width() / 5 * 4 - 50;
    m_mediaSlider.MoveWindow(&rtPos);

    rtPos.left = rtPos.right;
    rtPos.right = rt.Width() - 1;
    m_mediaPosition.MoveWindow(&rtPos);

    rtVolume.left = rt.left + rt.Width() / 5 * 4 - 10;
    rtVolume.top = rtPos.bottom+2;
    rtVolume.bottom = rtVolume.top + 15;
    rtVolume.right = rtVolume.left + rt.Width() / 5;
    m_volumeSlider.MoveWindow(&rtVolume);

    rtVolume.left = rtVolume.right;
    rtVolume.right = rt.right;
    m_volumeLevel.MoveWindow(&rtVolume);

    CRect rtSoundIcon = rtVolume;
    rtSoundIcon.left = rtVolume.left - 100;
    rtSoundIcon.right = rtSoundIcon.left + 32;
    rtSoundIcon.bottom = rtSoundIcon.top + 32;
    m_SoundIcon.MoveWindow(&rtSoundIcon);

    rtBtn.left += 3;
    rtBtn.bottom -= 3;
    rtBtn.top = rtBtn.bottom - 20;
    rtBtn.right = rtBtn.left + 20;
    m_StopBtn.MoveWindow(&rtBtn);

    rtBtn.left = rtBtn.right + SPACE;
    rtBtn.right = rtBtn.left + 20;
    m_BackwardBtn.MoveWindow(&rtBtn);

    rtBtn.left = rtBtn.right + SPACE;
    rtBtn.right = rtBtn.left + 32;
    rtBtn.top = rtBtn.bottom - 32;
    m_PlayPauseBtn.MoveWindow(&rtBtn);

    rtBtn.left = rtBtn.right + SPACE;
    rtBtn.right = rtBtn.left + 20;
    rtBtn.top = rtBtn.bottom - 20;
    m_ForwardBtn.MoveWindow(&rtBtn);
  }
}


void CMediaPlayerDialog::OnTimer(UINT_PTR nIDEvent)
{
  // TODO: Add your message handler code here and/or call default
//   CVLCPlayer::PLAYSTATE state = CVLCPlayer::Instance()->GetPlayState();
//   if (nIDEvent == WM_LOOPPLAY_TIMER && CVLCPlayer::e_Stopped == state)
//   {
//     PostMessage(WM_LOOPPLAY_TIMER);    
//   }

  CDialog::OnTimer(nIDEvent);
}

LRESULT CMediaPlayerDialog::OnLoopPlay(WPARAM wParam, LPARAM lParam)
{
  m_mediaSlider.SetPos(0);
  CVLCPlayer::Instance()->OpenMedia(m_strMp4Path);
  CVLCPlayer::Instance()->Play();

  return 0;
}

LRESULT CMediaPlayerDialog::OnResetBtnStatus(WPARAM wParam, LPARAM lParam)
{
  m_StopBtn.EnableWindow(TRUE);
  m_BackwardBtn.EnableWindow(TRUE);
  m_ForwardBtn.EnableWindow(TRUE);

  return 0;
}

LRESULT CMediaPlayerDialog::OnResetPlayStatus(WPARAM wParam, LPARAM lParam)
{
  ResetMediaSlider();
//   CVLCPlayer::Instance()->Stop();
  m_PlayPauseBtn.SetPlayState((bool)wParam);
  m_mediaPosition.SetWindowText("[00:00:00/00:00:00]");

  ::PostMessage(m_PlayPauseBtn.GetSafeHwnd(), WM_DRAWCIRCULARBTN, wParam, NULL);
  m_StopBtn.EnableWindow(true);

  return 0;
}




void CMediaPlayerDialog::OnStnClickedStaticSound()
{
  // TODO: Add your control notification handler code here
  static bool bMute = false;

  if (!bMute)
  {
    m_SoundIcon.SetIcon(m_hMuteIcon);
    bMute = true;

    CVLCPlayer::Instance()->Mute(true);
  }
  else
  {
    m_SoundIcon.SetIcon(m_hVolumeIcon);
    bMute = false;

    CVLCPlayer::Instance()->Mute(false);
  }
}


void CMediaPlayerDialog::OnNMCustomdrawSliderVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  // TODO: Add your control notification handler code here
  *pResult = 0;
}

