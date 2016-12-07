#include "StdAfx.h"
#include "EditTask.h"



//////////////////////////////////////////////////////////////////////////
CEditOpts::CEditOpts()
{
  memset(&m_EditOpts, 0, sizeof(m_EditOpts));
  m_EditOpts.dst_video_bitrate = -1;
  m_EditOpts.dst_pic_width = -1;
  m_EditOpts.dst_pic_height = -1;
  m_EditOpts.tail_switch_mode = -1;

  m_bAddTitle = m_bAddTail = false;
}




//////////////////////////////////////////////////////////////////////////
CEditTask::CEditTask()
{
  m_nTaskID = -1;
  m_strSourceName = _T("");
  m_strSourceAlias = _T("");
  m_strDestName = _T("");
  m_nProgress = 0;
  m_nTaskStatus = (UInt16b)ENUM_TASK_STATUS;
}

CEditTask::~CEditTask()
{
}


void CEditTask::SetTaskStatus(UInt16b status)
{
//   m_nTaskID = status.taskId;
//   m_nProgress = status.editProgress;
//   m_strDestName = status.billId;
  m_nTaskStatus = status;
}

//////////////////////////////////////////////////////////////////////////
CTaskManager* CTaskManager::m_pManager = NULL;

CTaskManager::CTaskManager()
{
  CreateRandIdList();
}

CTaskManager::~CTaskManager()
{
  m_idList.clear();
  TASKLISTDICT::iterator it = m_taskDict.begin();
  for (; it != m_taskDict.end(); ++it)
  {
    SAFE_DELETE(it->second);
  }
  m_taskDict.clear();
}

CTaskManager* CTaskManager::Instance()
{
  if (!m_pManager)
  {
    m_pManager = new CTaskManager;
    if (!m_pManager)
    {
      return NULL;
    }
  }

  return m_pManager;
}


BOOL CTaskManager::AddTask(CEditTask* pTask)
{
  BOOL bSame = FALSE;

  m_cs.Lock();

  TASKLISTDICT::const_iterator it = m_taskDict.begin();
  for (; it != m_taskDict.end(); ++it)
  {
    if ((it->second->GetSourceName() == pTask->GetSourceName())
        || (it->first == pTask->GetTaskID()))
    {
      bSame = TRUE;
      break;
    }
  }

  if (bSame)
  {
    m_cs.Unlock();

    return FALSE;
  }

  AddjustIdList(pTask->GetTaskID(), TRUE);
  m_taskDict.insert(std::pair<int, CEditTask*>(pTask->GetTaskID(), pTask));

  m_cs.Unlock();
  return TRUE;
}

BOOL CTaskManager::DelTask(UInt16b nTaskID)
{
  m_cs.Lock();

  TASKLISTDICT::iterator it = m_taskDict.find(nTaskID);
  if (it != m_taskDict.end())
  {
    m_taskDict.erase(it);

    AddjustIdList(nTaskID, FALSE);

    m_cs.Unlock();
    return TRUE;
  }

  m_cs.Unlock();
  return FALSE;
}

BOOL CTaskManager::RemoveAllTask()
{
  TASKLISTDICT::iterator it = m_taskDict.begin();
  for (; it != m_taskDict.end(); ++it)
  {
    delete it->second;
    it->second = NULL;
  }

  m_taskDict.clear();

  return TRUE;
}

const CEditTask* CTaskManager::GetTask(UInt16b nTaskID)
{
  m_cs.Lock();

  TASKLISTDICT::const_iterator it = m_taskDict.find(nTaskID);
  if (it != m_taskDict.end())
  {
    m_cs.Unlock();
    return it->second;
  }

  m_cs.Unlock();
  return NULL;
}


BOOL CTaskManager::UpdateTaskProgress(UInt16b nTaskID, UInt16b nProgress)
{
  m_cs.Lock();

  TASKLISTDICT::iterator it = m_taskDict.find(nTaskID);
  if (it != m_taskDict.end())
  {
    CEditTask*& pTask = it->second;
    pTask->SetTaskProgress(nProgress);

    m_cs.Unlock();
    return TRUE;
  }

  m_cs.Unlock();
  return FALSE;
}

