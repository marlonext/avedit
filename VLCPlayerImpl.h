#pragma once

#include <string>
// #include "vlc/vlc.h"
// #include "vlc/libvlc.h"
// #include "vlc/libvlc_media.h"
// #include "vlc/libvlc_media_player.h"
#include "VLCPlayer.h"
#include "./libvlc_include/vlc/vlc.h"
#include "./libvlc_include/vlc/libvlc.h"
#include "./libvlc_include/vlc/libvlc_media.h"
#include "./libvlc_include/vlc/libvlc_media_player.h"


class CVLCPlayer;
class CVLCLib;


class CVLCPlayerImpl
{
  friend class CVLCPlayer;

public:
  CVLCPlayerImpl();
  virtual ~CVLCPlayerImpl();

private:
  void Close();
  void Play();
  void Stop();
  void Pause();
  void PlayNext();
  void PlayPrev();
  void FastForward();
  void FastBackward();
  void Seek(float fPos);
  void SetPlayHwnd(void* pHwnd);
  void Init();
  void SetEventHandler(VLCEventHandler event, void* pUserData);
  int64_t GetLength();
  int64_t GetTime();
  void SetTime(int64_t newTime);
  void Mute(bool mute = true);
  bool GetMute();
  int  GetVolume();
  void SetVolume(int nVolume);
  void OpenMedia(const char* const pMediaPathName);
  libvlc_time_t GetDuration();
  void Release();
  libvlc_state_t GetPlayState();
  int GetAudioChannel();
  int GetWidth();
  int GetHeight();
  float GetFPS();

private:
  libvlc_instance_t*      m_pInst;
  libvlc_media_player_t*  m_pPlayer;
  libvlc_media_t*         m_pMedia;
  libvlc_event_manager_t* m_pEventManager;
  VLCEventHandler         m_EventHandler;
  VLCEventHandler         m_EventHandler1;

  std::string m_strPath;
};



class CVLCLib
{
  friend class CVLCPlayerImpl;

  typedef libvlc_instance_t* (*LIBVLC_NEW_FUN)( int argc , const char *const *argv );
  typedef libvlc_media_player_t* (*LIBVLC_MEDIA_PLAYER_NEW_FUN)( libvlc_instance_t *p_libvlc_instance );
  typedef libvlc_event_manager_t* (*LIBVLC_MEDIA_PLAYER_EVENT_MANAGER_FUN)( libvlc_media_player_t *p_mi );
  typedef libvlc_event_manager_t* (*LIBVLC_MEDIA_PLAYER_RELEASE_FUN)( libvlc_media_player_t *p_mi );
  typedef void (*LIBVLC_RELEASE_FUN)( libvlc_instance_t *p_instance );
  typedef int (*LIBVLC_MEDIA_PLAYER_PLAY_FUN) ( libvlc_media_player_t *p_mi );
  typedef void (*LIBVLC_MEDIA_PLAYER_STOP_FUN) ( libvlc_media_player_t *p_mi );
  typedef void (*LIBVLC_MEDIA_PLAYER_PAUSE_FUN) ( libvlc_media_player_t *p_mi );
  typedef void (*LIBVLC_MEDIA_PLAYER_SET_POSITION_FUN)( libvlc_media_player_t *p_mi, float f_pos );
  typedef void (*LIBVLC_MEDIA_PLAYER_SET_HWND_FUN)( libvlc_media_player_t *p_mi, void *drawable );
  typedef int (*LIBVLC_EVENT_ATTACH_FUN)( libvlc_event_manager_t *p_event_manager, libvlc_event_type_t i_event_type, libvlc_callback_t f_callback, void *user_data );
  typedef libvlc_time_t (*LIBVLC_MEDIA_PLAYER_GET_LENGTH_FUN)( libvlc_media_player_t *p_mi );
  typedef libvlc_time_t (*LIBVLC_MEDIA_PLAYER_GET_TIME_FUN)( libvlc_media_player_t *p_mi );
  typedef void (*LIBVLC_MEDIA_PLAYER_SET_TIME_FUN)( libvlc_media_player_t *p_mi, libvlc_time_t i_time );
  typedef void (*LIBVLC_AUDIO_SET_MUTE_FUN)( libvlc_media_player_t *p_mi, int status );
  typedef int (*LIBVLC_AUDIO_GET_MUTE_FUN)( libvlc_media_player_t *p_mi );
  typedef int (*LIBVLC_AUDIO_GET_VOLUME_FUN)( libvlc_media_player_t *p_mi );
  typedef int (*LIBVLC_AUDIO_SET_VOLUME_FUN)( libvlc_media_player_t *p_mi, int i_volume );
  typedef libvlc_media_t* (*LIBVLC_MEDIA_NEW_PATH_FUN)( libvlc_instance_t *p_instance, const char *path );
  typedef void (*LIBVLC_MEDIA_PLAYER_SET_MEDIA_FUN)( libvlc_media_player_t *p_mi, libvlc_media_t *p_md );
  typedef void (*LIBVLC_MEDIA_PARSE_FUN)( libvlc_media_t *p_md );
  typedef libvlc_time_t (*LIBVLC_MEDIA_GET_DURATION_FUN)( libvlc_media_t *p_md );
  typedef void (*LIBVLC_MEDIA_RELEASE_FUN)( libvlc_media_t *p_md );
  typedef libvlc_state_t (*LIBVLC_MEDIA_PLAYER_GET_STATE_FUN)( libvlc_media_player_t *p_mi );
  typedef int (*LIBVLC_AUDIO_GET_CHANNEL_FUN)( libvlc_media_player_t *p_mi );
  typedef int (*LIBVLC_VIDEO_GET_WIDTH_FUN)( libvlc_media_player_t *p_mi );
  typedef int (*LIBVLC_VIDEO_GET_HEIGHT_FUN)( libvlc_media_player_t *p_mi );
  typedef float (*LIBVLC_MEDIA_PLAYER_GET_FPS_FUN)( libvlc_media_player_t *p_mi );

public:
  static CVLCLib* VLCLibInstance();
  virtual ~CVLCLib();

private:
  CVLCLib();
  bool _LoadLib();
  static CVLCLib* m_pInstance;

