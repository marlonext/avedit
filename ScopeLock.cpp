#include "StdAfx.h"
#include "ScopeLock.h"


// CScopeLock::CScopeLock(void)
// {
// }
// 
// 
// CScopeLock::~CScopeLock(void)
// {
// }


// PReadWriteMutex::PReadWriteMutex()
//   : m_readerSemaphore(1, 1)
//   , m_readerCount(0)
//   , m_writerSemaphore(1, 1)
//   , m_writerCount(0)
// {
//   PTRACE(5, "PTLib\tCreated read/write mutex " << this);
// }
// 
// 
// PReadWriteMutex::~PReadWriteMutex()
// {
//   PTRACE(5, "PTLib\tDestroying read/write mutex " << this);
// 
//   EndNest(); // Destruction while current thread has a lock is OK
// 
//   /* There is a small window during destruction where another thread is on the
//      way out of EndRead() or EndWrite() where it checks for nested locks.
//      While the check is protected by mutex, there is a moment between one
//      check and the next where the object is unlocked. This is normally fine,
//      except for if a thread then goes and deletes the object out from under
//      the threads about to do the second check.
// 
//      Note if this goes into an endless loop then there is a big problem with
//      the user of the PReadWriteMutex, as it must be CONTINUALLY trying to use
//      the object when someone wants it gone. Technically this fix should be
//      done by the user of the class too, but it is easier to fix here than
//      there so practicality wins out!
//    */
//   while (!m_nestedThreads.empty())
//     PThread::Sleep(10);
// }
// 
// 
// PReadWriteMutex::Nest * PReadWriteMutex::GetNest()
// {
//   PWaitAndSignal mutex(m_nestingMutex);
//   NestMap::iterator it = m_nestedThreads.find(PThread::GetCurrentThreadId());
//   return it != m_nestedThreads.end() ? &it->second : NULL;
// }
// 
// 
// void PReadWriteMutex::EndNest()
// {
//   m_nestingMutex.Wait();
//   m_nestedThreads.erase(PThread::GetCurrentThreadId());
//   m_nestingMutex.Signal();
// }
// 
// 
// PReadWriteMutex::Nest & PReadWriteMutex::StartNest()
// {
//   PWaitAndSignal mutex(m_nestingMutex);
//   // The std::map will create the entry if it doesn't exist
//   return m_nestedThreads[PThread::GetCurrentThreadId()];
// }
// 
// 
// void PReadWriteMutex::StartRead()
// {
//   // Get the nested thread info structure, create one it it doesn't exist
//   Nest & nest = StartNest();
// 
//   // One more nested call to StartRead() by this thread, note this does not
//   // need to be mutexed as it is always in the context of a single thread.
//   nest.m_readerCount++;
// 
//   // If this is the first call to StartRead() and there has not been a
//   // previous call to StartWrite() then actually do the text book read only
//   // lock, otherwise we leave it as just having incremented the reader count.
//   if (nest.m_readerCount == 1 && nest.m_writerCount == 0)
//     InternalStartRead(nest);
// }
// 
// 
// void PReadWriteMutex::InternalWait(Nest & nest, PSync & sync) const
// {
//   nest.m_waiting = true;
// 
// #if PTRACING
//   if (sync.Wait(15000)) {
//     nest.m_waiting = false;
//     return;
//   }
// 
//   if (PTrace::CanTrace(1)) {
//     ostream & trace = PTrace::Begin(1, __FILE__, __LINE__);
//     trace << "PTLib\tPossible deadlock in read/write mutex " << this << " :\n";
//     for (std::map<PThreadIdentifier, Nest>::const_iterator it = m_nestedThreads.begin(); it != m_nestedThreads.end(); ++it) {
//       if (it != m_nestedThreads.begin())
//         trace << '\n';
//       trace << "  thread-id=" << it->first << " (0x" << std::hex << it->first << std::dec << "),"
//                 " readers=" << it->second.m_readerCount << ","
//                 " writers=" << it->second.m_writerCount;
//       if (!it->second.m_waiting)
//         trace << ", LOCKED";
//     }
//     trace << PTrace::End;
//   }
// 
//   sync.Wait();
// 
//   PTRACE(1, "PTLib\tPhantom deadlock in read/write mutex " << this);
// #else
//   sync.Wait();
// #endif
// 
// 
//   nest.m_waiting = false;
// }
// 
// 
// void PReadWriteMutex::InternalStartRead(Nest & nest)
// {
//   // Text book read only lock
// 
//   InternalWait(nest, m_starvationPreventer);
//    InternalWait(nest, m_readerSemaphore);
//     InternalWait(nest, m_readerMutex);
// 
//      m_readerCount++;
//      if (m_readerCount == 1)
//        InternalWait(nest, m_writerSemaphore);
// 
//     m_readerMutex.Signal();
//    m_readerSemaphore.Signal();
//   m_starvationPreventer.Signal();
// }
// 
// 
// void PReadWriteMutex::EndRead()
// {
//   // Get the nested thread info structure for the curent thread
//   Nest * nest = GetNest();
// 
//   // If don't have an active read or write lock or there is a write lock but
//   // the StartRead() was never called, then assert and ignore call.
//   if (nest == NULL || nest->m_readerCount == 0) {
//     PAssertAlways("Unbalanced PReadWriteMutex::EndRead()");
//     return;
//   }
// 
//   // One less nested lock by this thread, note this does not
//   // need to be mutexed as it is always in the context of a single thread.
//   nest->m_readerCount--;
// 
//   // If this is a nested read or a write lock is present then we don't do the
//   // real unlock, the decrement is enough.
//   if (nest->m_readerCount > 0 || nest->m_writerCount > 0)
//     return;
// 
//   // Do text book read lock
//   InternalEndRead(*nest);
// 
//   // At this point all read and write locks are gone for this thread so we can
//   // reclaim the memory.
//   EndNest();
// }
// 
// 
// void PReadWriteMutex::InternalEndRead(Nest & nest)
// {
//   // Text book read only unlock
// 
//   InternalWait(nest, m_readerMutex);
// 
//   m_readerCount--;
//   if (m_readerCount == 0)
//     m_writerSemaphore.Signal();
// 
//   m_readerMutex.Signal();
// }
// 
// 
// void PReadWriteMutex::StartWrite()
// {
//   // Get the nested thread info structure, create one it it doesn't exist
//   Nest & nest = StartNest();
// 
//   // One more nested call to StartWrite() by this thread, note this does not
//   // need to be mutexed as it is always in the context of a single thread.
//   nest.m_writerCount++;
// 
//   // If is a nested call to StartWrite() then simply return, the writer count
//   // increment is all we haev to do.
//   if (nest.m_writerCount > 1)
//     return;
// 
//   // If have a read lock already in this thread then do the "real" unlock code
//   // but do not change the lock count, calls to EndRead() will now just
//   // decrement the count instead of doing the unlock (its already done!)
//   if (nest.m_readerCount > 0)
//     InternalEndRead(nest);
// 
//   // Note in this gap another thread could grab the write lock, thus
// 
//   // Now do the text book write lock
//   InternalWait(nest, m_writerMutex);
// 
//   m_writerCount++;
//   if (m_writerCount == 1)
//     InternalWait(nest, m_readerSemaphore);
// 
//   m_writerMutex.Signal();
// 
//   InternalWait(nest, m_writerSemaphore);
// }
// 
// 
// void PReadWriteMutex::EndWrite()
// {
//   // Get the nested thread info structure for the curent thread
//   Nest * nest = GetNest();
// 
//   // If don't have an active read or write lock or there is a read lock but
//   // the StartWrite() was never called, then assert and ignore call.
//   if (nest == NULL || nest->m_writerCount == 0) {
//     PAssertAlways("Unbalanced PReadWriteMutex::EndWrite()");
//     return;
//   }
// 
//   // One less nested lock by this thread, note this does not
//   // need to be mutexed as it is always in the context of a single thread.
//   nest->m_writerCount--;
// 
//   // If this is a nested write lock then the decrement is enough and we
//   // don't do the actual write unlock.
//   if (nest->m_writerCount > 0)
//     return;
// 
//   // Begin text book write unlock
//   m_writerSemaphore.Signal();
// 
//   InternalWait(*nest, m_writerMutex);
// 
//   m_writerCount--;
//   if (m_writerCount == 0)
//     m_readerSemaphore.Signal();
// 
//   m_writerMutex.Signal();
//   // End of text book write unlock
// 
//   // Now check to see if there was a read lock present for this thread, if so
//   // then reacquire the read lock (not changing the count) otherwise clean up the
//   // memory for the nested thread info structure
//   if (nest->m_readerCount > 0)
//     InternalStartRead(*nest);
//   else
//     EndNest();
// }
// 
// 
// /////////////////////////////////////////////////////////////////////////////
// 
// PReadWaitAndSignal::PReadWaitAndSignal(const PReadWriteMutex & rw, PBoolean start)
//   : mutex((PReadWriteMutex &)rw)
// {
//   if (start)
//     mutex.StartRead();
// }
// 
// 
// PReadWaitAndSignal::~PReadWaitAndSignal()
// {
//   mutex.EndRead();
// }
