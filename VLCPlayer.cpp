#include "stdafx.h"
#include "VLCPlayer.h"
#include "VLCPlayerImpl.h"
#include "SAStatusLog.h"
#include "mediaPlayer.h"



CVLCPlayer* CVLCPlayer::m_pVLCPlayer = nullptr;
extern CSAStatusLog g_statusLog;
// extern CmediaPlayerApp theApp;

CVLCPlayer::CVLCPlayer()
  :m_pImpl(std::auto_ptr<CVLCPlayerImpl>(new CVLCPlayerImpl))
{
  const char* pVersion = libvlc_get_version();
  g_statusLog.StatusOut(_T("CVLCPlayer::CVLCPlayer, vlcversion"), pVersion);
}


CVLCPlayer::~CVLCPlayer()
{
//   Release();
}

CVLCPlayer* CVLCPlayer::Instance()
{
  if (!m_pVLCPlayer)
  {
    m_pVLCPlayer = new CVLCPlayer;
    if (!m_pVLCPlayer)
    {
      return NULL;
    }

    g_statusLog.StatusOut(_T("new CVLCPlayer success"));
  }

  return m_pVLCPlayer;
}

void CVLCPlayer::Close()
{
//   CReadLock l(m_lock);

  m_pImpl->Close();
}

void CVLCPlayer::SetPlayHwnd(void* pWnd)
{
//   CReadLock l(m_lock);

  m_pImpl->SetPlayHwnd(pWnd);
}

void CVLCPlayer::Play()
{
//   CReadLock l(m_lock);

  m_pImpl->Play();
}

void CVLCPlayer::Stop()
{
//   CReadLock l(m_lock);

  m_pImpl->Stop();
}

void CVLCPlayer::Pause()
{
//   CReadLock l(m_lock);

  m_pImpl->Pause();
}

void CVLCPlayer::PlayNext()
{
//   CReadLock l(m_lock);

  m_pImpl->PlayNext();
}

void CVLCPlayer::PlayPrev()
{
//   CReadLock l(m_lock);

  m_pImpl->PlayPrev();
}

void CVLCPlayer::FastForward()
{
//   CReadLock l(m_lock);

  m_pImpl->FastForward();
}

void CVLCPlayer::FastBackward()
{
//   CReadLock l(m_lock);

  m_pImpl->FastBackward();
}

void CVLCPlayer::Seek(float fPos)
{
//   CReadLock l(m_lock);

  m_pImpl->Seek(fPos);
}

void CVLCPlayer::SetEventHandler(VLCEventHandler event, void* pUserData)
{
//   CReadLock l(m_lock);

  m_pImpl->SetEventHandler(event, pUserData);
}


int64_t CVLCPlayer::GetLength()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetLength();
}

int64_t CVLCPlayer::GetTime()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetTime();
}

void CVLCPlayer::SetTime(int64_t newTime)
{
//   CReadLock l(m_lock);

  m_pImpl->SetTime(newTime);
}

void CVLCPlayer::Mute(bool mute)
{
//   CReadLock l(m_lock);

  m_pImpl->Mute(mute);
}

bool CVLCPlayer::GetMute()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetMute();
}

int  CVLCPlayer::GetVolume()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetVolume();
}

void CVLCPlayer::SetVolume(int nVolume)
{
//   CReadLock l(m_lock);

  m_pImpl->SetVolume(nVolume);
}

void CVLCPlayer::OpenMedia(const char* const pMediaPathName)
{
//   CReadLock l(m_lock);

  m_pImpl->OpenMedia(pMediaPathName);
}

libvlc_time_t CVLCPlayer::GetDuration()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetDuration();
}

void CVLCPlayer::Release()
{
//   CReadLock l(m_lock);

  m_pImpl->Release();
}

CVLCPlayer::PLAYSTATE CVLCPlayer::GetPlayState()
{
//   CReadLock l(m_lock);

  switch (m_pImpl->GetPlayState())
  {
  case libvlc_NothingSpecial:
  	  return e_NothingSpecial;

  case libvlc_Opening:
      return e_Opening;

  case libvlc_Buffering:
    return e_Buffering;

  case libvlc_Playing:
    return e_Playing;

  case libvlc_Paused:
    return e_Paused;

  case libvlc_Stopped:
    return e_Stopped;

  case libvlc_Ended:
    return e_Ended;

  default:
    return e_Error;
  }
}

int CVLCPlayer::GetAudioChannel()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetAudioChannel();
}

int CVLCPlayer::GetWidth()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetWidth();
}

int CVLCPlayer::GetHeight()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetHeight();
}

float CVLCPlayer::GetFPS()
{
//   CReadLock l(m_lock);

  return m_pImpl->GetFPS();
}
