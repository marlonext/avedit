#pragma once

#include "cs\editCsSkt.h"
#include <map>


class CEditOpts
{
public:
  CEditOpts();

  void SetDstFilePath(const TCHAR* pFilePath, unsigned int nPathLen)
  {
    memcpy(m_EditOpts.dst_file_path, pFilePath, nPathLen);
  }

  void SetTitlePicPath(const TCHAR* pPicPath, unsigned int nPathLen)
  {
    memcpy(m_EditOpts.title_pic_path, pPicPath, nPathLen);
  }

  void SetTailPicPath(const TCHAR* pPicPath, unsigned int nPathLen)
  {
    memcpy(m_EditOpts.tail_pic_path, pPicPath, nPathLen);
  }

  void SetTitleDuration(int64_t duration)
  {
    m_EditOpts.title_duration = duration;
  }

  void SetTailDuration(int64_t duration)
  {
    m_EditOpts.tail_duration = duration;
  }

  void SetAddTitle(bool bAdd) {m_bAddTitle = bAdd;}
  void SetAddTail(bool bAdd) {m_bAddTail = bAdd;}
  bool IsTitleAdded() {return m_bAddTitle;}
  bool isTailAdded() {return m_bAddTail;}
  const CS_EDIT_OPTS& GetEditOpts() {return m_EditOpts;}

private:
  CS_EDIT_OPTS m_EditOpts;    
  bool m_bAddTitle;
  bool m_bAddTail;
};

class CStreamOpts
{
public:
  CStreamOpts()
  {
    memset(m_StreamOpt.src_file_path, 0, sizeof(m_StreamOpt.src_file_path));
    m_StreamOpt.a_stream0_dst_id = -1;
    m_StreamOpt.v_stream0_dst_id = -1; 
  }

  void SetSrcFilePath(const TCHAR* pFilePath, unsigned int nPathLen)
  {
    if (pFilePath)
    {
      memcpy(m_StreamOpt.src_file_path, pFilePath, nPathLen);
    }
  }

private:
  CS_STREAM_MAP m_StreamOpt;
};

class CCutTime
{
public:
  CCutTime()
  {
    m_CutTime.startTimeInUs = 0; 
    m_CutTime.endTimeInUs = 0;   
    m_CutTime.streamMapNum  = -1;
    m_CutTime.switch_mode = -1;
  }

  void SetCutStartTime(int64_t StartTime)
  {
    m_CutTime.startTimeInUs = StartTime;
  }

  void SetCutEndTime(int64_t EndTime)
  {
    m_CutTime.endTimeInUs = EndTime;
  }

private:
  CS_CUT_TIME m_CutTime;
};

class CTaskManager;
class CEditTask
{
  friend class CTaskManager;
private:
  UInt16b     m_nTaskID;
  CString     m_strSourceName;
  CString     m_strSourceAlias;
  CString     m_strDestName;
  CString     m_strDir;
  UInt16b     m_nProgress;
  UInt16b     m_nTaskStatus;

public:
  CEditTask();
  ~CEditTask();

  void SetTaskID(UInt16b nID) {m_nTaskID = nID;}
  void SetSourceName(const CString& strSourceName) 
  {m_strSourceName = strSourceName;}
  void SetTaskAlias(const CString& strSourceAlias) 
  {m_strSourceAlias = strSourceAlias;}
  void SetTaskStatus(UInt16b status);
  void SetTaskProgress(int nProgress) {m_nProgress = nProgress;}
  void SetDestName(const CString& strDestName) {m_strDestName = strDestName;}
  void SetDir(const CString& strDir) {m_strDir = strDir;}

  UInt16b GetTaskID() const {return m_nTaskID;}
  const CString& GetSourceName() const {return m_strSourceName;}
  const CString& GetSourceAlias() const {return m_strSourceAlias;}
  const CString& GetDestName() const {return m_strDestName;}
  const CString& GetDir() const {return m_strDir;}
  UInt16b GetTaskProgress() const {return m_nProgress;}
  UInt16b GetTaskStatus() const {return m_nTaskStatus;}
};

typedef std::vector<int> TASKIDLIST;
typedef std::map<int, CEditTask*> TASKLISTDICT;
class CTaskManager
{
private:
  static CTaskManager* m_pManager;
  TASKLISTDICT m_taskDict;
  CCriticalSection m_cs;
  TASKIDLIST m_idList;

private:
  CTaskManager();
  void CreateRandIdList();
  void AddjustIdList(UInt16b nTaskID ,BOOL bAddTask);

public:
  static CTaskManager* Instance();
  ~CTaskManager();

  BOOL AddTask(CEditTask* pTask);
  BOOL DelTask(UInt16b nTaskID);
  BOOL RemoveAllTask();
  const CEditTask* GetTask(UInt16b nTaskID);
  BOOL UpdateTaskProgress(UInt16b nTaskID, UInt16b nProgress);
  BOOL UpdateTaskAlias(UInt16b nTaskID, const CString& strAlias);
  BOOL UpdateTaskStatus(const EDIT_TASK_STATUS& status);
  BOOL IsSameTask(const CString& strName);
  UInt16b AllocTaskID();

  const CString& GetTaskName(UInt16b nTaskID);
  const CString& GetTaskDir(UInt16b nTaskID);

  TASKLISTDICT& GetTaskListDict(){return m_taskDict;}
};
