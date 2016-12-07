#pragma once
#include <string>
#include <memory>
#include "stdint.h"
#include "vlc/vlc.h"
#include "vlc/libvlc_events.h"
#include "ScopeLock.h"


class CVLCPlayerImpl;

// Typedefs for VLC types
typedef struct libvlc_event_t           VLCEvent;                     ///< A vlc event.
typedef void(*VLCEventHandler)         (const VLCEvent *, void *);   ///< Event handler callback.

class CVLCPlayer
{
public:  
  virtual ~CVLCPlayer();
  static CVLCPlayer* Instance();

  typedef enum
  {
    e_NothingSpecial=0,
    e_Opening,
    e_Buffering,
    e_Playing,
    e_Paused,
    e_Stopped,
    e_Ended,
    e_Error
  }PLAYSTATE;

protected:
  CVLCPlayer();

public:
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
  PLAYSTATE GetPlayState();
  int GetAudioChannel();
  int GetWidth();
  int GetHeight();
  float GetFPS();

private:
  std::auto_ptr<CVLCPlayerImpl> m_pImpl;
  static CVLCPlayer* m_pVLCPlayer;

//   CMutexLock m_lock;
};