  HMODULE m_hLib;

  LIBVLC_NEW_FUN m_pLibVlcNew;
  LIBVLC_MEDIA_PLAYER_NEW_FUN m_pMediaPlayerNew;
  LIBVLC_MEDIA_PLAYER_EVENT_MANAGER_FUN m_pMediaPlayerEventManager;
  LIBVLC_MEDIA_PLAYER_RELEASE_FUN m_pMediaPlayerRelease;
  LIBVLC_RELEASE_FUN m_pRelease;
  LIBVLC_MEDIA_PLAYER_PLAY_FUN m_pMediaPlayerPlay;
  LIBVLC_MEDIA_PLAYER_STOP_FUN m_pMediaPlayerStop;
  LIBVLC_MEDIA_PLAYER_PAUSE_FUN m_pMediaPlayerPause;
  LIBVLC_MEDIA_PLAYER_SET_POSITION_FUN m_pMediaPlayerSetPosition;
  LIBVLC_MEDIA_PLAYER_SET_HWND_FUN m_pMediaPlayerSetHwnd;
  LIBVLC_EVENT_ATTACH_FUN m_pEventAttach;
  LIBVLC_MEDIA_PLAYER_GET_LENGTH_FUN m_pMediaPlayerGetLength;
  LIBVLC_MEDIA_PLAYER_GET_TIME_FUN m_pMediaPlayerGetTime;
  LIBVLC_MEDIA_PLAYER_SET_TIME_FUN m_pMediaPlayerSetTime;
  LIBVLC_AUDIO_SET_MUTE_FUN m_pAudioSetMute;
  LIBVLC_AUDIO_GET_MUTE_FUN m_pAudioGetMute;
  LIBVLC_AUDIO_GET_VOLUME_FUN m_pAudioGetVolume;
  LIBVLC_AUDIO_SET_VOLUME_FUN m_AudioSetVolume;
  LIBVLC_MEDIA_NEW_PATH_FUN m_pMediaNewPath;
  LIBVLC_MEDIA_PLAYER_SET_MEDIA_FUN m_pMediaPlayerSetMedia;
  LIBVLC_MEDIA_PARSE_FUN m_pMediaParse;
  LIBVLC_MEDIA_GET_DURATION_FUN m_pMediaGetDuration;
  LIBVLC_MEDIA_RELEASE_FUN m_pMediaRelease;
  LIBVLC_MEDIA_PLAYER_GET_STATE_FUN m_pMediaPlayerGetState;
  LIBVLC_AUDIO_GET_CHANNEL_FUN m_pAudioGetChannel;
  LIBVLC_VIDEO_GET_WIDTH_FUN m_pVideoGetWidth;
  LIBVLC_VIDEO_GET_HEIGHT_FUN m_pVideoGetHeight;
  LIBVLC_MEDIA_PLAYER_GET_FPS_FUN m_pMediaPlayerGetFps;

};
