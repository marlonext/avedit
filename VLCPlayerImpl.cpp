#include "stdafx.h"
#include "VLCPlayerImpl.h"
#include "SAStatusLog.h"


extern CSAStatusLog g_statusLog;


CVLCPlayerImpl::CVLCPlayerImpl()
{
  g_statusLog.StatusOut(_T("CVLCPlayerImpl::CVLCPlayerImpl"));

  Init();

  const char * const vlc_args[] = {
    "-I", "dumy",      // No special interface
    "--ignore-config", // Don't use VLC's config
    "--plugin-path= ./plugins" };

  // init vlc modules, should be done only once
  m_pInst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
//   m_pInst = libvlc_new(0, NULL);
  if (!m_pInst)
  {
    g_statusLog.StatusOut("libvlc_new return null");
    return;
  }

  g_statusLog.StatusOut("libvlc_new success");

  // Create a media player playing environment
  m_pPlayer = libvlc_media_player_new(m_pInst);
  if (!m_pPlayer)
  {
    g_statusLog.StatusOut("libvlc_media_player_new return null");
    return;
  }
  g_statusLog.StatusOut("libvlc_media_player_new success");

  // Create an event manager for the player for handling e.g. time change events
  m_pEventManager = libvlc_media_player_event_manager(m_pPlayer);
  if (!m_pEventManager)
  {
    g_statusLog.StatusOut("libvlc_media_player_event_manager return null");
    return;
  }

  g_statusLog.StatusOut("libvlc_media_player_event_manager success");
}


CVLCPlayerImpl::~CVLCPlayerImpl()
{
//   CAutoLock l(m_cs);

  libvlc_media_player_release (m_pPlayer);
  libvlc_release (m_pInst);
}

void CVLCPlayerImpl::Close()
{
//   CAutoLock l(m_cs);

  libvlc_media_player_release(m_pPlayer);
  libvlc_release(m_pInst);
}

void CVLCPlayerImpl::Play()
{
//   CAutoLock l(m_cs);
  if (m_pPlayer)
  {
    libvlc_media_player_play(m_pPlayer);
  }
}

void CVLCPlayerImpl::Stop()
{
//   CAutoLock l(m_cs);

#if 1
  SYSTEMTIME now;
  GetLocalTime(&now);

  g_statusLog.StatusOut(_T("%d:%d:%d:%d threadID = %d, CMediaPlayerDialog::UpdatePosition()"), 
    now.wHour, now.wMinute, now.wSecond, now.wMilliseconds, GetCurrentThreadId());
#endif

  if (m_pMedia)
  {
    libvlc_media_release(m_pMedia);
    m_pMedia = nullptr;
  }
  libvlc_media_player_stop(m_pPlayer);
}

void CVLCPlayerImpl::Pause()
{
//   CAutoLock l(m_cs);

  libvlc_media_player_pause(m_pPlayer);
}

void CVLCPlayerImpl::PlayNext()
{
//   CAutoLock l(m_cs);
}

void CVLCPlayerImpl::PlayPrev()
{
//   CAutoLock l(m_cs);
}

void CVLCPlayerImpl::FastForward()
{
//   CAutoLock l(m_cs);
}

void CVLCPlayerImpl::FastBackward()
{
//   CAutoLock l(m_cs);
}

//fpos value between 0.0 and 1.0
void CVLCPlayerImpl::Seek(float fPos)
{
//   CAutoLock l(m_cs);

  libvlc_media_player_set_position(m_pPlayer, fPos);
}

void CVLCPlayerImpl::SetPlayHwnd(void* pHwnd)
{
//   CAutoLock l(m_cs);

  if (pHwnd)
  {
    libvlc_media_player_set_hwnd(m_pPlayer, pHwnd);
  }
}

void CVLCPlayerImpl::Init()
{
  m_pInst = nullptr;
  m_pPlayer = nullptr;
  m_pMedia = nullptr;
  m_pEventManager = nullptr;
}

void CVLCPlayerImpl::SetEventHandler(VLCEventHandler event, void* pUserData)
{
//   CAutoLock l(m_cs);

  m_EventHandler = event;
  libvlc_event_attach(m_pEventManager, libvlc_MediaPlayerTimeChanged,
                      m_EventHandler, pUserData);

  libvlc_event_attach(m_pEventManager, libvlc_MediaPlayerPositionChanged,
                      m_EventHandler, pUserData);
}

int64_t CVLCPlayerImpl::GetLength()
{
//   CAutoLock l(m_cs);

  return libvlc_media_player_get_length(m_pPlayer);
}

int64_t CVLCPlayerImpl::GetTime()
{
//   CAutoLock l(m_cs);
  return libvlc_media_player_get_time(m_pPlayer);
}


void CVLCPlayerImpl::SetTime(int64_t newTime)
{
//   CAutoLock l(m_cs);
  libvlc_media_player_set_time(m_pPlayer, newTime);
}

