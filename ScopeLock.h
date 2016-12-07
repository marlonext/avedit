#pragma once

#include <Windows.h>

class CCS
{
public:
  CCS()
  {
    ::InitializeCriticalSection(&m_cs);
  }
  ~CCS()
  {
    ::DeleteCriticalSection(&m_cs);
  }

  void Wait()
  {
    ::EnterCriticalSection(&m_cs);
  }

  void Signal()
  {
    ::LeaveCriticalSection(&m_cs);
  }


private:

  CRITICAL_SECTION m_cs;
};

class CAutoLock
{
public:
  CAutoLock(CCS& cs)
    :m_cs(cs)
  {
    m_cs.Wait();
  }

  ~CAutoLock()
  {
    m_cs.Signal();
  }

private:
  CCS& m_cs;
};

// class PReadWriteMutex
// {
//   public:
//     PReadWriteMutex();
//     void StartRead();
//     void EndRead();
//     void StartWrite();
//     void EndWrite();
// 
//   protected:
//     PSemaphore readerSemaphore;
//     PMutex     readerMutex;
//     unsigned   readerCount;
//     PMutex     starvationPreventer;
// 
//     PSemaphore writerSemaphore;
//     PMutex     writerMutex;
//     unsigned   writerCount;
// 
//     class Nest
//     {
//       Nest() { readerCount = writerCount = 0; }
//       unsigned readerCount;
//       unsigned writerCount;
//     };
//     PDictionary<POrdinalKey, Nest> nestedThreads;
//     PMutex                         nestingMutex;
// 
//     Nest * GetNest() const;
//     Nest & StartNest();
//     void EndNest();
//     void InternalStartRead();
//     void InternalEndRead();
// };

class CMutexLock
{
public:
  CMutexLock()
  {
    m_hMutex = ::CreateMutex(nullptr, TRUE, nullptr);
  }

  ~CMutexLock()
  {
    ::CloseHandle(m_hMutex);
  }

  DWORD Wait()
  {
    return ::WaitForSingleObject(m_hMutex, INFINITE);
  }

  DWORD Wait(DWORD dwMillSecs)
  {
    return ::WaitForSingleObject(m_hMutex, dwMillSecs);
  }

  void Signal()
  {
    ::ReleaseMutex(m_hMutex);
  }

private:
  HANDLE m_hMutex;
};

class CReadLock
{
public:
  CReadLock(CMutexLock& lock)
    :m_lock(lock)
  {
    m_lock.Wait();
  }

  ~CReadLock()
  {
    m_lock.Signal();
  }

private:
  CMutexLock& m_lock;
};