BOOL CTaskManager::UpdateTaskAlias(UInt16b nTaskID, const CString& strAlias)
{
  if (strAlias.IsEmpty())
  {
    return FALSE;
  }

  m_cs.Lock();
  TASKLISTDICT::iterator it = m_taskDict.find(nTaskID);
  if (it != m_taskDict.end())
  {
    CEditTask*& pTask = it->second;
    pTask->SetTaskAlias(strAlias);

    m_cs.Unlock();
    return TRUE;
  }

  m_cs.Unlock();
  return FALSE;
}

BOOL CTaskManager::UpdateTaskStatus(const EDIT_TASK_STATUS& status)
{
  m_cs.Lock();

  TASKLISTDICT::iterator it = m_taskDict.find(status.taskId);
  if (it != m_taskDict.end())
  {
    CEditTask*& pTask = it->second;
    pTask->SetTaskStatus(status.taskStatus);
    pTask->SetTaskProgress(status.editProgress);

    m_cs.Unlock();
    return TRUE;
  }

  m_cs.Unlock();
  return FALSE;
}

BOOL CTaskManager::IsSameTask(const CString& strName)
{
  BOOL bSame = FALSE;

  m_cs.Lock();
  TASKLISTDICT::const_iterator it = m_taskDict.begin();
  for (; it != m_taskDict.end(); ++it)
  {
    const CEditTask* pTask = it->second;
    if (strName == pTask->m_strSourceName)
    {
      m_cs.Unlock();
      bSame = TRUE;
      break;
    }
  }

  m_cs.Unlock();
  return bSame;
}

void CTaskManager::CreateRandIdList()
{
  srand( (unsigned)time( NULL ) );

  //随机生成一些编号
  for (UInt16b i = 1; i <= MAX_TASKLIST_NUM; ++i)
  {
    m_idList.push_back(i);
  }

  for (UInt16b i = 0; i < MAX_TASKLIST_NUM; ++i)
  {
    int nRandIndex = rand() % (MAX_TASKLIST_NUM - i);

    int nTemp = m_idList[i];
    m_idList[i] = m_idList[nRandIndex];
    m_idList[nRandIndex] = nTemp;
  }
}

void CTaskManager::AddjustIdList(UInt16b nTaskID, BOOL bAddTask)
{
  unsigned i = 0, j = 0;

  if (bAddTask)
  {
    for (i = m_taskDict.size(); i < m_idList.size(); ++i)
    {
      if (m_idList[i] == nTaskID)
      {
        int nTempID = m_idList[m_taskDict.size()];
        m_idList[m_taskDict.size()] = nTaskID;
        m_idList[i] = nTempID;

        break;
      }
    }
    
  }
  else
  {
    for (; i < m_idList.size(); ++i)
    {
      if (m_idList[i] == nTaskID)
      {
        break;
      }
    }

    for (j = i + 1; j < m_idList.size(); ++j)
    {
      m_idList[i++] = m_idList[j];
    }

    m_idList[m_idList.size()-1] = nTaskID;
  }  
}

UInt16b CTaskManager::AllocTaskID()
{
//   TASKLISTDICT::const_iterator it = m_taskDict.begin();
//   for (; it != m_taskDict.end(); ++it)
//   {
//     const CEditTask* pTask = it->second;
//     if (strTaskName == pTask->GetSourceName())
//     {
//       return -1;
//     }
//   }

  return m_idList[m_taskDict.size()];
}

const CString& CTaskManager::GetTaskName(UInt16b nTaskID)
{
  TASKLISTDICT::const_iterator it = m_taskDict.find(nTaskID);
  if (it != m_taskDict.end())
  {
    return it->second->GetSourceName();
  }
}

const CString& CTaskManager::GetTaskDir(UInt16b nTaskID)
{
  TASKLISTDICT::const_iterator it = m_taskDict.find(nTaskID);
  if (it != m_taskDict.end())
  {
    return it->second->GetDir();
  }
}