void CVLCPlayerImpl::Mute(bool mute/* = true*/)
{
//   CAutoLock l(m_cs);
  libvlc_audio_set_mute(m_pPlayer, mute);
}

bool CVLCPlayerImpl::GetMute()
{
//   CAutoLock l(m_cs);
  return libvlc_audio_get_mute(m_pPlayer) != -1 ? true : false;
}

int  CVLCPlayerImpl::GetVolume()
{
//   CAutoLock l(m_cs);
  return libvlc_audio_get_volume(m_pPlayer);
}

void CVLCPlayerImpl::SetVolume(int nVolume)
{
//   CAutoLock l(m_cs);
  libvlc_audio_set_volume(m_pPlayer, nVolume);
}

void CVLCPlayerImpl::OpenMedia(const char* const pMediaPathName)
{
//   CAutoLock l(m_cs);

  m_pMedia = libvlc_media_new_path(m_pInst, pMediaPathName);
  if (!m_pMedia)
  {
    g_statusLog.StatusOut("libvlc_media_new_path return nullptr");
  }
  libvlc_media_add_option(m_pMedia, "input-repeat=-1");/*--input-fast-seek*/

  g_statusLog.StatusOut("libvlc_media_new_path success");

//   m_pPlayer = libvlc_media_player_new_from_media(m_pMedia);
  libvlc_media_player_set_media (m_pPlayer, m_pMedia); 
//   libvlc_media_release(m_pMedia);

  g_statusLog.StatusOut("OpenMedia success: %s", pMediaPathName);
}

libvlc_time_t CVLCPlayerImpl::GetDuration()
{
//   CAutoLock l(m_cs);

  libvlc_media_parse(m_pMedia);
  return libvlc_media_get_duration(m_pMedia);
//   return libvlc_media_player_get_length(m_pPlayer);
}

void CVLCPlayerImpl::Release()
{
//   CAutoLock l(m_cs);

  libvlc_media_release(m_pMedia);
  m_pMedia = nullptr;
}

//play state
// typedef enum libvlc_state_t
// {
//   libvlc_NothingSpecial=0,
//   libvlc_Opening,
//   libvlc_Buffering,
//   libvlc_Playing,
//   libvlc_Paused,
//   libvlc_Stopped,
//   libvlc_Ended,
//   libvlc_Error
// } libvlc_state_t;
libvlc_state_t CVLCPlayerImpl::GetPlayState()
{
//   CAutoLock l(m_cs);

  return libvlc_media_player_get_state(m_pPlayer);
}

int CVLCPlayerImpl::GetAudioChannel()
{
//   CAutoLock l(m_cs);

  return libvlc_audio_get_channel(m_pPlayer);
}

int CVLCPlayerImpl::GetWidth()
{
//   CAutoLock l(m_cs);

  return libvlc_video_get_width(m_pPlayer);
}

int CVLCPlayerImpl::GetHeight()
{
//   CAutoLock l(m_cs);

  return libvlc_video_get_height(m_pPlayer);
}

float CVLCPlayerImpl::GetFPS()
{
//   CAutoLock l(m_cs);

  return libvlc_media_player_get_fps(m_pPlayer);
}



//////////////////////////////////////////////////////////////////////////
CVLCLib* CVLCLib::m_pInstance = nullptr;

CVLCLib::CVLCLib()
{
  m_pLibVlcNew = nullptr;
  m_hLib = nullptr;
}

CVLCLib::~CVLCLib()
{
  SAFE_DELETE(m_pInstance);
  ::FreeLibrary(m_hLib);
}

CVLCLib* CVLCLib::VLCLibInstance()
{
  if (!m_pInstance)
  {
    m_pInstance = new CVLCLib;
    if (!m_pInstance)
    {
      return nullptr;
    }
  }

  return m_pInstance;
}

