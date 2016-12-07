#pragma once

#include "TimerShaft.h"

class CEditStatueFile
{
public:
  CEditStatueFile(void);
  virtual ~CEditStatueFile(void);
  BOOL Read(EDITSTATUSLIST& list);
  BOOL Write(const EDITSTATUS& statues);
  BOOL Update(const EDITSTATUS& status);
  BOOL Delete(UInt16b nTaskId);
  bool Init();
  BOOL IsEmpty();
  int GetItemCount() const {return m_nStatusCount;}
  BOOL Clear();
  bool IsInit() const {return m_bInit;}

private:
  FILE* m_fpStatues;
  int   m_nStatusCount;
  CCriticalSection m_cs;
  bool m_bInit;
};


//////////////////////////////////////////////////////////////////////////
#define MAX_CUT_TIME 4
typedef struct tagEditTaskRecord
{
  int m_nID;
  CS_EDIT_OPTS  m_editOpts;
  int           m_nCutTimeCount;
  CS_CUT_TIME*  m_pCutTime;
  BYTE m_bFlag;
}EDIT_TASK_RECORD;

typedef std::vector<EDIT_TASK_RECORD*> EDIT_TASK_LIST;
typedef std::map<int, CS_EDIT_TASK*> EDIT_TASK_DICT;


class CTimeRecord
{
public:
  CTimeRecord();
  virtual ~CTimeRecord();

  BOOL Read(EDIT_TASK_LIST& taskList);
  BOOL Write(const EDIT_TASK_RECORD& editTaskRecord);
  BOOL Delete(int nTaskId);
  inline BOOL IsEmpty() const {return m_nCount == 0 ? TRUE : FALSE;}
  bool Init();
  BOOL Clear();

private:
//   FILE* m_fpTime;
  int m_nCount;
  CCriticalSection m_cs;

  HANDLE m_hFile;
};