bool CVLCLib::_LoadLib()
{
  m_hLib = ::LoadLibrary("libvlc.dll");
  do 
  {
    if (m_hLib)
    {
      m_pLibVlcNew = (LIBVLC_NEW_FUN)::GetProcAddress(m_hLib, "libvlc_new");
      if (!m_pLibVlcNew)
      {
        return false;
      }

      m_pMediaPlayerNew = (LIBVLC_MEDIA_PLAYER_NEW_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_new");
      if (!m_pMediaPlayerNew)
      {
        return false;
      }

      m_pMediaPlayerEventManager = (LIBVLC_MEDIA_PLAYER_EVENT_MANAGER_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_event_manager");
      if (!m_pMediaPlayerEventManager)
      {
        return false;
      }

      m_pMediaPlayerRelease = (LIBVLC_MEDIA_PLAYER_RELEASE_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_release");
      if (!m_pMediaPlayerRelease)
      {
        return false;
      }

      m_pRelease = (LIBVLC_RELEASE_FUN)::GetProcAddress(m_hLib, "libvlc_release");
      if (!m_pRelease)
      {
        return false;
      }

      m_pMediaPlayerPlay = (LIBVLC_MEDIA_PLAYER_PLAY_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_play");
      if (!m_pMediaPlayerPlay)
      {
        return false;
      }

      m_pMediaPlayerStop = (LIBVLC_MEDIA_PLAYER_STOP_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_stop");
      if (!m_pMediaPlayerStop)
      {
        return false;
      }

      m_pMediaPlayerPause = (LIBVLC_MEDIA_PLAYER_PAUSE_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_pause");
      if (!m_pMediaPlayerPause)
      {
        return false;
      }

      m_pMediaPlayerSetPosition = (LIBVLC_MEDIA_PLAYER_SET_POSITION_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_set_position");
      if (!m_pMediaPlayerSetPosition)
      {
        return false;
      }

      m_pMediaPlayerSetHwnd = (LIBVLC_MEDIA_PLAYER_SET_HWND_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_set_hwnd");
      if (!m_pMediaPlayerSetHwnd)
      {
        return false;
      }

      m_pEventAttach = (LIBVLC_EVENT_ATTACH_FUN)::GetProcAddress(m_hLib, "libvlc_event_attach");
      if (!m_pEventAttach)
      {
        return false;
      }

      m_pMediaPlayerGetLength = (LIBVLC_MEDIA_PLAYER_GET_LENGTH_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_get_length");
      if (!m_pMediaPlayerGetLength)
      {
        return false;
      }

      m_pMediaPlayerGetTime = (LIBVLC_MEDIA_PLAYER_GET_TIME_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_get_time");
      if (!m_pMediaPlayerGetTime)
      {
        return false;
      }

      m_pMediaPlayerSetTime = (LIBVLC_MEDIA_PLAYER_SET_TIME_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_set_time");
      if (!m_pMediaPlayerSetTime)
      {
        return false;
      }

      m_pAudioSetMute = (LIBVLC_AUDIO_SET_MUTE_FUN)::GetProcAddress(m_hLib, "libvlc_audio_set_mute");
      if (!m_pAudioSetMute)
      {
        return false;
      }

      m_pAudioGetMute = (LIBVLC_AUDIO_GET_MUTE_FUN)::GetProcAddress(m_hLib, "libvlc_audio_get_mute");
      if (!m_pAudioGetMute)
      {
        return false;
      }

      m_pAudioGetVolume = (LIBVLC_AUDIO_GET_VOLUME_FUN)::GetProcAddress(m_hLib, "libvlc_audio_get_volume");
      if (!m_pAudioGetVolume)
      {
        return false;
      }

      m_AudioSetVolume = (LIBVLC_AUDIO_SET_VOLUME_FUN)::GetProcAddress(m_hLib, "libvlc_audio_set_volume");
      if (!m_AudioSetVolume)
      {
        return false;
      }

      m_pMediaNewPath = (LIBVLC_MEDIA_NEW_PATH_FUN)::GetProcAddress(m_hLib, "libvlc_media_new_path");
      if (!m_pMediaNewPath)
      {
        return false;
      }

      m_pMediaPlayerSetMedia = (LIBVLC_MEDIA_PLAYER_SET_MEDIA_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_set_media");
      if (!m_pMediaPlayerSetMedia)
      {
        return false;
      }

      m_pMediaParse = (LIBVLC_MEDIA_PARSE_FUN)::GetProcAddress(m_hLib, "libvlc_media_parse");
      if (!m_pMediaParse)
      {
        return false;
      }

      m_pMediaGetDuration = (LIBVLC_MEDIA_GET_DURATION_FUN)::GetProcAddress(m_hLib, "libvlc_media_get_duration");
      if (!m_pMediaGetDuration)
      {
        return false;
      }

      m_pMediaRelease = (LIBVLC_MEDIA_RELEASE_FUN)::GetProcAddress(m_hLib, "libvlc_media_release");
      if (!m_pMediaRelease)
      {
        return false;
      }

      m_pMediaPlayerGetState = (LIBVLC_MEDIA_PLAYER_GET_STATE_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_get_state");
      if (!m_pMediaPlayerGetState)
      {
        return false;
      }

      m_pAudioGetChannel = (LIBVLC_AUDIO_GET_CHANNEL_FUN)::GetProcAddress(m_hLib, "libvlc_audio_get_channel");
      if (!m_pAudioGetChannel)
      {
        return false;
      }

      m_pVideoGetWidth = (LIBVLC_VIDEO_GET_WIDTH_FUN)::GetProcAddress(m_hLib, "libvlc_video_get_width");
      if (!m_pVideoGetWidth)
      {
        return false;
      }

      m_pVideoGetHeight = (LIBVLC_VIDEO_GET_HEIGHT_FUN)::GetProcAddress(m_hLib, "libvlc_video_get_height");
      if (!m_pVideoGetHeight)
      {
        return false;
      }

      m_pMediaPlayerGetFps = (LIBVLC_MEDIA_PLAYER_GET_FPS_FUN)::GetProcAddress(m_hLib, "libvlc_media_player_get_fps");
      if (!m_pMediaPlayerGetFps)
      {
        return false;
      }
    }
  } while (0);

  return true;
}

